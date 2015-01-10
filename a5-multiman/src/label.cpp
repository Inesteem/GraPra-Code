#include "label.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>
using namespace std;


void Label::setup_display(){
	// make texture_ref invalid	
	texture.id = -1;
	make_gui_texture();
	initialize_gui_overlay();

	make_unit_matrix4x4f(&model);
	model.row_col(0,0) = (float)texture_width(texture)*3.f / (float)texture_height(texture);
	model.row_col(1,1) = 2.f;
	model.row_col(0,3) = 44.f;
	model.row_col(1,3) = 0.5f;		
	
}

// Konstruktors

Label::Label(){

	texture_name = "label_texture";	
	label_shader = find_shader("text-shader");
	label_cam = current_camera();	
	cairo_surface = 0;
	cairo_surface_data = 0;
	nChars = 3;
	fontSize = 20;
	
	
}

Label::Label(int fontSize, int nChars, const char *camera_name,const char *shader_name){
	
	Label();
	this->nChars = nChars;
	this->fontSize = fontSize;
	label_cam = find_camera(camera_name);
	label_shader = find_shader(shader_name);

}

// Setters

void Label::set_nChars(unsigned int nChars){

	this->nChars = nChars;

}
	
void Label::set_fontSize(unsigned int fontSize){

	this->fontSize = fontSize;

}

void Label::set_shader(const char *shader_name){

	label_shader = find_shader(shader_name);

}

void Label::set_camera(const char *camera_name){

	label_cam = find_camera(camera_name);

}

// Intern helper functions

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

void Label::do_cairo_stuff(std::string displayed, vec3f color, std::string name){
		cairo = create_cairo_context(nChars * fontSize, fontSize, 4, &cairo_surface, &cairo_surface_data);

		if (!valid_texture_ref(texture)) {
			tex_params_t p = default_fbo_tex_params();
			p.wrap_s = GL_CLAMP_TO_EDGE;
			p.wrap_t = GL_CLAMP_TO_EDGE;
			texture = make_empty_texture(name.c_str(), nChars * fontSize, fontSize, GL_TEXTURE_2D, GL_RGBA8, GL_UNSIGNED_BYTE, GL_RGBA, &p);
		}
		
		
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


		save_texture_as_png(texture, "./test.png");
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


void Label::make_gui_texture() {
	
	std::stringstream stream;
	stream << 0;
	std::string sn = stream.str();
	do_cairo_stuff(sn, texture_color, texture_name);
	
}


void Label::update_gui_texture_int(int n){
	// update health_texture
	std::stringstream stream;
	stream << n;
	std::string sn = stream.str();
	do_update_cairo_stuff(texture, sn, texture_color, texture_name);

}


void Label::update_gui_texture_long(long l){
	// update health_texture
		std::stringstream stream;
		int secs = (int)l/1000;
		//secs %= 60;
		update_gui_texture_int(secs);
	
}


void Label::initialize_gui_overlay() {

	mesh = make_mesh("quad", 2);
	vec3f pos[4] = { {-0.5,0,-0.5}, {0.5,0,-0.5}, {0.5,0,0.5}, {-0.5,0,0.5} };
	//vec3f pos[4] = { {0,0,-10}, {1,0,-10}, {1,1,-10}, {0,1,-10} };
	vec2f tc[4] = { {0,1}, {1,1}, {1,0}, {0,0} };
	unsigned int idx[6] = { 0, 1, 2, 2, 3, 0 };
	
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, 4, 3, (float *) pos, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, 4, 2, (float *) tc, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(mesh, 6, idx, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);
	
}

void Label::render_gui_overlay() {
	
		
	camera_ref old_cam = current_camera();
	use_camera(find_camera("lcam"));
		
	bind_shader(label_shader);

	int loc = glGetUniformLocation(gl_shader_object(label_shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

	loc = glGetUniformLocation(gl_shader_object(label_shader), "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);

	glDepthMask(GL_FALSE);

	loc = glGetUniformLocation(gl_shader_object(label_shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, model.col_major);

	bind_texture(texture, 0);
	loc = glGetUniformLocation(gl_shader_object(label_shader), "tex");
	glUniform1i(loc, 0);

	bind_mesh_to_gl(mesh);
	draw_mesh_as(mesh,GL_TRIANGLE_STRIP);
	unbind_mesh_from_gl(mesh);

	unbind_shader(label_shader);

	glDepthMask(GL_TRUE);

	use_camera(old_cam);
	unbind_texture(texture);
}

//some new stuff

void Label::update_label_model(matrix4x4f model){
	this-> model = model;
}
void Label::update_label_pos(float x, float y, long l){
	
	make_unit_matrix4x4f(&model);	
	model.row_col(0,3) = x;
	model.row_col(2,3) = y;
	//model.row_col(2,3) = z;

	float secs = l/10;

	float angle = -90*M_PI/180;

	matrix4x4f scale;
	vec3f svec = {0.07, 0.1,0.1};
//	vec3f svec = {0.035, 0.035,0.035};
	make_scale_matrix4x4f(&scale,&svec);
	multiply_matrices4x4f(&model,&model,&scale);
	
	matrix4x4f rot;
	vec3f axis = {1,0,0};
	make_rotation_matrix4x4f(&rot, &axis,angle);
//	multiply_matrices4x4f(&model,&model,&rot);
// model = rot * model;
}