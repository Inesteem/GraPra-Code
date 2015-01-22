#ifndef __RENDERING_H__ 
#define __RENDERING_H__ 

#include <vector>

#include <shader.h>
#include "drawelement.h"

typedef std::vector<drawelement*> drawelement_container_t;

namespace render_settings {
	extern int player_step_timeslice;
	extern float character_radius;
	extern float character_float_h;
	extern int screenres_x, screenres_y;
    extern float tile_size_x, tile_size_y;
    extern float height_factor;
    extern std::vector<vec3f>  player_colors;
};

vec3f get_player_color(int id);
void set_player_color(int id, vec3f color);

void setup_dir_light(shader_ref shader);

float random_float();

#endif

