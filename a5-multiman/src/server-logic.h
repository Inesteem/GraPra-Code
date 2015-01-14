#ifndef __SERVER_LOGIC
#define __SERVER_LOGIC

#include <iostream>
#include <vector>

using namespace std;

class GameObject
{
public:
	int x, y;

	GameObject(int x, int y) : x(x), y(y) {}

};

class Troup;
class Building : public GameObject
{
        unsigned int id;
public: 
	Building(int x, int y) : GameObject(x, y) {}

    void Update();
    void IncomingTroup(Troup troup);
};

class Troup : public GameObject
{
public:
    Troup(Building *sourceBuilding, Building *destinationBuilding, unsigned int unitCount);

    void Update();
};

class GameStage
{
public:
	int mapX, mapY;

	void init(int x, int y);
	void spawnHouse(int x, int y);
	void spawnTree(int x, int y);
    void spawnTroup(unsigned int sourceBuildingID, unsigned int destinationBuildingID, unsigned int unitCount);

	vector<Building> buildings;
    vector<Troup> troup;
};

#endif
