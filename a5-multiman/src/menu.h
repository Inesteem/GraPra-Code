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
	

	shader_ref 	gameshader;
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
	
	int row = 0;
	int max_rows = 5;
	
	vector<Label *>labels;
	vector<std::string> strings = {"Host Game", "Fraktion", "Level", "Players", "Join Game"};
	vector<int> nums = {-1,1,1,2,-1};
	vector<int> max_nums = {-1,2,1,4,-1};
	vector<int> min_nums = {-1,1,1,1,-1};
	
	client_message_reader *message_reader;
	void update_label();
	
	bool enter;
	
public:

	void init();
	//0 = paused_game, 1 = start_game, 2 = game_loose, 3 = game_won
	void draw_background(int state, bool blend);
	void draw_font();
	void draw(int state, bool blend);
	
	/* Host Game
	 * Fraktion
	 * Level 
	 * PLayers
	 * Join Game
	 * Eingabe
	 */
	int get_row();
	int get_row_max(){return max_rows;}
	void increase_row();
	void decrease_row();
	
	void increase_mom_row();
	void decrease_mom_row();
	
	void set_hostname(char *hostname);
	
		
	
	
};



#endif
