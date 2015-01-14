#include "server-logic.h"
#include "messages.h"

void GameStage::init(int x, int y)
{
	mapX = x;
	mapY = y;
}

void GameStage::spawnHouse(int x, int y)
{
	Building b(x,y);
	buildings.push_back(b);
}

void GameStage::spawnTree(int x, int y)
{
    //Tree t(x,y);
    //trees.push_back(t);
}
