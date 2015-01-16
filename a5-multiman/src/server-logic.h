#ifndef __SERVER_LOGIC
#define __SERVER_LOGIC

#include <iostream>
#include <list>
#include <unordered_map>

#include "wall-timer.h"
#include "server-networking.h"

using namespace std;

class GameStage;
class GameObject
{
public:
    unsigned int m_x, m_y, m_id;
    GameStage *m_gameStage;

    GameObject(GameStage* gameStage, unsigned int x, unsigned int y, unsigned int id) : m_gameStage(gameStage), m_x(x), m_y(y), m_id(id) {}

};

class PathNode
{
public:
    int mapX, mapY;

    PathNode() {}
    PathNode(unsigned int x, unsigned int y) : mapX(x), mapY(y) {}
};

class Troup;
class Path
{
    void Init();
    float AbsoluteDistance(PathNode a, PathNode b);
    PathNode GetHighestPriorityOpenNode();
    bool OpenNodesExists();
    void ExpandNode(PathNode current, PathNode endPosition);
    void RetracePath(PathNode startPosition, PathNode current);

    void FindDirectPath(PathNode &source, PathNode &destination);
    void FindPathAStar(PathNode startPosition, PathNode endPosition);

    unsigned int m_mapX, m_mapY;

    bool **m_open;
    bool **m_closed;
    float **m_priority;
    float **m_cost;
    PathNode **m_parent;

    Troup *m_troup;

public:
    list<PathNode> m_nodes;

    Path(Troup *troup, PathNode &source, PathNode &destination, unsigned int x, unsigned int y);
};

class Building : public GameObject
{
        unsigned int m_unitCount;

        wall_time_timer m_generateUnitsTimer;
        const static unsigned int m_unitGenerationTime = 1000;
public: 
    Building(GameStage *gameStage, unsigned int x, unsigned int y, unsigned int id);

    void Update();
    void IncomingTroup(Troup *troup);
    void KillUnits(unsigned int unitCount);
};

class Troup : public GameObject
{
    unsigned int m_stepTime = 1000;
    wall_time_timer m_stepTimer;
public:
    unsigned int m_unitCount;

    Building *m_source;
    Building *m_destination;

    Path *m_path;

    Troup(GameStage *gameStage, Building *sourceBuilding, Building *destinationBuilding, unsigned int unitCount, unsigned int id);

    bool Update();
};

class GameStage
{
    static unsigned int s_nextBuilding;
    static unsigned int s_nextTroup;
public:
    int m_mapX, m_mapY;
    bool **m_map;

    GameStage() {}

    void init(unsigned int x, unsigned int y);
    void Update();
    Building* spawnHouse(unsigned int x, unsigned int y);
    Troup* spawnTroup(unsigned int sourceBuildingID, unsigned int destinationBuildingID, unsigned int unitCount);

    unordered_map<unsigned int, Building*> m_buildings;
    unordered_map<unsigned int, Troup*> m_troups;
};

#endif
