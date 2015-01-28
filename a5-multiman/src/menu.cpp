#define NUM_PLAYERS nums[4]
#define COLOR nums[3]
#define LEVEL nums[2]
#define FRAKTION nums[1]

#define COLOR_ID 3
#define FRAKTION_ID 1

#include "menu.h"
#include "game.h"
#include "rendering.h"
#include "mouseactions.h"

#include <GL/glew.h>
#include <GL/freeglut.h>


void Menu::init(Game *game, bool *render_menu){
    m_game = game;

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
		std::stringstream stream;
		stream << strings[i];
		labels[i]->update_label_pos(fovy*0.61, -1000, fovy - (fovy/(max_rows+8)*(i+1)+8));		
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
                labels[row]->set_color(m_game->player_colors[nums[row]-1]);
	
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
unsigned int Menu::get_level(){
    return LEVEL;
}

int Menu::get_frac(){
		
	return FRAKTION;
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



IconBar::IconBar(Game *game, int fraction) : m_game(game) {
	this->fraction = fraction;
	
	vec3f cam_pos = {0,0,0}, cam_dir = {0,0,-1}, cam_up = {0,1,0};
	cam = make_orthographic_cam((char*)"gui cam", &cam_pos, &cam_dir, &cam_up, fovy, 0, 50, 0, near, far);	
	
    player_color = m_game->get_player_color(PLAYER_ID);
	
	background =					find_texture("interface_pm");
	background_menu =				find_texture("interface_pm_menu");
	if(fraction == 2){
		background =				find_texture("interface_bbm");
		background_menu =			find_texture("interface_bbm_menu");
	}
	shader = find_shader("simple-menu-shader");
	
	init_modelmatrices();
	init_buttons();

	mesh = make_mesh("quad", 2);
	vec3f pos[4] = { {0,0,-10}, {1,0,-10}, {1,1,-10}, {0,1,-10} };	
	vec2f tc[4] = { {0,1}, {1,1}, {1,0}, {0,0} };
	unsigned int idx[6] = { 0, 1, 2, 2, 3, 0 };	
	
	
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, 4, 3, (float *) pos, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, 4, 2, (float *) tc, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(mesh, 6, idx, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);	

	label_1 = new Label(9, 3, "text-shader");
	label_2 = new Label(9, 3, "text-shader");
	label_1->setup_display();
	label_2->setup_display();
	label_1->set_camera(cam);
	label_2->set_camera(cam);
	label_1->set_size(vec2f(4,2));
	label_2->set_size(vec2f(4,2));
	label_1->set_color(vec3f(0.678, 0.956, 0.928));	
	label_2->set_color(vec3f(0.678, 0.956, 0.928));	
	label_1->update_label_pos(buttons[2].model.row_col(0,3) + 10.5*offset_button_y, -1000, buttons[2].model.row_col(1,3)+2.4*offset_button_y);
	label_2->update_label_pos(buttons[3].model.row_col(0,3) + 10.5*offset_button_y, -1000, buttons[3].model.row_col(1,3)+2.4*offset_button_y);
}


void IconBar::draw(){
	

    player_color = m_game->get_player_color(PLAYER_ID);
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
//	glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);


	vec3f color = vec3f(0.8,0.2,0.8);
	loc = glGetUniformLocation(gl_shader_object(shader), "color");
	glUniform3fv(loc, 1,(float *)&color);		

	loc = glGetUniformLocation(gl_shader_object(shader), "p_color");
	glUniform3fv(loc, 1,(float *)&player_color);	

	if(!open_menu)
		bind_texture(background, 0);
	else
		bind_texture(background_menu, 0);
	
	loc = glGetUniformLocation(gl_shader_object(shader), "tex");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(gl_shader_object(shader), "depth");
	glUniform1f(loc,depth_background);


	bind_mesh_to_gl(mesh);
	draw_mesh(mesh);
	
	if(!open_menu)
		unbind_texture(background);
	else
		unbind_texture(background_menu);
	
	
	//buttons etc
	
	loc = glGetUniformLocation(gl_shader_object(shader), "depth");
	glUniform1f(loc,depth_button);	

	glDisable(GL_BLEND);

	color = vec3f(-1,-1,-1);
	loc = glGetUniformLocation(gl_shader_object(shader), "color");
	glUniform3fv(loc, 1,(float *)&color);	
	
	
	draw_buttons_2();
		
	
	if(building_selected){
	
		label_1->render_gui_overlay();
		label_2->render_gui_overlay();
	}

	unbind_mesh_from_gl(mesh);
	
	unbind_shader(shader);

	
	//if(building_selected)
	//	draw_building();
		
	use_camera(old_cam);	
	
}

void IconBar::draw_building(){
//	glDepthMask(GL_FALSE);
	camera_ref old_cam = current_camera();
	vec3f cam_pos = {0,0,0}, cam_dir = {0,0,-1}, cam_up = {0,1,0};
	camera_ref buildingcam = make_perspective_cam((char*)"a cam", &cam_pos, &cam_dir, &cam_up,  45, 1, 1, 100);

	use_camera(buildingcam);

	vec3f light_dir = { 1.f, -0.6f, -0.4f };
	vec3f pos, right, up;
	extract_dir_vec3f_of_matrix(&light_dir, lookat_matrix_of_cam(cam));
	extract_pos_vec3f_of_matrix(&pos, lookat_matrix_of_cam(cam));
	extract_right_vec3f_of_matrix(&pos, lookat_matrix_of_cam(cam));
	extract_up_vec3f_of_matrix(&pos, lookat_matrix_of_cam(cam));
	normalize_vec3f(&light_dir);

	matrix4x4f scale ;		
	vec3f s = vec3f(0.5,0.5, 0.5);
	make_scale_matrix4x4f(&scale, &s);

	vec3f b_pos = pos + (light_dir*2);

	shader_ref shader = find_shader("alpha-color-shader");

	bind_shader(shader);

	static float rotation = 0;
	rotation += 0.007;
	if(rotation == std::numeric_limits<float>::max()-2)
		rotation = 0;


	matrix4x4f model = building->get_model();
//	model.row_col(0,3) = 0.9 * fovy;
	model.row_col(0,3) = b_pos.x + 2;
//	model.row_col(1,3) = 0.2 * fovy;
	model.row_col(1,3) = b_pos.y - 2;
//	model.row_col(2,3) = 0.1;
	model.row_col(2,3) = b_pos.z;
	model.row_col(0,0) = 2;
	model.row_col(1,1) = 2;

	matrix4x4f rot;
	
	vec3f rot_vec = vec3f(0,1,0);
	make_rotation_matrix4x4f(&rot,&rot_vec, rotation);
//	model =  model * rot;

	
	for (vector<drawelement*>::iterator it = building->get_obj()->drawelements->begin(); it != building->get_obj()->drawelements->end(); ++it) {
		drawelement *de = *it;

		de->Modelmatrix(&model);
		
		int loc = glGetUniformLocation(gl_shader_object(shader), "light_dir");
		glUniform3f(loc, light_dir.x, light_dir.y, light_dir.z);
		
		loc = glGetUniformLocation(gl_shader_object(shader), "light_col");
		glUniform3f(loc, 0.6f, 0.7f, 0.8f);		
		
		
		de->apply_default_matrix_uniforms(shader);

		loc = glGetUniformLocation(gl_shader_object(shader), "model");
		glUniformMatrix4fv(loc, 1, GL_FALSE, model.col_major);	
		
			

		de->apply_default_tex_uniforms_and_bind_textures(shader);
		
		de->draw_em();
	}	

	unbind_shader(shader);
//	use_camera(old_cam);

	glDepthMask(GL_TRUE);

}

void IconBar::draw_buttons_2(){
	
	//menu_button

	loc = glGetUniformLocation(gl_shader_object(shader), "depth");
	glUniform1f(loc,depth_button_s);	

	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, buttons[4].model.col_major);	

	bind_texture(buttons[4].textures[buttons[4].state], 0);	

	loc = glGetUniformLocation(gl_shader_object(shader), "tex");	
	glUniform1i(loc, 0);	
	
	draw_mesh(mesh);	
	
	unbind_texture(buttons[4].textures[buttons[4].state]);	
	
	if(!building_selected) return;

	//unit_count
	
	vec3f color = vec3f(-1,-1,-1);
	loc = glGetUniformLocation(gl_shader_object(shader), "color");
	glUniform3fv(loc, 1,(float *)&color);			
	
	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, buttons[2].model.col_major);	
	
	bind_texture(buttons[2].textures[buttons[2].state], 0);
	loc = glGetUniformLocation(gl_shader_object(shader), "tex");
	glUniform1i(loc, 0);

	draw_mesh(mesh);

	unbind_texture(buttons[2].textures[buttons[2].state]);
	
	//defence/production
	
	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, buttons[3].model.col_major);	
	
	bind_texture(buttons[3].textures[buttons[3].state], 0);
	loc = glGetUniformLocation(gl_shader_object(shader), "tex");
	glUniform1i(loc, 0);

	draw_mesh(mesh);

	unbind_texture(buttons[3].textures[buttons[3].state]);
	
	//settlement_button

	loc = glGetUniformLocation(gl_shader_object(shader), "depth");
	glUniform1f(loc,depth_button_s);	

	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, buttons[0].model.col_major);	

	bind_texture(buttons[0].textures[buttons[0].state], 0);	

	loc = glGetUniformLocation(gl_shader_object(shader), "tex");	
	glUniform1i(loc, 0);	

	draw_mesh(mesh);	
	
	unbind_texture(buttons[0].textures[buttons[0].state]);
	
	//turret_button	

	loc = glGetUniformLocation(gl_shader_object(shader), "depth");
	glUniform1f(loc,depth_button_t);	
		
	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, buttons[1].model.col_major);	


	bind_texture(buttons[1].textures[buttons[1].state], 0);
	
	loc = glGetUniformLocation(gl_shader_object(shader), "tex");
	glUniform1i(loc, 0);

	
	draw_mesh(mesh);	
	
	unbind_texture(buttons[1].textures[buttons[1].state]);	
		
	
}
//defect
void IconBar::draw_buttons(){
	
	vec3f color = vec3f(-1,-1,-1);
	loc = glGetUniformLocation(gl_shader_object(shader), "color");
	glUniform3fv(loc, 1,(float *)&color);		

	//unit_button
	
	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, buttons[3].model.col_major);	


	bind_texture(buttons[3].textures[buttons[3].state],0);
	loc = glGetUniformLocation(gl_shader_object(shader), "tex");	
	glUniform1i(loc, 0);	
	
	draw_mesh(mesh);	

	unbind_texture(buttons[3].textures[buttons[3].state]);

	

	//settlement_button

	loc = glGetUniformLocation(gl_shader_object(shader), "depth");
	glUniform1f(loc,depth_button_s);	

	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, buttons[1].model.col_major);	

	bind_texture(buttons[1].textures[buttons[1].state], 0);

	loc = glGetUniformLocation(gl_shader_object(shader), "tex");	
	glUniform1i(loc, 0);	
	
	draw_mesh(mesh);	
	
	unbind_texture(buttons[1].textures[buttons[1].state]);
	
	//turret_button	

	loc = glGetUniformLocation(gl_shader_object(shader), "depth");
	glUniform1f(loc,depth_button_t);	
		
	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, buttons[2].model.col_major);	


	bind_texture(buttons[2].textures[buttons[2].state], 0);
	
	loc = glGetUniformLocation(gl_shader_object(shader), "tex");
	glUniform1i(loc, 0);
	
	draw_mesh(mesh);	
	
	unbind_texture(buttons[2].textures[buttons[2].state]);	
	
	buttons[4].draw(shader, mesh);
		
}

void IconBar::init_modelmatrices(){
	
	//BACKGROUND
	make_unit_matrix4x4f(&model_background);
	model_background.row_col(0,0) = fovy;
	model_background.row_col(1,1) = fovy;
	model_background.row_col(0,3) = 0.f;
	model_background.row_col(1,3) = 0.f;
	
		
}

void IconBar::init_buttons(){
	int tex_count[5] = 					{5,5,2,4,1};
	int offsets[5] = 					{1,2,2,2};
	const char *texture_names[17] = { 	"u_b_s2","u_b_s3","nu_b_s2", "nu_b_s3", "nu_b_sm",
										"u_b_t1","u_b_t2","nu_b_t1", "nu_b_t2", "nu_b_tm",
										"pacman_units","bbm_units",
										"pacman_unit_production","pacman_defence","bbm_unit_production","bbm_defence",
										"menu"};
	vec3f black = 						{0.1,0.1,0.1};					
	vec3f none = 						{-1,-1,-1};				
	bool clickable[5] = 				{true,true,false,false, true};
	float depth[5] = 					{depth_button_s,depth_button_t,depth_button,depth_button,depth_button_s};
	float depth_acc[5] = 				{depth_acc_button_s,depth_acc_button_t,depth_button,depth_button,depth_acc_button_s};
	

	float x_offset = 1.5*offset_button_y;
	
	matrix4x4f model;
	int j = 0;
	
	for(int i = 0; i < 5; i++){
		
		make_unit_matrix4x4f(&model);
		model.row_col(0,0) = 0.12 * fovy;	
		model.row_col(1,1) = scale_button_y;
		model.row_col(1,3) = offset_button_y;
		model.row_col(0,3) = x_offset;
		x_offset += model.row_col(0,0) + offsets[i]*offset_button_y;	
	
		if(i == 4){
			model.row_col(0,0) = 0.09 * fovy;
			model.row_col(1,1) = 0.04 * fovy;
			model.row_col(0,3) = 3.9*offset_button_y;
			model.row_col(1,3) = fovy - 0.85 * scale_button_y;
		}
		
		buttons.push_back(Button(model,depth[i],depth_acc[i], none, false,clickable[i]));
		int j_max = j+tex_count[i]; 
		for(;j < j_max; j++){
			buttons[i].add_texture(texture_names[j]);
			cout << i << " : " << texture_names[j] << endl;
		}
		

		
	}
	if(fraction == 1)
		buttons[2].state = 0;
	else 
		buttons[2].state = 1;
}


int IconBar::click(int x, int y, vec3f (*ptr)(int x, int y)){
	
	camera_ref old_camera = current_camera();
	use_camera(cam);
	vec3f pos = ptr(x,y);

    use_camera(old_camera);


	float rel_depth = (pos.z-near)/(far-near);
	float rel_y = pos.y/fovy;


	if(-rel_depth >= (depth_button_s - depth_acc_button_s) && -rel_depth <= (depth_button_s + depth_acc_button_s)){
		
		if(rel_y > 0.5){
			cout << "click_menu : " << rel_y << " : " << pos.y << endl;
			return 4;
		}
		else {
			cout << "click_settlement" << endl;
			return 0;
		}	
	}
	else if(-rel_depth >= (depth_button_t - depth_acc_button_t) && -rel_depth <= (depth_button_t + depth_acc_button_t)){	

		cout << "click_turret" << endl;
		return 1;
	}

	return -1;

	
}

void IconBar::clicked_menu(){
	open_menu = !open_menu;
}

int IconBar::scale_button(int b, bool smaller){
	int i = -1, index = b;

	
	if(!smaller){
		if(button_pressed < 0 || button_pressed >= buttons.size())
			return -1;
		index = button_pressed;
		i = 1;
	} else {
		button_pressed = b;	
	}


	
	float scale = 0.008 * fovy * i;
	float offset = -0.5*scale;

	if( buttons[index].clickable && buttons[index].state <= 1){
		buttons[index].model.row_col(0,0) += scale;
		buttons[index].model.row_col(1,1) += scale;
		buttons[index].model.row_col(0,3) += offset;
		buttons[index].model.row_col(1,3) += offset;

		if(b != button_pressed && !smaller){
			button_pressed = -1;
			return -1;
		}
		if(!smaller)
			button_pressed = -1;

	} else {
		button_pressed = -1;
		return -1;
	}		

	return index;

}


void IconBar::update(){
	
		if(building == nullptr)
			return;
		
		int type  = building->get_type();
		int level = building->get_level();
		int state = building->get_state();
		int unit_count = building->get_unit_count();
		
		std::stringstream l_1,l_2;
		l_1 << unit_count;
		label_1->update_gui_texture_string(&l_1);
		
		using namespace msg::building_state;
		
		//settlement
		if(type == 0){
			s_upgradeable	= building->check_for_upgrade_settlement(state+1);
			s_level		 	= level;
	
			if(s_upgradeable)
				buttons[0].state = level;
			else 
				buttons[0].state = 2 + level; 		
			
			t_level 		= 0;
			t_upgradeable 	= building->check_for_upgrade_turret(turret_lvl1);
			if(t_upgradeable)
				buttons[1].state = 0;
			else 
				buttons[1].state = 2; 
				
			if(fraction == 1)
				buttons[3].state = 0;
			else
				buttons[3].state = 2;
			l_2 << building->get_unit_production();
			label_2->update_gui_texture_string(&l_2);
		} 
		//turret
		else if (type == 1){
			t_upgradeable 	= building->check_for_upgrade_turret(state+1);
			t_level 		= level;
			
			if(t_upgradeable)
				buttons[1].state = level+1;
			else 
				buttons[1].state = 2 + level + 1; 			
			
			s_level 		= 0;
			s_upgradeable 	= building->check_for_upgrade_settlement(house_lvl1);

			if(s_upgradeable)
				buttons[0].state = 0;
			else 
				buttons[0].state = 2; 		

			if(fraction == 1)
				buttons[3].state = 1;
			else 
				buttons[3].state = 3;
			l_2 << building->get_defence_value();
			label_2->update_gui_texture_string(&l_2);

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
