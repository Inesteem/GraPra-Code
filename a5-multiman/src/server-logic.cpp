#include "server-logic.h"
#include "messages.h"

#include <libcgl/impex.h>


using namespace std;

unsigned int GameStage::s_nextBuilding = 0;
unsigned int GameStage::s_nextTroup = 0;


int player_frac[10];

void GameStage::init(unsigned int x, unsigned int y)
{
	for(int i = 0; i < 10; i++)
		player_frac[i] = 0;
		
    m_mapX = x;
    m_mapY = y;
}

void GameStage::handle_client_settings(unsigned int playerId, unsigned int colorId, unsigned int frac ){
	player_frac[playerId] = frac;
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

        exit(0);

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

    for(int i = 0; i < m_armies.size(); i++) {
        m_armies[i]->Update();
        // TODO delete armies if update returns true
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

void GameStage::addArmy(unsigned int sourceBuildingID, unsigned int destinationBuildingID, unsigned int unitCount)
{
    Building *a = m_buildings[sourceBuildingID];
    Building *b = m_buildings[destinationBuildingID];

    Army *army = new Army(this, a, b, unitCount);
    m_armies.push_back(army);
}

Army::Army(GameStage *gameStage, Building *sourceBuilding, Building *destinationBuilding, unsigned int unitCount)
    : GameObject(gameStage, 0, 0, 0)
{
    int unitsLeft = unitCount;

    for(;;) {
        int troupUnits = unitsLeft > s_maxTroupSize ? s_maxTroupSize : unitsLeft;
        cout << "creating troup, size: " << troupUnits << endl;
        unitsLeft -= s_maxTroupSize;
        Troup *t = new Troup(gameStage, sourceBuilding, destinationBuilding, troupUnits, GameStage::s_nextTroup++);
        m_toSpawn.push_back(t);

        if(unitsLeft <= 0) break;
    }

    m_spawnTimer.start();

    Troup *t = m_toSpawn[0];
    m_toSpawn.erase(m_toSpawn.begin());

    m_gameStage->addTroup(t);
}

bool Army::Update()
{
    if(m_toSpawn.size() == 0) return true;

    if(m_spawnTimer.look() < wall_time_timer::msec(m_spawnTime)) return false;

    cout << "spawning a troup in army" << endl;

    m_spawnTimer.restart();

    Troup *t = m_toSpawn[0];
    m_toSpawn.erase(m_toSpawn.begin());

    m_gameStage->addTroup(t);

    if(m_toSpawn.size() == 0) return true;
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

void GameStage::addTroup(Troup *troup)
{
    if(troup->m_unitCount > troup->m_source->m_unitCount) {
        // discard this troup
        delete troup;
        return;
    }
    troup->m_source->KillUnits(troup->m_unitCount);

    m_troups[troup->m_id] = troup;

    cout << "Troup starting at (" << troup->m_source->m_x << ", " << troup->m_source->m_y << "), dest (" << troup->m_destination->m_x << ", " << troup->m_destination->m_y << ")" << endl;

    msg::spawn_troup_server sts = make_message<msg::spawn_troup_server>();
    sts.destinationId = troup->m_destination->m_id;
    sts.playerId = troup->m_source->m_player;
    sts.sourceId = troup->m_source->m_id;
    sts.troupId = troup->m_id;
    sts.unitCount = troup->m_unitCount;
    sts.frac = player_frac[troup->m_source->m_player];
    broadcast(&sts);

    troup->NextDestination();
}
void GameStage::upgrade_building_house(unsigned int buildingId){
    Building *building = m_buildings[buildingId];

    if(building->m_state == msg::building_state::house_lvl3) return; // already highest level

    msg::building_upgrade bu = make_message<msg::building_upgrade>();
    bu.buildingId = buildingId;

    int units = 0;

    if(building->m_state == msg::building_state::construction_site){ 
        units = msg::upgrade_cost::UpgradeToHouseLvl1;
        if(units > building->m_unitCount) return; // not enough units
        building->m_state = msg::building_state::house_lvl1;
        
	}else if(building->m_state == msg::building_state::turret_lvl1 || building->m_state == msg::building_state::turret_lvl2) {    

        units = msg::upgrade_cost::RebuildingToHouseLvl1;
        if(units > building->m_unitCount) return; // not enough units
        building->m_state = msg::building_state::house_lvl1;
		
    } else if(building->m_state == msg::building_state::house_lvl1) {
        units = msg::upgrade_cost::UpgradeToHouseLvl2;
        if(units > building->m_unitCount) return; // not enough units
        building->m_state = msg::building_state::house_lvl2;
        
    } else if(building->m_state == msg::building_state::house_lvl2) {
        units = msg::upgrade_cost::UpgradeToHouseLvl3;
        if(units > building->m_unitCount) return; // not enough units
        building->m_state = msg::building_state::house_lvl3;
    } else return;

    bu.state = building->m_state;
    bu.frac = player_frac[building->m_player];
    broadcast(&bu);
		
	m_buildings[buildingId]->KillUnits(units);
}

void GameStage::upgrade_building_turret(unsigned int buildingId){
    Building *building = m_buildings[buildingId];

    if(building->m_state == msg::building_state::turret_lvl2) return; // already highest level

    msg::building_upgrade bu = make_message<msg::building_upgrade>();
    bu.buildingId = buildingId;

    int units = 0;

    if(building->m_state == msg::building_state::construction_site || building->m_state == msg::building_state::house_lvl1) {
        units = msg::upgrade_cost::UpgradeToTurretLvl1;
        if(units > building->m_unitCount) return; // not enough units
        building->m_state = msg::building_state::turret_lvl1;

	} else if(building->m_state == msg::building_state::house_lvl2 || building->m_state == msg::building_state::house_lvl3){
        units = msg::upgrade_cost::RebuildingToTurretLvl1;
        if(units > building->m_unitCount) return; // not enough units
        building->m_state = msg::building_state::turret_lvl1;
        
	} else if(building->m_state == msg::building_state::turret_lvl1) {
        units = msg::upgrade_cost::UpgradeToTurretLvl2;
        if(units > building->m_unitCount) return; // not enough units
        building->m_state = msg::building_state::turret_lvl2;
    } else return;

    bu.state = building->m_state;
    bu.frac = player_frac[building->m_player];
    broadcast(&bu);

    m_buildings[buildingId]->KillUnits(units);
}

Troup::Troup(GameStage *gameStage, Building *sourceBuilding, Building *destinationBuilding, unsigned int unitCount, unsigned int id)
    : GameObject(gameStage, 0, 0, id), m_unitCount(unitCount), m_source(sourceBuilding), m_destination(destinationBuilding)
{
    m_x = sourceBuilding->m_x;
    m_y = sourceBuilding->m_y;

    PathNode sourceNode(sourceBuilding->m_x, sourceBuilding->m_y);

    PathNode destinationNode(destinationBuilding->m_x, destinationBuilding->m_y);

    m_path = new Path(this, sourceNode, destinationNode, gameStage->m_mapX, gameStage->m_mapY);

    m_stepTimer.start();
}

bool Troup::NextDestination()
{
    PathNode currentDestination = m_path->m_nodes.front();
    m_path->m_nodes.pop_front();

    m_x = currentDestination.mapX;
    m_y = currentDestination.mapY;

    PathNode nextDestination = m_path->m_nodes.front();

    cout << "Troup " << m_id << " now at destination (" << m_x << "," << m_y << "), next (" << nextDestination.mapX << "," << nextDestination.mapY << ")" << endl;

//    if((m_x < 0 || m_x > 32) && (m_y < 0 || m_y > 32)){
//        exit(-1); //TODO
//    }
    if(m_path->m_nodes.empty()) {

        // arrived at destination
        cout << "Troup " << m_id << " arrived at destination." << endl;
        delete m_path;
        return true;
    }

    msg::next_troup_destination ntd = make_message<msg::next_troup_destination>();
    ntd.mapX = nextDestination.mapX;
    ntd.mapY = nextDestination.mapY;
    ntd.troupId = m_id;
    ntd.time = m_stepTime;
    //cout << "NTD: (" << ntd.mapX << "," << ntd.mapY << ") id " << ntd.troupId << ",time " << ntd.time << endl;
    broadcast(&ntd);

    return false;
}

bool Troup::Update()
{
    if(m_stepTimer.look() < m_stepTime) {
        return false;
    }

    //cout << m_stepTimer.look() << endl;
    m_stepTimer.restart();
    //cout << m_stepTimer.look() << endl;

    //cout << "troup " << m_id << " to position (" << m_x << ", " << m_y << ")" << endl;
    return NextDestination();
}

Building::Building(GameStage *gameStage, unsigned int x, unsigned int y, unsigned int id)
    : GameObject(gameStage, x, y, id), m_player(-1), m_state(msg::building_state::construction_site)
{
    m_unitCount = 0;
    m_generateUnitsTimer.start();
}

void Building::Update()
{
    if(m_player == - 1 || m_state == msg::building_state::turret_lvl1 || m_state == msg::building_state::turret_lvl2 || m_state == msg::building_state::construction_site) return;

    unsigned int upgradeRate = msg::unit_generation_time::UpgradeRateLvl1;
    if(m_state == msg::building_state::house_lvl2) {
        upgradeRate = msg::unit_generation_time::UpgradeRateLvl2;
    } else if(m_state == msg::building_state::house_lvl3) {
        upgradeRate = msg::unit_generation_time::UpgradeRateLvl3;
    }

    if(m_generateUnitsTimer.look() >= wall_time_timer::msec(upgradeRate) && m_unitCount < 500) {
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

    cout << "incoming troup, src player: " << src->m_player << ", dest player: " << dest->m_player << endl;

    if(src->m_player == dest->m_player) {
        // own troup
        m_unitCount += troup->m_unitCount;
        msg::building_unit_generated bug = make_message<msg::building_unit_generated>();
        bug.newUnitCount = m_unitCount;
        bug.buildingId = m_id;
        broadcast(&bug);
    } else {
        unsigned int battleValue = troup->m_unitCount;
        if(this->m_state == msg::building_state::turret_lvl1) {
            battleValue = troup->m_unitCount / msg::defence_value::TowerLvl1;
        } else if(this->m_state == msg::building_state::turret_lvl2) {
            battleValue = troup->m_unitCount / msg::defence_value::TowerLvl2;
        }

        if(battleValue <= dest->m_unitCount) {
            m_unitCount -= battleValue;
            msg::building_unit_generated bug = make_message<msg::building_unit_generated>();
            bug.newUnitCount = m_unitCount;
            bug.buildingId = m_id;
            broadcast(&bug);
        } else {           
            dest->m_state = msg::building_state::house_lvl1;
            //msg::building_upgrade bu = make_message<msg::building_upgrade>();
            //bu.buildingId = dest->m_id;
            //bu.state = dest->m_state;
            //broadcast(&bu);

            msg::building_owner_changed boc = make_message<msg::building_owner_changed>();
            boc.buildingId = dest->m_id;
            boc.oldOwner = dest->m_player;
            boc.newOwner = src->m_player;
            boc.frac = player_frac[m_player];

            dest->m_player = src->m_player;

            unsigned int diedUnits = dest->m_unitCount;
            if(this->m_state == msg::building_state::turret_lvl1) {
                diedUnits = dest->m_unitCount * msg::defence_value::TowerLvl1;
            } else if(this->m_state == msg::building_state::turret_lvl2) {
                diedUnits = dest->m_unitCount * msg::defence_value::TowerLvl2;
            }
            dest->m_unitCount = troup->m_unitCount - diedUnits;

            boc.newUnitCount = dest->m_unitCount;
            broadcast(&boc);
        }
    }


}

Path::Path(Troup *troup, PathNode &source, PathNode &destination, unsigned int x, unsigned int y) : m_mapX(x), m_mapY(y), m_troup(troup)
{
    //FindDirectPath(source, destination);
    FindPathAStar(source, destination);

    //DumpPath("pathdebug.png");
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

void Path::RetracePath(PathNode startPosition, PathNode current, PathNode endPosition)
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

    m_nodes.erase(--m_nodes.end());
    m_nodes.push_back(endPosition);
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
            //cout << "BLOCKED" << endl;
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
            RetracePath(startPosition, current, endPosition);
            return;
        }

        m_open[current.mapY][current.mapX] = false;
        m_closed[current.mapY][current.mapX] = true;

        //logState(current, startPosition, endPosition);

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
