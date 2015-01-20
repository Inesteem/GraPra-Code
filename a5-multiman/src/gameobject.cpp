#define update_1 10
#define update_2 100


#include "gameobject.h"
#include "rendering.h"
#include "label.h"
#include "limits"
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
 
 
    mesh = make_mesh(name.c_str(),3);
    bind_mesh_to_gl(mesh);
    add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, loader.objdata.vertices, 3, (float*) loader.objdata.vertex_data , GL_STATIC_DRAW);
    add_vertex_buffer_to_mesh(mesh, "in_norm", GL_FLOAT, loader.objdata.vertices, 3, (float *) loader.objdata.normal_data, GL_STATIC_DRAW);
    add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, loader.objdata.vertices, 2, (float *) loader.objdata.texcoord_data, GL_STATIC_DRAW);
    add_index_buffer_to_mesh(mesh, loader.objdata.groups->triangles * 3, (unsigned int *) loader.objdata.groups->v_ids, GL_STATIC_DRAW);
    unbind_mesh_from_gl(mesh);
    tex_params_t p = default_tex_params();
    tex = make_texture_ub(("tex"), loader.objdata.groups->mtl->tex_d, GL_TEXTURE_2D, &p);
}


Obj::Obj(string name, int id, string filename, shader_ref shader, vec3f bb_min, vec3f bb_max):id(id),name(name),shader(shader), bb_min(bb_min), bb_max(bb_max){
    ObjLoader loader(name.c_str(), filename.c_str());
    loader.pos_and_norm_shader = shader;
    loader.pos_norm_and_tc_shader = shader;
    loader.default_shader = shader;
    vec3f min, max;
    loader.BoundingBox(min, max);
    loader.ScaleVertexDataToFit(bb_min,bb_max);
    vec3f new_pos = vec3f(0,-1,-1);
    loader.TranslateTo(new_pos);
    drawelements = new vector<drawelement*>();
    loader.GenerateNonsharingMeshesAndDrawElements(*drawelements);

	//unuseful?

    mesh = make_mesh(name.c_str(),3);
    bind_mesh_to_gl(mesh);
    add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, loader.objdata.vertices, 3, (float*) loader.objdata.vertex_data , GL_STATIC_DRAW);
    add_vertex_buffer_to_mesh(mesh, "in_norm", GL_FLOAT, loader.objdata.vertices, 3, (float *) loader.objdata.normal_data, GL_STATIC_DRAW);
    add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, loader.objdata.vertices, 2, (float *) loader.objdata.texcoord_data, GL_STATIC_DRAW);
    add_index_buffer_to_mesh(mesh, loader.objdata.groups->triangles * 3, (unsigned int *) loader.objdata.groups->v_ids, GL_STATIC_DRAW);
    unbind_mesh_from_gl(mesh);
    tex_params_t p = default_tex_params();
    tex = make_texture_ub(("tex"), loader.objdata.groups->mtl->tex_d, GL_TEXTURE_2D, &p);  
    
}



Obj::Obj(string name, int id, string filename, shader_ref shader,vec3f scale):id(id),name(name),shader(shader){
    ObjLoader loader(name.c_str(), filename.c_str());
    loader.TranslateToOrigin();
    loader.pos_and_norm_shader = shader;
    loader.pos_norm_and_tc_shader = shader;
    loader.default_shader = shader;
    mesh = make_mesh(name.c_str(),3);
    bind_mesh_to_gl(mesh);
    add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, loader.objdata.vertices, 3, (float*) loader.objdata.vertex_data , GL_STATIC_DRAW);
    add_vertex_buffer_to_mesh(mesh, "in_norm", GL_FLOAT, loader.objdata.vertices, 3, (float *) loader.objdata.normal_data, GL_STATIC_DRAW);
    add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, loader.objdata.vertices, 2, (float *) loader.objdata.texcoord_data, GL_STATIC_DRAW);
    add_index_buffer_to_mesh(mesh, loader.objdata.groups->triangles * 3, (unsigned int *) loader.objdata.groups->v_ids, GL_STATIC_DRAW);
    unbind_mesh_from_gl(mesh);
    tex_params_t p = default_tex_params();
    tex = make_texture_ub(("tex"), loader.objdata.groups->mtl->tex_d, GL_TEXTURE_2D, &p);

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
void ObjHandler::addObj(string name, string filename, shader_ref shader, vec3f bb_min, vec3f bb_max){

    objs.push_back(Obj(name,objs.size(),filename, shader, bb_min, bb_max));
}



void ObjHandler::addObj_withScale(string name, string filename, shader_ref shader, vec3f scale){
    objs.push_back(Obj(name,objs.size(),filename,shader,scale));
}

void ObjHandler::addMeshObj(string name, mesh_ref mesh, shader_ref shader, texture_ref tex){
    objs.push_back(Obj(name, objs.size(), mesh, tex, shader));
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
    m_pos = vec2f(x,y);

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
Building::Building(Obj *obj,Obj *selection_circle,Obj *upgrade_arrow, string name, int x, int y, unsigned int owner,int size, float height,unsigned int id):
    GameObject(obj, name ,find_shader("pos+norm+tc"), height), id(id),
    m_owner(owner) , m_size(size), selection_circle(selection_circle), upgrade_arrow(upgrade_arrow)

{

    selection_circle->mesh;
	unit_count = 0;
    identifier = 'b';
    m_pos = vec2f(x,y);

    m_model.col_major[3 * 4 + 0] = m_pos.x*render_settings::tile_size_x;
    m_model.col_major[3 * 4 + 1] = m_center.y + m_height;
    m_model.col_major[3 * 4 + 2] = m_pos.y*render_settings::tile_size_y;
    
    make_unit_matrix4x4f(&arrow_model);
    arrow_model.col_major[3 * 4 + 0] = m_pos.x*render_settings::tile_size_x;
    arrow_model.col_major[3 * 4 + 1] = m_center.y + height+5;
    arrow_model.col_major[3 * 4 + 2] = m_pos.y*render_settings::tile_size_y;
    

    
    label= new Label();
    label->setup_display();
    label->update_label_pos(2*x, 2*y, height+2);
 
    
    state = 0;

}

void Building::update_unit_count(int count){
	unit_count = count;
	label->update_gui_texture_int(count);
}

int Building::get_unit_count(){
	return unit_count;
}

int Building::get_state(){
	return state;
}


void Building::upgrade(Obj *obj, int state){
	m_obj = obj;
	this->state = state;
}


void Building::draw(){
	static float rotation = 0;
	rotation += 0.007;
	if(rotation == std::numeric_limits<float>::max()-2)
		rotation = 0;

//    if(m_owner == -1){
		if(state <=0){
		matrix4x4f shovel_model;
		
		vec3f rot_vec = vec3f(0,1,0);
		make_rotation_matrix4x4f(&shovel_model,&rot_vec, rotation);
		shovel_model =  m_model * shovel_model;
		int i = 0;
		for(vector<drawelement*>::iterator it = m_obj->drawelements->begin(); it != m_obj->drawelements->end(); ++it) {
			drawelement *de = *it;
			de->Modelmatrix(&m_model);
			if(i==1)
				de->Modelmatrix(&shovel_model);
			de->bind();
			setup_dir_light(m_shader);
			de->apply_default_matrix_uniforms();
			de->apply_default_tex_uniforms_and_bind_textures();

			de->draw_em();
			de->unbind();
			i++;
		}
		
	} else if (state == 1) {
		draw_state_1();
		
	} else {
		GameObject::draw();
	}
	
	label->update_gui_texture_int(unit_count);
	label->render_gui_overlay();
	
	if(check_for_upgrade(true)){
		matrix4x4f arrow_model_2;
		vec3f rot_vec = vec3f(0,1,0);
		make_rotation_matrix4x4f(&arrow_model_2,&rot_vec, rotation/2);
		arrow_model_2 =  arrow_model * arrow_model_2;		
		    arrow_model_2.col_major[3 * 4 + 1] += sin( rotation/2 );
		for (vector<drawelement*>::iterator it = upgrade_arrow->drawelements->begin(); it != upgrade_arrow->drawelements->end(); ++it) {
		
			drawelement *de = *it;
			de->Modelmatrix(&arrow_model_2);
			de->bind();
			setup_dir_light(m_shader);
			de->apply_default_matrix_uniforms();
			de->apply_default_tex_uniforms_and_bind_textures();
			de->draw_em();
			de->unbind();
		}		
		
	}

    draw_selection_circle(3);
}

void Building::draw_state_1(){
	shader_ref shader_t = find_shader("alpha-color-shader");
	bind_shader(shader_t);

	glEnable(GL_BLEND);
	int i = 0;
	for (vector<drawelement*>::iterator it = m_obj->drawelements->begin(); it != m_obj->drawelements->end(); ++it) {
		drawelement *de = *it;
		de->Modelmatrix(&m_model);
		setup_dir_light(shader_t);
		de->apply_default_matrix_uniforms(shader_t);
		de->apply_default_tex_uniforms_and_bind_textures(shader_t);
//		mesh_ref mesh_1 = *de->meshes.begin();
//		cout << mesh_name(mesh_1) << endl;
//		cout << de->name << endl;
		int loc;
		
		texture_ref tex_alpha = find_texture("alpha_mask_1");
		
		bind_texture(tex_alpha, 1);
		loc = glGetUniformLocation(gl_shader_object(shader_t), "alpha_tex");
		glUniform1i(loc, 1);	
		vec4f color = vec4f(0,0,0,1);
		float use_alpha = 0;

		if(i == 2)
			use_alpha = 0;
		if(i == 0 || i == 1)
			color = vec4f(1,0,0,1);
			

			

		
		loc = glGetUniformLocation(gl_shader_object(shader_t), "color");
		glUniform4fv(loc, 1,(float *)&color);				

		loc = glGetUniformLocation(gl_shader_object(shader_t), "use_alpha");
		glUniform1f(loc,use_alpha);

		
		de->draw_em();
		i++;

	}	
//		cout << i << endl;

	glDisable(GL_BLEND);
	unbind_shader(shader_t);

	
}


float Building::dist_to(vec3f &pos){
    vec3f npos = vec3f(m_model.col_major[3 * 4 + 0], m_model.col_major[3 * 4 + 1], m_model.col_major[3 * 4 + 2]);
    vec3f dist = npos - pos;
    return length_of_vec3f(&dist);

}

bool Building::check_for_upgrade(bool up){
	if(!up)
		return true;
	
	if(state == 0 && unit_count >= update_1)
		return true;
	
//	else if(state == 1 && unit_count >= update_2)
//		return true;
		
	return false;
		
}

unsigned int Building::get_owner_id(){
	return m_owner;
}

unsigned int Building::get_id(){
	return id;
}

void Building::draw_selection_circle(int size){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    vec3f color = get_player_color(m_owner);
    bind_shader(find_shader("selection_circle_shader"));

    matrix4x4f tmp;
    make_unit_matrix4x4f(&tmp);

    tmp = tmp*m_model;



    tmp.col_major[0 * 4 + 0] =  m_model.col_major[0 * 4 + 0] * (render_settings::tile_size_x+2) * size;
    tmp.col_major[1 * 4 + 1] =  m_model.col_major[1 * 4 + 1] * (render_settings::tile_size_x+2) * size;
    tmp.col_major[2 * 4 + 2] =  m_model.col_major[2 * 4 + 2] * (render_settings::tile_size_y+2) * size;
    tmp.col_major[3 * 4 + 0] =  m_pos.x * render_settings::tile_size_x - ((render_settings::tile_size_x+2) * size )/2.0f * m_center.x;
    tmp.col_major[3 * 4 + 1] +=  0.0f;
    tmp.col_major[3 * 4 + 2] =  m_pos.y * render_settings::tile_size_y - ((render_settings::tile_size_y+2) * size )/2.0f *  m_center.z;

    int loc = glGetUniformLocation(gl_shader_object(find_shader("selection_circle_shader")), "proj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

    loc = glGetUniformLocation(gl_shader_object(find_shader("selection_circle_shader")), "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);

    loc = glGetUniformLocation(gl_shader_object(find_shader("selection_circle_shader")), "model");
    glUniformMatrix4fv(loc, 1, GL_FALSE, tmp.col_major);

    loc = glGetUniformLocation(gl_shader_object(find_shader("selection_circle_shader")), "color");
    glUniform3fv(loc,1,(float *) &color);

    bind_texture(find_texture("selection_circle"), 0);
    loc = glGetUniformLocation(gl_shader_object(find_shader("selection_circle_shader")), "tex");
    glUniform1i(loc, 0);

    bind_mesh_to_gl(selection_circle->mesh);

    draw_mesh(selection_circle->mesh);

    unbind_mesh_from_gl(selection_circle->mesh);
    unbind_shader(find_shader("selection_circle_shader"));

    unbind_texture(find_texture("selection_circle"));
    glDisable(GL_BLEND);
}

void Building::change_owner(unsigned int owner){
    m_owner = owner;
}

//UNITGROUP

UnitGroup::UnitGroup(Obj *obj, simple_heightmap *sh, string name, vec2f start, vec2f end, unsigned int owner, unsigned int unit_count, float time_to_rech_end, float height, unsigned int m_id):
    GameObject(obj,name,find_shader("pos+norm+tc"), height),
    m_owner(owner), m_start(start), m_end(end),
    m_unit_count(unit_count), m_sh(sh), m_id(m_id),
    m_time_to_reach_end(time_to_rech_end), m_spawned(0)
{
    m_modelmatrices = vector<matrix4x4f>();
    m_cur_heights = vector<float>();
    m_dest_heights = vector<float>();
    m_up_speed = vector<float>();
    m_row_size = vector<unsigned int>();
    m_pos = start;
    identifier = 'u';
    vec3f tmp = obj->bb_min + obj->bb_max;
    tmp /= 2;
    m_view_dir = vec2f(end.x - start.x, end.y - start.y);
    normalize_vec2f(&m_view_dir);
    m_model.col_major[3 * 4 + 0] = m_pos.x*render_settings::tile_size_x;
    m_model.col_major[3 * 4 + 1] = m_center.y + m_height;
    m_model.col_major[3 * 4 + 2] = m_pos.y*render_settings::tile_size_y;

//    matrix4x4f testUnit;
//    make_unit_matrix4x4f(&testUnit);
//    m_modelmatrices.push_back(testUnit);
//    m_spawned++;
}
void UnitGroup::force_position(vec2f pos){
    m_pos = pos;

    m_model.col_major[3 * 4 + 0] = m_pos.x*render_settings::tile_size_x;
    m_model.col_major[3 * 4 + 1] =  m_center.y + m_sh->get_height(m_pos.x, m_pos.y);
    m_model.col_major[3 * 4 + 2] = m_pos.y*render_settings::tile_size_y;

    update_model_matrices();
}

int get_coooooords(int index, int size){
    int k = size +1;
    return k/2 + index;
}

void UnitGroup::update_model_matrices(){

//    cout << m_modelmatrices.size() << endl;
//    cout << m_cur_heights.size() << endl;
    unsigned int row_size = 0;
    for(unsigned int i = 0;  i < m_row_size.size(); ++i){
//       cout << "array size" <<m_row_size.size() << endl;
        for(int j = 0; j < m_row_size[i]; ++j){
//            cout << "row size "<<row_size << endl;
//            cout << "model size" << m_modelmatrices.size() << endl;
            vec2f ortho = vec2f(m_view_dir.y, -m_view_dir.x);
            normalize_vec2f(&ortho);
            m_modelmatrices.at(row_size ).col_major[3 * 4 + 0] = m_model.col_major[3 * 4 + 0] + get_coooooords(j,m_row_size[i])* ortho.x  - i *m_view_dir.x;
            m_modelmatrices.at(row_size ).col_major[3 * 4 + 1] = m_center.y + m_cur_heights.at(i);// m_sh->get_height(m_pos.x + cos(2*M_PI * ((float) i/(float)m_spawned)), m_pos.y + sin(2*M_PI * ((float) i/(float)m_spawned)) );
            m_modelmatrices.at(row_size ).col_major[3 * 4 + 2] = m_model.col_major[3 * 4 + 2] + get_coooooords(j,m_row_size[i])* ortho.y  - i *m_view_dir.y;
            row_size += 1;
        }
    }
}

void UnitGroup::move_to(vec2f pos, float time_to_reach){
  //  force_position(m_end);
   // force_position(m_end);
	vec2f pos_1 = vec2f(m_model.col_major[3 * 4 + 0], m_model.col_major[3 * 4 + 2]);
    m_start = m_end;

    m_end = pos;
    m_view_dir = m_end - m_start;
    m_time_to_reach_end = time_to_reach;
    m_timer.restart();

    update_model_matrices();
}

void UnitGroup::update(){
    float cur_time = m_timer.look();

    if(cur_time < m_time_to_reach_end){

        if(m_spawned < m_unit_count && m_spawn_timer.look() > time_to_spawn){
            spawn_unit_row(std::min((unsigned int) 5,m_unit_count-m_spawned));
            m_rows++;
            m_spawn_timer.restart();
        }
    //    cout << "start: " << m_start.x << "," << m_start.y << endl;
   //     cout << "pos: " << m_pos.x << "," << m_pos.y << endl;
  //      cout << "end: " << m_end.x << "," << m_end.y << endl;
        m_pos.x = m_start.x + cur_time*(m_end.x-m_start.x)/m_time_to_reach_end;
        m_pos.y = m_start.y + cur_time*(m_end.y-m_start.y)/m_time_to_reach_end;
        float x = (float) m_start.x + cur_time*((float)m_end.x-(float)m_start.x)/m_time_to_reach_end;
        float y = (float) m_start.y + cur_time*((float)m_end.y-(float)m_start.y)/m_time_to_reach_end;

//        cout << x << " " << y << endl;
        m_model.col_major[3 * 4 + 0] = x * render_settings::tile_size_x;
        m_model.col_major[3 * 4 + 1] = m_center.y + m_sh->get_height(x, y);
        m_model.col_major[3 * 4 + 2] = y * render_settings::tile_size_y;
        update_dest_heights();
        update_cur_heights();
        update_model_matrices();
    }

}
void UnitGroup::update_cur_heights(){
    float time = m_timer.look();
    float end = m_sh->get_height(m_end.x,m_end.y);
    float start = m_sh->get_height(m_start.x,m_start.y);
    for(int i = 0; i < m_cur_heights.size(); ++i){
        m_cur_heights[i] = start + time * (end - start)/m_time_to_reach_end;
    }
}

void UnitGroup::update_dest_heights(){

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
void UnitGroup::spawn_unit_row(unsigned int size){
    m_row_size.push_back(size);
    for(int i = 0; i < size; ++i){
//        cout << size << endl;
        m_spawned++;

        matrix4x4f tmp;
        make_unit_matrix4x4f(&tmp);
        m_modelmatrices.push_back(tmp);
        float t = 1.0f;
        m_cur_heights.push_back(t);
        m_dest_heights.push_back(t);
        m_up_speed.push_back(t);
    }
}


