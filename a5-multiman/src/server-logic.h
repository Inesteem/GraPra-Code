#ifndef __SERVER_LOGIC
#define __SERVER_LOGIC

#include <iostream>
#include <vector>
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

class Troup;
class Building : public GameObject
{
        unsigned int m_unitCount;

        wall_time_timer m_generateUnitsTimer;
        const static unsigned int m_unitGenerationTime = 3000;
public: 
    Building(GameStage *gameStage, unsigned int x, unsigned int y, unsigned int id);

    void Update();
    void IncomingTroup(Troup troup);
    void KillUnits(unsigned int unitCount) { m_unitCount -= unitCount; }
};

class Troup : public GameObject
{
    Building *m_source;
    Building *m_destination;
    unsigned int m_unitCount;
public:
    Troup(GameStage *gameStage, Building *sourceBuilding, Building *destinationBuilding, unsigned int unitCount, unsigned int id);

    bool Update();
};

class GameStage
{
    static unsigned int s_nextBuilding;
    static unsigned int s_nextTroup;
public:
    int m_mapX, m_mapY;

    GameStage() {}

    void init(unsigned int x, unsigned int y);
    void Update();
    Building* spawnHouse(unsigned int x, unsigned int y);
    Troup* spawnTroup(unsigned int sourceBuildingID, unsigned int destinationBuildingID, unsigned int unitCount);

    unordered_map<unsigned int, Building*> m_buildings;
    unordered_map<unsigned int, Troup*> m_troups;
};

#endif
