#include <shader.h>
#include <camera.h>
#include <mesh.h>
#include <texture.h>
#include <libmcm/vectors.h>
#include <libmcm/matrix.h>
#include <libcgl/impex.h>
#include <impex.h>

#include <string>
#include <sstream>
#include <cstdlib>

#include "wall-timer.h"
#include "game.h"
#include "label.h"
#include "messages.h"





#ifndef __MENU_H__
#define __MENU_H__

class Menu{
	
public:
	enum Mode { GAMESTART, GAMEPAUSED, GAMELOOSE, GAMEWON };
	
	
protected:

	Mode mode = GAMESTART;

	shader_ref 	menushader;
	camera_ref 	gamecam;
	mesh_ref 	gamemesh;
	
	texture_ref game_won;
	texture_ref game_loose;
	texture_ref game_paused;
	texture_ref game_start;
	texture_ref black_screen;

	matrix4x4f model;
	vec3f grey = vec3f(0.7,0.7,0.7);
	vec3f white = vec3f(1.,1.,1.);
	
//	vec2f choosen_size = vec2f(29,6);
	vec2f choosen_size = vec2f(20,3);
//	vec2f normal_size = vec2f(28,5);
	vec2f normal_size = vec2f(19,2);
	
	int row = 0;
	int max_rows = 6;
	
	vector<Label *>labels;
	vector<std::string> strings = {"Host Game", "Fraktion", "Level","Color", "Players", "Join Game"};
	vector<const char*> level_names = {"lvl01","lvl02"};
	vector<const char*> color_names = {"Color","Grey","Red", "Yellow", "Violet", "Cyan","Blue","Green", "Pink","LightBlue","PaleYellow"};
    vector<vec3f> player_colors = { vec3f(1.,1.,1.),vec3f(0.7,0.7,0.7), vec3f(1,0,0),vec3f(1,1,0),vec3f(1,0,1),vec3f(0,1,1),vec3f(0,0,1),
								vec3f(0,1,0),vec3f(1,0.5,1),vec3f(0.5,1,1),vec3f(1,1,0.5) };
	vector<int> nums = {-1,1,1,1,1,-1};
	vector<int> max_nums = {-1,2,2,11,4,-1};
	vector<int> min_nums = {-1,1,1,1,1,-1};
	
	client_message_reader *message_reader;
	void update_label(bool choosen);
	
	bool enter;
	bool *render_menu;
	
public:



	void init(bool *render_menu);
	//0 = paused_game, 1 = start_game, 2 = game_loose, 3 = game_won
	void draw_background(bool blend);
	void draw_font();
	void draw(bool blend);
	
	/* Host Game
	 * Fraktion
	 * Level 
	 * PLayers
	 * Join Game
	 * Eingabe
	 */
	int get_row();
	int get_row_max(){return max_rows;}
	vec3f get_player_color();
	int get_num_players();
	const char *get_level();
	
	void increase_row();
	void decrease_row();
	
	void increase_mom_row();
	void decrease_mom_row();
	
	void set_hostname(char *hostname);
	void set_enter(bool enter){this->enter = enter;}
	void set_mode(Mode mode);
	void set_render_menu(bool rm){*render_menu = rm;}

	void reset_menu();

};



class IconBar{
	
	//0 = Pacman, 1 = Bomberman
	unsigned int frac = 0;
	unsigned int t_level = 1;
	unsigned int s_level = 1;
	// 1 = settlement, 2 = tower, else = none
	unsigned int pic;
	
	texture_ref background;
	texture_ref fraction[2];
	texture_ref upgrade_button_tower_1;
	texture_ref upgrade_button_tower_2;
	texture_ref upgrade_button_settlement_1;
	texture_ref upgrade_button_settlement_2;
	texture_ref picture;
	
	matrix4x4f *models[4];
	
	matrix4x4f  model_background;
	matrix4x4f  model_fraction;
	matrix4x4f  model_button_t;
	matrix4x4f  model_button_s;
	matrix4x4f  model_picture;

	shader_ref 	shader;
	camera_ref 	cam;
	mesh_ref 	mesh;
	
	void init_modelmatrices();
	void draw_fraction();
	
public: 

	IconBar();
	void draw();
	
	
};


#endif
