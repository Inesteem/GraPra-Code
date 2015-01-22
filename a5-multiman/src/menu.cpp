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


void Menu::init(){
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


void Menu::draw(int state, bool blend){
	
	draw_background(state, blend);
	draw_font();
}

void Menu::draw_font(){

	for(int i = 0; i < max_rows; i++)	
		labels[i]->render_gui_overlay();

}


void Menu::draw_background(int state, bool blend){
	
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
		
		switch (state){
			case 0 : tex = game_paused; break;
			case 1 : tex = game_start; break;
			case 2 : tex = game_loose; break;
			case 3 : tex = game_won; break;
			default : tex = black_screen; 
		}

		loc = glGetUniformLocation(gl_shader_object(menushader), "model");
		glUniformMatrix4fv(loc, 1, GL_FALSE, model.col_major);

		loc = glGetUniformLocation(gl_shader_object(menushader), "tex");

		bind_texture(tex, 0);

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
char *Menu::get_level(){
	return level_names[LEVEL-1];
}


vec3f Menu::get_player_color(){
	if(COLOR == 1)
		return vec3f(-1,-1,-1);
		
	return player_colors[COLOR-1];
}
