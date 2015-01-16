#ifndef SIMPLE_HEIGHTMAP_H
#define SIMPLE_HEIGHTMAP_H


#include "drawelement.h"


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
    vec3f sample_normal(int x, int y);
    float get_height(float x, float y);
    void init(const std::string filename, int width, int height);
    void update();
    void draw();

   // void add_units(int count, int x, int y);



private:

    unsigned int m_width, m_height;

    vector<float> m_heights;

    matrix4x4f m_model;
    mesh_ref m_mesh;
    shader_ref m_shader;
    vector<char> m_gamefield;

    //vector<GameObject> m_other_stuff;
    texture_ref grass = find_texture("terrain_1");
    texture_ref stone = find_texture("terrain_2");
    texture_ref water = find_texture("terrain_0");
    texture_ref snow = find_texture("terrain_3");
};



#endif // SIMPLE_HEIGHTMAP_H
