#include "server-logic.h"
#include "messages.h"

using namespace std;

unsigned int GameStage::s_nextBuilding = 0;
unsigned int GameStage::s_nextTroup = 0;

void GameStage::init(unsigned int x, unsigned int y)
{
    m_mapX = x;
    m_mapY = y;
}

void GameStage::Update()
{
    for(auto& building : m_buildings) {
        building.second->Update();
    }

    vector<unsigned int> toDelete;

    for(auto& troup : m_troups) {
        if(troup.second->Update()) {
            // troup arrived
            toDelete.push_back(troup.second->m_id);

            msg::troup_arrived ta = make_message<msg::troup_arrived>();
            ta.troupId = troup.second->m_id;
            broadcast(&ta);

            troup.second->m_destination->IncomingTroup(troup.second);
        }
    }

    for(auto& troupToDelete : toDelete) {
        Troup *t = m_troups[troupToDelete];
        m_troups.erase(troupToDelete);
        delete(t);
    }
}

Building* GameStage::spawnHouse(unsigned int x, unsigned int y)
{
    Building *b = new Building(this, x,y, GameStage::s_nextBuilding);
    m_buildings[GameStage::s_nextBuilding++] =  b;
    return b;
}

Troup* GameStage::spawnTroup(unsigned int sourceBuildingID, unsigned int destinationBuildingID, unsigned int unitCount)
{
    Building *a = m_buildings[sourceBuildingID];
    a->KillUnits(unitCount);

    Building *b = m_buildings[destinationBuildingID];
    Troup *t = new Troup(this, a, b, unitCount, s_nextTroup);
    m_troups[s_nextTroup++] = t;

    cout << "Troup starting at (" << a->m_x << ", " << a->m_y << "), dest (" << b->m_x << ", " << b->m_y << ")" << endl;

    msg::spawn_troup_server sts = make_message<msg::spawn_troup_server>();
    sts.destinationId = destinationBuildingID;
    sts.playerId = 0;
    sts.sourceId = sourceBuildingID;
    sts.troupId = t->m_id;
    broadcast(&sts);

    return t;
}


Troup::Troup(GameStage *gameStage, Building *sourceBuilding, Building *destinationBuilding, unsigned int unitCount, unsigned int id)
    : GameObject(gameStage, 0, 0, id), m_unitCount(unitCount), m_source(sourceBuilding), m_destination(destinationBuilding)
{
    m_x = sourceBuilding->m_x;
    m_y = sourceBuilding->m_y;

    PathNode sourceNode;
    sourceNode.mapX = sourceBuilding->m_x;
    sourceNode.mapY = sourceBuilding->m_y;

    PathNode destinationNode;
    destinationNode.mapX = destinationBuilding->m_x;
    destinationNode.mapY = destinationBuilding->m_y;

    m_path = new Path(sourceNode, destinationNode);

    m_stepTimer.start();
}

bool Troup::Update()
{
    if(m_stepTimer.look() < wall_time_timer::msec(m_stepTime)) {
        return false;
    }

    m_stepTimer.restart();

    PathNode nextDestination = m_path->m_nodes.front();
    m_path->m_nodes.pop_front();

    m_x = nextDestination.mapX;
    m_y = nextDestination.mapY;

    if(m_path->m_nodes.empty()) {
        // arrived at destination
        cout << "Troup " << m_id << " arrived at destination." << endl;
        delete m_path;
        return true;
    }

    msg::next_troup_destination ntd = make_message<msg::next_troup_destination>();
    ntd.mapX = m_x;
    ntd.mapY = m_y;
    ntd.troupId = m_id;
    broadcast(&ntd);

    cout << "Updated troup " << m_id << " to position (" << m_x << ", " << m_y << ")" << endl;
    return false;
}

Building::Building(GameStage *gameStage, unsigned int x, unsigned int y, unsigned int id)
    : GameObject(gameStage, x, y, id)
{
    m_unitCount = 20;
    m_generateUnitsTimer.start();
}

void Building::Update()
{
    if(m_generateUnitsTimer.look() >= wall_time_timer::msec(m_unitGenerationTime)) {
        m_generateUnitsTimer.restart();
        m_unitCount++;

        msg::building_unit_generated bug = make_message<msg::building_unit_generated>();
        bug.newUnitCount = m_unitCount;
        bug.buildingId = m_id;
        broadcast(&bug);
    }
}

void Building::IncomingTroup(Troup *troup)
{
    m_unitCount += troup->m_unitCount;
}

Path::Path(PathNode &source, PathNode &destination)
{
    PathNode current = source;

    while(current.mapX != destination.mapX || current.mapY != destination.mapY) {
        PathNode newNode;

        if(current.mapX < destination.mapX) {
            current.mapX++;
        } else if(current.mapX > destination.mapX) {
            current.mapX--;
        }
        newNode.mapX = current.mapX;

        if(current.mapY < destination.mapY) {
            current.mapY++;
        } else if(current.mapY > destination.mapY) {
            current.mapY--;
        }
        newNode.mapY = current.mapY;

        m_nodes.push_back(newNode);
    }
}


/*

void Path::ExpandNode(GraphNode *current, GraphNode *endPosition)
{
    //cout << "current cost: " << current->GetCurrentCost() << endl;
    //cout << "edge size: " << current->GetEdges().size() << endl;
    //cout << "id: " << current->GetId() << endl;

    for(auto & edge : current->GetEdges()) {
        //cout << "get neighbour" << endl;
        GraphNode *neighbour = edge.GetDestination();

        //cout << "is closed?" << endl;
        if(neighbour->IsClosed()) {
            //cout << "-> Node " << neighbour->GetId() << " already closed" << endl;
            continue;
        }

        // g Wert für den neuen Weg berechnen: g Wert des Vorgängers plus
        // die Kosten der gerade benutzten Kante
        //cout << "calculate cost" << endl;
        float cost = current->GetCurrentCost() + edge.GetCost();
        //cout << "-> Node " << neighbour->GetId() << ", edge cost: " << edge.GetCost() << ", total cost: " << cost << endl;
        // wenn der Nachfolgeknoten bereits auf der Open List ist,
        // aber der neue Weg nicht besser ist als der alte - tue nichts
        //cout << "abort" << endl;
        if(neighbour->IsOpen() and cost >= neighbour->GetCurrentCost()) {
            //cout << "   Node already open, no lower cost" << endl;
            continue;
        }
        // Vorgängerzeiger setzen und g Wert merken
        //cout << "save path info" << endl;
        neighbour->SetPathParent(current);
        neighbour->SetCurrentCost(cost);

        // f Wert des Knotens in der Open List aktualisieren
        // bzw. Knoten mit f Wert in die Open List einfügen

        //cout << "update open list" << endl;
        float priority = cost + neighbour->DistanceToDestination(endPosition);
        neighbour->SetPriority(priority);
        if(!neighbour->IsOpen()) {
            neighbour->Open();
            m_openQueue.push(neighbour);
            //cout << "   inserted node with priority " << priority << " and cost " << cost << endl;
        } else {
            m_openQueue.push(neighbour);
            //cout << "   updated node with priority " << priority << " and cost " << endl;
        }
    }
}

void Path::ResetQueue()
{
    while(!m_openQueue.empty())
    {
        m_openQueue.pop();
    }

    for(auto & graphNode : graphNodes) {
        graphNode.second->Reset();
    }
}


void Path::FindPathAStar(MapPosition startPosition, MapPosition endPosition)
{
    bool found = false;

    //cout << "Reset queue" << endl;
    ResetQueue();

    // enqueue start node
    //cout << "push start node" << endl;
    startPosition.start->SetPriority(startPosition.start->DistanceToDestination(endPosition.end));
    startPosition.start->SetCurrentCost(0.0);
    m_openQueue.push(startPosition.start);


    GraphNode *current;
    do {
        //getchar();
        // get node with highest priority
        //cout << "get current node" << endl;
        current = (GraphNode*) m_openQueue.top();
        m_openQueue.pop();

        //cout << "-------------------------------------------------" << endl;
        //cout << "Popped head (id " << current->GetId() << ", priority = " << current->GetPriority() << "), queue size = " << m_openQueue.size() << endl;

        if(current == endPosition.end) {
            // found path
            found = true;
            break;
        }

        //cout << "close current node" << endl;
        current->Close();

        //cout << "expand current node" << endl;

        ExpandNode(current,endPosition.end);
    } while(!m_openQueue.empty());

    if(found) {
        Path *path = retracePathFromDestination(current);
        return path;
    } else {
        return 0;
    }
}
*/
