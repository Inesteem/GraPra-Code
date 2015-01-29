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
#include "label.h"
#include "menu.h"

// TODO REMOVE
#include "effect.h"

#include <framebuffer.h>
#include <texture.h>
#include <cstdio>


#include <libcgl/scheme.h>
#include <libcgl/impex.h>

#include <libguile.h>
#include <math.h>

#include <unistd.h>

//fork?
#include <iostream>
#include <string>
#include <sys/types.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <thread>
#include <mutex>

using namespace std;
using namespace moac;
#define doc(X)

//functions
void init_framebuffer();
void init_matrices();
void render_gui_overlay(bool gameover);
void actual_main();
void reset();
void simple_loop();

//important game logic bools
bool standard_mouse = false;
bool init_menu = false;
bool wireframe = false;
bool screenshot = false;
bool render_menu = true;
static bool reload_pending = false;

//loop   
static wall_time_timer key_timer;
static int frames = 0;
static wall_time_timer frames_timer;



// TODO REMOVE
UpgradeEffect *testEffect;


//important game logic pointer
simple_heightmap *sh;
ObjHandler *objhandler;
Game *game;
client_message_reader *messageReader;
Action *action;
Menu *menu = nullptr;
LoadScreen *loadscreen;

//keys
unsigned char navi_key = 0;
unsigned char key_to_move_up = 'i',
key_to_move_down = 'k',
key_to_move_left = 'j',
key_to_move_right = 'l';


//stuff
framebuffer_ref the_fbuf;
texture_ref shadowmap;
texture_ref color;
matrix4x4f *T;
char hostname[1024];
int fraction = 1;

thread *t;
mutex load_mutex;
list<const char *> obj_to_load = { 
	"tree", 				"./render-data/models/tree.obj",				"pos+norm+tc",
    "tropical_tree", 		"./render-data/models/tropical_tree.obj", 		"pos+norm+tc",
    "building_lot", 		"./render-data/models/building_lot.obj", 		"alpha-color-shader",
    "upgrade_arrow",	 	"./render-data/models/cube.obj", 				"alpha-color-shader",
    "house_pacman_lvl1", 	"./render-data/models/siedlung.obj", 			"alpha-color-shader",
    "house_pacman_lvl2",	"./render-data/models/siedlung_lvl2.obj",		"alpha-color-shader",
    "house_pacman_lvl3",	"./render-data/models/siedlung_lvl3.obj", 		"alpha-color-shader",
    "house_bbm_lvl1", 		"./render-data/models/house_bbm_lvl1.obj",		"alpha-color-shader",
    "house_bbm_lvl2", 		"./render-data/models/house_bbm_lvl2.obj",		"alpha-color-shader",
    "house_bbm_lvl3", 		"./render-data/models/house_bbm_lvl3.obj",		"alpha-color-shader",
    "turret_bbm_lvl1", 		"./render-data/models/tower_bbm_lvl1.obj", 		"alpha-color-shader",
    "turret_pacman_lvl1", 	"./render-data/models/simple_tower_pacman.obj", "alpha-color-shader",
    "bomberman",			"./render-data/models/bbm-nolegs.obj",			"unit-shader",
    "tonkrug",				"./render-data/models/tonkrug.obj",				"alpha-color-shader",
    "plants", 				"./render-data/models/plants.obj", 				"plant-shader"

	}; 
 
void load_obj(ObjHandler *objhandler,list<const char *> *obj_to_load, mutex *load_mutex) {
	while(!obj_to_load->empty()){
		load_mutex->lock();
		
		if(obj_to_load->size() == 0)
			return;
		
		const char *name = obj_to_load->front();
		obj_to_load->pop_front();
		
		
		const char *path = obj_to_load->front();
		obj_to_load->pop_front();
		const char *shader = obj_to_load->front();
		obj_to_load->pop_front();
		
		cout << name << " " << path << " " << shader << endl;
		
		load_mutex->unlock();
		
		objhandler->addObj(name, path, find_shader(shader));
		
		loadscreen->draw();	
		
	}
 
}

void start_threads(bool start_threads, ObjHandler *objhandler,list<const char *> *obj_to_load, mutex *load_mutex){
	
	if(start_threads){
		
		thread threads[1];
		
		for(int i = 0;i<1;i++){
			threads[i] = thread (load_obj,objhandler,obj_to_load,load_mutex);	
		}

		for(int i = 0;i<1;i++){
			threads[i].join();	
		}
	} else
		load_obj(objhandler,obj_to_load,load_mutex);

	vector<string> filenames;
    filenames.push_back("./render-data/models/pacman_1.obj");
    filenames.push_back("./render-data/models/pacman_2.obj");
    objhandler->makeObjFMS(filenames,"pacman",find_shader("ip2-shader"));

	loadscreen->draw();	

    mesh_ref mesh = make_mesh("selection_circle",2);
    vector<vec3f> pos = { vec3f(0,0,0) , vec3f(1,0,0), vec3f(1,0,1) , vec3f(0,0,1) };
    vector<vec2f> tc = { vec2f(0,0), vec2f(0,1), vec2f(1,1), vec2f(1,0) } ;
    vector<unsigned int> index =  { 0 ,1 ,2 , 2 ,3 ,0 };
    bind_mesh_to_gl(mesh);
    add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, 4, 3, (float*) pos.data() , GL_STATIC_DRAW);
    add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, 4, 2, (float*) tc.data() , GL_STATIC_DRAW);
    add_index_buffer_to_mesh(mesh, index.size(), (unsigned int *) index.data(), GL_STATIC_DRAW);
    unbind_mesh_from_gl(mesh);
    objhandler->addMeshObj("selection_circle",mesh,find_shader("selection_circle_shader"),find_texture("selection_circle.png") );

	loadscreen->draw();

    pos.clear();
    tc.clear();
    index.clear();
    mesh_ref tmesh = make_mesh("boom",2);
    pos = { vec3f(-0.5,-0.5,0), vec3f(0.5,-0.5,0), vec3f(0.5,0.5,0), vec3f(-0.5,0.5,0) };
    tc = { vec2f(0,1), vec2f(1,1), vec2f(1,0), vec2f(0,0) };
     index =  { 0, 1, 2, 2, 3, 0 };
    bind_mesh_to_gl(tmesh);
    add_vertex_buffer_to_mesh(tmesh, "in_pos", GL_FLOAT, 4, 3, (float*) pos.data() , GL_STATIC_DRAW);
    add_vertex_buffer_to_mesh(tmesh, "in_tc", GL_FLOAT, 4, 2, (float*) tc.data() , GL_STATIC_DRAW);
    add_index_buffer_to_mesh(tmesh, index.size(), (unsigned int *) index.data(), GL_STATIC_DRAW);
    unbind_mesh_from_gl(tmesh);
    objhandler->addMeshObj("boom",tmesh,find_shader("boom-shader"),find_texture("boom") );

	loadscreen->draw();	
	
}




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



void mouse_move(int x, int y) {
	
    if(render_menu)
        return;

    action->update_mouse_pos(x,y);
}

void mouse(int button, int state, int x, int y) {
    // TODO REMOVE
    if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
        testEffect->Start();

    }

    if(render_menu || !messageReader->m_init_done) {
        return;
    }

    if(standard_mouse){
        standard_mouse_func(button, state, x, y);
    } else {
        if(button == GLUT_LEFT_BUTTON){ //eigene Gebaeude auswaehlen
            if(state == GLUT_DOWN){
                if(action->check_button_clicked(x,y, 0) == -1)
					action->handle_base_selection(x,y);
            }else
                action->check_button_clicked(x,y, 1);


        }
        else if(button == GLUT_RIGHT_BUTTON){
            if(state == GLUT_DOWN){
                action->handle_enemys_base(x,y);

            }
            else {
                action->finish();
            }
        }
        else if(button == 3 && state != GLUT_DOWN)
            standard_keyboard('r', x, y);

        else if(button == 4 && state != GLUT_DOWN)
            standard_keyboard('f', x, y);

    }
}

void reset_labels(){
	for(int i = 0; i < labels.size();i++){
		labels[i]->recalculate_pos();
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
    vec3f pos = vec3f(cam_pos.x, 0 , cam_pos.z);
    pos = pos - cam_pos;
      float dist = length_of_vec3f(&pos);
    switch (key) {
//    case 'e' : game->getBuilding(0)->change_size(10); break;
    case 27:
        quit(0);
    case 'f':

        if( dist < 60){
            copy_vec3f(&tmp, &cam_dir);
            mul_vec3f_by_scalar(&tmp, &tmp, -cgl_cam_move_factor);
            add_components_vec3f(&cam_pos, &cam_pos, &tmp);
        }
        break;
    case 'r':

       if(dist > 15 ){
            copy_vec3f(&tmp, &cam_dir);
            mul_vec3f_by_scalar(&tmp, &tmp, cgl_cam_move_factor);
            add_components_vec3f(&cam_pos, &cam_pos, &tmp);
        }
        break;
    case 'a':
        if(cam_pos.x < sh->get_size().x){
            copy_vec3f(&tmp, &cam_right);
            mul_vec3f_by_scalar(&tmp, &tmp, -cgl_cam_move_factor);
            add_components_vec3f(&cam_pos, &cam_pos, &tmp);
        }
        break;
    case 'd':
        if(cam_pos.x > 0){
            copy_vec3f(&tmp, &cam_right);
            mul_vec3f_by_scalar(&tmp, &tmp, cgl_cam_move_factor);
            add_components_vec3f(&cam_pos, &cam_pos, &tmp);
        }
        break;
    case 's':
        if(cam_pos.z > 0){
            copy_vec3f(&tmp, &cam_up);
            tmp = vec3f(0,0,1);
            mul_vec3f_by_scalar(&tmp, &tmp, -cgl_cam_move_factor);
            add_components_vec3f(&cam_pos, &cam_pos, &tmp);
        }
        break;
    case 'w':
        if(cam_pos.z < sh->get_size().y-10){
            copy_vec3f(&tmp, &cam_up);
            tmp = vec3f(0,0,1);
            mul_vec3f_by_scalar(&tmp, &tmp, cgl_cam_move_factor);
            add_components_vec3f(&cam_pos, &cam_pos, &tmp);
        }
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
    reset_labels();
}

static int eingabe= 0;
int max_length = 15;
static int index_hostname= 0;

void reset_hostname(){
    menu->set_enter(false);
    eingabe = 0;
    for(int i = 0; i <= index_hostname ; i++){
        hostname[i] = '\0';
    }
    index_hostname = 0;
}

void menu_keyhandler(unsigned char key, int state){
	msg::client_left cl = make_message<msg::client_left>();
    switch(key){

    //Backspace
    case 8 :  if(eingabe == 1){
            hostname[index_hostname] = '\0';
            if(index_hostname > 0)
                index_hostname--;
			}
            hostname[index_hostname] = '<';
			menu->set_hostname(hostname);
			break;

        //Enter
        //host game
    case 13 : if(menu->get_row() == 0){
            game->deliver_settings(menu->get_frac(),menu->get_player_color()-1);
			action->init_iconbar(menu->get_frac());
			
			
            pid_t pID = fork();

            unsigned int level = menu->get_level();
            int numplayers = menu->get_num_players();
            std::string s = std::to_string(numplayers);
            std::string l = std::to_string(level);


            if (pID == 0) {// child

                char resolved_path[100];
                realpath(".", resolved_path);
                std::string str = resolved_path;
                std::string str_2 = resolved_path;
                str += "/src/multiman_server ";
                str_2 += "/src/multiman_server";
                str += s;
                str += " ";
                str += level;
                str += " $*; echo '-- '; echo 'press return to close this terminal'; read";

             //   execl("/usr/bin/xterm","/usr/bin/xterm", "-geometry", "200x100+0+900", "-e", str.c_str(), NULL);
                execl(str_2.c_str(),str_2.c_str(), s.c_str(), l.c_str(), NULL);

            }
            else if (pID < 0){// failed to fork

                cerr << "Failed to fork" << endl;
                exit(1);
                // Throw exception
            }
            else {// parent
                sleep(1);
                gethostname(hostname, 1023);
                cout << ">" << hostname << "<" << endl;
                messageReader->networking_prologue(hostname);
                reset_hostname();
                render_menu = false;
                menu->reset_menu();
            }
        }
        //join game
        if(menu->get_row() == menu->get_row_max()-1){
            if(eingabe==0){
                eingabe = 1;
                hostname[index_hostname] = '<';
                menu->set_enter(true);
                menu->set_hostname(hostname);
			} else {
				game->deliver_settings(menu->get_frac(),menu->get_player_color()-1);
				action->init_iconbar(menu->get_frac());
                hostname[index_hostname] = '\0';
                cout << ">" << hostname << "<" << endl;
                //todo: fehlerbehandlung
                messageReader->networking_prologue(hostname);
                render_menu = false;
                menu->reset_menu();
                reset_hostname();
            }

        }

        if(menu->get_row() == -1){
            menu->set_mode(menu->GAMESTART);
        }

        break;
        //esc
    case 27 : 	
				cl.playerId = PLAYER_ID;
				game->m_messageReader->send_message(cl);
				exit(0); break;

    default : if(eingabe == 1 && index_hostname < max_length){
            hostname[index_hostname] = key;
            hostname[++index_hostname] = '<';
            menu->set_hostname(hostname);
        } else
            cout << "normal : " << (int)key << endl;

    }
}

void special_keyhandler( int key, int x, int y ){

    if(!render_menu)
        return;

    switch(key){
    case GLUT_KEY_LEFT:
        reset_hostname();
        menu->decrease_mom_row();
        break;
    case GLUT_KEY_RIGHT:
        reset_hostname();
        menu->increase_mom_row();
        break;
    case GLUT_KEY_UP:
        reset_hostname();
        menu->increase_row();
        break;
    case GLUT_KEY_DOWN:
        reset_hostname();
        menu->decrease_row();
        break;
    default :
        cout << "special : " << key << endl;
    }
}


void keyhandler(unsigned char key, int x, int y) {

    if(render_menu){

		if(!messageReader->m_init_done) {
			msg::client_left cl = make_message<msg::client_left>();
			switch(key){
				case 'Y': reset();break;
				case 'N': cl.playerId = PLAYER_ID;
							game->m_messageReader->send_message(cl);
							exit(0); break;
			}
			
		}

        menu_keyhandler(key, 0);
        return;

    }



    if (key == 'W')      wireframe = !wireframe;
    else if (key == 'R') reload_pending = true;
    else if (key == 'E'){
        for(int i = 0; i < 4; i++){
            Building *b = game->getBuilding(i);
            cout << "Building " << i << " contains " << b->get_unit_count() << " units" << endl;
        }
    }
    else if (key == 'M') standard_mouse = !standard_mouse;
    else if (key == 'S') reload_pending = true;
    else if (key == 'T') screenshot = true;
    else if (key == 'P') ;//use_camera(find_camera("playercam"));
    else if (key == 'L') ;//use_camera(find_camera("lightcam"));
    else if (key == 'u') action->upgrade_settlement();
    else if (key == 't') action->upgrade_turret();
    else if (key == 'p') game->get_planes();
    else if (key == 'r' || key == 'f'){//skip zoom with keyboard//
    }
    else if (key ==  27){}

    
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

    render_time_table render_timer; 
void loop() {

    //
    // administrative
    //

    if (reload_pending) {
        scm_c_eval_string("(load-shaders)");
        reload_pending = false;
    }

//    render_time_table render_timer;
    render_timer.start_frame();

//    static wall_time_timer key_timer;
//    static int frames = 0;
//    static wall_time_timer frames_timer;
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
    if(messageReader->m_init_done) {
        game->update();
		testEffect->Update();
    }
    render_timer.done_with("updates");

    //
    // pre render pass
    //

    //shadowmapping
    /*
        //glViewport(0,0,2048,2048);
        //glColorMask(0,0,0,0);

        //Cam_Setup
        camera_ref actual_cam = current_camera();
        //use_camera(find_camera("lightcam"));

        static int i = 1;

        if(i == 1){
            vec3f pos = {-22.877605,38.488937,52.607487};
            vec3f dir = {  0.811107, -0.486664, -0.324443};
            vec3f up = { 0.584898,0.674882,0.449921};
            change_lookat_of_cam(find_camera("lightcam"),&pos, &dir,&up);
            recompute_gl_matrices_of_cam(find_camera("lightcam"));
            i = 0;
        }

        bind_framebuffer(the_fbuf);
        glClear(GL_DEPTH_BUFFER_BIT);
        glClearDepth(1);

        if(messageReader->m_init_done) {
            game->draw();
        }

        unbind_framebuffer(the_fbuf);

        if(screenshot){
            save_texture_as_png(shadowmap, "./screenshot.png");
            save_texture_as_png(color, "./screenshot_42.png");
            screenshot = false;
            std::cout << "Took a shot!" << std::endl;
        }

        init_matrices();

        //shadowmapping end
        */
    render_timer.done_with("pre-pass");

    //
    // actual render
    //

    glClearColor(0,0,0,1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    /*
        //shadowmapping
        shader_ref shader = find_shader("terrain");
        bind_shader(shader);

        bind_texture(shadowmap, 4);
        int loc = glGetUniformLocation(gl_shader_object(shader), "shadowmap");
        glUniform1i(loc, 4);


        glColorMask(1,1,1,1);
        //use_camera(actual_cam);
        glViewport(0,0,1024,1024);
        //shadowmapping end
*/

    use_camera(find_camera("playercam"));

    if(messageReader->m_init_done) {
		game->draw();
		action->draw();
		testEffect->Render();
	} else {
		render_menu = true;
		register_display_function(simple_loop);
		register_idle_function(simple_loop); 
		
	}
    render_timer.done_with("draw");

    //
    // finishing up
    //
    check_for_gl_errors("display");
    swap_buffers();
    unbind_texture(shadowmap);

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


void reset(){
	
	init_menu = false;

    register_display_function(simple_loop);
    register_idle_function(simple_loop);

	delete menu;	
    menu = new Menu();
    render_menu = true;
    
		
	menu->reset_menu();
	menu->init(game, &render_menu);
    menu->set_mode(menu->GAMESTART);
    
    for(int i = 0; i < labels.size();i++){
		delete labels[i];
	}
	labels.clear();
    
    delete sh;
    delete game;
	delete messageReader;

    sh = new simple_heightmap();
    game = new Game(objhandler,sh, messageReader,menu);
    messageReader = new client_message_reader(game);
    game->set_action(action);
    
    messageReader = new client_message_reader(game);
    action = new Action(game, objhandler,&reset);
    game->set_action(action);
    	
    init_menu = true;
    
}

void simple_loop(){
			//
		// administrative
		//


//		render_time_table render_timer;
		render_timer.start_frame();

//		static wall_time_timer key_timer;
//		static int frames = 0;
//		static wall_time_timer frames_timer;
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

		use_camera(find_camera("playercam"));


			
		glClearColor(0,0,0,1);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		render_timer.done_with("draw");

	
		if(!render_menu) {
				
			messageReader->read_and_handle();
			if(messageReader->m_init_done){
				register_display_function(loop);
				register_idle_function(loop); 
			}
		
		
		} else if(init_menu)
			menu->draw(false);

		//
		// finishing up
		//
		check_for_gl_errors("display");
		swap_buffers();

		render_timer.done_with("swap");
	
	
}

void resize(int w, int h)
{
    standard_resize_func(w, h);
}


void actual_main() {



    register_scheme_functions_for_key_handling();
    load_configfile("multiman.scm");
    cout << "cfg done" << endl;
    glDebugMessageCallbackARB(gl_error, 0);

    // glut initialization
    //

    glutSpecialFunc(special_keyhandler);

    register_display_function(simple_loop);
    register_idle_function(simple_loop);
    register_keyboard_function(keyhandler);
    register_keyboard_up_function(keyhandler_up);
    register_resize_function(resize);

    register_mouse_function(mouse);
    register_mouse_motion_function(mouse_move);
    glutIgnoreKeyRepeat(1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearDepth(1.0);

    // set different cursors
    glutSetCursor(GLUT_CURSOR_INFO);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    loadscreen = new LoadScreen(find_camera("orthocam"));
	
	loadscreen->draw();	
	
    objhandler = new ObjHandler();   
//	t = new thread(start_threads,true, objhandler,&obj_to_load,&load_mutex);  
//	t->join();
//	delete t;
	start_threads(false, objhandler,&obj_to_load,&load_mutex);

    vec3f o = vec3f(0,10,0);
    vec3f c = vec3f(0, 1, 0);
    testEffect = new UpgradeEffect(o, c);

    init_framebuffer();
    sh = new simple_heightmap();


    menu = new Menu();

    game = new Game(objhandler,sh, messageReader,menu);


    menu->init(game, &render_menu);
    menu->set_mode(menu->GAMESTART);
	init_menu = true;

    messageReader = new client_message_reader(game);
    action = new Action(game, objhandler,&reset);
    game->set_action(action);	
		
    	
    enter_glut_main_loop();
    
}


int main(int argc, char **argv) {

    render_settings::screenres_x = cmdline.res_x;
    render_settings::screenres_y = cmdline.res_y;
    append_image_path("./render-data/images/");
    // 	int guile_mode = with_guile;
    int guile_mode = guile_cfg_only;
    startup_cgl("multiman", 3, 3, argc, argv, render_settings::screenres_x, render_settings::screenres_y, actual_main, guile_mode, true, 0);
    return 0;
}



void init_framebuffer(){
    tex_params_t params;

    params.min =GL_LINEAR; //GL_NEAREST GL_LINEAR
    params.mag =GL_LINEAR;
    params.wrap_s =GL_CLAMP_TO_EDGE; //GL_REPEAT GL_CLAMP
    params.wrap_t =GL_CLAMP_TO_EDGE;
    params.mipmapping = false;
    //render_settings::screenres_x, render_settings::screenres_y
    the_fbuf = make_framebuffer("greybuf2048", 1024, 1024);
    shadowmap = make_empty_texture("greymap2048", 1024, 1024, GL_TEXTURE_2D, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_COMPONENT, &params);//_32F
    color = make_empty_texture("greymap2048_c", 1024, 1024, GL_TEXTURE_2D, GL_RGBA, GL_FLOAT, GL_RGBA, &params);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    bind_framebuffer(the_fbuf);

    attach_texture_as_depthbuffer(the_fbuf, "greybuf", shadowmap);
    attach_texture_as_colorbuffer(the_fbuf, "greybuf", color);

    draw_buffers_done(the_fbuf);
    check_framebuffer_setup(the_fbuf);
    unbind_framebuffer(the_fbuf);

}

void init_matrices(){

    if(T == NULL)
        T = new matrix4x4f();

    camera_ref cam =  find_camera("lightcam");
    matrix4x4f PL = *(projection_matrix_of_cam(cam));
    matrix4x4f VL = *(gl_view_matrix_of_cam(cam));

    matrix4x4f S;
    vec3f scale = {0.5,0.5,0.5};
    make_scale_matrix4x4f(&S, &scale);
    S.row_col(0,3) = 0.5;
    S.row_col(1,3) = 0.5;
    S.row_col(2,3) = 0.5;

    shader_ref shader = find_shader("terrain");
    bind_shader(shader);

    int loc = glGetUniformLocation(gl_shader_object(shader), "T");
    glUniformMatrix4fv(loc, 1, GL_FALSE, T->col_major);

    unbind_shader(shader);
}

