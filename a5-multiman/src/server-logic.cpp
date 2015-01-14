#include "server-logic.h"
#include "messages.h"

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

    for(auto& troup : m_troups) {
        troup.second->Update();
    }
}

void GameStage::spawnHouse(unsigned int x, unsigned int y)
{
    Building *b = new Building(this, x,y, s_nextBuilding);
    m_buildings[s_nextBuilding++] =  b;
}

void GameStage::spawnTroup(unsigned int sourceBuildingID, unsigned int destinationBuildingID, unsigned int unitCount)
{
    Building *a = m_buildings[sourceBuildingID];
    a->KillUnits(unitCount);

    Building *b = m_buildings[destinationBuildingID];
    Troup *t = new Troup(this, a, b, unitCount, s_nextTroup);
    m_troups[s_nextTroup] = t;
}


Troup::Troup(GameStage *gameStage, Building *sourceBuilding, Building *destinationBuilding, unsigned int unitCount, unsigned int id)
    : GameObject(gameStage, 0, 0, id), m_unitCount(unitCount), m_source(sourceBuilding), m_destination(destinationBuilding)
{
    m_x = sourceBuilding->m_x;
    m_y = sourceBuilding->m_y;
}

void Troup::Update()
{

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

void Building::IncomingTroup(Troup troup)
{

}
