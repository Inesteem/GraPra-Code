#include "gameobject.h"
#include "rendering.h"
#include "label.h"
#include <cmath>
//wrapper for objloader
Obj::Obj(string name, int id, string filename, shader_ref shader):id(id),name(name),shader(shader){
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
Obj::Obj(string name, int id, string filename, shader_ref shader,vec3f scale):id(id),name(name),shader(shader){
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

Obj::Obj(string name, int id, mesh_ref mesh, texture_ref tex, shader_ref shader): id(id), name(name), mesh(mesh), tex(tex), shader(shader){

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


Obj *ObjHandler::get_selection_circle(){
    string name = "selection_circle";
    for(int i = 0; i < objs.size(); ++i){
        if(objs[i].name == name){

            return &objs[i];
        }
    }
    mesh_ref mesh = make_mesh(name.c_str(),2);
    vector<vec3f> pos = { vec3f(0,0,0) , vec3f(1,0,0), vec3f(1,0,1) , vec3f(0,0,1) };
    vector<vec2f> tc = { vec2f(0,0), vec2f(0,1), vec2f(1,1), vec2f(1,0) } ;
    vector<unsigned int> index =  { 0 ,1 ,2 , 2 ,3 ,0 };
    bind_mesh_to_gl(mesh);
    add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, 4, 3, (float*) pos.data() , GL_STATIC_DRAW);
    add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, 4, 2, (float*) tc.data() , GL_STATIC_DRAW);
   // add_vertex_buffer_to_mesh(m_mesh, "in_normal", GL_FLOAT, m_width*m_height, 3,nullptr, GL_STATIC_DRAW );
    add_index_buffer_to_mesh(mesh, index.size(), (unsigned int *) index.data(), GL_STATIC_DRAW);
    unbind_mesh_from_gl(mesh);
    cout << "pushed" << endl;
    objs.push_back(Obj(name,objs.size(),mesh,find_texture("selection_circle"),find_shader("selection_circle_shader")));

    return get_selection_circle();

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

    matrix4x4f tmp;
    make_unit_matrix4x4f(&tmp);
    vec3f axis = vec3f(0,1,0);
    make_rotation_matrix4x4f(&tmp,&axis,2*random_float());

    m_model = m_model*tmp;
}

//BUILDINGS
Building::Building(Obj *obj,Obj *selection_circle, string name, int x, int y, unsigned int owner,int size, float height,unsigned int id):
    GameObject(obj, name ,find_shader("pos+norm+tc"), height), id(id),
    m_owner(owner) , m_size(size), selection_circle(selection_circle)

{

	unit_count = 0;
    identifier = 'b';
    m_pos = vec2i(x,y);

    m_model.col_major[3 * 4 + 0] = m_pos.x*render_settings::tile_size_x;
    m_model.col_major[3 * 4 + 1] = m_center.y + m_height;
    m_model.col_major[3 * 4 + 2] = m_pos.y*render_settings::tile_size_y;
    
    label= new Label();
    label->setup_display();
    label->update_label_pos(2*x, 2*y, height+2);

}

void Building::update_unit_count(int count){
	unit_count = count;
	label->update_gui_texture_int(count);
}

int Building::get_unit_count(){
	return unit_count;
}


void Building::draw(){
	label->update_gui_texture_int(unit_count);
    GameObject::draw();
	label->render_gui_overlay();
}

float Building::dist_to(vec3f &pos){
    vec3f npos = vec3f(m_model.col_major[3 * 4 + 0], m_model.col_major[3 * 4 + 1], m_model.col_major[3 * 4 + 2]);
    vec3f dist = npos - pos;
    return length_of_vec3f(&dist);

}

unsigned int Building::get_owner_id(){
	return m_owner;
}

unsigned int Building::get_id(){
	return id;
}

void Building::draw_selection_circle(){
    vec3f color(1.0f,0,0);
    bind_shader(selection_circle->shader);

    matrix4x4f tmp;
    make_unit_matrix4x4f(&tmp);

    tmp = tmp*m_model;

    tmp.col_major[3 * 4 + 1] +=  0.5f;

    tmp.col_major[0 * 4 + 0] =  m_model.col_major[0 * 4 + 0] * 2*m_size;
    tmp.col_major[1 * 4 + 1] = m_model.col_major[1 * 4 + 1] * 2*m_size;
    tmp.col_major[2 * 4 + 2] = m_model.col_major[2 * 4 + 2] * 2*m_size;

    int loc = glGetUniformLocation(gl_shader_object(selection_circle->shader), "proj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

    loc = glGetUniformLocation(gl_shader_object(selection_circle->shader), "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);

    loc = glGetUniformLocation(gl_shader_object(selection_circle->shader), "model");
    glUniformMatrix4fv(loc, 1, GL_FALSE, tmp.col_major);

    loc = glGetUniformLocation(gl_shader_object(selection_circle->shader), "color");
    glUniform3fv(loc,1,(float *) &color);

    bind_texture(selection_circle->tex, 0);
    loc = glGetUniformLocation(gl_shader_object(selection_circle->shader), "tex");
    glUniform1i(loc, 0);

    bind_mesh_to_gl(selection_circle->mesh);

    draw_mesh(selection_circle->mesh);

    unbind_mesh_from_gl(selection_circle->mesh);
    unbind_shader(selection_circle->shader);

}

//UNITGROUP

UnitGroup::UnitGroup(Obj *obj, simple_heightmap *sh, string name, vec2i start, vec2i end, unsigned int owner, unsigned int unit_count, float time_to_rech_end, float height, unsigned int m_id):
    GameObject(obj,name,find_shader("pos+norm+tc"), height),
    m_owner(owner), m_start(start), m_end(end),
    m_unit_count(unit_count), m_sh(sh), m_id(m_id),
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

    matrix4x4f testUnit;
    make_unit_matrix4x4f(&testUnit);
    m_modelmatrices.push_back(testUnit);
    m_spawned++;
}
void UnitGroup::force_position(vec2i pos){
    m_pos = pos;

    m_model.col_major[3 * 4 + 0] = m_pos.x*render_settings::tile_size_x;
    m_model.col_major[3 * 4 + 1] =  m_center.y + m_sh->get_height(m_pos.x, m_pos.y);
    m_model.col_major[3 * 4 + 2] = m_pos.y*render_settings::tile_size_y;

    update_model_matrices();
}

void UnitGroup::update_model_matrices(){

    for(int i = 0; i < m_modelmatrices.size(); ++i){


        m_modelmatrices[i].col_major[3 * 4 + 0] = m_model.col_major[3 * 4 + 0];// + 2*cos(2*M_PI * ((float) i/(float)m_spawned));
        m_modelmatrices[i].col_major[3 * 4 + 1] = m_model.col_major[3 * 4 + 1];
        m_modelmatrices[i].col_major[3 * 4 + 2] = m_model.col_major[3 * 4 + 2];// + 2*sin(2*M_PI * ((float) i/(float)m_spawned));
    }
}

void UnitGroup::move_to(vec2i pos, float time_to_reach){
  //  force_position(m_end);
   // force_position(m_end);
	vec2f pos_1 = vec2f(m_model.col_major[3 * 4 + 0], m_model.col_major[3 * 4 + 2]);
    m_start = m_end;

    m_end = pos;
    m_time_to_reach_end = time_to_reach;
    m_timer.restart();

    update_model_matrices();
}

void UnitGroup::update(){
    float cur_time = m_timer.look();

    if(cur_time < m_time_to_reach_end){

        if(m_spawned < m_unit_count && m_spawn_timer.look() > time_to_spawn){
            m_spawned++;
            matrix4x4f tmp;
            make_unit_matrix4x4f(&tmp);
            m_modelmatrices.push_back(tmp);
            m_spawn_timer.restart();
        }
    //    cout << "start: " << m_start.x << "," << m_start.y << endl;
   //     cout << "pos: " << m_pos.x << "," << m_pos.y << endl;
  //      cout << "end: " << m_end.x << "," << m_end.y << endl;
        m_pos.x = m_start.x + cur_time*(m_end.x-m_start.x)/m_time_to_reach_end;
        m_pos.y = m_start.y + cur_time*(m_end.y-m_start.y)/m_time_to_reach_end;
        float x = (float) m_start.x + cur_time*((float)m_end.x-(float)m_start.x)/m_time_to_reach_end;
        float y = (float) m_start.y + cur_time*((float)m_end.y-(float)m_start.y)/m_time_to_reach_end;
  //      cout << x << " " << y << endl;
        m_model.col_major[3 * 4 + 0] = x * render_settings::tile_size_x;
        m_model.col_major[3 * 4 + 1] = m_center.y + m_sh->get_height(x, y);
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
			de->draw_em();
			de->unbind();

		}
    }
}


