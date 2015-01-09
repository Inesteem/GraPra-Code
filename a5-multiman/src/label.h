#ifndef __LABEL_H__ 
#define __LABEL_H__ 
#include <shader.h>
#include <mesh.h>
#include <texture.h>

#include <list>

#include <cairo/cairo.h>

#include "wall-timer.h"
#include "rendering.h"
#include "drawelement.h"
#include "objloader.h"

#include <vector>
#include <math.h>
#include <algorithm>
#include <queue>
#include <string>
#include <sstream>
#include <cstdlib>

class Label {
		
		// Setup Cairo
		const unsigned int fontSize = 20;
	//	double fontSize = 100;

		cairo_surface_t *cairo_surface = 0;
		unsigned char *cairo_surface_data = 0;

		int nchars = 25;
//		int nchars = 4;
		cairo_t *cairo; 	
		unsigned char *data;
		
		// The overlay textures
		// SET id's to -1 in constructor !!!!!!!!!!
		texture_ref health_header_texture;
		texture_ref health_texture;
		texture_ref score_header_texture;
		texture_ref score_texture;
		texture_ref gameover_score_texture;

		// Names for the textures
		std::string health_header_name = "health_header";
		std::string health_name = "health";
		std::string score_header_name = "score_header";
		std::string score_name = "score";
		std::string gameover_score_name = "gameover_score";

		// Names for the texture headers
		std::string health_header = "Lifes: ";
		std::string score_header = "Score: ";

		// Texture Colors
		vec3f health_header_color = {1.f, 0.f, 0.f};
		vec3f health_color = {1.f, 0.f, 0.f};
		vec3f score_header_color = {0.f, 0.f, 1.f};
		vec3f score_color = {0.f, 0.f, 1.f};			
		vec3f gameover_score_color = {174.f/255.f, 0.f, 0.f};
		
		// Helpful functions
		// Helper functions
		void do_cairo_stuff(texture_ref &texture, std::string display, vec3f color, std::string name);
		void do_update_cairo_stuff(texture_ref &texture, std::string display, vec3f color, std::string name);
		cairo_t* create_cairo_context(int width, int height, int channels, cairo_surface_t **surf, unsigned char **buffer);
		// End GUI


		// You will have to change this methods if there are any new displays
		void make_gui_textures();
		public:
		void update_gui_textures_health(int lifes);
		void update_gui_textures_gameoverscore(long newscore);
		void update_gui_textures_score(int newscore);
		// gui-overlay.cpp
		void set_restart();
		void setup_display();
		void render_gui_overlay(bool gameover);
		void initialize_gui_overlay();


};
	extern Label cool_display;

#endif
