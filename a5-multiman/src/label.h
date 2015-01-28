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
#include "mouseactions.h"
#include "game.h"

#include <vector>
#include <math.h>
#include <algorithm>
#include <queue>
#include <string>
#include <sstream>
#include <cstdlib>


#include <GL/glew.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>


class Gameobject;
class Obj;
class ObjHandler;

class Label {
	
		// Setup Cairo
		unsigned int 	fontSize;
		cairo_surface_t *cairo_surface;
		unsigned char 	*cairo_surface_data;
		int 			nChars;
		cairo_t 		*cairo; 	
		unsigned char 	*data;
		
		// Some Helper-Attributes
		mesh_ref 	mesh;
		shader_ref 	label_shader;	
		camera_ref 	label_camera;	
		matrix4x4f model;
		// The overlay textures
		// SET id's to -1 in constructor !!!!!!!!!!
		texture_ref texture;
		std::string texture_name = "label_texture";		
        vec3f 		texture_color = {1.f, 1.f, 1.f};
		vec3f 		color = {1.f, 1.f, 1.f};
		vec3f 		pos;
		vec3f 		original_pos = {-1,-1,-1};
		bool		use_cam;

		vec2f BillboardSize;
		
		
		// Intern helper functions
		void do_cairo_stuff(std::string display);
		void do_update_cairo_stuff(std::string display);
		cairo_t* create_cairo_context(int width, int height, int channels, cairo_surface_t **surf, unsigned char **buffer);
		// End GUI


		void make_gui_texture();
		
	public:
		// Konstruktors
		Label();
		Label(int fontSize, int nchars, const char *shader_name);

		// Setters
		void set_nChars(unsigned int nChars);
		void set_fontSize(unsigned int fontSize);
		void set_shader(const char *shader_name);
		void set_camera(camera_ref camera);
		void set_shader(shader_ref shader);
		void set_size(vec2f size);
		void set_texture(const char *name);
		void set_color(vec3f color);
		void set_model(matrix4x4f model);
		void recalculate_pos();
		
		void setup_display();
		void change_mesh();

		texture_ref get_texture();


		void update_label_pos(float x, float y, float z);
		void update_label_model(matrix4x4f model);
		void update_gui_texture_int(int n);
		void update_gui_texture_string(std::stringstream *stream);
		// gui-overlay.cpp
		void render_gui_overlay();
		void initialize_gui_overlay();


};


class SlideBar {

		
		// Some Helper-Attributes
		int 		max_count;
		Label 		label_mom_count;
		int 		mom_count;
		mesh_ref 	mesh;
		shader_ref 	sbar_shader;
		camera_ref 	sbar_camera;
			
		texture_ref texture;
		std::string texture_name = "sbar_texture";		
		vec3f 		texture_color = {1.f, 0.f, 0.f};
		
		vec3f 		pos;
		vec3f 		screen_pos;
		matrix4x4f	bar_model;

		int 		unit_count;
		float 		LifeLevel;
		bool		down;

		
	public:
		// Konstruktors
		SlideBar();

		// Setters
		void set_shader(const char *shader_name);
		void set_texture(const char *texture_name);
		void set_camera(const char *camera_name);

		void update_pos(float x, float y);
		void update_mouse_pos(float x, float y);
		void update_unit_count(int count);
		void set_max_count(int count);
		void dec_max_count(int count);
		// gui-overlay.cpp
		void render_slidebar();
		void initialize_slidebar();
		void reset_bar();
		
		int get_unit_count();

		// Setters

};




#endif
