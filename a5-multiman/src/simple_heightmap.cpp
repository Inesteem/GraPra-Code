
#include "simple_heightmap.h"
#include "rendering.h"
#include <vector>
simple_heightmap::simple_heightmap()
    {


}


void simple_heightmap::init(ObjHandler *objhandler, const std::string filename, int width, int height){
    m_buildings = vector<Building>();
    m_trees = vector<Tree>();
    m_objhandler = objhandler;
    m_mesh = make_mesh("mesh_heightmap", 1);
    vec3f* colors(load_image3f(filename.c_str(), &m_width, &m_height));
   // cout << m_width << " " << m_height << endl;
    if(width != m_width || height != m_height){
        cerr << filename << " width or height doesnt match received width or height!" << endl;
    }
    m_gamefield = vector<char>(m_width*m_height);
    for(int i = 0; i < m_gamefield.size(); i++){
        m_gamefield[i] = 'n';
    }

    vector<vec3f> pos = vector<vec3f>(m_width*m_height);
    m_heights = vector<float>(m_width*m_height);
    for(int i = 0; i < m_height; ++i){
        for(int j = 0; j < m_width; ++j){


            pos[i + j * m_height] = vec3f(j*render_settings::tile_size_x,0,i*render_settings::tile_size_y);
            pos[i + j *m_height].y = colors[i + j *m_height].x * 10;
            m_heights[i + j *m_height] = colors[i + j *m_height].x * 10;
//            if(colors[i + j *m_height].y > 0.8){
//                m_gamefield[i] = 't';
//                m_gameobjects.push_back(Tree(objhandler->getObjByName("tree"),"tree",i,j));
//            }
//            if(colors[i + j *m_height].x > 0.8) {
//                m_gamefield[i] = 'b';
//                m_gameobjects.push_back(Building(objhandler->getObjByName("building_lot"),"building_lot",i,j,0));

//            }

        }


    }
    m_shader = find_shader("heightmap_shader");
    make_unit_matrix4x4f(&m_model);
    std::vector<unsigned int> index;
    for(int i = 0; i < m_height-1; ++i){
        for(int j = 0; j < m_width; ++j){
            if(i%2 == 0){
            index.push_back(i + j*m_height);
            index.push_back((i+1)  + j*m_height);
            } else {
                index.push_back((i+1) + (m_width -j -1) * m_height);
                index.push_back(i+ (m_width-j-1)*m_height)  ;

            }

        }
    }


    bind_mesh_to_gl(m_mesh);
    add_vertex_buffer_to_mesh(m_mesh, "in_pos", GL_FLOAT, m_width*m_height, 3, (float*) pos.data() , GL_STATIC_DRAW);
    //add_vertex_buffer_to_mesh(m_mesh, "in_tc", GL_FLOAT, m_width*m_height, 3, nullptr , GL_STATIC_DRAW);
   // add_vertex_buffer_to_mesh(m_mesh, "in_normal", GL_FLOAT, m_width*m_height, 3,nullptr, GL_STATIC_DRAW );
    add_index_buffer_to_mesh(m_mesh, index.size(), (unsigned int *) index.data(), GL_STATIC_DRAW);
    unbind_mesh_from_gl(m_mesh);
}

float simple_heightmap::get_height(int x, int y){
    return m_heights[y + x * m_height];
}

void simple_heightmap::add_building(string name, int size, int x, int y){
    m_buildings.push_back(Building(m_objhandler->getObjByName(name),name,x,y, 0,size, get_height(x,y)));
}

void simple_heightmap::add_tree(int x, int y){
    m_trees.push_back(Tree(m_objhandler->getObjByName("tree"),"tree",x,y,get_height(x,y)));
}


void simple_heightmap::draw(){

    bind_shader(m_shader);



    int loc = glGetUniformLocation(gl_shader_object(m_shader), "proj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

    loc = glGetUniformLocation(gl_shader_object(m_shader), "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);

    loc = glGetUniformLocation(gl_shader_object(m_shader), "model");
    glUniformMatrix4fv(loc, 1, GL_FALSE, m_model.col_major);

    bind_texture(grass, 0);
    loc = glGetUniformLocation(gl_shader_object(m_shader), "grass");
    glUniform1i(loc, 0);
    bind_texture(stone, 1);
    loc = glGetUniformLocation(gl_shader_object(m_shader), "stone");
    glUniform1i(loc, 1);
    bind_texture(water, 2);
    loc = glGetUniformLocation(gl_shader_object(m_shader), "water");
    glUniform1i(loc, 2);
    bind_texture(snow, 3);
    loc = glGetUniformLocation(gl_shader_object(m_shader), "snow");
    glUniform1i(loc, 3);


    bind_mesh_to_gl(m_mesh);

    draw_mesh_as(m_mesh,GL_TRIANGLE_STRIP);

    unbind_mesh_from_gl(m_mesh);
    unbind_shader(m_shader);

    unbind_texture(grass);
    unbind_texture(stone);
    unbind_texture(water);
    unbind_texture(snow);

    for(int i = 0; i < m_buildings.size(); ++i){
        m_buildings[i].draw();
    }



}




