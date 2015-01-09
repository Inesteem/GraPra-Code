#include "label.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>


Label cool_display;

	void Label::setup_display(){
		// make texture_ref invalid	
		health_header_texture.id = -1;
		health_texture.id = -1;
		score_header_texture.id = -1;
		score_texture.id = -1;
		gameover_score_texture.id = -1;

		// Create Textures
		make_gui_textures();
		
	}


	void Label::make_gui_textures() {

		// score header	
		do_cairo_stuff(score_header_texture, score_header, score_header_color, score_header_name);
		
		// score 
		std::stringstream score_stream;
		score_stream << 0;
		std::string score = score_stream.str();
		do_cairo_stuff(score_texture, score, score_color, score_name);

		// health header
		do_cairo_stuff(health_header_texture, health_header, health_header_color, health_header_name);
		
		// health
		std::stringstream health_stream;
		health_stream << 3;
		std::string health = health_stream.str();
		do_cairo_stuff(health_texture, health, health_color, health_name);

		// gameover score
		std::stringstream gameover_score_stream;
		gameover_score_stream << 0;
		std::string gameover_score = gameover_score_stream.str();
		do_cairo_stuff(gameover_score_texture, gameover_score, gameover_score_color, gameover_score_name);

	}
	
	void Label::update_gui_textures_health(int lifes){
		// update health_texture
		std::stringstream health_stream;
		health_stream << lifes;
		std::string health = health_stream.str();
		do_update_cairo_stuff(health_texture, health, health_color, health_name);


	}
	
	void Label::update_gui_textures_score(int newscore){
		// update score_texture
		std::stringstream score_stream;
		score_stream << newscore;
		std::string score = score_stream.str();
		do_update_cairo_stuff(score_texture, score, score_color, score_name);

	}
	
	
	void Label::set_restart(){
		std::stringstream gameover_score_stream;	
		gameover_score_stream << "    Press 'r' to restart";
		std::string gameover_score = gameover_score_stream.str();
		do_update_cairo_stuff(gameover_score_texture, gameover_score, gameover_score_color, gameover_score_name);
	}
	
	
	void Label::update_gui_textures_gameoverscore(long newscore){
		// update gameover score
		std::stringstream gameover_score_stream;
		int secs = (int)newscore/1000;
		int mins = (int)secs/60;
		int hours = (int)mins/60;
		secs %= 60;
		mins %= 60;
		hours %= 60;
		
		gameover_score_stream << "Playtime: ";
		if(hours < 10)
			gameover_score_stream << 0;
		gameover_score_stream << hours << "h ";
		if(mins < 10)
			gameover_score_stream << 0;
		gameover_score_stream << mins << "m ";
		if(secs < 10)
			gameover_score_stream << 0;
		gameover_score_stream << secs << "s";
		

	
		std::string gameover_score = gameover_score_stream.str();
		do_update_cairo_stuff(gameover_score_texture, gameover_score, gameover_score_color, gameover_score_name);

	}


// For stuff like score and health we will use cairo as in bbm

	cairo_t* Label::create_cairo_context(int width, int height, int channels, cairo_surface_t **surf, unsigned char **buffer) {
		cairo_t *cr;

		// create cairo-surface/context to act as OpenGL-texture source //
		*buffer = (unsigned char*)calloc(channels * width * height, sizeof(unsigned char));
		if (!*buffer) {
			printf("create_cairo_context() - Couldn't allocate surface-buffer\n");
			return 0;
		}

		*surf = cairo_image_surface_create_for_data(*buffer, CAIRO_FORMAT_ARGB32, width, height, channels * width);
		if (cairo_surface_status(*surf) != CAIRO_STATUS_SUCCESS) {
			free(*buffer);
			printf("create_cairo_context() - Couldn't create surface\n");
			return 0;
		}

		cr = cairo_create(*surf);
		if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
			free(*buffer);
			printf("create_cairo_context() - Couldn't create context\n");
			return 0;
		}
		
		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
		cairo_set_source_rgb(cr, 0, 0, 0);
		cairo_paint(cr);
		
		return cr;
	}

	void Label::do_cairo_stuff(texture_ref &texture, std::string displayed, vec3f color, std::string name){
		cairo = create_cairo_context(nchars * fontSize, fontSize, 4, &cairo_surface, &cairo_surface_data);

		if (!valid_texture_ref(texture)) {
			tex_params_t p = default_fbo_tex_params();
			p.wrap_s = GL_CLAMP_TO_EDGE;
			p.wrap_t = GL_CLAMP_TO_EDGE;
			texture = make_empty_texture(name.c_str(), nchars * fontSize, fontSize, GL_TEXTURE_2D, GL_RGBA8, GL_UNSIGNED_BYTE, GL_RGBA, &p);
		}
			
		texture = find_texture(name.c_str());

		cairo_set_font_size(cairo, fontSize);
		cairo_select_font_face(cairo, "DejaVu Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);	
		cairo_move_to(cairo, 0, fontSize);
		cairo_set_source_rgb(cairo, 0, 0, 0);
		cairo_paint(cairo);
		cairo_set_source_rgb(cairo, color.z, color.y, color.x);
		cairo_show_text(cairo, displayed.c_str());
		
		bind_texture(texture, 0);
		data = cairo_image_surface_get_data(cairo_surface);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture_width(texture), texture_height(texture), GL_RGBA, GL_UNSIGNED_BYTE, data);
		unbind_texture(texture);
	}


	void Label::do_update_cairo_stuff(texture_ref &texture, std::string displayed, vec3f color, std::string name){

		texture = find_texture(name.c_str());

		//cairo_select_font_face(cairo, "StayPuft", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);	
		//cairo_set_font_size(cairo, fontSize);
		

		cairo_set_font_size(cairo, fontSize);
		cairo_select_font_face(cairo, "DejaVu Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);	
		cairo_move_to(cairo, 0, fontSize);
		cairo_set_source_rgb(cairo, 0, 0, 0);
		cairo_paint(cairo);
		cairo_set_source_rgb(cairo, color.z, color.y, color.x);
		cairo_show_text(cairo, displayed.c_str());
		
		bind_texture(texture, 0);
		data = cairo_image_surface_get_data(cairo_surface);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture_width(texture), texture_height(texture), GL_RGBA, GL_UNSIGNED_BYTE, data);
		unbind_texture(texture);
	}

	using namespace std;
	static camera_ref cam;
	static mesh_ref mesh;
	static shader_ref text_shader;
	static texture_ref texture;


	void Label::initialize_gui_overlay() {
		static bool initialized = false;		
		if (initialized) return;
		initialized = true;

		vec3f cam_pos = {0,0,0}, cam_dir = {0,0,-1}, cam_up = {0,1,0};
		cam = make_orthographic_cam((char*)"gui cam", &cam_pos, &cam_dir, &cam_up, 50, 0, 50, 0, 0.01, 1000);

		mesh = make_mesh("quad", 2);
		vec3f pos[4] = { {0,0,-10}, {1,0,-10}, {1,1,-10}, {0,1,-10} };
		vec2f tc[4] = { {0,1}, {1,1}, {1,0}, {0,0} };
		unsigned int idx[6] = { 0, 1, 2, 2, 3, 0 };
		
		bind_mesh_to_gl(mesh);
		add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, 4, 3, (float *) pos, GL_STATIC_DRAW);
		add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, 4, 2, (float *) tc, GL_STATIC_DRAW);
		add_index_buffer_to_mesh(mesh, 6, idx, GL_STATIC_DRAW);
		unbind_mesh_from_gl(mesh);


		text_shader = find_shader("text-shader");
	}

	void Label::render_gui_overlay(bool gameover) {
		
		initialize_gui_overlay();
		// switch the camera	
		camera_ref old_cam = current_camera();
		use_camera(cam);
		
		// bind the shader	
		bind_shader(text_shader);

		int loc = glGetUniformLocation(gl_shader_object(text_shader), "proj");
		glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

		loc = glGetUniformLocation(gl_shader_object(text_shader), "view");
		glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);


//		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);

		texture_ref tex;
		matrix4x4f model;

		// Render score header
		tex = score_header_texture;

		make_unit_matrix4x4f(&model);
		model.row_col(0,0) = (float)texture_width(tex)*2.f / (float)texture_height(tex);
		model.row_col(1,1) = 2.f;
		model.row_col(0,3) = 37.f;
		model.row_col(1,3) = 0.5f;

		loc = glGetUniformLocation(gl_shader_object(text_shader), "model");
		glUniformMatrix4fv(loc, 1, GL_FALSE, model.col_major);

		bind_texture(tex, 0);
		loc = glGetUniformLocation(gl_shader_object(text_shader), "tex");
		glUniform1i(loc, 0);
		

		bind_mesh_to_gl(mesh);
		draw_mesh(mesh);
		unbind_mesh_from_gl(mesh);

		// Render score
		tex = score_texture;

		make_unit_matrix4x4f(&model);
		model.row_col(0,0) = (float)texture_width(tex)*3.f / (float)texture_height(tex);
		model.row_col(1,1) = 2.f;
		model.row_col(0,3) = 44.f;
		model.row_col(1,3) = 0.5f;

		loc = glGetUniformLocation(gl_shader_object(text_shader), "model");
		glUniformMatrix4fv(loc, 1, GL_FALSE, model.col_major);

		bind_texture(tex, 0);
		loc = glGetUniformLocation(gl_shader_object(text_shader), "tex");
		glUniform1i(loc, 0);


		bind_mesh_to_gl(mesh);
		draw_mesh(mesh);
		unbind_mesh_from_gl(mesh);


		// Render health header
		tex = health_header_texture;

		make_unit_matrix4x4f(&model);
		model.row_col(0,0) = (float)texture_width(tex)*2.f / (float)texture_height(tex);
		model.row_col(1,1) = 2.f;
		model.row_col(0,3) = 37.f;
		model.row_col(1,3) = 46.5f;

		loc = glGetUniformLocation(gl_shader_object(text_shader), "model");
		glUniformMatrix4fv(loc, 1, GL_FALSE, model.col_major);

		bind_texture(tex, 0);
		loc = glGetUniformLocation(gl_shader_object(text_shader), "tex");
		glUniform1i(loc, 0);
		

		bind_mesh_to_gl(mesh);
		draw_mesh(mesh);
		unbind_mesh_from_gl(mesh);

		// Render health
		tex = health_texture;

		make_unit_matrix4x4f(&model);
		model.row_col(0,0) = (float)texture_width(tex)*3.f / (float)texture_height(tex);
		model.row_col(1,1) = 2.f;
		model.row_col(0,3) = 43.f;
		model.row_col(1,3) = 46.5f;

		loc = glGetUniformLocation(gl_shader_object(text_shader), "model");
		glUniformMatrix4fv(loc, 1, GL_FALSE, model.col_major);

		bind_texture(tex, 0);
		loc = glGetUniformLocation(gl_shader_object(text_shader), "tex");
		glUniform1i(loc, 0);

		bind_mesh_to_gl(mesh);
		draw_mesh(mesh);
		unbind_mesh_from_gl(mesh);
		

		unbind_shader(text_shader);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		unbind_shader(text_shader);

		glDepthMask(GL_TRUE);
//		glDisable(GL_BLEND);
		if(gameover){
			bind_shader(text_shader);	
			// Render gameover score
			tex = gameover_score_texture;
			
			make_unit_matrix4x4f(&model);
			model.row_col(0,0) = (float)texture_width(tex)*4.f / (float)texture_height(tex);
			model.row_col(1,1) = 3.f;
			//model.row_col(0,3) = 18.f;
			model.row_col(0,3) = 2.f;
			model.row_col(1,3) = 10.5f;

			loc = glGetUniformLocation(gl_shader_object(text_shader), "model");
			glUniformMatrix4fv(loc, 1, GL_FALSE, model.col_major);

			bind_texture(tex, 0);
			loc = glGetUniformLocation(gl_shader_object(text_shader), "tex");
			glUniform1i(loc, 0);


			bind_mesh_to_gl(mesh);
			draw_mesh(mesh);
			unbind_mesh_from_gl(mesh);
			unbind_shader(text_shader);

			glDepthMask(GL_TRUE);
			//glDisable(GL_BLEND);		
			
		}


		use_camera(old_cam);
	}



