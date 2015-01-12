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
    simple_heightmap(ObjHandler *objhandler,const std::string filename, float scale);
    void toggle_next_scaling();
    void update();
    void draw();
    void add_tree(int x, int y);
    void add_building(int level, int x, int y);



private:
    ObjHandler *m_objhandler;
    unsigned int m_width, m_height;
    float m_scale_factor;
    matrix4x4f m_model;
    mesh_ref m_mesh;
    shader_ref m_shader;
    vector<char> m_gamefield;
    vector<GameObject> m_gameobjects;
    texture_ref grass = find_texture("terrain_1");
    texture_ref stone = find_texture("terrain_2");
};



#endif // SIMPLE_HEIGHTMAP_H
