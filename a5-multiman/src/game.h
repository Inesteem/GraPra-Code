#ifndef GAME_H
#define GAME_H

#include "simple_heightmap.h"
#include "gameobject.h"
#include "wall-timer.h"
#include "menu.h"



class client_message_reader;
class moac::Action;

class Game
{
public:
    Game(ObjHandler *objhandler, simple_heightmap *sh, client_message_reader *messageReader, Menu *menu);
    void add_tree(int x, int y, int type);
    void add_building(string name, int size, int x, int y, unsigned int id);
    void add_unit_group(unsigned int sourceId, unsigned int destinationId, unsigned int count, unsigned int troupId);

    void change_building_owner(int building_id, int new_owner);
    void update_unit_group(unsigned int x, unsigned int y, unsigned int troupId, unsigned int time);
    void upgrade_building(unsigned int buildingId, unsigned int state);
    
    void set_selected(Building *building);
    void set_action(moac::Action *action);

    void init(string filename, int widht, int height, int id);
    Building* get_building_at(vec3f pos);

    void draw();
    void update();

	void game_over(int winner_id);


    vector<vec3f> *get_planes();

    Building* get_last_selected_building();
	ObjHandler *get_objhandler();

    void update_building_unit_count(unsigned int id, unsigned int unit_count);
	void troup_arrived(unsigned int troupId);
    Building *getBuilding(unsigned int id);
	
	client_message_reader *m_messageReader;
    int m_player_id;
    vec3f player_color;
    vector<vec3f> planes;
    Menu *menu; 
    moac::Action *action;                      
                                    
private:

   ObjHandler *m_objhandler;
   simple_heightmap *m_sh;
   Building *m_selected = nullptr;
   vector<Tree> m_trees;
   vector<Building> m_buildings;
   vector<UnitGroup> m_unitgroups;
   
   
};

extern int PLAYER_ID;
#endif // GAME_H
