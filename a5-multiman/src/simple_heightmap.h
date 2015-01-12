#ifndef SIMPLE_HEIGHTMAP_H
#define SIMPLE_HEIGHTMAP_H


#include "drawelement.h"
#include "gameobject.h"

#include <mesh.h>
#include <libmcm/vectors.h>
#include <libmcm/matrix.h>
#include <libcgl/impex.h>

#include <libcgl/texture.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;


class simple_heightmap
{
public:
    simple_heightmap();
    float get_height(int x, int y);
    void init(ObjHandler *objhandler, const std::string filename, int width, int height);
    void update();
    void draw();
    void add_tree(int x, int y);
    void add_building(string name, int size, int x, int y);
   // void add_units(int count, int x, int y);



private:
    ObjHandler *m_objhandler;
    unsigned int m_width, m_height;

    vector<float> m_heights;

    matrix4x4f m_model;
    mesh_ref m_mesh;
    shader_ref m_shader;
    vector<char> m_gamefield;
    vector<Tree> m_trees;
    vector<Building> m_buildings;
    //vector<GameObject> m_other_stuff;
    texture_ref grass = find_texture("terrain_1");
    texture_ref stone = find_texture("terrain_2");
    texture_ref water = find_texture("terrain_0");
    texture_ref snow = find_texture("terrain_3");
};



#endif // SIMPLE_HEIGHTMAP_H
