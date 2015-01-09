#include "gameobject.h"

//wrapper for objloader
Obj::Obj(string name, int id, string filename, shader_ref shader, float scale):id(id),name(name){
    ObjLoader loader(name.c_str(), filename.c_str());
    loader.TranslateToOrigin();
    loader.pos_and_norm_shader = shader;
    loader.pos_norm_and_tc_shader = shader;
    loader.default_shader = shader;
     loader.ScaleVertexData(vec3f(scale,scale,scale));
     drawelements = new vector<drawelement*>();
    loader.GenerateNonsharingMeshesAndDrawElements(*drawelements);

}

ObjHandler::ObjHandler(){

}



//handler for all .obj
void ObjHandler::addObj(string name, string filename, shader_ref shader, float scale){

    objs.push_back(Obj(name,objs.size(),filename, shader, scale));
}


Obj ObjHandler::getObjByID(int id){

    return objs.at(id);
}

Obj ObjHandler::getObjByName(string name){
    for(int i = 0; i < objs.size(); ++i){
        if(objs[i].name == name){

            return objs[i];
        }
    }
    cout << "Couldn't find " + name + " Obj" << endl;
    exit(-1);
}

//represents a gameobject
GameObject::GameObject(ObjHandler &objhandler, std::string name, shader_ref shader): m_shader(shader)
{
    m_drawelements = (objhandler.getObjByName(name).drawelements);
    make_unit_matrix4x4f(&m_model);

}

void GameObject::multiply_model_matrix(matrix4x4f other){
    matrix4x4f tmp;
   // multiply_matrices4x4f(&tmp, &m_model, &other);
    //m_model = tmp;
}

void GameObject::draw(){
    m_drawelements->front()->Modelmatrix(&m_model);

    for (vector<drawelement*>::iterator it = m_drawelements->begin(); it != m_drawelements->end(); ++it) {
        drawelement *de = *it;

        de->bind();
        de->apply_default_matrix_uniforms();
        //de->apply_default_tex_uniforms_and_bind_textures();
        int loc = glGetUniformLocation(gl_shader_object(m_shader), "proj");
        glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

        loc = glGetUniformLocation(gl_shader_object(m_shader), "view");
        glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);

        loc = glGetUniformLocation(gl_shader_object(m_shader), "model");
        glUniformMatrix4fv(loc, 1, GL_FALSE, m_model.col_major);


        de->draw_em();
        de->unbind();
    }
}

Tree::Tree(ObjHandler &objhandler, string name, int x, int y): GameObject(objhandler,name, find_shader("tree-shader")){
    identifier = 't';
    m_pos = vec2i(x,y);
    m_model.col_major[1  + 4 * 3] = m_pos.x;
    m_model.col_major[2  + 4 * 3] = 0;
    m_model.col_major[3  + 4 * 3] = m_pos.y;

}

Building::Building(ObjHandler &objhandler,string name, int x, int y, unsigned int owner):
    GameObject(objhandler, name ,find_shader("building-shader")),
    m_owner(owner)

{
    identifier = 'b';
    m_pos = vec2i(x,y);

    m_model.col_major[1  + 4 * 3] = m_pos.x;
    m_model.col_major[2  + 4 * 3] = 0;
    m_model.col_major[3  + 4 * 3] = m_pos.y;

}


