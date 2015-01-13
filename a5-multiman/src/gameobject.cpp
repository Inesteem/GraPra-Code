#include "gameobject.h"
#include "rendering.h"
#include <cmath>
//wrapper for objloader
Obj::Obj(string name, int id, string filename, shader_ref shader):id(id),name(name){
    ObjLoader loader(name.c_str(), filename.c_str());
    loader.TranslateToOrigin();
    loader.pos_and_norm_shader = shader;
    loader.pos_norm_and_tc_shader = shader;
    loader.default_shader = shader;
    vec3f min, max;
    loader.BoundingBox(min, max);
    bb_min = vec3f(0,0,0);
     bb_max = vec3f(render_settings::tile_size_x,(max.y-min.y)/(max.x-min.x)*render_settings::tile_size_x,(max.z-min.z)/(max.x-min.x)*render_settings::tile_size_y);
     loader.ScaleVertexDataToFit(bb_min,bb_max);
     drawelements = new vector<drawelement*>();
    loader.GenerateNonsharingMeshesAndDrawElements(*drawelements);

}
Obj::Obj(string name, int id, string filename, shader_ref shader,vec3f scale):id(id),name(name){
    ObjLoader loader(name.c_str(), filename.c_str());
    loader.TranslateToOrigin();
    loader.pos_and_norm_shader = shader;
    loader.pos_norm_and_tc_shader = shader;
    loader.default_shader = shader;
//    vec3f min, max;
//    loader.BoundingBox(min, max);
//    bb_min = vec3f(0,0,0);
//     bb_max = vec3f(render_settings::tile_size_x,(max.y-min.y)/(max.x-min.x)*render_settings::tile_size_x,(max.z-min.z)/(max.x-min.x)*render_settings::tile_size_y);
//     loader.ScaleVertexDataToFit(bb_min,bb_max);
    loader.ScaleVertexData(scale);
    drawelements = new vector<drawelement*>();
    loader.GenerateNonsharingMeshesAndDrawElements(*drawelements);

}


//handler for all .obj
ObjHandler::ObjHandler(){

}


//adds an .obj
void ObjHandler::addObj(string name, string filename, shader_ref shader){

    objs.push_back(Obj(name,objs.size(),filename, shader));
}

void ObjHandler::addObj_withScale(string name, string filename, shader_ref shader, vec3f scale){
    objs.push_back(Obj(name,objs.size(),filename,shader,scale));
}

Obj* ObjHandler::getObjByID(int id){

    return &objs.at(id);
}

Obj* ObjHandler::getObjByName(string name){
    for(int i = 0; i < objs.size(); ++i){
        if(objs[i].name == name){

            return &objs[i];
        }
    }
    cout << "Couldn't find " + name + " Obj" << endl;
    exit(-1);
}
//GAMEOBJECT
//represents a gameobject
GameObject::GameObject(Obj *obj, std::string name, shader_ref shader, float height):m_obj(obj),m_name(name), m_shader(shader), m_height(height)
{
    vec3f tmp = obj->bb_min + obj->bb_max;
    tmp /= 2;
    m_center = tmp;
    make_unit_matrix4x4f(&m_model);

}

void GameObject::set_model_matrix(matrix4x4f new_model){
    m_model = new_model;
}
matrix4x4f GameObject::get_model_matrix(){
    return m_model;
}

void GameObject::set_height(float height){
    m_model.col_major[3 * 4 + 1] = m_model.col_major[3 * 4 + 1] + height;
}

void GameObject::draw(){
   // m_obj->drawelements->front()->Modelmatrix(&m_model);

    for (vector<drawelement*>::iterator it = m_obj->drawelements->begin(); it != m_obj->drawelements->end(); ++it) {
        drawelement *de = *it;
        de->Modelmatrix(&m_model);
        de->bind();
        setup_dir_light(m_shader);
        de->apply_default_matrix_uniforms();
        de->apply_default_tex_uniforms_and_bind_textures();
       // int loc = glGetUniformLocation(gl_shader_object(m_shader), "proj");
        //glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

//        loc = glGetUniformLocation(gl_shader_object(m_shader), "view");
//        glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);

//        loc = glGetUniformLocation(gl_shader_object(m_shader), "model");
//        glUniformMatrix4fv(loc, 1, GL_FALSE, m_model.col_major);

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


        de->draw_em();
        de->unbind();
    }
}
//TREE
Tree::Tree(Obj *obj, string name, int x, int y, float height): GameObject(obj,name, find_shader("pos+norm+tc"), height){
    identifier = 't';
    m_pos = vec2i(x,y);

    m_model.col_major[3 * 4 + 0] = m_pos.x*render_settings::tile_size_x;
    m_model.col_major[3 * 4 + 1] = m_center.y + m_height;
    m_model.col_major[3 * 4 + 2] = m_pos.y*render_settings::tile_size_y;


}

//BUILDINGS
Building::Building(Obj *obj, string name, int x, int y, unsigned int owner,int size, float height):
    GameObject(obj, name ,find_shader("pos+norm+tc"), height),
    m_owner(owner) , m_size(size)

{
    identifier = 'b';
    m_pos = vec2i(x,y);

    m_model.col_major[3 * 4 + 0] = m_pos.x*render_settings::tile_size_x;
    m_model.col_major[3 * 4 + 1] = m_center.y + m_height;
    m_model.col_major[3 * 4 + 2] = m_pos.y*render_settings::tile_size_y;
    
    label.setup_display();
//	label.set_camera("lcam");
//	label.set_shader("special-text-shader");;
    label.update_label_pos(2*x, 2*y, height+2);

}

void Building::draw_label(){
	label.render_gui_overlay();
}
//UNITGROUP

UnitGroup::UnitGroup(Obj *obj, simple_heightmap *sh, string name, vec2i start, vec2i end, unsigned int owner, unsigned int unit_count, float time_to_rech_end, float height):
    GameObject(obj,name,find_shader("pos+norm+tc"), height),
    m_owner(owner), m_start(start), m_end(end),
    m_unit_count(unit_count), m_sh(sh),
    m_time_to_reach_end(time_to_rech_end), m_spawned(0)
{
    m_modelmatrices = vector<matrix4x4f>();
    m_pos = start;
    identifier = 'u';
    vec3f tmp = obj->bb_min + obj->bb_max;
    tmp /= 2;
    m_view_dir = vec2f(end.x - start.x, end.y - start.y);
    normalize_vec2f(&m_view_dir);
    m_model.col_major[3 * 4 + 0] = m_pos.x*render_settings::tile_size_x;
    m_model.col_major[3 * 4 + 1] = m_center.y + m_height;
    m_model.col_major[3 * 4 + 2] = m_pos.y*render_settings::tile_size_y;


}
void UnitGroup::force_position(vec2i pos){
    m_pos = pos;
    m_model.col_major[3 * 4 + 0] = m_pos.x*render_settings::tile_size_x;
    m_model.col_major[3 * 4 + 1] =  m_center.y + m_sh->get_height(m_pos.x, m_pos.y);
    m_model.col_major[3 * 4 + 2] = m_pos.y*render_settings::tile_size_y;
}

void UnitGroup::update_model_matrices(){

    for(int i = 0; i < m_modelmatrices.size(); ++i){


        m_modelmatrices[i].col_major[3 * 4 + 0] = m_model.col_major[3 * 4 + 0] + 2*cos(2*M_PI * ((float) i/(float)m_spawned));
        m_modelmatrices[i].col_major[3 * 4 + 1] = m_model.col_major[3 * 4 + 1];
        m_modelmatrices[i].col_major[3 * 4 + 2] = m_model.col_major[3 * 4 + 2] + 2*sin(2*M_PI * ((float) i/(float)m_spawned));
    }
}

void UnitGroup::move_to(vec2i pos, float time_to_reach){
    force_position(m_end);
    m_end = pos;
    m_time_to_reach_end = time_to_reach;
    m_timer.restart();
}

void UnitGroup::update(){
    float cur_time = m_timer.look();

    if(cur_time < m_time_to_reach_end){

        if(m_spawned < m_unit_count && m_spawn_timer.look() > TIME_TO_SPAWN){
            m_spawned++;
            matrix4x4f tmp;
            make_unit_matrix4x4f(&tmp);
            m_modelmatrices.push_back(tmp);
            m_spawn_timer.restart();
        }
        m_pos.x = m_start.x + cur_time*(m_end.x-m_start.x)/m_time_to_reach_end;
        m_pos.y = m_start.y + cur_time*(m_end.y-m_start.y)/m_time_to_reach_end;
        float x = (float) m_start.x + cur_time*((float)m_end.x-(float)m_start.x)/m_time_to_reach_end;
        float y = (float) m_start.x + cur_time*((float)m_end.x-(float)m_start.x)/m_time_to_reach_end;
        m_model.col_major[3 * 4 + 0] = x * render_settings::tile_size_x;
        m_model.col_major[3 * 4 + 1] = m_center.y + m_sh->get_height(m_pos.x, m_pos.y);
        m_model.col_major[3 * 4 + 2] = y * render_settings::tile_size_y;

        update_model_matrices();
    }

}

void UnitGroup::draw(){
    for(int i = 0; i < m_spawned; ++i){
    for (vector<drawelement*>::iterator it = m_obj->drawelements->begin(); it != m_obj->drawelements->end(); ++it) {
        drawelement *de = *it;
        de->Modelmatrix(&m_modelmatrices[i]);
        de->bind();
        setup_dir_light(m_shader);
        de->apply_default_matrix_uniforms();
        de->apply_default_tex_uniforms_and_bind_textures();
       // int loc = glGetUniformLocation(gl_shader_object(m_shader), "proj");
        //glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

//        loc = glGetUniformLocation(gl_shader_object(m_shader), "view");
//        glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);

//        loc = glGetUniformLocation(gl_shader_object(m_shader), "model");
//        glUniformMatrix4fv(loc, 1, GL_FALSE, m_model.col_major);

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


        de->draw_em();
        de->unbind();

    }
    }
}


