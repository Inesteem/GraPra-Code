
#include "game.h"
#include "messages.h"
#include "mouseactions.h"
#include "clientside-networking.h"
#include "rendering.h"

int PLAYER_ID;
int FRACTION;
int COLORID;
static int first_base = 0;
wall_time_timer rot_timer;

Game::Game(ObjHandler *objhandler, simple_heightmap *sh, client_message_reader *message_reader, Menu *menu): m_objhandler(objhandler), m_sh(sh), m_messageReader(message_reader),menu(menu)
{
	rot_timer.restart();
	player_color = vec3f(0,0,1);
}

void Game::set_action(moac::Action *action){	
	this->action = action;
}


void Game::add_building(string name, int size, int x, int y, unsigned int id){

    m_buildings.push_back(Building(this,m_objhandler->getObjByName(name), m_objhandler->getObjByName("selection_circle"),m_objhandler->getObjByName("upgrade_arrow"), name,x,y, -1 ,size, m_sh->get_height(x,y), id));
    Building b = m_buildings.back();
    vec2f pos = b.get_pos();
    pos = vec2f(pos.x  , pos.y );
    m_sh->set_heights(pos,m_sh->get_height(x,y),b.m_size);

 

	planes.push_back(vec3f(x,-1,y));		
		
	

}
void Game::change_building_owner(int building_id, int new_owner, FRACTIONS frac){
    for(int i = 0; i < m_buildings.size(); i++){
        if(m_buildings[i].get_id() == building_id){
            if(m_buildings[i].get_owner_id() == -1 && new_owner != -1) {
                this->upgrade_building(i, msg::building_state::house_lvl1,frac);

				//focus cam to base at the begin of the game
				if(first_base == 0 && new_owner == PLAYER_ID){
					first_base = 1;
					vec3f cam_pos, cam_dir, cam_up;
					matrix4x4f m = m_buildings[i].get_model_matrix();
					vec3f b_pos = vec3f(m.row_col(0,3),m.row_col(1,3),m.row_col(2,3));
					
				    matrix4x4f *lookat_matrix = lookat_matrix_of_cam(current_camera());
					extract_dir_vec3f_of_matrix(&cam_dir, lookat_matrix);
					extract_up_vec3f_of_matrix(&cam_up, lookat_matrix);
					
					cam_pos = b_pos - (cam_dir * 20);
					change_lookat_of_cam(current_camera(), &cam_pos, &cam_dir, &cam_up);
					recompute_gl_matrices_of_cam(current_camera());
				
				}
				
            }
			
			if(m_selected != nullptr && building_id == m_selected->get_id()){
				m_selected == nullptr;
				action->lost_building(building_id);
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
        case 1 : m_trees.push_back(Tree(this, m_objhandler->getObjByName("tropical_tree"),"tropical_tree",x,y,m_sh->get_height(x,y))); break;
        default : m_trees.push_back(Tree(this, m_objhandler->getObjByName("tree"),"tree",x,y,m_sh->get_height(x,y)));
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
	first_base = 0;
    m_player_id = id;
    PLAYER_ID = id;
	
	msg::client_settings cs = make_message<msg::client_settings>();
	cout << id << ": sent message : " << FRACTION << endl;
	cs.playerId = id;
	cs.frac = FRACTION;
	cs.colorId = COLORID;
	m_messageReader->send_message(cs);  
    
    m_sh->init(filename, widht, height);

    m_snow = new SnowEffect(vec3f(widht * render_settings::tile_size_x / 2.0 ,10 , height * render_settings::tile_size_y / 2.0), 100);
}

vec3f Game::get_player_color(int playerId){
    if(playerId == -1) {
        return vec3f(0.2,0.2,0.2);
    }

    try {
        return player_colors.at(m_color_id_for_player[playerId]);
    } catch (...){

    }

    return vec3f(0.2,0.2,0.2);
}

ObjHandler* Game::get_objhandler(){
	return m_objhandler;
}

vector<Building>* Game::get_buildings(){

            return &m_buildings;

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

    cout << "spawning enemies at: " << start.x << "," << start.y << " count: " << count << " frac : " << frac  << endl;
    if(frac == PAC ){
        m_unitgroups.push_back(UnitGroup(this, m_objhandler->getObjByName("pacman"),m_sh,"pacman",start,end,owner,count, 10000, m_sh->get_height(start.x, start.y), troupId, 0.5f,true));
    } else if(frac == BOMB) {
        m_unitgroups.push_back(UnitGroup(this, m_objhandler->getObjByName("bomberman"),m_sh,"bomberman",start,end,owner,count,10000,m_sh->get_height(start.x, start.y), troupId, 0.5f, false));
    }
}

void Game::add_stuff(unsigned int x, unsigned int y){
	//todo: more stuff
	//stuff.push_back(RandomStuff(m_objhandler->getObjByName("tonkrug"),"tonkrug",x,y,m_sh->get_height(x,y)));
    stuff.push_back(RandomStuff(this, m_objhandler->getObjByName("plants"),"plants",x,y,m_sh->get_height(x,y), 1));
}


void Game::upgrade_building(unsigned int buildingId, unsigned int state, FRACTIONS frac){
    for(int i = 0; i < m_buildings.size(); i++){
		if(m_buildings[i].get_id() == buildingId){
			//TODO: const names
			if(frac == PAC){
                switch(state){
                case msg::building_state::house_lvl1 : {m_buildings[i].upgrade(m_objhandler->getObjByName("house_pacman_lvl1"),state);
                    m_buildings[i].change_size(2);
                    Building b = m_buildings[i];
                    vec2f pos = b.get_pos();
                    pos = vec2f( pos.x /** render_settings::tile_size_x - ((render_settings::tile_size_x)*b.m_size)/2.0f */ , pos.y /** render_settings::tile_size_y - ((render_settings::tile_size_y)*b.m_size)/2.0f*/);
                    m_sh->set_heights(pos,m_sh->get_height(pos.x,pos.y),b.m_size);
                    break;}
                case msg::building_state::house_lvl2 : {m_buildings[i].upgrade(m_objhandler->getObjByName("house_pacman_lvl2"),state);
                    m_buildings[i].change_size(3);
                    Building b = m_buildings[i];
                    vec2f pos = b.get_pos();
                    pos = vec2f(pos.x /** render_settings::tile_size_x - ((render_settings::tile_size_x)*b.m_size)/2.0f */, pos.y /** render_settings::tile_size_y - ((render_settings::tile_size_y)*b.m_size)/2.0f*/);
                    m_sh->set_heights(pos,m_sh->get_height(pos.x,pos.y),b.m_size);
                    break;}
                case msg::building_state::house_lvl3 :{ m_buildings[i].upgrade(m_objhandler->getObjByName("house_pacman_lvl3"),state);
                    m_buildings[i].change_size(4);
                    Building b = m_buildings[i];
                    vec2f pos = b.get_pos();
                    pos = vec2f(pos.x /** render_settings::tile_size_x - ((render_settings::tile_size_x)*b.m_size)/2.0f */ , pos.y /** render_settings::tile_size_y - ((render_settings::tile_size_y)*b.m_size)/2.0f*/);
            //        m_sh->set_heights(pos,m_sh->get_height(pos.x,pos.y),b.m_size); 
					break;}
                case msg::building_state::turret_lvl1 :{
                    m_buildings[i].upgrade(m_objhandler->getObjByName("turret_pacman_lvl1"),state);
                    m_buildings[i].change_size(1);
                    Building b = m_buildings[i];
                    vec2f pos = b.get_pos();
                    pos = vec2f(pos.x /** render_settings::tile_size_x - ((render_settings::tile_size_x)*b.m_size)/2.0f */, pos.y /** render_settings::tile_size_y - ((render_settings::tile_size_y)*b.m_size)/2.0f*/);
                    m_sh->set_heights(pos,m_sh->get_height(pos.x,pos.y),b.m_size);
                    break;}
                case msg::building_state::turret_lvl2 :{
                    m_buildings[i].upgrade(m_objhandler->getObjByName("turret_pacman_lvl1"),state);
                    m_buildings[i].change_size(2);
                    Building b = m_buildings[i];
                    vec2f pos = b.get_pos();
                    pos = vec2f(pos.x /** render_settings::tile_size_x - ((render_settings::tile_size_x)*b.m_size)/2.0f */, pos.y /** render_settings::tile_size_y - ((render_settings::tile_size_y)*b.m_size)/2.0f*/);
                    break;}
                default :{ m_buildings[i].upgrade(m_objhandler->getObjByName("building_lot"), state);
                    m_buildings[i].change_size(1);
                    Building b = m_buildings[i];
                    vec2f pos = b.get_pos();
                    pos = vec2f(pos.x /** render_settings::tile_size_x - ((render_settings::tile_size_x)*b.m_size)/2.0f */ , pos.y /** render_settings::tile_size_y - ((render_settings::tile_size_y)*b.m_size)/2.0f*/);
                    m_sh->set_heights(pos,m_sh->get_height(pos.x,pos.y),b.m_size);}
                }
            } else {
				switch(state){
                    case msg::building_state::house_lvl1 : {m_buildings[i].upgrade(m_objhandler->getObjByName("house_bbm_lvl1"),state);
                    m_buildings[i].change_size(2);
                    Building b = m_buildings[i];
                    vec2f pos = b.get_pos();
                    pos = vec2f(pos.x /** render_settings::tile_size_x - ((render_settings::tile_size_x)*b.m_size)/2.0f */ , pos.y /** render_settings::tile_size_y - ((render_settings::tile_size_y)*b.m_size)/2.0f*/);
                    m_sh->set_heights(pos,m_sh->get_height(pos.x,pos.y),b.m_size);
                    break;}
                case msg::building_state::house_lvl2 :{ m_buildings[i].upgrade(m_objhandler->getObjByName("house_bbm_lvl2"),state);
                    m_buildings[i].change_size(3);
                    Building b = m_buildings[i];
                    vec2f pos = b.get_pos();
                    pos = vec2f(pos.x /** render_settings::tile_size_x - ((render_settings::tile_size_x)*b.m_size)/2.0f */ , pos.y /** render_settings::tile_size_y - ((render_settings::tile_size_y)*b.m_size)/2.0f*/);
                    m_sh->set_heights(pos,m_sh->get_height(pos.x,pos.y),b.m_size);
                    break;}
                case msg::building_state::house_lvl3 :{ m_buildings[i].upgrade(m_objhandler->getObjByName("house_bbm_lvl3"),state);
                    m_buildings[i].change_size(4);
                    Building b = m_buildings[i];
                    vec2f pos = b.get_pos();
                    pos = vec2f(pos.x /** render_settings::tile_size_x - ((render_settings::tile_size_x)*b.m_size)/2.0f */ , pos.y /** render_settings::tile_size_y - ((render_settings::tile_size_y)*b.m_size)/2.0f*/);
           //         m_sh->set_heights(pos,m_sh->get_height(pos.x,pos.y),b.m_size);
                    break;}
                case msg::building_state::turret_lvl1 :{m_buildings[i].upgrade(m_objhandler->getObjByName("turret_bbm_lvl1"),state);
                    m_buildings[i].change_size(1);
                    Building b = m_buildings[i];
                    vec2f pos = b.get_pos();
                    pos = vec2f(pos.x /** render_settings::tile_size_x - ((render_settings::tile_size_x)*b.m_size)/2.0f */ , pos.y /** render_settings::tile_size_y - ((render_settings::tile_size_y)*b.m_size)/2.0f*/);
                    m_sh->set_heights(pos,m_sh->get_height(pos.x,pos.y),b.m_size);
                    break;}
                case msg::building_state::turret_lvl2 :{m_buildings[i].upgrade(m_objhandler->getObjByName("turret_bbm_lvl1"),state);
                    m_buildings[i].change_size(2);
                    Building b = m_buildings[i];
                    vec2f pos = b.get_pos();
                    pos = vec2f(pos.x /** render_settings::tile_size_x - ((render_settings::tile_size_x)*b.m_size)/2.0f */ , pos.y /** render_settings::tile_size_y - ((render_settings::tile_size_y)*b.m_size)/2.0f*/);
                    break;}
                default :{ m_buildings[i].upgrade(m_objhandler->getObjByName("building_lot"), state);
                    Building b = m_buildings[i];
                    m_buildings[i].change_size(1);
                    vec2f pos = b.get_pos();
                    pos = vec2f(pos.x /** render_settings::tile_size_x - ((render_settings::tile_size_x)*b.m_size)/2.0f */ , pos.y /** render_settings::tile_size_y - ((render_settings::tile_size_y)*b.m_size)/2.0f*/);
                    m_sh->set_heights(pos,m_sh->get_height(pos.x,pos.y),b.m_size);}
				} 					
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

void Game::deliver_settings(unsigned int frac, unsigned int colorId){
	FRACTION = frac;
	COLORID = colorId;
}	


void Game::draw(){
	m_sh->draw();

    m_snow->Render();

    for(int i = 0; i < m_trees.size(); ++i){
        m_trees[i].draw();
    }

    for(int i = 0; i < m_unitgroups.size(); ++i){
        m_unitgroups[i].draw();
    }
    for(int i = 0; i < m_buildings.size(); ++i){
        m_buildings[i].draw();
    }
    
    for(int i = 0; i < stuff.size(); ++i){
        stuff[i].draw();
    }


    if (m_selected != 0 && m_selected != nullptr){
       m_selected->draw_selection_circle(m_selected->m_size);
    }

}

void Game::update(){
    m_snow->Update();
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
	
}

