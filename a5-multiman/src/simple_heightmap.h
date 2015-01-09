#ifndef SIMPLE_HEIGHTMAP_H
#define SIMPLE_HEIGHTMAP_H

#include <mesh.h>
#include "drawelement.h"
#include <string>
#include <libmcm/vectors.h>
#include <libmcm/matrix.h>
#include <libcgl/impex.h>
#include <iostream>
#include <vector>
#include "gameobject.h"

using namespace std;


class simple_heightmap
{
public:
    simple_heightmap(ObjHandler *objhandler,const std::string filename, float scale);
    void toggle_next_scaling();
    void update();
    void draw();



private:
    ObjHandler *m_objhandler;
    unsigned int m_width, m_height;
    float m_scale_factor;
    matrix4x4f m_model;
    mesh_ref m_mesh;
    shader_ref m_shader;
    vector<char> m_gamefield;
    vector<GameObject> m_gameobjects;

};



#endif // SIMPLE_HEIGHTMAP_H
