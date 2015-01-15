#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "cmdline.h"
#include "heightmap.h"
#include "simple_heightmap.h"
#include "wall-timer.h"
#include "rendering.h"
#include "drawelement.h"
#include "objloader.h"
#include "clientside-networking.h"
#include "gameobject.h"
#include "game.h"
#include "messages.h"
#include "mouseactions.h"

#include <libcgl/scheme.h>
#include <libcgl/impex.h>

#include <libguile.h>
#include <math.h>


using namespace std;
using namespace moac;


Action *action;

#define doc(X)

void render_gui_overlay(bool gameover);
//heightmap *the_heightmap;
simple_heightmap *sh;
ObjHandler *objhandler;
Game *game;
client_message_reader *messageReader;

bool standard_mouse = false;
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


void mouse_move(int x, int y) {
	action->update_mouse_pos(x,y);
}

void mouse(int button, int state, int x, int y) {
    if(standard_mouse){
         standard_mouse_func(button, state, x, y);
    } else {
        if(button == GLUT_LEFT_BUTTON){ //eigene Gebaeude auswaehlen
            if(state == GLUT_DOWN){
                action->handle_base_selection(x,y);
            }

        }
		if(button == GLUT_RIGHT_BUTTON){
			if(state == GLUT_DOWN){
				action->handle_enemys_base(x,y);
				
			}
			else {
				action->finish();
			}
		}            
        
    }
 


}


void standard_keyboard(unsigned char key, int x, int y)
{
        vec3f tmp;
        vec3f cam_right, cam_pos, cam_dir, cam_up;
        matrix4x4f *lookat_matrix = lookat_matrix_of_cam(current_camera());
        extract_pos_vec3f_of_matrix(&cam_pos, lookat_matrix);
        extract_dir_vec3f_of_matrix(&cam_dir, lookat_matrix);
        extract_up_vec3f_of_matrix(&cam_up, lookat_matrix);
        extract_right_vec3f_of_matrix(&cam_right, lookat_matrix);
        switch (key) {
                case 27:
                        quit(0);
                case 'f':
                        copy_vec3f(&tmp, &cam_dir);
                        mul_vec3f_by_scalar(&tmp, &tmp, -cgl_cam_move_factor);
                        add_components_vec3f(&cam_pos, &cam_pos, &tmp);
                        break;
                case 'r':
                        copy_vec3f(&tmp, &cam_dir);
                        mul_vec3f_by_scalar(&tmp, &tmp, cgl_cam_move_factor);
                        add_components_vec3f(&cam_pos, &cam_pos, &tmp);
                        break;
                case 'a':
                        copy_vec3f(&tmp, &cam_right);
                        mul_vec3f_by_scalar(&tmp, &tmp, -cgl_cam_move_factor);
                        add_components_vec3f(&cam_pos, &cam_pos, &tmp);
                        break;
                case 'd':
                        copy_vec3f(&tmp, &cam_right);
                        mul_vec3f_by_scalar(&tmp, &tmp, cgl_cam_move_factor);
                        add_components_vec3f(&cam_pos, &cam_pos, &tmp);
                        break;
                case 's':
                        copy_vec3f(&tmp, &cam_up);
                        mul_vec3f_by_scalar(&tmp, &tmp, -cgl_cam_move_factor);
                        add_components_vec3f(&cam_pos, &cam_pos, &tmp);
                        break;
                case 'w':
                        copy_vec3f(&tmp, &cam_up);
                        mul_vec3f_by_scalar(&tmp, &tmp, cgl_cam_move_factor);
                        add_components_vec3f(&cam_pos, &cam_pos, &tmp);
                        break;
                case 'R':
                        cgl_shader_reload_pending = true;
                        break;
                case 'p':
                        printf("campos:   %f %f %f\n", cam_pos.x, cam_pos.y, cam_pos.z);
                        printf("camdir:   %f %f %f\n", cam_dir.x, cam_dir.y, cam_dir.z);
                        printf("camup:    %f %f %f\n", cam_up.x, cam_up.y, cam_up.z);
                        break;
        }
        make_lookat_matrixf(lookat_matrix, &cam_pos, &cam_dir, &cam_up);
        recompute_gl_matrices_of_cam(current_camera());
}


void keyhandler(unsigned char key, int x, int y) {
	if (key == 'W')      wireframe = !wireframe;
	else if (key == 'R') reload_pending = true;
    else if (key == 'M') standard_mouse = !standard_mouse;
    else if (key == 'S') reload_pending = true;
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

    messageReader->read_and_handle();

	// 
	// update logic
	//
    game->update();
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


    //the_heightmap->draw();
    game->draw();
    
	action->draw();
	
	
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
	check_for_gl_errors("OpenGL Error Callback (gl_error)\n$ gdb src/multiman\n$ break gl_error\n$ run");
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
	load_configfile("multiman.scm");
	cout << "cfg done" << endl;
	//glDebugMessageCallbackARB(gl_error, 0);

	// glut initialization
	//
	register_display_function(loop);
	register_idle_function(loop);
	register_keyboard_function(keyhandler);
	register_keyboard_up_function(keyhandler_up);

    register_mouse_function(mouse);
    register_mouse_motion_function(mouse_move);
	glutIgnoreKeyRepeat(1);

	use_camera(find_camera("playercam"));

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0);


	// 
	// further initializations may go here
	//
    //the_heightmap = new heightmap("./render-data/images/eire.png", 6);
    objhandler = new ObjHandler();
        objhandler->addObj("tree", "./render-data/models/tree.obj", find_shader("pos+norm+tc"));
        objhandler->addObj("building_lot", "./render-data/models/building_lot.obj", find_shader("pos+norm+tc"));
        objhandler->addObj("status_bar", "./render-data/models/menu.obj", find_shader("pos+norm+tc"));
      //  objhandler->addObj("bomb","./render-data/models/bbm.obj", find_shader("pos+norm+tc"));

    sh = new simple_heightmap();

    game = new Game(objhandler,sh, messageReader);

    messageReader = new client_message_reader(game);
       messageReader->networking_prologue();

	// set different cursors

	glutSetCursor(GLUT_CURSOR_INFO);
	
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	action = new Action(game);

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
	startup_cgl("multiman", 3, 3, argc, argv, render_settings::screenres_x, render_settings::screenres_y, actual_main, guile_mode, true, 0);
	return 0;
}


