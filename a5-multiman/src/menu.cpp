#define NUM_PLAYERS nums[4]
#define COLOR nums[3]
#define LEVEL nums[2]
#define FRAKTION nums[1]

#define COLOR_ID 3
#define FRAKTION_ID 1

#include "menu.h"
#include "mouseactions.h"

#include <GL/glew.h>
#include <GL/freeglut.h>


void Menu::init(bool *render_menu){
	this->render_menu = render_menu;
	enter = false;

	vec3f cam_pos = {0,0,0}, cam_dir = {0,0,-1}, cam_up = {0,1,0};
	float fovy = 50;
	gamecam = make_orthographic_cam((char*)"gui cam", &cam_pos, &cam_dir, &cam_up, fovy, 0, 50, 0, 0.01, 1000);
	

	gamemesh = make_mesh("quad", 2);
	vec3f pos[4] = { {0,0,-10}, {1,0,-10}, {1,1,-10}, {0,1,-10} };
	vec2f tc[4] = { {0,1}, {1,1}, {1,0}, {0,0} };
	unsigned int idx[6] = { 0, 1, 2, 2, 3, 0 };
	
	bind_mesh_to_gl(gamemesh);
	add_vertex_buffer_to_mesh(gamemesh, "in_pos", GL_FLOAT, 4, 3, (float *) pos, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(gamemesh, "in_tc", GL_FLOAT, 4, 2, (float *) tc, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(gamemesh, 6, idx, GL_STATIC_DRAW);
	unbind_mesh_from_gl(gamemesh);

	menushader = find_shader("simple-menu-shader");
	
	game_loose = find_texture("game_loose");
	game_won = find_texture("game_won");
	game_paused = find_texture("game_paused");
	game_start = find_texture("game_start");
	black_screen = find_texture("black_screen");


	make_unit_matrix4x4f(&model);
	model.row_col(0,0) = 50.f;
	model.row_col(1,1) = 50.f;
	model.row_col(0,3) = 0.f;
	model.row_col(1,3) = 0.f;	
	
		
	for(int i = 0; i < max_rows; i++){
		labels.push_back(new Label(9, 13, "text-shader"));
		if(i!=0){
			labels[i]->set_size(normal_size);	
		
		} else
			labels[i]->set_size(choosen_size);	
		
		labels[i]->set_camera(gamecam);
		labels[i]->setup_display();
//		labels[i]->change_mesh();
		std::stringstream stream;
		stream << strings[i];
	//	labels[i]->update_label_pos(fovy*0.6, -1000, fovy - (fovy/(max_rows+3)*(i+1)+7));	
		labels[i]->update_label_pos(fovy*0.578, -1000, fovy - (fovy/(max_rows+8)*(i+1)+8));	
		row = i;
		
		if(i == 0)
			update_label(true);
		else 
			update_label(false);
	}
	row = 0;
}


void Menu::draw(bool blend){
	
	draw_background(blend);
	draw_font();
}

void Menu::draw_font(){

	if(mode == 0){
		for(int i = 0; i < max_rows; i++)	
			labels[i]->render_gui_overlay();
	}
	
}




void Menu::draw_background(bool blend){
	
		camera_ref old_cam = current_camera();
		use_camera(gamecam);
		bind_shader(menushader);

		int loc = glGetUniformLocation(gl_shader_object(menushader), "proj");
		glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

		loc = glGetUniformLocation(gl_shader_object(menushader), "view");
		glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);


		if(blend){
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
		}
		
		glDepthMask(GL_FALSE);
		
		texture_ref tex;
		
		switch (mode){
			case GAMESTART : tex = game_start; break;
			case GAMEPAUSED : tex = game_paused; break;
			case GAMELOOSE : tex = game_loose; break;
			case GAMEWON 	: tex = game_won; break;
			default : tex = black_screen; 
		}

		loc = glGetUniformLocation(gl_shader_object(menushader), "model");
		glUniformMatrix4fv(loc, 1, GL_FALSE, model.col_major);


		vec3f color = vec3f(-1,-1,-1);
		loc = glGetUniformLocation(gl_shader_object(menushader), "color");
		glUniform3fv(loc, 1,(float *)&color);		


		loc = glGetUniformLocation(gl_shader_object(menushader), "depth");
		glUniform1f(loc,depth);


		bind_texture(tex, 0);
		loc = glGetUniformLocation(gl_shader_object(menushader), "tex");
		glUniform1i(loc, 0);

		bind_mesh_to_gl(gamemesh);
		draw_mesh(gamemesh);
		unbind_mesh_from_gl(gamemesh);

		glDepthMask(GL_TRUE);
		unbind_shader(menushader);
		glDisable(GL_BLEND);
		use_camera(old_cam);
}

int Menu::get_row(){
	return row;
}

void Menu::increase_row(){
	if(row > 0){
		labels[row]->set_size(normal_size);
		update_label(false);
		row--;
		labels[row]->set_size(choosen_size);
		update_label(true);
	}
}

void Menu::decrease_row(){
	if(row < max_rows-1){
			
		labels[row]->set_size(normal_size);	
		update_label(false);
		row++;
		labels[row]->set_size(choosen_size);
		update_label(true);
	}
}

void Menu::increase_mom_row(){
	if(nums[row] == -1 || nums[row] == max_nums[row])
		return;
		
	nums[row]++;
	update_label(true);
	
}

void Menu::decrease_mom_row(){
	if(nums[row] == -1 || nums[row] == min_nums[row])
		return;
	
	nums[row]--;
	update_label(true);
	
}

void Menu::update_label(bool choosen){
	if(!enter){
		if(choosen && (row != COLOR_ID || nums[row] == 1))
			labels[row]->set_color(white);
		else if (!choosen && (row != COLOR_ID || nums[row] == 1)) 
			labels[row]->set_color(grey);
		
		std::stringstream s;
		if(nums[row] != min_nums[row] && nums[row] != -1)
			s << "< ";
		else
			s << "    ";
		if(row == COLOR_ID){
				
 			s << color_names[nums[row]-1];	
			if(nums[row] != 1)
				labels[row]->set_color(player_colors[nums[row]-1]);
	
		} else if(row == FRAKTION_ID) {
			
			if(nums[row] == 1)
				s << "Pacman";	
			else
				s << "Bomberman";
		
		} else {
		
			s << strings[row];	
			if(nums[row] != -1)
				s << " : " << nums[row];
				
		}
		
		if(nums[row] != max_nums[row] && nums[row] != -1)	
			s << " >";
		
		labels[row]->update_gui_texture_string(&s); 
	}
}

void Menu::set_hostname(char * hostName){
	std::stringstream hostname;
	hostname << "    " << hostName;
	labels[max_rows-1]->update_gui_texture_string(&hostname);
}


int Menu::get_num_players(){
	return NUM_PLAYERS;
}
const char *Menu::get_level(){
	return level_names[(int)LEVEL-1];
}


vec3f Menu::get_player_color(){

	if(COLOR == 1)
		return vec3f(-1,-1,-1);
		
	return player_colors[COLOR-1];
}

void Menu::set_mode(Mode mode){ 
	this->mode = mode;
	if(mode == GAMESTART)
		row = 0;
	else 
		row = -1;
}

void Menu::reset_menu(){
	enter = false;
	row = 0;
}
/*
//label must be fully initialized, inclusive camera and shader
MenuEntry::MenuEntry(int id, vector<const char*> entry_names, Label *label ) : id(id), label(label), entry_names(entry_names){
	reset();
}
	
	
void MenuEntry::increase_row(){
	if(pos >= max_pos -1)
		return;
		
	pos++;
	update_label();
	
}
void MenuEntry::decrease_row(){
	if(pos <= 0)
		return;
		
	pos--;
	update_label();
	
}
void MenuEntry::update_label(){
	
	if(max_pos == 0)
		return;
	
	std::stringstream s;
	if(pos != 0)
		s << "< ";
	else
		s << "    ";	
	
	s << entry_names[pos];
	
	if(pos < max_pos -1)	
		s << " >";
	
	if(choosen){
		label->set_color(choosen_color); 
		if(use_color && pos != 0)
			label->set_color(colors[pos]); 
		
		label->set_size(choosen_size);
	}
	else{
		if(!use_color || pos == 0)
			label->set_color(normal_color);
		label->set_size(normal_size);	
	}
	label->update_gui_texture_string(&s); 
}
void MenuEntry::choose(bool s){
	choosen = s;
	update_label();
}
void MenuEntry::draw(){
	label->render_gui_overlay();
}
void MenuEntry::reset(){
		
	pos = 0;
	max_pos = entry_names.size();
	choosen = false;
	choosen_color = vec3f(0.7,0.7,0.7);
	normal_color = vec3f(1.,1.,1.);
	choosen_size = vec2f(20,3);
	normal_size = vec2f(19,2);	
	use_color = false;
	update_label();	
}


void MenuEntry::set_colors(vector<vec3f> colors){
	if(colors.size() != max_pos)
		return;
	
	use_color = true;
	this->colors = colors;
	
}




template < typename T > 
void EntryManager::add_entry(std::vector<const char*> entry_names, vector< T > entries, vector<vec3f> colors, Label *label, int id){
	
	MenuEntry *entry;
	
//	if(entries.size() != 0)
//		entry = new ChooseEntry(id,entry_names, label, entries);
//	else
//		entry = new MenuEntry(id,entry_names, label);
		
//	entry->set_colors(colors);
//	entry_list.push_back(entry);
}


MenuEntry *EntryManager::next_entry(){
	if(pos < entry_list.size()-1){
		entry_list[pos]->choose(false);
		pos++;
		entry_list[pos]->choose(true);
	}
	
	
	return entry_list[pos];
}
MenuEntry *EntryManager::previous_entry(){
	if(pos > 0){
		entry_list[pos]->choose(false);
		pos--;
		entry_list[pos]->choose(true);
	}
	return entry_list[pos];	
	
}	

*/



IconBar::IconBar(){
	
	vec3f cam_pos = {0,0,0}, cam_dir = {0,0,-1}, cam_up = {0,1,0};
	cam = make_orthographic_cam((char*)"gui cam", &cam_pos, &cam_dir, &cam_up, fovy, 0, 50, 0, near, far);	
	
	background = 					find_texture("iconbar");
	button		 = 					find_texture("button");
	fraction[0] = 					find_texture("pacman");
	fraction[1] = 					find_texture("terrain_hm");
	upgrade_button_turret[0] = 		find_texture("u_b_t1");
	upgrade_button_turret[1] = 		find_texture("u_b_t2");
	noupgrade_button_turret[0] = 	find_texture("nu_b_t1");
	noupgrade_button_turret[1] = 	find_texture("nu_b_t2");
	noupgrade_button_turret[2] = 	find_texture("terrain_hm");
	upgrade_button_settlement[0] = 	find_texture("u_b_s2");
	upgrade_button_settlement[1] = 	find_texture("u_b_s3");
	noupgrade_button_settlement[0] =find_texture("nu_b_s2");
	noupgrade_button_settlement[1] =find_texture("nu_b_s3");
	noupgrade_button_settlement[2] =find_texture("terrain_hm");
	picture[0] =					find_texture("terrain_hm");
	picture[1] =					find_texture("dorf");
	
	shader = find_shader("simple-menu-shader");
	
	init_modelmatrices();

	mesh = make_mesh("quad", 2);
	vec3f pos[4] = { {0,0,-10}, {1,0,-10}, {1,1,-10}, {0,1,-10} };	
	vec2f tc[4] = { {0,1}, {1,1}, {1,0}, {0,0} };
	unsigned int idx[6] = { 0, 1, 2, 2, 3, 0 };	
	
	
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, 4, 3, (float *) pos, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, 4, 2, (float *) tc, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(mesh, 6, idx, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);	
	
//	label = new Label();	
//	label->set_nChars(13);	
	label = new Label(9, 13, "text-shader");
	label->setup_display();
	label->set_camera(cam);
	label->set_size(vec2f(11,1));
	label->set_color(vec3f(0.678, 0.956, 0.928));	
	label->update_label_pos(model_button_u.row_col(0,3) + 13*offset_button_y, -1000, model_button_u.row_col(1,3)+3*offset_button_y);
}


void IconBar::draw(){
	

	camera_ref old_cam = current_camera();
	use_camera(cam);
	bind_shader(shader);

	loc = glGetUniformLocation(gl_shader_object(shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

	loc = glGetUniformLocation(gl_shader_object(shader), "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);
	
	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, model_background.col_major);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_TRUE);


	vec3f color = vec3f(0.1,0.1,0.1);
	loc = glGetUniformLocation(gl_shader_object(shader), "color");
	glUniform3fv(loc, 1,(float *)&color);		


	bind_texture(background, 0);
	loc = glGetUniformLocation(gl_shader_object(shader), "tex");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(gl_shader_object(shader), "depth");
	glUniform1f(loc,depth_background);


	bind_mesh_to_gl(mesh);
	draw_mesh(mesh);
	
	unbind_texture(background);
	glDisable(GL_BLEND);

	//buttons etc

	color = vec3f(-1,-1,-1);
	loc = glGetUniformLocation(gl_shader_object(shader), "color");
	glUniform3fv(loc, 1,(float *)&color);	
	
	loc = glGetUniformLocation(gl_shader_object(shader), "depth");
	glUniform1f(loc,depth_button);	

	
	draw_fraction();
	if(building_selected){
		draw_picture();	
		draw_buttons();
		label->render_gui_overlay();
	}


	unbind_mesh_from_gl(mesh);
	
	
	unbind_shader(shader);
	use_camera(old_cam);
}

void IconBar::draw_fraction(){
	
	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, model_fraction.col_major);	
	
	bind_texture(fraction[frac], 0);
	loc = glGetUniformLocation(gl_shader_object(shader), "tex");
	glUniform1i(loc, 0);
	
	draw_mesh(mesh);	
	
	unbind_texture(fraction[frac]);

}

void IconBar::draw_buttons(){

	//label_button
	
	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, model_button_u.col_major);	


	bind_texture(button, 0);
	loc = glGetUniformLocation(gl_shader_object(shader), "tex");	
	glUniform1i(loc, 0);	
	
	draw_mesh(mesh);	

	unbind_texture(button);

	

	//settlement_button

	loc = glGetUniformLocation(gl_shader_object(shader), "depth");
	glUniform1f(loc,depth_button_s);	

	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, model_button_s.col_major);	

	if(s_upgradeable)
		bind_texture(upgrade_button_settlement[s_level], 0);
	else
		bind_texture(noupgrade_button_settlement[s_level], 0);

	loc = glGetUniformLocation(gl_shader_object(shader), "tex");	
	glUniform1i(loc, 0);	
	
	draw_mesh(mesh);	
	
	if(s_upgradeable)
		unbind_texture(upgrade_button_settlement[s_level]);
	else
		unbind_texture(noupgrade_button_settlement[s_level]);
	
	//turret_button	

	loc = glGetUniformLocation(gl_shader_object(shader), "depth");
	glUniform1f(loc,depth_button_t);	
		
	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, model_button_t.col_major);	

	if(t_upgradeable)
		bind_texture(upgrade_button_turret[t_level], 0);
	else
		bind_texture(noupgrade_button_turret[t_level], 0);

	loc = glGetUniformLocation(gl_shader_object(shader), "tex");
	glUniform1i(loc, 0);
	
	draw_mesh(mesh);	
	
	if(s_upgradeable)
		unbind_texture(upgrade_button_turret[t_level]);
	else
		unbind_texture(noupgrade_button_turret[t_level]);		
		
}

void IconBar::draw_picture(){

	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, model_picture.col_major);	

	bind_texture(picture[pic], 0);
	loc = glGetUniformLocation(gl_shader_object(shader), "tex");	
	glUniform1i(loc, 0);	
	
	draw_mesh(mesh);	
	
	unbind_texture(picture[pic]);
}

void IconBar::init_modelmatrices(){
	

	models[0] = &model_background;
	models[1] = &model_button_t;
	models[2] = &model_button_s;
	models[3] = &model_button_u;
	models[4] = &model_picture;
	models[5] = &model_fraction;

	for(int i = 0; i < 6; i++){
		make_unit_matrix4x4f(models[i]);
		models[i]->row_col(1,1) = scale_button_y;
		models[i]->row_col(1,3) = offset_button_y;
	}

	
	//BACKGROUND
	model_background.row_col(0,0) = fovy;
	model_background.row_col(1,1) = 0.77 * fovy;
	model_background.row_col(0,3) = 0.f;
	model_background.row_col(1,3) = 0.f;
	

	
	float x_offset = offset_button_y;
	
	//FRACTION
	model_fraction.row_col(0,0) = 0.05 * fovy;
	model_fraction.row_col(0,3) = x_offset;

	x_offset += model_fraction.row_col(0,0) + 2*offset_button_y;
	
	//BUTTONS
	model_button_s.row_col(0,0) = 0.12 * fovy;
	model_button_s.row_col(0,3) = x_offset;
	

	x_offset += model_button_s.row_col(0,0) + offset_button_y;

	model_button_t.row_col(0,0) = 0.12 * fovy;
	model_button_t.row_col(0,3) = x_offset;
	
	x_offset += model_button_t.row_col(0,0) + offset_button_y;
	
	model_button_u.row_col(0,0) = 0.12 * fovy;
	model_button_u.row_col(0,3) = x_offset;	
	
	//PICTURE
	
	model_picture.row_col(1,1) = 0.12 * fovy;
	model_picture.row_col(0,0) = 0.12 * fovy;
	model_picture.row_col(0,3) = 0.85 * fovy;
	model_picture.row_col(1,3) = 0.02 * fovy;
	
		
}

int IconBar::click(int x, int y, vec3f (*ptr)(int x, int y)){
	
	camera_ref old_camera = current_camera();
	use_camera(cam);
	
	vec3f pos = ptr(x,y);

	float rel_depth=(pos.z-near)/(far-near);

	if(-rel_depth >= (depth_button_s - depth_acc_button_s) && -rel_depth <= (depth_button_s + depth_acc_button_s)){
		return 0;
		cout << "click_settlement" << endl;
		
	}
	else if(-rel_depth >= (depth_button_t - depth_acc_button_t) && -rel_depth <= (depth_button_t + depth_acc_button_t)){
		return 1;
		cout << "click_turret" << endl;
	}
	use_camera(old_camera);	

	return -1;
	
}

void IconBar::scale_button(int b, bool greater){

	int i = -1;
	if(greater){
		if(button_pressed < 0 || button_pressed > 1)
			return;
		b = button_pressed;
		i = 1;
	}
	
	float scale = 0.008 * fovy * i;
	float offset = -0.5*scale;

	
	switch(b){
		//settlement
		case 0 : 
				if(!s_upgradeable) return;
		
				button_pressed = 0; 
				model_button_s.row_col(0,0) += scale;
				model_button_s.row_col(1,1) += scale;
				model_button_s.row_col(0,3) += offset;
				model_button_s.row_col(1,3) += offset;
				break;
		//turret
		case 1 : 
				if(!t_upgradeable) return;
				
				button_pressed = 1;
				model_button_t.row_col(0,0) += scale;
				model_button_t.row_col(1,1) += scale;
				model_button_t.row_col(0,3) += offset;
				model_button_t.row_col(1,3) += offset;
				break;
		
		default : return;
	}
	
	if(greater)
		button_pressed = -1;
	
}


void IconBar::update(){
	
		if(building == nullptr)
			return;
		
		int type  = building->get_type();
		int level = building->get_level();
		int state = building->get_state();
		int unit_count = building->get_unit_count();
		
		std::stringstream s;
		s << "units : " << unit_count;
		label->update_gui_texture_string(&s);
		
		using namespace msg::building_state;
		
		//settlement
		if(type == 0){
			s_upgradeable	= building->check_for_upgrade_settlement(state+1);
			s_level		 	= level;
			t_level 		= 0;
			t_upgradeable 	= building->check_for_upgrade_turret(turret_lvl1);
		} 
		//turret
		else if (type == 1){
			t_upgradeable 	= building->check_for_upgrade_turret(state+1);
			t_level 		= level;
			s_level 		= 0;
			s_upgradeable 	= building->check_for_upgrade_settlement(house_lvl1);
		}
		else {
			cout << "menu : unknown type delivered" << endl;
			t_upgradeable = false;
			s_upgradeable = false;
		}
		
}

void IconBar::selected_building(Building *building){
	this->building = building;
	if(building == nullptr)
		building_selected = false;
	else
		building_selected = true;
	
}
