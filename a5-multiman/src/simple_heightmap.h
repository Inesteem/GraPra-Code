#ifndef SIMPLE_HEIGHTMAP_H
#define SIMPLE_HEIGHTMAP_H


#include "drawelement.h"
#include "rendering.h"

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
//    void re_init( vector<vec3f> *planes);
    void update();
    void draw();
    vec2f get_size(){
        return vec2f(m_g_width*render_settings::tile_size_x,m_g_height*render_settings::tile_size_y);
    }

   // void add_units(int count, int x, int y);


    bool init_done = false;

private:

    unsigned int m_width, m_height;
    unsigned int m_g_width, m_g_height;
    vector<vec3f> m_heights;
    

    matrix4x4f m_model;
    mesh_ref m_mesh;
    mesh_ref m_mesh_2;
    bool render_planes = false;
    shader_ref m_shader;
    vector<char> m_gamefield;

    //vector<GameObject> m_other_stuff;
    texture_ref grass = find_texture("terrain_1");
    texture_ref stone = find_texture("terrain_2");
    texture_ref water = find_texture("terrain_0");
    texture_ref snow = find_texture("terrain_3");
    texture_ref height_map;
	std::string filename;

};



#endif // SIMPLE_HEIGHTMAP_H
