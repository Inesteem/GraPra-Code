#define update_1 10
#define update_2 20

#include "game.h"
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

Obj::Obj(string name, int id, string filename, shader_ref shader, int changes):id(id),name(name),shader(shader){
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

    mesh = make_mesh(name.c_str(),1);
    bind_mesh_to_gl(mesh);
    add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, loader.objdata.vertices, 3, (float*) loader.objdata.vertex_data , GL_STATIC_DRAW);
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

Obj::Obj(string name, int id, vector<string> filenames, shader_ref shader):name(name), id(id), shader(shader){
    vector<ObjLoader> loaders;
    mesh = make_mesh(name.c_str(),filenames.size()*2 + 1);
    for(int i = 0; i < filenames.size(); ++i){
        loaders.push_back( ObjLoader(filenames[i].c_str(),filenames[i].c_str()));
        loaders[i].TranslateToOrigin();
        loaders[i].pos_and_norm_shader = shader;
        loaders[i].pos_norm_and_tc_shader = shader;
        loaders[i].default_shader = shader;
        string inpos = "in_pos_";
        string innorm = "in_norm_";

        inpos  += to_string(i);
        innorm += to_string(i);
        cout << inpos << innorm << endl;
        bind_mesh_to_gl(mesh);
        add_vertex_buffer_to_mesh(mesh, inpos.c_str(), GL_FLOAT, loaders[i].objdata.vertices, 3, (float*) loaders[i].objdata.vertex_data , GL_STATIC_DRAW);
        add_vertex_buffer_to_mesh(mesh, innorm.c_str(), GL_FLOAT, loaders[i].objdata.vertices, 3, (float *) loaders[i].objdata.normal_data, GL_STATIC_DRAW);
        unbind_mesh_from_gl(mesh);

    }


    loaders[0].BoundingBox(bb_min,bb_max);

    bind_mesh_to_gl(mesh);
    add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, loaders[0].objdata.vertices, 2, (float *) loaders[0].objdata.texcoord_data, GL_STATIC_DRAW);
    add_index_buffer_to_mesh(mesh, loaders[0].objdata.groups->triangles * 3, (unsigned int *) loaders[0].objdata.groups->v_ids, GL_STATIC_DRAW);
    unbind_mesh_from_gl(mesh);

//    tex_params_t p = default_tex_params();
//    tex = make_texture_ub(("tex"), loaders[0].objdata.groups->mtl->tex_d, GL_TEXTURE_2D, &p);
}


//handler for all .obj
ObjHandler::ObjHandler(){

}


//adds an .obj
void ObjHandler::addObj_changeable(string name, string filename, shader_ref shader, int changes){
    objs.push_back(Obj(name,objs.size(),filename, shader, changes));	
}
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
void ObjHandler::makeObjFMS(vector<string> filenames, string name, shader_ref shader){
    objs.push_back(Obj(name,objs.size(),filenames, shader));
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
	settlement = true;
    turret = false;
    selection_circle->mesh;
	unit_count = 0;
    identifier = 'b';
    m_pos = vec2f(x,y);

    vec3f tmp = obj->bb_min + obj->bb_max;
    tmp *= m_size;
    tmp /= 2;
    m_center = tmp;
    m_model.col_major[0 * 4 + 0] = m_size;
    m_model.col_major[1 * 4 + 1] = m_size;
    m_model.col_major[2 * 4 + 2] = m_size;
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
    vec3f color = get_player_color(m_owner);
    label->set_color(color);
    
    state = msg::building_state::construction_site;

    vec3f effectPos = vec3f(m_pos.x*render_settings::tile_size_x, m_center.y + m_height, m_pos.y*render_settings::tile_size_y);
    vec3f effectCol = vec3f(0,0,1.0f);
    m_upgradeEffect = new UpgradeEffect(effectPos, effectCol);
}

Building::~Building()
{
    //delete m_upgradeEffect;
}

void Building::change_size(int size){
    m_size = size;
    m_model.col_major[0 * 4 + 0] = m_size;
    m_model.col_major[1 * 4 + 1] = m_size;
    m_model.col_major[2 * 4 + 2] = m_size;

    vec3f tmp = m_obj->bb_min + m_obj->bb_max;
    tmp *= size;
    tmp /= 2;
    m_center = tmp;
     m_model.col_major[3 * 4 + 1] = m_center.y + m_height;
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

int Building::get_type(){
	
	if(settlement)		
		return 0;
	return 1;
	
}

int Building::get_level(){
	
	if(		state == msg::building_state::house_lvl1 || state == msg::building_state::turret_lvl1)		
		return 0;
	else if(state == msg::building_state::house_lvl2 || state == msg::building_state::turret_lvl2)	
		return 1;
    else if(state == msg::building_state::house_lvl3)
		return 2;		
	else
		return -1;
	
}


void Building::upgrade(Obj *obj, int state){
	m_obj = obj;
	this->state = state;
	
    if(state == msg::building_state::turret_lvl1 || state == msg::building_state::turret_lvl2){
		settlement = false;
		turret = true;
	} 
	else if(state == msg::building_state::house_lvl1 || state == msg::building_state::house_lvl2 || state == msg::building_state::house_lvl3){
		settlement = true;
		turret = false;
	}
	
    //m_upgradeEffect->Start();
}


void Building::draw(){
    m_upgradeEffect->Update();
    m_upgradeEffect->Render();

	static float rotation = 0;
	rotation += 0.007;
	if(rotation == std::numeric_limits<float>::max()-2)
		rotation = 0;

        if(state <= msg::building_state::construction_site){
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
		
    } else if (state == msg::building_state::house_lvl1) {
		draw_state_house_1();
		
    } else if (state == msg::building_state::turret_lvl1) {
		draw_state_turret_1();
		
	} else {
		GameObject::draw();
	}
	
	label->update_gui_texture_int(unit_count);
	label->render_gui_overlay();
	
	bool upgradeable = check_for_upgrade_settlement(state+1);
	if(turret)
		upgradeable = check_for_upgrade_turret(state+1);
	
	
	
	if(upgradeable && PLAYER_ID == m_owner){
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

void Building::draw_state_turret_1(){

	for (vector<drawelement*>::iterator it = m_obj->drawelements->begin(); it != m_obj->drawelements->end(); ++it) {
			drawelement *de = *it;
			de->Modelmatrix(&m_model);
			de->bind();
			setup_dir_light(m_shader);
			de->apply_default_matrix_uniforms();
			de->apply_default_tex_uniforms_and_bind_textures();
			de->draw_em();
			de->unbind();
	}	
	
	
}

void Building::draw_state_house_1(){
	shader_ref shader_t = find_shader("alpha-color-shader");
	bind_shader(shader_t);

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
			use_alpha = 1;
		if(i == 0 || i == 1){
			
			vec3f col = get_player_color(m_owner);
			color = vec4f(col.x,col.y,col.z,1);
		}

			

		
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

bool Building::check_for_upgrade_turret(int state){

	using namespace msg::building_state;
	using namespace msg::upgrade_cost;
	
	if(settlement){
		
		if(state == turret_lvl1){
			if(this->state == house_lvl1 && unit_count >= UpgradeToTurretLvl1)
				return true;
			
			if(this->state > house_lvl1 && unit_count >= RebuildingToTurretLvl1)
				return true;
			
		}
	}
	
	else if(turret){
		
		if(this->state >= state){
			cout << "turret has at least this state : " << state << endl;
			return false;
		}		
		
		if(this->state == turret_lvl1 && state == turret_lvl2 && unit_count >= UpgradeToTurretLvl2)
			return true;
	}	
	
	return false;
	
}

bool Building::check_for_upgrade_settlement(int state){

	using namespace msg::building_state;
	using namespace msg::upgrade_cost;	


	if(settlement){
		
		if(this->state >= state){
			cout << "settlement has at least this state : " << state << endl;
			return false;
		}		
		
		if(this->state == construction_site && 	state == house_lvl1 && unit_count >= UpgradeToHouseLvl1)
			return true;

		if(this->state == house_lvl1 		&&	state == house_lvl2 && unit_count >= UpgradeToHouseLvl2)
			return true;

		if(this->state == house_lvl2 		&&	state == house_lvl3 && unit_count >= UpgradeToHouseLvl3)
			return true;
	}
	else if(turret){
		if(state == house_lvl1 && unit_count >= RebuildingToHouseLvl1)
			return true;
	}
		
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
   // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthMask(GL_TRUE);
    vec3f color = get_player_color(m_owner);
    bind_shader(find_shader("selection_circle_shader"));

    matrix4x4f tmp;
    make_unit_matrix4x4f(&tmp);

    tmp = tmp*m_model;



    tmp.col_major[0 * 4 + 0] =  /*m_model.col_major[0 * 4 + 0] **/ (render_settings::tile_size_x+2) * size;
    tmp.col_major[1 * 4 + 1] =  /*m_model.col_major[1 * 4 + 1] * */(render_settings::tile_size_x+2) * size;
    tmp.col_major[2 * 4 + 2] =  /*m_model.col_major[2 * 4 + 2] * */(render_settings::tile_size_y+2) * size;
    tmp.col_major[3 * 4 + 0] =  m_pos.x * render_settings::tile_size_x - ((render_settings::tile_size_x+2) * size )/2.0f;
    tmp.col_major[3 * 4 + 1] =  m_height+0.4;
    tmp.col_major[3 * 4 + 2] =  m_pos.y * render_settings::tile_size_y - ((render_settings::tile_size_y+2) * size )/2.0f;

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
//    glDepthMask(GL_TRUE);
}

void Building::change_owner(unsigned int owner){
    m_owner = owner;
    vec3f color = get_player_color(m_owner);
	label->set_color(color);    

    m_upgradeEffect->ChangeParticleColor(color);
    m_upgradeEffect->Start();
}

//UNITGROUP

UnitGroup::UnitGroup(Obj *obj, simple_heightmap *sh, string name, vec2f start, vec2f end, unsigned int owner, unsigned int unit_count, float time_to_rech_end, float height, unsigned int m_id):
    GameObject(obj,name,find_shader("pos+norm+tc"), height),
    m_owner(owner), m_start(start), m_end(end),
    m_unit_count(unit_count), m_sh(sh), m_id(m_id),
    m_time_to_reach_end(time_to_rech_end), m_spawned(0), m_start_b(start), m_end_b(end)
{
    m_another_timer.restart();
    m_modelmatrices = vector<matrix4x4f>();
    m_cur_heights = vector<float>();
    m_dest_heights = vector<float>();
    m_up_speed = vector<float>();
    m_row_size = vector<unsigned int>();
    m_pos = vec2f(start.x, start.y);
    move = false;
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




void UnitGroup::update_model_matrices(){
//    cout << m_units.size() << endl;
    for(int i = 0; i < m_units.size(); ++i){

        vec2f ortho = vec2f(m_view_dir.y, -m_view_dir.x);
        normalize_vec2f(&ortho);
        normalize_vec2f(&m_view_dir);
        vec2f pos = vec2f(m_pos.x + ortho.x *  m_units[i].m_pos_group.x -  m_view_dir.x *  m_units[i].m_pos_group.y , m_pos.y + ortho.y *  m_units[i].m_pos_group.x -  m_view_dir.y *  m_units[i].m_pos_group.y) ;
        vec2f start = vec2f(m_start.x + ortho.x * (float) i -  m_view_dir.x * (float) m_rows,m_start.y + ortho.y * (float) i -  m_view_dir.y * (float) m_rows) ;
        vec2f end = vec2f(m_end.x + ortho.x * (float) i -  m_view_dir.x * (float) m_rows,m_end.y + ortho.y * (float) i -  m_view_dir.y * (float) m_rows) ;
        float time = m_timer.look();
        float fend = m_sh->get_height(end.x,end.y);
        float fstart = m_sh->get_height(start.x,start.y);

           float height = fstart + time * (fend - fstart)/m_time_to_reach_end;


        m_units[i].update(pos,height);
    }

//    cout << m_modelmatrices.size() << endl;
//    cout << m_cur_heights.size() << endl;
//    unsigned int row_size = 0;
//    for(unsigned int i = 0;  i < m_row_size.size(); ++i){
////       cout << "array size" <<m_row_size.size() << endl;
//        for(int j = 0; j < m_row_size[i]; ++j){
////            cout << "row size "<<row_size << endl;
////            cout << "model size" << m_modelmatrices.size() << endl;
//            vec2f ortho = vec2f(m_view_dir.y, -m_view_dir.x);
//            normalize_vec2f(&ortho);
//            m_modelmatrices.at(row_size ).col_major[3 * 4 + 0] = m_model.col_major[3 * 4 + 0] + get_coooooords(j,m_row_size[i])* ortho.x  - i *m_view_dir.x;
//            m_modelmatrices.at(row_size ).col_major[3 * 4 + 1] = m_center.y + m_cur_heights.at(i);// m_sh->get_height(m_pos.x + cos(2*M_PI * ((float) i/(float)m_spawned)), m_pos.y + sin(2*M_PI * ((float) i/(float)m_spawned)) );
//            m_modelmatrices.at(row_size ).col_major[3 * 4 + 2] = m_model.col_major[3 * 4 + 2] + get_coooooords(j,m_row_size[i])* ortho.y  - i *m_view_dir.y;
//            row_size += 1;
//        }
//    }
}

void UnitGroup::move_to(vec2f pos, float time_to_reach){
  //  force_position(m_end);
   // force_position(m_end);
//	vec2f pos_1 = vec2f(m_model.col_major[3 * 4 + 0], m_model.col_major[3 * 4 + 2]);
//    cout <<"start: "<< m_start.x << ","<< m_start.y << endl;
//    cout <<"end: " <<m_end.x << ","<< m_end.y << endl;
    if(move){
        m_start = m_end;
        m_end = pos;
        m_view_dir = m_end - m_start;
        m_time_to_reach_end = time_to_reach;
        m_timer.restart();

    } else {
        move = true;
        m_start = m_start_b;
        m_end = pos;
        m_view_dir = m_end - m_start;
        m_time_to_reach_end = time_to_reach;
        m_timer.restart();
        for(int i = 0; i < m_units.size(); ++i){
            m_units[i].move = true;
        }
    }

    update_model_matrices();

}

void UnitGroup::update(){
    if(move){
        float cur_time = m_timer.look();

        if(cur_time < m_time_to_reach_end+100){

//            if(m_spawned < m_unit_count && m_spawn_timer.look() > time_to_spawn){
//                spawn_unit_row(std::min((unsigned int) 5,m_unit_count-m_spawned));
//                m_rows++;
//                m_spawn_timer.restart();
//            }

            while(m_spawned < m_unit_count ){
                spawn_unit_row(std::min((unsigned int) 5, m_unit_count-m_spawned));
                m_rows++;
            }
            //        cout << "start: " << m_start.x << "," << m_start.y << endl;
            //        cout << "pos: " << m_pos.x << "," << m_pos.y << endl;
            //        cout << "end: " << m_end.x << "," << m_end.y << endl;
            m_pos.x = m_start.x + cur_time*(m_end.x-m_start.x)/m_time_to_reach_end;
            m_pos.y = m_start.y + cur_time*(m_end.y-m_start.y)/m_time_to_reach_end;
            float x = (float) m_start.x + cur_time*((float)m_end.x-(float)m_start.x)/m_time_to_reach_end;
            float y = (float) m_start.y + cur_time*((float)m_end.y-(float)m_start.y)/m_time_to_reach_end;

            //        cout << x << " " << y << endl;
            m_model.col_major[3 * 4 + 0] = x * render_settings::tile_size_x;
            m_model.col_major[3 * 4 + 1] = m_center.y + m_sh->get_height(x, y);
            m_model.col_major[3 * 4 + 2] = y * render_settings::tile_size_y;

            update_model_matrices();
        }
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

void UnitGroup::draw_mesh(){
    for(int i =0; i < m_spawned; ++i){
        bind_shader(m_obj->shader);

        int loc = glGetUniformLocation(gl_shader_object(m_obj->shader), "proj");
        glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

        loc = glGetUniformLocation(gl_shader_object(m_obj->shader), "view");
        glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);

        loc = glGetUniformLocation(gl_shader_object(m_obj->shader), "model");
        glUniformMatrix4fv(loc, 1, GL_FALSE, m_units[i].getModel()->col_major);

        loc = glGetUniformLocation(gl_shader_object(m_obj->shader), "light_dir");
        glUniform3f(loc, 0.7, 1.2,0.3);
        loc = glGetUniformLocation(gl_shader_object(m_obj->shader), "light_col");
        glUniform3f(loc, 1,1,1);
        loc = glGetUniformLocation(gl_shader_object(m_obj->shader), "time");
        glUniform1f(loc, m_another_timer.look());
        vec3f cam_pos;
        extract_pos_vec3f_of_matrix(&cam_pos, lookat_matrix_of_cam(current_camera()));
        loc = glGetUniformLocation(gl_shader_object(m_obj->shader), "eye_pos");
        glUniform3f(loc, cam_pos.x, cam_pos.y, cam_pos.z);


//        bind_texture(m_obj->tex, 0);
//        loc = glGetUniformLocation(gl_shader_object(m_obj->shader), "diffuse_tex");
//        glUniform1i(loc, 0);

        bind_mesh_to_gl(m_obj->mesh);

        draw_mesh_as(m_obj->mesh,GL_TRIANGLES);
        unbind_mesh_from_gl(m_obj->mesh);
        unbind_shader(m_obj->shader);

    }
}

void UnitGroup::draw_drawelement(){

    for(int i = 0; i < m_spawned; ++i){

		for (vector<drawelement*>::iterator it = m_obj->drawelements->begin(); it != m_obj->drawelements->end(); ++it) {
			drawelement *de = *it;

            de->Modelmatrix(m_units[i].getModel());
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

    int new_size = size;

    for(int i = -(new_size/2); i <= new_size/2; ++i){

        m_view_dir = m_end - m_start;
     //   if(length_of_vec2f(&m_view_dir) < 0.0001) break;
        vec2f ortho = vec2f(m_view_dir.y, -m_view_dir.x);

        normalize_vec2f(&ortho);
        normalize_vec2f(&m_view_dir);
        vec2f pos = vec2f(m_pos.x + ortho.x *  i -  m_view_dir.x *  m_rows , m_pos.y + ortho.y *  i -  m_view_dir.y *  m_rows) ;
        vec2f start = vec2f(m_start.x + ortho.x * (float) i -  m_view_dir.x * (float) m_rows,m_start.y + ortho.y * (float) i -  m_view_dir.y * (float) m_rows) ;
        vec2f end = vec2f(m_end.x + ortho.x * (float) i -  m_view_dir.x * (float) m_rows,m_end.y + ortho.y * (float) i -  m_view_dir.y * (float) m_rows) ;


        m_units.push_back(Unit(pos
                              ,m_view_dir
                               , vec2f(i,m_rows)
                               ,start
                               ,end
                               ,m_sh
                              , m_center.y));
        m_spawned++;

    }
}


// UNIT

Unit::Unit(vec2f pos, vec2f view_dir, vec2f pos_group, vec2f start, vec2f end, simple_heightmap *sh, float base_height): m_pos(pos), m_view_dir(view_dir), m_pos_group(pos_group), m_start(start), m_end(end), m_sh(sh), m_base_height(base_height){
    make_unit_matrix4x4f(&m_model);
    m_model.col_major[3 * 4 + 1] = base_height + sh->get_height(pos.x, pos.y);
    movement_timer.restart();
    move = true;
}

matrix4x4f* Unit::getModel(){
    return &m_model;
}

void Unit::update(vec2f new_pos, float height){

    if(move){
//        cout << "m_pos " << m_pos.x << " " << m_pos.y << endl;
//        cout << "m_start " << m_start.x << " " << m_start.y << endl;
//        cout << "m_end " << m_end.x << " " << m_end.y << endl;
//        cout << "newpos " << new_pos.x << " " << new_pos.y << endl;
    m_start = m_pos;
    m_end = new_pos;
    vec2f tmp = m_end - m_pos;
    if(length_of_vec2f(&tmp) > 0.1) {
        m_view_dir =  tmp;
    }

//    cout << length_of_vec2f(&m_view_dir) << endl;
    m_speed = BASE_SPEED + length_of_vec2f(&m_view_dir)/(1*render_settings::tile_size_x);
    normalize_vec2f(&m_view_dir);
    m_pos +=  m_view_dir * m_speed * 0.05 ;
    vec2f dis = m_end - m_pos;
    if(length_of_vec2f(&dis) < 0.001){
        m_pos = m_end;
    }


    m_dest_height  = m_sh->get_height((m_pos + m_view_dir*2).x, (m_pos + m_view_dir*2).y);
    m_cur_height = m_model.col_major[3 * 4 + 1];

    m_up_speed = ( m_dest_height - m_cur_height ) /5;

    m_model.col_major[3 * 4 + 0] = m_pos.x * render_settings::tile_size_x;
    m_model.col_major[3 * 4 + 1] = m_base_height + m_sh->get_height(m_pos.x,m_pos.y);
    m_model.col_major[3 * 4 + 2] = m_pos.y * render_settings::tile_size_y;
    }
//    movement_timer.restart();

}


Pacman::Pacman(Obj *obj, unsigned int owner, unsigned m_id, int height, float time) :  GameObject(obj, obj->name ,obj->shader, height), owner(owner), m_id(m_id), time(time)  {
	
}

void Pacman::draw(){
	time += 0.007;
	if(time == std::numeric_limits<float>::max()-2)
		time = 0;
	
	
}

