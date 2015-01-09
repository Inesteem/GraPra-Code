
#include "simple_heightmap.h"
#include <vector>
simple_heightmap::simple_heightmap(ObjHandler *objhandler, const std::string filename, float scale): m_objhandler(objhandler), m_scale_factor(scale)
{
    m_mesh = make_mesh("mesh_heightmap", 1);
    vec3f* colors(load_image3f(filename.c_str(), &m_width, &m_height));
    cout << m_width << " " << m_height << endl;

    m_gamefield = vector<char>(m_width*m_height);
    for(int i = 0; i < m_gamefield.size(); i++){
        m_gamefield[i] = 'n';
    }

    vector<vec3f> pos = vector<vec3f>(m_width*m_height);
    for(int i = 0; i < m_height; ++i){
        for(int j = 0; j < m_width; ++j){

          //  pos[i + j *m_height].y = colors[i + j *m_height].z;
            pos[i + j * m_height] = vec3f(j,0,i);

            if(colors[i + j *m_height].y > 0.8){
                m_gamefield[i] = 't';
                m_gameobjects.push_back(Tree(*objhandler,"wuerfel",i,j));
            }
            if(colors[i + j *m_height].x > 0.8) {
                m_gamefield[i] = 'b';
                m_gameobjects.push_back(Building(*objhandler,"wuerfel",i,j,0));
            }

        }


    }
    m_shader = find_shader("heightmap_shader");
    make_unit_matrix4x4f(&m_model);
    m_model.col_major[0 + 4 * 0] *= m_scale_factor;
    m_model.col_major[1 + 4 * 1] *= m_scale_factor;
    m_model.col_major[2 + 4 * 2] *= m_scale_factor;
    std::vector<unsigned int> index;
    for(int i = 0; i < m_height-1; ++i){
        for(int j = 0; j < m_width; ++j){
            if(i%2 == 0){
            index.push_back(i + j*m_height);
            index.push_back((i+1)  + j*m_height);
            } else {
                index.push_back(i+ (m_width-j-1)*m_height)  ;
                index.push_back((i+1) + (m_width -j -1) * m_height);
            }

        }
    }


//    for(int i = 0; i < m_height-1; ++i){
//        for(int j = 0; j < m_width-1; ++j){
//            index.push_back(i   + j * m_height);
//            index.push_back(i+1 + j * m_height);
//            index.push_back(i+1 + (j+1) * m_height);
//            index.push_back(i+1 + (j+1) * m_height);
//            index.push_back(i+1 + (j) * m_height);
//            index.push_back(i + (j) * m_height);
//        }
//    }
    bind_mesh_to_gl(m_mesh);
    add_vertex_buffer_to_mesh(m_mesh, "in_pos", GL_FLOAT, m_width*m_height, 3, (float*) pos.data() , GL_STATIC_DRAW);
    //add_vertex_buffer_to_mesh(m_mesh, "in_tc", GL_FLOAT, m_width*m_height, 3, nullptr , GL_STATIC_DRAW);
   // add_vertex_buffer_to_mesh(m_mesh, "in_normal", GL_FLOAT, m_width*m_height, 3,nullptr, GL_STATIC_DRAW );
    add_index_buffer_to_mesh(m_mesh, index.size(), (unsigned int *) index.data(), GL_STATIC_DRAW);
    unbind_mesh_from_gl(m_mesh);

}


void simple_heightmap::draw(){

    bind_shader(m_shader);



    int loc = glGetUniformLocation(gl_shader_object(m_shader), "proj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

    loc = glGetUniformLocation(gl_shader_object(m_shader), "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);

    loc = glGetUniformLocation(gl_shader_object(m_shader), "model");
    glUniformMatrix4fv(loc, 1, GL_FALSE, m_model.col_major);

    bind_mesh_to_gl(m_mesh);

    draw_mesh_as(m_mesh,GL_TRIANGLE_STRIP);

    unbind_mesh_from_gl(m_mesh);
    unbind_shader(m_shader);
    for(int i = 0; i < m_gameobjects.size(); ++i){
        m_gameobjects[i].multiply_model_matrix(m_model);
        m_gameobjects[i].draw();
    }

}

void simple_heightmap::toggle_next_scaling(){
    if(m_scale_factor == 0.2f) {
        m_scale_factor = 0.1f;
    } else if(m_scale_factor == 0.1f){
        m_scale_factor = 1;
    } else if(m_scale_factor == 1){
        m_scale_factor = 0.5f;
    } else if(m_scale_factor == 0.5f){
        m_scale_factor = 0.2f;
    }
    make_unit_matrix4x4f(&m_model);
    m_model.col_major[0 + 4 * 0] *= m_scale_factor;
    m_model.col_major[1 + 4 * 1] *= m_scale_factor;
    m_model.col_major[2 + 4 * 2] *= m_scale_factor;
}


