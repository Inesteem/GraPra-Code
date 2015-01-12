#include "gameobject.h"
#include "rendering.h"
//wrapper for objloader
Obj::Obj(string name, int id, string filename, shader_ref shader, float scale):id(id),name(name){
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


//handler for all .obj
ObjHandler::ObjHandler(){

}


//adds an .obj
void ObjHandler::addObj(string name, string filename, shader_ref shader, float scale){

    objs.push_back(Obj(name,objs.size(),filename, shader, scale));
}


Obj ObjHandler::getObjByID(int id){

    return objs.at(id);
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

//represents a gameobject
GameObject::GameObject(Obj *obj, std::string name, shader_ref shader):m_obj(obj),m_name(name), m_shader(shader)
{

    make_unit_matrix4x4f(&m_model);

}

void GameObject::multiply_model_matrix(matrix4x4f other){

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

Tree::Tree(Obj *obj, string name, int x, int y): GameObject(obj,name, find_shader("pos+norm+tc")){
    identifier = 't';
    m_pos = vec2i(x,y);
    vec3f tmp = obj->bb_min + obj->bb_max;
    tmp /= 2;
    m_model.col_major[3 * 4 + 0] = m_pos.x*render_settings::tile_size_x+render_settings::tile_size_x/2;
    m_model.col_major[3 * 4 + 1] = tmp.y;
    m_model.col_major[3 * 4 + 2] = m_pos.y*render_settings::tile_size_y+render_settings::tile_size_y/2;


}

Building::Building(Obj *obj, string name, int x, int y, unsigned int owner):
    GameObject(obj, name ,find_shader("pos+norm+tc")),
    m_owner(owner)

{
    identifier = 'b';
    m_pos = vec2i(x,y);
    vec3f tmp = obj->bb_min + obj->bb_max;
    tmp /= 2;
    m_model.col_major[3 * 4 + 0] = m_pos.x*render_settings::tile_size_x+render_settings::tile_size_x/2;
    m_model.col_major[3 * 4 + 1] = tmp.y;
    m_model.col_major[3 * 4 + 2] = m_pos.y*render_settings::tile_size_y+render_settings::tile_size_y/2;

}


