#include "rendering.h"

#include "wall-timer.h"

#include <cstdlib>

using namespace std;


namespace render_settings {
	int screenres_x = 1000, screenres_y = 1000;
    float tile_size_x = 2, tile_size_y = 2;
};

void setup_dir_light(shader_ref shader) {
	vec3f light_dir = { 1.f, -0.6f, -0.4f };
	normalize_vec3f(&light_dir);

	int loc = glGetUniformLocation(gl_shader_object(shader), "light_dir");
	glUniform3f(loc, light_dir.x, light_dir.y, light_dir.z);
	loc = glGetUniformLocation(gl_shader_object(shader), "light_col");
	glUniform3f(loc, 0.6f, 0.7f, 0.8f);
}

float random_float() {
	return ((rand() % 32768) / 16384.0f) - 1.0f;
}
