#include "label.h"
#include "gameobject.h"

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
	cairo_surface = 0;
	cairo_surface_data = 0;
	nChars = 3;
	fontSize = 20;
	use_cam = false;
	BillboardSize = vec2f(1.0,1.0);
}

Label::Label(int fontSize, int nChars, const char *shader_name){
	
	Label();
	this->nChars = nChars;
	this->fontSize = fontSize;
	label_shader = find_shader(shader_name);

}


// Setters

void Label::set_nChars(unsigned int nChars){

	this->nChars = nChars;

}

void Label::set_color(vec3f color){
	this->color = color;
}

	
void Label::set_fontSize(unsigned int fontSize){

	this->fontSize = fontSize;

}

void Label::set_shader(const char *shader_name){
	label_shader = find_shader(shader_name);
}


void Label::set_shader(shader_ref shader){
	label_shader = shader;
}


void Label::set_camera(camera_ref camera){
	use_cam = true;
	label_camera = camera;
}

void Label::change_mesh(){
	
	mesh = make_mesh("quad", 2);
	vec3f pos[4] = { {0,0,-10}, {1,0,-10}, {1,1,-10}, {0,1,-10} };
	vec2f tc[4] = { {0,1}, {1,1}, {1,0}, {0,0} };
	unsigned int idx[6] = { 0, 1, 2, 2, 3, 0 };
	
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, 4, 3, (float *) pos, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, 4, 2, (float *) tc, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(mesh, 6, idx, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);
}

void Label::set_size(vec2f size){
	BillboardSize = size;
}

void Label::set_texture(const char *name){
	texture = find_texture(name);
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

void Label::do_cairo_stuff(std::string displayed){
		fontSize +=2;
		cairo = create_cairo_context(nChars * fontSize, fontSize, 4, &cairo_surface, &cairo_surface_data);

		if (!valid_texture_ref(texture)) {
			tex_params_t p = default_fbo_tex_params();
			p.wrap_s = GL_CLAMP_TO_EDGE;
			p.wrap_t = GL_CLAMP_TO_EDGE;
			texture = make_empty_texture(texture_name.c_str(), nChars * fontSize, fontSize, GL_TEXTURE_2D, GL_RGBA8, GL_UNSIGNED_BYTE, GL_RGBA, &p);
		}
		
	do_update_cairo_stuff(" 1000 ");

}

void Label::do_update_cairo_stuff(std::string displayed){


	cairo_set_font_size(cairo, fontSize);
//	cairo_select_font_face(cairo, "DejaVu Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);	
//	cairo_select_font_face(cairo, "Trebuchet MS Bold Italic", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);	
	cairo_select_font_face(cairo, "Impact", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);	
	cairo_move_to(cairo, 0, fontSize-2);
	cairo_set_source_rgb(cairo, 0, 0, 0);
	cairo_paint(cairo);
	cairo_set_source_rgb(cairo, 1, 1, 1);
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
	do_cairo_stuff(sn);
	
}


void Label::update_gui_texture_int(int n){

	std::stringstream stream;
	stream << n;
	std::string sn = stream.str();
	do_update_cairo_stuff(sn);
//	do_cairo_stuff(sn, texture_color, texture_name);
}


void Label::update_gui_texture_string(std::stringstream *stream){
	std::string sn = stream->str();
	do_update_cairo_stuff(sn);
}


void Label::set_model(matrix4x4f model){
	this-> model = model;		
}


void Label::initialize_gui_overlay() {

	mesh = make_mesh("quad", 2);
    vec3f pos[4] = { {-0.5,-0.5,0}, {0.5,-0.5,0}, {0.5,0.5,0}, {-0.5,0.5,0} };	
	vec2f tc[4] = { {0,1}, {1,1}, {1,0}, {0,0} };
	unsigned int idx[6] = { 0, 1, 2, 2, 3, 0 };

//	mesh = make_mesh("quad", 2);
//	vec3f pos[4] = { {0,0,-10}, {1,0,-10}, {1,1,-10}, {0,1,-10} };
//	vec2f tc[4] = { {0,1}, {1,1}, {1,0}, {0,0} };
//	unsigned int idx[6] = { 0, 1, 2, 2, 3, 0 };


	
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, 4, 3, (float *) pos, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, 4, 2, (float *) tc, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(mesh, 6, idx, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);
	
}

void Label::render_gui_overlay() {

    glDepthMask(GL_TRUE);

	camera_ref old_camera = current_camera();
	if(use_cam)
		use_camera(label_camera);


	bind_shader(label_shader);

	int loc;

	matrix4x4f view = *gl_view_matrix_of_cam(current_camera());
	
	vec3f CameraRight_worldspace = {view.row_col(0,0), view.row_col(0,1), view.row_col(0,2)};
	vec3f CameraUp_worldspace = {view.row_col(1,0), view.row_col(1,1), view.row_col(1,2)};

	loc = glGetUniformLocation(gl_shader_object(label_shader), "CameraRight_worldspace");
	glUniform3fv(loc, 1,(float *)&CameraRight_worldspace);		
	
	loc = glGetUniformLocation(gl_shader_object(label_shader), "CameraUp_worldspace");
	glUniform3fv(loc, 1,(float *)&CameraUp_worldspace);		
	
	loc = glGetUniformLocation(gl_shader_object(label_shader), "BillboardPos");
	glUniform3fv(loc, 1,(float *)&pos);		
	
	loc = glGetUniformLocation(gl_shader_object(label_shader), "BillboardSize");
	glUniform2fv(loc, 1,(float *)&BillboardSize);		
	
	loc = glGetUniformLocation(gl_shader_object(label_shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);
		
	loc = glGetUniformLocation(gl_shader_object(label_shader), "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, view.col_major);

	
	loc = glGetUniformLocation(gl_shader_object(label_shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, model.col_major);

	bind_texture(texture, 0);
	unsigned char *data = cairo_image_surface_get_data(cairo_surface);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture_width(texture), texture_height(texture), GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	loc = glGetUniformLocation(gl_shader_object(label_shader), "tex");
	glUniform1i(loc, 0);
	
	loc = glGetUniformLocation(gl_shader_object(label_shader), "color");
	glUniform3fv(loc, 1,(float *)&color);		


	bind_mesh_to_gl(mesh);
	draw_mesh_as(mesh,GL_TRIANGLE_STRIP);
	unbind_mesh_from_gl(mesh);

	unbind_shader(label_shader);



	unbind_texture(texture);
	
	use_camera(old_camera);
}

texture_ref Label::get_texture(){
	return texture;
	
}

//some new stuff

void Label::update_label_model(matrix4x4f model){
	this-> model = model;
}
void Label::update_label_pos(float x, float y, float z){
	if(original_pos.x == -1)
		original_pos = vec3f(x,z,y);
	pos = vec3f(x,z,y);
	make_unit_matrix4x4f(&model);	
	model.row_col(0,3) = x;
	model.row_col(2,3) = y;
	model.row_col(1,3) = z;


	float angle = 90*M_PI/180;

	matrix4x4f scale;
	vec3f svec = {0.07, 0.1,0.1};
//	vec3f svec = {10, 10,10};
	make_scale_matrix4x4f(&scale,&svec);
	multiply_matrices4x4f(&model,&model,&scale);
	
	matrix4x4f rot;
	vec3f axis = {0,1,0};
	make_rotation_matrix4x4f(&rot, &axis,angle);
//	multiply_matrices4x4f(&model,&model,&rot);
}

void Label::recalculate_pos(){

	vec3f cam_pos, dir, new_pos;
	extract_pos_vec3f_of_matrix(&cam_pos, lookat_matrix_of_cam(current_camera()));
	dir = cam_pos - original_pos;
	normalize_vec3f(&dir);
	new_pos = cam_pos - (dir*10);
	update_label_pos(new_pos.x,new_pos.z,new_pos.y);
};




	/*SlideBar*/

SlideBar::SlideBar(){
	

	sbar_shader = find_shader("count-shader");
	max_count = 200;
	mom_count = 0;	
	pos = vec3f(0,0,0);
	screen_pos = vec3f(0,0,0);
	texture = find_texture("slidebar2");
	LifeLevel = 0;
	down = false;


	vec3f cam_pos = {0,0,0}, cam_dir = {0,0,-1}, cam_up = {0,1,0};
	sbar_camera = make_orthographic_cam((char*)"gui cam", &cam_pos, &cam_dir, &cam_up, 50, 0, 50, 0, 0.01, 1000);
}


void SlideBar::initialize_slidebar(){

	mesh = make_mesh("quad", 2);
	vec3f pos[4] = { {0,0,-10}, {1,0,-10}, {1,1,-10}, {0,1,-10} };
	vec2f tc[4] = { {0,1}, {1,1}, {1,0}, {0,0} };
	unsigned int idx[6] = { 0, 1, 2, 2, 3, 0 };

	
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, 4, 3, (float *) pos, GL_STATIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, 4, 2, (float *) tc, GL_STATIC_DRAW);
	add_index_buffer_to_mesh(mesh, 6, idx, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);
	

	label_mom_count.setup_display();
	label_mom_count.set_camera(sbar_camera);
	label_mom_count.set_size(vec2f(3,1));
	
}


void SlideBar::set_max_count(int count){
	max_count = count;
}

void SlideBar::dec_max_count(int count){
	max_count -= count;
}
//wird eigentlich nicht benoetigt
void SlideBar::update_unit_count(int count){
	mom_count = count;
}


void SlideBar::render_slidebar(){

	label_mom_count.update_gui_texture_int(mom_count);
	camera_ref old_camera = current_camera();
	use_camera(sbar_camera);
	bind_shader(sbar_shader);
	sbar_shader = find_shader("count-shader");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glDepthMask(GL_FALSE);	

	int loc;


	loc = glGetUniformLocation(gl_shader_object(sbar_shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, bar_model.col_major);
	
	loc = glGetUniformLocation(gl_shader_object(sbar_shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);
		
	loc = glGetUniformLocation(gl_shader_object(sbar_shader), "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);


	bind_texture(texture, 0);
	loc = glGetUniformLocation(gl_shader_object(sbar_shader), "tex");
	glUniform1i(loc, 0);
	
	loc = glGetUniformLocation(gl_shader_object(sbar_shader), "LifeLevel");
	glUniform1f(loc,LifeLevel);

	float d = 0;
	if(down)
		d = 1;

	loc = glGetUniformLocation(gl_shader_object(sbar_shader), "down");
	glUniform1f(loc,d);

	bind_mesh_to_gl(mesh);
	draw_mesh_as(mesh,GL_TRIANGLE_STRIP);
	unbind_mesh_from_gl(mesh);

	
	label_mom_count.render_gui_overlay();

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);	
	unbind_shader(sbar_shader);
	unbind_texture(texture);
	use_camera(old_camera);
}

void SlideBar::update_mouse_pos(float x, float y){
	mom_count = 0;
	float LifeLevel_max = 0.955;
	float mouse_diff_max = 187;
	float mouse_diff = screen_pos.y - y;
	LifeLevel = 0;
	
	if(mouse_diff == 0 || max_count == 0)
		return;
	
	int sign = 1;
	if(down)
		sign = -1;
		
	LifeLevel = mouse_diff/mouse_diff_max;
	if(down){
		
		if(LifeLevel >= 0){
			LifeLevel = 0;	
			mom_count = 0;
			return;
		}
		if(LifeLevel < -LifeLevel_max)
			LifeLevel = -LifeLevel_max;		
	
		if(LifeLevel < 0){
			float loc_count = (max_count * (-LifeLevel+1-LifeLevel_max));
			if(loc_count == 0){		
				mom_count = 0;
				return;
			}			
			mom_count = loc_count;
			if(mom_count - loc_count <= 0.999)
				mom_count++;
		}
	} else {
			
		if(LifeLevel <= 0){
			LifeLevel = 0;	
			mom_count = 0;
			return;
		}
			
		if(LifeLevel >= LifeLevel_max)
			LifeLevel = LifeLevel_max;
		
		if(LifeLevel > 0){
			float loc_count = (max_count * (LifeLevel+1-LifeLevel_max));
			if(loc_count == 0){	
				mom_count = 0;
				return;
			}
			mom_count = loc_count;
			if(mom_count - loc_count <= 0.999)
				mom_count++;
		}
	}
	


}


void SlideBar::update_pos(float x, float y){

	screen_pos = vec3f(x,y,0);
	
	if(y-200 < 0){
		down = true;
		y += 200;
	}	
	camera_ref old_camera = current_camera();
	use_camera(sbar_camera);	
	pos = moac::ClickWorldPosition(x, y);
	vec3f label_pos = moac::ClickWorldPosition(x+21, y+4);
	use_camera(old_camera);

	float angle = 90*M_PI/180;
	
	make_unit_matrix4x4f(&bar_model);	
	bar_model.row_col(0,0) = 0.2;
	bar_model.row_col(1,1) = 10.f;
	bar_model.row_col(0,3) = pos.x;
	bar_model.row_col(1,3) = pos.y;	

	matrix4x4f scale;
	vec3f svec = {7, 1,1};
	make_scale_matrix4x4f(&scale,&svec);
	multiply_matrices4x4f(&bar_model,&bar_model,&scale);

	matrix4x4f rot;
	vec3f axis = {1,0,0};
	make_rotation_matrix4x4f(&rot, &axis,angle);
//	multiply_matrices4x4f(&model,&model,&rot);
//	model *= rot;

	label_mom_count.update_label_pos(label_pos.x, label_pos.z, label_pos.y);
	
	

	matrix4x4f m_label;
	make_unit_matrix4x4f(&m_label);	
	m_label.row_col(0,0) = 2;
	m_label.row_col(1,1) = 100.f;
	m_label.row_col(0,3) = pos.x;
	m_label.row_col(1,3) = pos.y;			
	
	matrix4x4f s_label;
	vec3f svec_label = {70, 1,1};
	make_scale_matrix4x4f(&s_label,&svec_label);
	multiply_matrices4x4f(&m_label,&m_label,&s_label);

//	label_mom_count.update_label_model(m_label);	
	
}

void SlideBar::reset_bar(){
	mom_count = 0;
	LifeLevel = 0;
	down = false;
}

//Setters

void SlideBar::set_shader(const char *shader_name){
	sbar_shader = find_shader(shader_name);
}
void SlideBar::set_texture(const char *texture_name){
	texture = find_texture(texture_name);
}
void SlideBar::set_camera(const char *camera_name){
	sbar_camera = find_camera(camera_name);
}

int SlideBar::get_unit_count(){
	return mom_count;
}
