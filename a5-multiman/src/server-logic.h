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

class Building : public GameObject
{
public: 
	Building(int x, int y) : GameObject(x, y) {}
};

class Tree : public GameObject
{
public:
	Tree(int x, int y) : GameObject(x, y) {}

};

class GameStage
{
public:
	int mapX, mapY;

	void init(int x, int y);
	void spawnHouse(int x, int y);
	void spawnTree(int x, int y);

	vector<Building> buildings;
	vector<Tree> trees;
};

#endif
