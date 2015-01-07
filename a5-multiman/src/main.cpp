#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "cmdline.h"
#include "wall-timer.h"
#include "rendering.h"
#include "drawelement.h"
#include "objloader.h"

#include <libcgl/scheme.h>
#include <libcgl/impex.h>

#include <libguile.h>
#include <math.h>


using namespace std;

#define doc(X)

void render_gui_overlay(bool gameover);

unsigned char navi_key = 0;
unsigned char key_to_move_up = 'i',
			  key_to_move_down = 'k',
			  key_to_move_left = 'j',
			  key_to_move_right = 'l';

bool wireframe = false;

SCM_DEFINE(s_set_keymap, "define-keymap", 1, 0, 0, (SCM str), "") {
	cout << "def km" << endl;
	char *map = scm_to_locale_string(str);
	cout << "map = " << map << endl;
	if (strlen(map) != 5) {
		cerr << "the map has to contain exactly 5 characters." << endl;
		return SCM_BOOL_F;
	}
	key_to_move_up = map[0];
	key_to_move_down = map[1];
	key_to_move_left = map[2];
	key_to_move_right = map[3];
	return SCM_BOOL_T;
}

static bool reload_pending = false;

void keyhandler(unsigned char key, int x, int y) {
	if (key == 'W')      wireframe = !wireframe;
	else if (key == 'R') reload_pending = true;
	else {
		navi_key = key;
		standard_keyboard(key,x,y);
	}
}

void keyhandler_up(unsigned char key, int x, int y) {
	if (navi_key == key)
		navi_key = 0;
}

void check_navigation_keys() {
	if (navi_key)
		standard_keyboard(navi_key, 0, 0);
}

float fps_buf[5];
int fps_id = 0;

SCM_DEFINE(s_fps, "fps", 0, 0, 0, (), "") {
	float f = 0;
	for (int i = 0; i < 5; ++i) f+= fps_buf[i];
	return scm_from_double(f/5);
}

struct render_time_table {
	wall_time_timer timer;
	std::list<std::pair<std::string, float> > stamps;
	void start_frame() {
		stamps.push_back(make_pair("start", 0.f));
		timer.start();
	}
	void done_with(const std::string &what) {
		stamps.push_back(make_pair(what, timer.look()));
	}
	void print_summary() {
		size_t len = 0;
		for (std::list<std::pair<std::string, float> >::iterator it = stamps.begin(); it != stamps.end(); ++it)
			len = max(len, it->first.length());

		std::list<std::pair<std::string, float> >::iterator it = stamps.begin(), last = stamps.begin();
		it++;
		while (it != stamps.end()) {
			cout << std::setw(len+2) << it->first << ": " << (it->second - last->second) << " ms" << endl;
			it++;
			last++;
		}
	}
};

drawelement_container_t ufo;

void loop() {

	// 
	// administrative
	//
	
	if (reload_pending) {
		scm_c_eval_string("(load-shaders)");
		reload_pending = false;
	}

	render_time_table render_timer;
	render_timer.start_frame();

	static wall_time_timer key_timer;
	static int frames = 0;
	static wall_time_timer frames_timer;
	++frames;
	float t = frames_timer.look(); // in ms
	if (t >= wall_time_timer::sec(1)) {
		float per_sec = (frames*1000.0f)/t;
		fps_buf[fps_id++%5] = per_sec;
	}

	if (key_timer.look() > wall_time_timer::msec(20)) {
		check_navigation_keys();
		key_timer.restart();
	}

	render_timer.done_with("keys");

	// 
	// update logic
	//
	
	render_timer.done_with("updates");

	// 
	// pre render pass
	//
	
	render_timer.done_with("pre-pass");

	//
	// actual render
	//

	glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (drawelement_container_t::iterator it = ufo.begin(); it != ufo.end(); ++it) {
		drawelement *de = *it;
		de->bind();
		de->apply_default_matrix_uniforms();
		de->apply_default_tex_uniforms_and_bind_textures();
		setup_dir_light(de->Shader());
		de->draw_em();
		de->unbind();
	}

	render_timer.done_with("draw");

	// 
	// finishing up
	//
	
	check_for_gl_errors("display");
	swap_buffers();

	render_timer.done_with("swap");
// 	render_timer.print_summary();
// 	cout << "-----" << endl;
}

void gl_error(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, void*) {
	check_for_gl_errors("OpenGL Error Callback (gl_error)\n$ gdb src/terrain\n$ break gl_error\n$ run");
}

SCM_DEFINE(s_res, "screenres", 0, 0, 0, (), "") {
	return scm_values(scm_list_2(scm_from_int(render_settings::screenres_x), scm_from_int(render_settings::screenres_y)));
}

void register_scheme_functions_for_key_handling() {
#ifndef SCM_MAGIC_SNARFER
#include "main.x"
#endif
}

extern "C" {
	void load_configfile(const char *);
}

void actual_main() {
	register_scheme_functions_for_key_handling();
	load_configfile("terrain.scm");
	cout << "cfg done" << endl;
	//glDebugMessageCallbackARB(gl_error, 0);

	// glut initialization
	//
	register_display_function(loop);
	register_idle_function(loop);
	register_keyboard_function(keyhandler);
	register_keyboard_up_function(keyhandler_up);
	register_mouse_function(standard_mouse_func);
	glutIgnoreKeyRepeat(1);

	use_camera(find_camera("playercam"));

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0);

	// 
	// further initializations may go here
	//
		ObjLoader loader("ufo", "./render-data/models/ufo.obj");
		loader.TranslateToOrigin();
		loader.pos_and_norm_shader = find_shader("pos+norm");
		loader.pos_norm_and_tc_shader = find_shader("pos+norm+tc");
		loader.GenerateNonsharingMeshesAndDrawElements(ufo);
		matrix4x4f mat;
		vec3f axis = {1,0,0};
		make_rotation_matrix4x4f(&mat, &axis, -M_PI/2);
		ufo.front()->Modelmatrix(&mat);

	
	// 
	// pass control to the renderer. won't return.
	//
	enter_glut_main_loop();
}


int main(int argc, char **argv)
{
	parse_cmdline(argc, argv);
	render_settings::screenres_x = cmdline.res_x;
	render_settings::screenres_y = cmdline.res_y;
	append_image_path("./render-data/images/");
// 	int guile_mode = with_guile;
	int guile_mode = guile_cfg_only;
	startup_cgl("terrain", 3, 3, argc, argv, render_settings::screenres_x, render_settings::screenres_y, actual_main, guile_mode, true, 0);
	return 0;
}


