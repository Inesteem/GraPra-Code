#define update_1 10
#define update_2 100


#include "server-logic.h"
#include "messages.h"

#include <libcgl/impex.h>


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
    if(m_gameOver) return;

    int winner = checkGameOver();

    if(winner > -1) {
        m_gameOver = true;

        msg::game_over go = make_message<msg::game_over>();
        go.winner = winner;
        broadcast(&go);

        return;
    }

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

int GameStage::checkGameOver() {
    int winningPlayer = -1;
    for(auto& b : m_buildings) {
        Building *building = (Building*) b.second;

        if(building->m_player == -1) {
            return -1;
        }

        if(winningPlayer == -1) {
            winningPlayer = building->m_player;
            continue;
        }

        if(winningPlayer != building->m_player) {
            return -1;
        }
    }

    return winningPlayer;
}

Building* GameStage::spawnHouse(unsigned int x, unsigned int y)
{
    Building *b = new Building(this, x,y, GameStage::s_nextBuilding);
    m_buildings[GameStage::s_nextBuilding++] =  b;
    return b;
}

void Building::KillUnits(unsigned int unitCount){
		m_unitCount -= unitCount; 	
        msg::building_unit_generated bug = make_message<msg::building_unit_generated>();
        bug.newUnitCount = m_unitCount;
        bug.buildingId = m_id;
        broadcast(&bug);
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
    t->Update();

    return t;
}
void GameStage::upgrade_building(unsigned int buildingId, unsigned int state){

	
	msg::building_upgrade bu = make_message<msg::building_upgrade>();
	bu.buildingId = buildingId;
	bu.state = state;
	broadcast(&bu);		
	
	int units = 0;
	
	if(state == 1)
		units = update_1;
	else if(state == 2)
		units = update_2;
	
	m_buildings[buildingId]->KillUnits(units);
}

Troup::Troup(GameStage *gameStage, Building *sourceBuilding, Building *destinationBuilding, unsigned int unitCount, unsigned int id)
    : GameObject(gameStage, 0, 0, id), m_unitCount(unitCount), m_source(sourceBuilding), m_destination(destinationBuilding), m_waiting(false)
{
    m_x = sourceBuilding->m_x;
    m_y = sourceBuilding->m_y;

    PathNode sourceNode(sourceBuilding->m_x, sourceBuilding->m_y);

    PathNode destinationNode(destinationBuilding->m_x, destinationBuilding->m_y);

    m_path = new Path(this, sourceNode, destinationNode, gameStage->m_mapX, gameStage->m_mapY);

    m_stepTimer.start();
}
static int first_run = 0;
bool Troup::Update()
{
    if(m_waiting) {
        if(m_stepTimer.look() < 50) {
                return false;
        } else {
            m_stepTimer.restart();
            m_waiting = false;
        }
    }

    if(m_stepTimer.look() < m_stepTime - 50) {
        return false;
    }

    m_waiting = true;

	cout << m_stepTimer.look() << endl;
    m_stepTimer.restart();
	cout << m_stepTimer.look() << endl;

    PathNode nextDestination = m_path->m_nodes.front();
    m_path->m_nodes.pop_front();

    m_x = nextDestination.mapX;
    m_y = nextDestination.mapY;
//    if((m_x < 0 || m_x > 32) && (m_y < 0 || m_y > 32)){
//        exit(-1); //TODO
//    }
    if(m_path->m_nodes.empty()) {

        // arrived at destination
        cout << "Troup " << m_id << " arrived at destination." << endl;
        delete m_path;
        return true;
    }
    if( first_run == 0 ){
        first_run = 1;
        return false;
    }
    msg::next_troup_destination ntd = make_message<msg::next_troup_destination>();
    ntd.mapX = m_x;
    ntd.mapY = m_y;
    ntd.troupId = m_id;
    ntd.time = m_stepTime;
    cout << "NTD: (" << ntd.mapX << "," << ntd.mapY << ") id " << ntd.troupId << ",time " << ntd.time;
    broadcast(&ntd);

    cout << "\d troup " << m_id << " to position (" << m_x << ", " << m_y << ")" << endl;
    return false;
}

Building::Building(GameStage *gameStage, unsigned int x, unsigned int y, unsigned int id)
    : GameObject(gameStage, x, y, id), m_player(-1)
{
    m_unitCount = 0;
    m_generateUnitsTimer.start();
}

void Building::Update()
{
    if(m_player == - 1) return;

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
    Building *src = troup->m_source;
    Building *dest = troup->m_destination;

    cout << "src player: " << src->m_player << ", dest player: " << dest->m_player << endl;

    if(src->m_player == dest->m_player) {
        m_unitCount += troup->m_unitCount;
        msg::building_unit_generated bug = make_message<msg::building_unit_generated>();
        bug.newUnitCount = m_unitCount;
        bug.buildingId = m_id;
        broadcast(&bug);
    } else {
        if(troup->m_unitCount <= dest->m_unitCount) {
            m_unitCount -= troup->m_unitCount;
            msg::building_unit_generated bug = make_message<msg::building_unit_generated>();
            bug.newUnitCount = m_unitCount;
            bug.buildingId = m_id;
            broadcast(&bug);
        } else {
            msg::building_owner_changed boc = make_message<msg::building_owner_changed>();
            boc.buildingId = dest->m_id;
            boc.oldOwner = dest->m_player;
            boc.newOwner = src->m_player;

            dest->m_player = src->m_player;
            dest->m_unitCount = troup->m_unitCount - dest->m_unitCount;

            boc.newUnitCount = dest->m_unitCount;
            broadcast(&boc);
        }
    }


}

Path::Path(Troup *troup, PathNode &source, PathNode &destination, unsigned int x, unsigned int y) : m_mapX(x), m_mapY(y), m_troup(troup)
{
    //FindDirectPath(source, destination);
    FindPathAStar(source, destination);

    DumpPath("pathdebug.png");
}

void Path::FindDirectPath(PathNode &source, PathNode &destination)
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
    float dy = fabs(b.mapY - b.mapY);

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

    //m_nodes.insert(m_nodes.begin(), current);

    do {
        //cout << "current: "  << current.mapX << "," << current.mapY << endl;
        current = m_parent[current.mapY][current.mapX];
        //cout << "parent: " << current.mapX << "," << current.mapY << endl;
        if(current.mapX == -1 && current.mapY == -1) {
            break;
        }

        m_nodes.insert(m_nodes.begin(), current);

    } while(1);
}

void Path::ExpandNode(PathNode current, PathNode endPosition)
{
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
        if(!m_troup->m_gameStage->m_map[neighbour.mapY][neighbour.mapX]) {
            // check if way is blocked
            cout << "BLOCKED" << endl;
            continue;
        }

        //cout << "neighbour (" << neighbour.mapX << "," << neighbour.mapY << "), parent: (" << current.mapX << ", " << current.mapY << ")"  << endl;

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

void Path::logState(PathNode current, PathNode startPosition, PathNode endPosition)
{
    cout << "-------------------------------------------------------" << endl;
    for(unsigned int r = 0; r < m_mapY; r++) {
        cout << endl;
        for(unsigned int c = 0; c < m_mapX; c++) {
            if(startPosition.mapX == c && startPosition.mapY == r) {
                cout << "S";
            } else if(endPosition.mapX == c && endPosition.mapY == r) {
                cout << "E";
            } else if(current.mapX == c && current.mapY == r) {
                cout << "C";
            } else if(m_closed[r][c]) {
                cout << " ";
            } else if(m_open[r][c]) {
                cout << "o";
            } else {
                cout << "~";
            }
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

        logState(current, startPosition, endPosition);

        ExpandNode(current,endPosition);
    } while(OpenNodesExists());

    // no path found
    cout << "No path found." << endl;

    m_nodes.clear();
    m_nodes.push_back(startPosition);
    m_nodes.push_back(endPosition);
}

void Path::DumpPath(string file)
{
    vec3f *color = new vec3f[m_mapX * m_mapY];
    for(int r = 0; r < m_mapY; r++) {
        cout << endl;
        for(int c = 0; c < m_mapX; c++) {
            color[(m_mapY - 1 - r) * m_mapX + c]= m_troup->m_gameStage->m_map[r][c] ? vec3f(1,1,1) : vec3f(0,0,0);
            cout << (m_troup->m_gameStage->m_map[r][c] ? "  " : ". ");
        }
    }

    for(auto& pathNode : m_nodes) {
        int y = pathNode.mapX;
        int x = pathNode.mapY;
        if(x >= m_mapX || y >= m_mapY) {
            cout << "Wrong node (" << x << ", " << y << ")" << endl;
            continue;
        }
        color[(m_mapY - 1 - y) + x * m_mapX] = vec3f(1, 0, 0);
    }

    int startX = this->m_troup->m_source->m_x;
    int startY = this->m_troup->m_source->m_y;
    int endX = this->m_troup->m_destination->m_x;
    int ednY = this->m_troup->m_destination->m_y;

    save_png3f(color, m_mapX, m_mapY, ("./render-data/images/" + file).c_str());
}
