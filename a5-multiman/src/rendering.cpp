#include "rendering.h"

#include "wall-timer.h"

#include <cstdlib>

using namespace std;


namespace render_settings {
	int screenres_x = 1000, screenres_y = 1000;
    float tile_size_x = 2, tile_size_y = 2;
    float height_factor = 10; 
}



/* REMOVE
void set_player_color(int id, vec3f color){
	using namespace render_settings;
	for(int i = 0; i < player_colors.size(); i++){
		if(player_colors[i].x == color.x && player_colors[i].y == color.y && player_colors[i].z == color.z){
			player_colors[i] = player_colors[id];
			player_colors[id] = color;
			break;
		}
	}
	
}*/


void setup_dir_light(shader_ref shader) {
	vec3f light_dir = { 1.f, -0.6f, -0.4f };
	normalize_vec3f(&light_dir);

	int loc = glGetUniformLocation(gl_shader_object(shader), "light_dir");
	glUniform3f(loc, light_dir.x, light_dir.y, light_dir.z);
	loc = glGetUniformLocation(gl_shader_object(shader), "light_col");
	glUniform3f(loc, 0.6f, 0.7f, 0.8f);
	loc = glGetUniformLocation(gl_shader_object(shader), "light_col_water");
	glUniform3f(loc, 0.4f, 0.4f, 0.4f);
}

float random_float() {
    //return ((rand() % 32768) / 16384.0f) - 1.0f;
    return rand() / ((float) RAND_MAX); // random float in [0.0;1.0]
}
