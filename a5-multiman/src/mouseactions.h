
#ifndef __MOUSEACTIONS_H__ 
#define __MOUSEACTIONS_H__ 


#include <vector>
#include <math.h>
#include <unistd.h>

#include <camera.h>

#include <shader.h>
#include <mesh.h>
#include <texture.h>
#include "messages.h"

#include <list>

#include <cairo/cairo.h>

#include "rendering.h"

#include <vector>
#include <math.h>
#include <algorithm>
#include <queue>
#include <string>
#include <sstream>
#include <cstdlib>


class Building;
class ObjHandler;
class SlideBar;
class IconBar;
class Game;
class client_message_reader;

namespace moac {

	void getScreenpos(vec2f* in,vec2f* out);
	vec3f ClickWorldPosition(int screenX, int screenY);

	struct Action {
		
		Building *own_building;
		Building *enemys_building;
		bool ob_set, eb_set, prepare_attack, render_status_bar;
		SlideBar *slidebar;
		IconBar *iconbar;
		Game *game;
        void (*ptr)();
		
		
		camera_ref orthocam;
		
        Action(Game *game, ObjHandler *objhandler,void (*ptr)());
		bool handle_base_selection(float x, float y);
		bool handle_enemys_base(float x, float y);
		void start(float x, float y);
		void finish();
		void update_mouse_pos(float x, float y);
		void draw();
		void upgrade_settlement();
		void upgrade_turret();
		int check_button_clicked(int x, int y, int state);
		void update_iconbar();
        void reset( );
        void init_iconbar(int fraction);
        void call_menu(int button);
        void lost_building(int id);
	
	};	
	

}

#endif

