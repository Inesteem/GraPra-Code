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
};

void setup_dir_light(shader_ref shader);

float random_float();

#endif

