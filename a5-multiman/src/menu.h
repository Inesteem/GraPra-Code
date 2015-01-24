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
#include "gameobject.h"





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
	
	float depth = 0.001;
	
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
/*
class MenuEntry {
	
	int pos;
	int max_pos;
	int id;
	Label *label;
	vec3f choosen_color;
	vec3f normal_color;
	vec2f choosen_size;
	vec2f normal_size;
	vector<const char*> entry_names;
	vector<vec3f> colors;
	bool choosen, use_color;

public:	
	MenuEntry(int id, std::vector<const char*> entry_names, Label *label);
	void increase_row();
	void decrease_row();
	void update_label();
	void choose(bool s);
	void draw();
	void reset();
	void set_colors(vector<vec3f> colors);
	int get_id(){return id;}
};

template< typename T > 
class ChooseEntry : public MenuEntry {
	vector< T > entries;

public:	
	ChooseEntry(int id, vector<const char*> entry_names, Label *label, vector< T > entries):
		MenuEntry(id, entry_names,label), entries(entries) {}
	
	T get_choosen_entry(){
		return entries[pos];
	}
	
	
	
};

class EntryManager{
	int pos = 0;
	vector<MenuEntry*> entry_list;
	
	template < typename T > 
	void add_entry(std::vector<const char*> entry_names, vector< T > entries, vector<vec3f> colors, Label *label, int id);
	MenuEntry *next_entry();
	MenuEntry *previous_entry();	
	
};
*/


class IconBar{
	
	Building *building = nullptr;
	
	int loc = -1;
	int button_pressed = -1;
	
	//0 = Pacman, 1 = Bomberman
	unsigned int frac = 0;	
	unsigned int t_level = 0;
	unsigned int s_level = 0;
	// 1 = settlement, 2 = turret, else = none
	unsigned int pic = 0;
	
	bool t_upgradeable = true;
	bool s_upgradeable = true;
	bool building_selected = false;
	
	float depth_background = 0.09;
	float depth_button = 0.02;	
	float depth_button_s = 0.001;	
	float depth_acc_button_s = 0.0001;	
	float depth_button_t = 0.0001;
	float depth_acc_button_t = 0.00005;
	
	float fovy = 50;
	float near = 0.01;
	float far = 1000;
	
	float offset_button_y = 0.01 * fovy;
	float scale_button_y = 0.05 * fovy;
	
	texture_ref background;
	texture_ref button;
	texture_ref fraction[2];
	texture_ref upgrade_button_turret[2];
	texture_ref noupgrade_button_turret[3];
	texture_ref upgrade_button_settlement[2];
	texture_ref noupgrade_button_settlement[3];
	texture_ref picture[2];
	
	matrix4x4f *models[6];
	
	matrix4x4f  model_background;
	matrix4x4f  model_fraction;
	matrix4x4f  model_button_t;
	matrix4x4f  model_button_s;
	matrix4x4f  model_button_u;
	matrix4x4f  model_picture;

	shader_ref 	shader;
	camera_ref 	cam;
	mesh_ref 	mesh;
	Label		*label;
	
	void init_modelmatrices();
	void draw_fraction();
	void draw_buttons();
	void draw_picture();
	
public: 

	IconBar();
	void draw();
	int click(int x, int y, vec3f (*ptr)(int x, int y));
	void scale_button(int b, bool greater);
	void update();
	void selected_building(Building *building);
	
	
};


#endif
