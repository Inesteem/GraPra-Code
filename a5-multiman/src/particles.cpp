#include <iostream>
#include <GL/glew.h>
#include <libguile.h>

#include "particles.h"
#include "rendering.h"

using namespace std;

void register_scheme_functions_for_particles();

Particles::Particles(unsigned int N, float liftetime_factor, float size, vec3f &color) : color(color), N(N), position(0), direction(0), lifetime(0), start(0), end(0), particle_lifetime_factor(liftetime_factor), particle_size(size) {
	position = new vec3f[N];
	direction = new vec3f[N];
	lifetime = new float[N];
	index_buffer = new unsigned int[N];
	for (int i = 0; i < N; ++i) index_buffer[i] = i;
	mesh = make_mesh("particle-data", 2);
	bind_mesh_to_gl(mesh);
	add_vertex_buffer_to_mesh(mesh, "particle-positions", GL_FLOAT, N, 3, 0, GL_DYNAMIC_DRAW);
	add_vertex_buffer_to_mesh(mesh, "particle-lifetimes", GL_FLOAT, N, 1, 0, GL_DYNAMIC_DRAW);
	add_index_buffer_to_mesh(mesh, N, index_buffer, GL_STATIC_DRAW);
	unbind_mesh_from_gl(mesh);

	shader = find_shader("particle-flare-shader");
	if (!valid_shader_ref(shader))
		cerr << "ERROR: particle shader not found." << endl;


	tex = find_texture("test");
	if (!valid_texture_ref(tex))
		cerr << "ERROR: particle texture not found." << endl;

// 	register_scheme_functions_for_particles();
}

void Particles::ChangeColor(vec3f &col)
{
    color = col;
}

Particles::~Particles() {
	delete [] position;
	delete [] direction;
	delete [] lifetime;
}

unsigned int Particles::advance(unsigned int &pointer) {
	pointer = (pointer+1) % N;
	return pointer;
}

void Particles::Add(vec3f &pos, vec3f &dir, int alive_for) {
	unsigned int at = end;
	if (advance(end) == start)
		advance(start);
	position[at] = pos;
	direction[at] = dir;
	lifetime[at] = alive_for * particle_lifetime_factor;
}

void Particles::Update() {
	int bound = (end >= start) ? end : end+N;
	for (int i = start; i < bound; ++i) {
		int idx = i%N;
		lifetime[idx] -= 1;
		if (lifetime[idx] <= 0 && idx == start)
			advance(start);
		vec3f step;
		mul_vec3f_by_scalar(&step, direction+idx, 0.25);
		add_components_vec3f(position+idx, position+idx, &step);
	}
	bind_mesh_to_gl(mesh);
	change_vertex_buffer_data(mesh, "particle-positions", GL_FLOAT, 3, (void*)position, GL_DYNAMIC_DRAW);
	change_vertex_buffer_data(mesh, "particle-lifetimes", GL_FLOAT, 1, (void*)lifetime, GL_DYNAMIC_DRAW);
	unbind_mesh_from_gl(mesh);
}

void Particles::Render() {
	int loc;

	glPointSize(20);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_TEST);

	bind_shader(shader);

	loc = glGetUniformLocation(gl_shader_object(shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

	loc = glGetUniformLocation(gl_shader_object(shader), "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);

	loc = glGetUniformLocation(gl_shader_object(shader), "screenres");
	glUniform2f(loc, render_settings::screenres_x, render_settings::screenres_y);
	
	loc = glGetUniformLocation(gl_shader_object(shader), "near_far");
	glUniform2f(loc, camera_near(current_camera()), camera_far(current_camera()));

    loc = glGetUniformLocation(gl_shader_object(shader), "color");
    glUniform3f(loc, color.x, color.y, color.z);

	loc = glGetUniformLocation(gl_shader_object(shader), "tex");
	bind_texture(tex, 0);
	glUniform1i(loc, 0);

	if (valid_texture_ref(depthtex)) {
		loc = glGetUniformLocation(gl_shader_object(shader), "depthtex");
		bind_texture(depthtex, 1);
		glUniform1i(loc, 1);
	}
	else
		cerr << "invalid depth texture for soft particles (" << depthtex.id << ") . enjoy." << endl;

	loc = glGetUniformLocation(gl_shader_object(shader), "particle_size");
	glUniform1f(loc, particle_size);

	loc = glGetUniformLocation(gl_shader_object(shader), "max_lifetime");
	glUniform1f(loc, max_particle_lifetime);

	static int frame = 0;
	bind_mesh_to_gl(mesh);
	if (start < end) {
 		glDrawElementsBaseVertex(GL_POINTS, end-start, GL_UNSIGNED_INT, 0, start);
	}
	else if (end < start) {
 		glDrawElementsBaseVertex(GL_POINTS, N-end-1, GL_UNSIGNED_INT, 0, start);
 		glDrawElementsBaseVertex(GL_POINTS, end, GL_UNSIGNED_INT, 0, 0);
	}
	unbind_texture(depthtex);
	unbind_mesh_from_gl(mesh);
	unbind_shader(shader);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void Particles::RegisterDepthTex(texture_ref depth) {
	depthtex = depth;
}

texture_ref Particles::depthtex, Particles::wpostex, Particles::normtex, Particles::difftex;

Particles *particles = 0;

int max_particle_lifetime = 20;
SCM_DEFINE(s_set_particle_lifetime, "set-particle-lifetime!", 1, 0, 0, (SCM t), "") {
	int i = scm_to_int(t);
	max_particle_lifetime = i;
	return t;
}

SCM_DEFINE(s_set_particle_size, "set-particle-size!", 1, 0, 0, (SCM t), "") {
	float f = scm_to_double(t);
// 	particle_size = f;
	return t;
}

void register_scheme_functions_for_particles() {
#ifndef SCM_MAGIC_SNARFER
#include "particles.x"
#endif
}
