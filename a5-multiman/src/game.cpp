#include "game.h"
#include "messages.h"
#include "clientside-networking.h"

Game::Game(ObjHandler *objhandler, simple_heightmap *sh, client_message_reader *message_reader): m_objhandler(objhandler), m_sh(sh), m_messageReader(message_reader)
{
}

void Game::add_building(string name, int size, int x, int y, unsigned int id){
    m_buildings.push_back(Building(m_objhandler->getObjByName(name), m_objhandler->get_selection_circle(),name,x,y, 0,size, m_sh->get_height(x,y), id));
}

void Game::add_tree(int x, int y){
    m_trees.push_back(Tree(m_objhandler->getObjByName("tree"),"tree",x,y,m_sh->get_height(x,y)));
}

void Game::init(string filename, int widht, int height){
    m_sh->init(filename, widht, height);
}

void Game::add_unit_group(vec2i start, vec2i end, unsigned int count, unsigned int id){
    cout << "spawning enemies at: " << start.x << "," << start.y << " count: " << count << endl;
    m_unitgroups.push_back(UnitGroup(m_objhandler->getObjByName("tree"),m_sh,"bomb",start,end,0,count, 3000, m_sh->get_height(start.x, start.y)));

    msg::spawn_troup_client stc = make_message<msg::spawn_troup_client>();
    stc.playerId = 0;

    // TDOO use actual building ids, now sending from house 0 to 6
    stc.sourceId = id;
    stc.destinationId = 6;
    m_messageReader->send_message(stc);
}

Building* Game::get_building_at(vec3f pos){
    int k = 0;
    float dist = m_buildings[0].dist_to(pos);
    for(int i = 0; i < m_buildings.size(); ++i){
 //       cout << "Distance to nearest Building: " << m_buildings[i].dist_to(pos) << endl;
        if(dist > m_buildings[i].dist_to(pos)){
            dist = m_buildings[i].dist_to(pos);
            k = i;
        }
    }

    if( dist < 3.0f) {
        m_selected = &m_buildings[k];
 //       cout << "Selected building: " << m_selected->get_pos().x << "," << m_selected->get_pos().y << endl;
        return &m_buildings[k];
    }
    m_selected = nullptr;
    return nullptr;
}

Building* Game::get_last_selected_building(){
    return m_selected;
}

void Game::draw(){
    m_sh->draw();

    for(int i = 0; i < m_buildings.size(); ++i){
        m_buildings[i].draw();
    }

    for(int i = 0; i < m_trees.size(); ++i){
        m_trees[i].draw();
    }

    for(int i = 0; i < m_unitgroups.size(); ++i){
        m_unitgroups[i].draw();
    }
    if (m_selected != nullptr){
        m_selected->draw_selection_circle();
    }

}

void Game::update(){
    for(int i = 0; i < m_unitgroups.size(); ++i){
        m_unitgroups[i].update();
   }
}
