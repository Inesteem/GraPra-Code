#include "game.h"
#include "messages.h"
#include "clientside-networking.h"

Game::Game(ObjHandler *objhandler, simple_heightmap *sh, client_message_reader *message_reader): m_objhandler(objhandler), m_sh(sh), m_messageReader(message_reader)
{
}

void Game::add_building(string name, int size, int x, int y){
    m_buildings.push_back(Building(m_objhandler->getObjByName(name),name,x,y, 0,size, m_sh->get_height(x,y)));
}

void Game::add_tree(int x, int y){
    m_trees.push_back(Tree(m_objhandler->getObjByName("tree"),"tree",x,y,m_sh->get_height(x,y)));
}

void Game::init(string filename, int widht, int height){
    m_sh->init(filename, widht, height);
}

void Game::add_unit_group(vec2i start, vec2i end, unsigned int count){
    cout << "spawning enemies at: " << start.x << "," << start.y << " count: " << count << endl;
    m_unitgroups.push_back(UnitGroup(m_objhandler->getObjByName("tree"),m_sh,"bomb",start,end,0,count, 3000, m_sh->get_height(start.x, start.y)));

    msg::spawn_troup_client stc = make_message<msg::spawn_troup_client>();
    m_messageReader->send_message(stc);
}

Building* Game::get_building_at(int x, int y){
    for(int i = 0; i < m_buildings.size(); ++i){
        if(m_buildings[i].get_pos().x == x && m_buildings[i].get_pos().y == y){
            return &m_buildings[i];
        }
    }
    return nullptr;
}

void Game::draw(){
    m_sh->draw();

    for(int i = 0; i < m_buildings.size(); ++i){
        m_buildings[i].draw();
    }
    for(int i = 0; i < m_buildings.size(); ++i){
        m_buildings[i].draw_label();
    }


    for(int i = 0; i < m_trees.size(); ++i){
        m_trees[i].draw();
    }

    for(int i = 0; i < m_unitgroups.size(); ++i){
        m_unitgroups[i].draw();
    }

}

void Game::update(){
    for(int i = 0; i < m_unitgroups.size(); ++i){
        m_unitgroups[i].update();
   }
}
