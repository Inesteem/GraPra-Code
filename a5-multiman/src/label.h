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
		unsigned int 	fontSize;
		cairo_surface_t *cairo_surface;
		unsigned char 	*cairo_surface_data;
		int 			nChars;
		cairo_t 		*cairo; 	
		unsigned char 	*data;
		
		// Some Helper-Attributes
		camera_ref 	label_cam;
		mesh_ref 	mesh;
		shader_ref 	label_shader;	
		matrix4x4f model;
		// The overlay textures
		// SET id's to -1 in constructor !!!!!!!!!!
		texture_ref texture;
		std::string texture_name = "label_texture";		
		vec3f 		texture_color = {1.f, 0.f, 0.f};
		vec3f 		pos;
		
		// Intern helper functions
		void do_cairo_stuff(std::string display, vec3f color, std::string name);
		void do_update_cairo_stuff(texture_ref &texture, std::string display, vec3f color, std::string name);
		cairo_t* create_cairo_context(int width, int height, int channels, cairo_surface_t **surf, unsigned char **buffer);
		// End GUI


		void make_gui_texture();
		
	public:
		// Konstruktors
		Label();
		Label(int fontSize, int nchars, const char *camera_name, const char *shader_name);

		// Setters
		void set_nChars(unsigned int nChars);
		void set_fontSize(unsigned int fontSize);
		void set_shader(const char *shader_name);
		void set_camera(const char *camera_name);
		
		void setup_display();


		void update_label_pos(float x, float y, long l);
		void update_label_model(matrix4x4f model);
		void update_gui_texture_int(int n);
		void update_gui_texture_long(long l);
		// gui-overlay.cpp
		void render_gui_overlay();
		void initialize_gui_overlay();


};


class SlideBar {

		
		// Some Helper-Attributes
		label max_count;
		label mom_count;
		camera_ref 	label_cam;
		mesh_ref 	mesh;
		shader_ref 	label_shader;	
		matrix4x4f model;
		
		texture_ref texture;
		texture_ref texture_background;
		std::string texture_name = "label_texture";		
		vec3f 		texture_color = {1.f, 0.f, 0.f};
		vec3f 		pos;
		int 		unit_count;

		void make_gui_texture();
		
	public:
		// Konstruktors
		SlideBar();

		// Setters
		void set_shader(const char *shader_name);
		void set_camera(const char *camera_name);
		
		void setup_display();

		void update_pos(float x, float y, long l);
		void update_model(matrix4x4f model);
		void update_unit_count(int n);
		// gui-overlay.cpp
		void render_gui_overlay();
		void initialize_gui_overlay();


};



#endif
