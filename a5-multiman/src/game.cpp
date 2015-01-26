
#include "game.h"
#include "messages.h"
#include "mouseactions.h"
#include "clientside-networking.h"
#include "rendering.h"

int PLAYER_ID;

Game::Game(ObjHandler *objhandler, simple_heightmap *sh, client_message_reader *message_reader, Menu *menu): m_objhandler(objhandler), m_sh(sh), m_messageReader(message_reader),menu(menu)
{
	player_color = vec3f(0,0,1);
}

void Game::set_action(moac::Action *action){	
	this->action = action;
}


void Game::add_building(string name, int size, int x, int y, unsigned int id){
    m_buildings.push_back(Building(m_objhandler->getObjByName(name), m_objhandler->getObjByName("selection_circle"),m_objhandler->getObjByName("upgrade_arrow"), name,x,y, -1 ,size, m_sh->get_height(x,y), id));
		
	planes.push_back(vec3f(x,-1,y));		
		
	

}

void Game::change_building_owner(int building_id, int new_owner, FRACTIONS frac){
    for(int i = 0; i < m_buildings.size(); i++){
        if(m_buildings[i].get_id() == building_id){
            if(m_buildings[i].get_owner_id() == -1 && new_owner != -1) {
                this->upgrade_building(i, msg::building_state::house_lvl1,frac);
            }
            m_buildings[i].change_owner(new_owner);

            return;
        }
    }
}

void Game::update_building_unit_count(unsigned int id, unsigned int unit_count){
    for(int i = 0; i < m_buildings.size(); i++){
		if(m_buildings[i].get_id() == id){					

			m_buildings[i].update_unit_count(unit_count);

			if(m_selected != nullptr && m_buildings[i].get_id() == m_selected->get_id())
				action->update_iconbar();	

			return;
		}
	}
}

void Game::troup_arrived(unsigned int troupId){
    auto it = m_unitgroups.begin();
    while (it != m_unitgroups.end()){

        if(troupId == it->m_id){
            it = m_unitgroups.erase(it);

        }  else {
            ++it;
        }


    }

}

void Game::add_tree(int x, int y, int type){
	switch(type){
		case 1 : m_trees.push_back(Tree(m_objhandler->getObjByName("tropical_tree"),"tropical_tree",x,y,m_sh->get_height(x,y))); break;
		default : m_trees.push_back(Tree(m_objhandler->getObjByName("tree"),"tree",x,y,m_sh->get_height(x,y)));
	}
}

vector<vec3f> *Game::get_planes(){
	
	for(int i = 0; i < m_buildings.size(); i++){
		planes[i].y += m_buildings[i].get_center().y;		
	}
//    m_sh->re_init(&planes);
	return &planes;
}

void Game::init(string filename, int widht, int height, int id){
    m_player_id = id;
    PLAYER_ID = id;
    vec3f color = menu->get_player_color();
    if(color.x != -1){
		set_player_color(id,color);
	}
    
    m_sh->init(filename, widht, height);
}

ObjHandler* Game::get_objhandler(){
	return m_objhandler;
}

void Game::update_unit_group(unsigned int x, unsigned int y, unsigned int troupId, unsigned int time){	

    for(int i = 0; i < m_unitgroups.size(); ++i){
		if(troupId == m_unitgroups[i].m_id){
            m_unitgroups[i].move_to(vec2f(x, y), time);
			return;
		}
	}	
}
void Game::add_unit_group(unsigned int sourceId, unsigned int destinationId, unsigned int count, unsigned int troupId, FRACTIONS frac, int owner){
	Building *source = getBuilding(sourceId);
	Building *destination = getBuilding(destinationId);
    vec2f start = source->get_pos();
    vec2f end = destination->get_pos();

    cout << "spawning enemies at: " << start.x << "," << start.y << " count: " << count << endl;
    if(frac == PAC ){
        m_unitgroups.push_back(UnitGroup(m_objhandler->getObjByName("pacman"),m_sh,"pacman",start,end,owner,count, 10000, m_sh->get_height(start.x, start.y), troupId, 0.5f,true));
    } else if(frac == BOMB) {
        m_unitgroups.push_back(UnitGroup(m_objhandler->getObjByName("bomberman"),m_sh,"bomberman",start,end,owner,count,10000,m_sh->get_height(start.x, start.y), troupId, 0.5f, false));
    }
}

void Game::upgrade_building(unsigned int buildingId, unsigned int state, FRACTIONS frac){
    for(int i = 0; i < m_buildings.size(); i++){
		if(m_buildings[i].get_id() == buildingId){
			//TODO: const names
			switch(state){
                case msg::building_state::house_lvl1 : m_buildings[i].upgrade(m_objhandler->getObjByName("house_pacman"),state); break;
                case msg::building_state::turret_lvl1 : m_buildings[i].upgrade(m_objhandler->getObjByName("turret_pacman"),state); break;
				default : m_buildings[i].upgrade(m_objhandler->getObjByName("building_lot"), state);
			}
			return;
		}
	}	
	
}


Building* Game::get_building_at(vec3f pos){
    const float selectionRadius = 20.0;

    int k = 0;
    float dist = m_buildings[0].dist_to(pos);
    for(int i = 0; i < m_buildings.size(); ++i){
 //       cout << "Distance to nearest Building: " << m_buildings[i].dist_to(pos) << endl;
        if(dist > m_buildings[i].dist_to(pos)){
            dist = m_buildings[i].dist_to(pos);
            k = i;
        }
    }

    if( dist < selectionRadius) {
 //       cout << "Selected building: " << m_selected->get_pos().x << "," << m_selected->get_pos().y << endl;
        return &m_buildings[k];
    }
    return nullptr;
}


Building* Game::get_last_selected_building(){
    return m_selected;
}


void Game::set_selected(Building *building){
	m_selected = building;
	
}


void Game::draw(){
	m_sh->draw();


    for(int i = 0; i < m_trees.size(); ++i){
        m_trees[i].draw();
    }

    for(int i = 0; i < m_unitgroups.size(); ++i){
        m_unitgroups[i].draw();
    }
    for(int i = 0; i < m_buildings.size(); ++i){
        m_buildings[i].draw();
    }


    if (m_selected != 0 && m_selected != nullptr){
       m_selected->draw_selection_circle(m_selected->m_size);
    }

}

void Game::update(){
    for(int i = 0; i < m_unitgroups.size(); ++i){
        m_unitgroups[i].update();
   }
}

Building* Game::getBuilding(unsigned int id)
{
	for(int i = 0; i < m_buildings.size(); i++){
		if(id == m_buildings[i].get_id()) {
			return &m_buildings[i];
		}
		
	}
	
	return nullptr;
}



void Game::game_over(int winner_id){
	
	if(winner_id == PLAYER_ID)
		menu->set_mode(menu->GAMEWON);
	else
		menu->set_mode(menu->GAMELOOSE);
	menu->set_render_menu(true);
	
}

