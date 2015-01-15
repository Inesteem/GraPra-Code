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
    sts.unitCount = unitCount;
    broadcast(&sts);

    return t;
}


Troup::Troup(GameStage *gameStage, Building *sourceBuilding, Building *destinationBuilding, unsigned int unitCount, unsigned int id)
    : GameObject(gameStage, 0, 0, id), m_unitCount(unitCount), m_source(sourceBuilding), m_destination(destinationBuilding)
{
    m_x = sourceBuilding->m_x;
    m_y = sourceBuilding->m_y;

    PathNode sourceNode(sourceBuilding->m_x, sourceBuilding->m_y);

    PathNode destinationNode(destinationBuilding->m_x, destinationBuilding->m_y);

    m_path = new Path(sourceNode, destinationNode, gameStage->m_mapX, gameStage->m_mapY);

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

Path::Path(PathNode &source, PathNode &destination, unsigned int x, unsigned int y) : m_mapX(x), m_mapY(y)
{
    /*PathNode current = source;

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
    }*/

    FindPathAStar(source, destination);
}

void Path::Init()
{
    m_open = new bool*[m_mapY];
    m_closed = new bool*[m_mapY];
    m_priority = new float*[m_mapY];
    m_cost = new float*[m_mapY];
    m_parent = new PathNode*[m_mapY];

    for(unsigned int r = 0; r < m_mapY; r++) {
        m_open[r] = new bool[m_mapX];
        m_closed[r] = new bool[m_mapX];
        m_priority[r] = new float[m_mapX];
        m_cost[r] = new float[m_mapX];
        m_parent[r] = new PathNode[m_mapX];

        for(unsigned int c = 0; c < m_mapX; c++) {
            m_open[r][c] = false;
            m_closed[r][c] = false;
            m_priority[r][c] = -1;
            m_cost[r][c] = -1;
            m_parent[r][c] = PathNode(-1, -1);
        }
    }
}

float Path::AbsoluteDistance(PathNode a, PathNode b)
{
    float dx = fabs(a.mapX - b.mapX);
    float dy = fabs(b.mapX - b.mapY);

    return sqrtf(dx*dx + dy*dy);
}

bool Path::OpenNodesExists()
{
    for(unsigned int r = 0; r < m_mapY; r++) {
        for(unsigned int c = 0; c < m_mapX; c++) {
            if(m_open[r][c]) return true;
        }
    }

    return false;
}

PathNode Path::GetHighestPriorityOpenNode()
{
    float priority = 99999999999.0f;
    PathNode ret(-1, -1);

    for(unsigned int r = 0; r < m_mapY; r++) {
        for(unsigned int c = 0; c < m_mapX; c++) {
            if(m_open[r][c] && m_priority[r][c] < priority) {
                priority = m_priority[r][c];
                ret = PathNode(c, r);
            }
        }
    }

    return ret;
}

void Path::RetracePath(PathNode startPosition, PathNode current)
{
    m_nodes.clear();

    /*for(unsigned int r = 0; r < m_mapY; r++) {
        cout << endl;
        for(unsigned int c = 0; c < m_mapX; c++) {
            cout << "(" << m_parent[r][c].mapX << "," << m_parent[r][c].mapY << ") ";
        }
    }*/

    m_nodes.push_back(current);

    do {
        cout << "current: "  << current.mapX << "," << current.mapY << endl;
        current = m_parent[current.mapY][current.mapX];
        cout << "parent: " << current.mapX << "," << current.mapY << endl;

        m_nodes.push_back(current);
        if(current.mapX == -1 && current.mapY == -1) {
            break;
        }
    } while(1);
}

void Path::ExpandNode(PathNode current, PathNode endPosition)
{
    //cout << "current: (" << current.mapX << ", " << current.mapY << ")" << endl;

    vector<PathNode> neighbours;
    if(current.mapX > 0 && current.mapY > 0) neighbours.push_back(PathNode(current.mapX - 1, current.mapY - 1));

    if(current.mapY > 0) neighbours.push_back(PathNode(current.mapX    , current.mapY - 1));

    if(current.mapX < m_mapX - 1 && current.mapY > 0) neighbours.push_back(PathNode(current.mapX + 1, current.mapY - 1));

    if(current.mapX > 0) neighbours.push_back(PathNode(current.mapX - 1, current.mapY));

    if(current.mapX < m_mapX - 1) neighbours.push_back(PathNode(current.mapX + 1, current.mapY));

    if(current.mapX > 0 && current.mapY < m_mapY - 1) neighbours.push_back(PathNode(current.mapX - 1, current.mapY + 1));

    if(current.mapY < m_mapY - 1) neighbours.push_back(PathNode(current.mapX    , current.mapY + 1));

    if(current.mapX < m_mapX - 1 && current.mapY < m_mapY - 1) neighbours.push_back(PathNode(current.mapX + 1, current.mapY + 1));

    for(auto & neighbour : neighbours) {
        cout << "neighbour (" << neighbour.mapX << "," << neighbour.mapY << "), parent: (" << current.mapX << ", " << current.mapY << ")"  << endl;

        if(m_closed[neighbour.mapY][neighbour.mapX]) {
            //cout << "-> Node already closed" << endl;
            continue;
        }

        // g Wert für den neuen Weg berechnen: g Wert des Vorgängers plus
        // die Kosten der gerade benutzten Kante
        //cout << "calculate cost" << endl;
        float cost = m_cost[current.mapY][current.mapX] + 1;
        //cout << "-> Node " << neighbour->GetId() << ", edge cost: " << edge.GetCost() << ", total cost: " << cost << endl;
        // wenn der Nachfolgeknoten bereits auf der Open List ist,
        // aber der neue Weg nicht besser ist als der alte - tue nichts
        //cout << "abort" << endl;
        if(m_open[neighbour.mapY][neighbour.mapX] && cost >= m_cost[neighbour.mapY][neighbour.mapX]) {
            //cout << "   Node already open, no lower cost" << endl;
            continue;
        }
        // Vorgängerzeiger setzen und g Wert merken
        m_parent[neighbour.mapY][neighbour.mapX] = current;
        m_cost[neighbour.mapY][neighbour.mapX] = cost;

        // f Wert des Knotens in der Open List aktualisieren    osm::id_t id;
        // bzw. Knoten mit f Wert in die Open List einfügen

        float priority = cost + AbsoluteDistance(neighbour, endPosition);
        m_priority[neighbour.mapY][neighbour.mapX] = priority;
        if(!m_open[neighbour.mapY][neighbour.mapX]) {
            m_open[neighbour.mapY][neighbour.mapX] = true;
            //cout << "   inserted node with priority " << priority << " and cost " << cost << endl;
        } else {
            //cout << "   updated node with priority " << priority << " and cost " << endl;
        }
    }
}


void Path::FindPathAStar(PathNode startPosition, PathNode endPosition)
{
    //cout << "Reset queue" << endl;

    Init();

    // enqueue start node
    m_priority[startPosition.mapY][startPosition.mapX] = AbsoluteDistance(startPosition, endPosition);
    m_cost[startPosition.mapY][startPosition.mapX] = 0.0f;
    m_open[startPosition.mapY][startPosition.mapX] = true;

    do {
        //getchar();
        // get node with highest priority
        PathNode current = GetHighestPriorityOpenNode(); // -> set open to false in this position

        if(current.mapX == endPosition.mapX && current.mapY == endPosition.mapY) {
            // found path
            cout << "Found path!" << endl;
            RetracePath(startPosition, current);
            return;
        }

        m_open[current.mapY][current.mapX] = false;
        m_closed[current.mapY][current.mapX] = true;
        ExpandNode(current,endPosition);
    } while(OpenNodesExists());

    // no path found
    cout << "No path found." << endl;
    RetracePath(startPosition, endPosition);
}
