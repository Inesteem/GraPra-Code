#ifndef GAME_H
#define GAME_H

#include "simple_heightmap.h"
#include "gameobject.h"
#include "wall-timer.h"

class client_message_reader;

class Game
{
public:
    Game(ObjHandler *objhandler, simple_heightmap *sh, client_message_reader *messageReader);
    void add_tree(int x, int y);
    void add_building(string name, int size, int x, int y);
    void add_unit_group(vec2i start, vec2i end, unsigned int count);
    void init(string filename, int widht, int height);
    Building* get_building_at(vec3f pos);
    void draw();
    void update();
    Building* get_last_selected_building();

private:
   ObjHandler *m_objhandler;
   simple_heightmap *m_sh;
   client_message_reader *m_messageReader;
   Building *m_selected;
   vector<Tree> m_trees;
   vector<Building> m_buildings;
   vector<UnitGroup> m_unitgroups;
};

#endif // GAME_H
