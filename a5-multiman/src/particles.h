#ifndef __PARTICLES_H__ 
#define __PARTICLES_H__ 

#include <shader.h>
#include <mesh.h>
#include <texture.h>

struct Particles {
	unsigned int N;
	vec3f *position;
	vec3f *direction;
    vec3f color;
	float *lifetime;
	unsigned int *index_buffer;
	unsigned int start, end;
	mesh_ref mesh;
	shader_ref shader;
	texture_ref tex;
	static texture_ref depthtex, wpostex, normtex, difftex;
	float particle_lifetime_factor;
	float particle_size;

public:
    Particles(unsigned int N, float lifetime_factor, float size, vec3f &color);
	~Particles();
	unsigned int advance(unsigned int &pointer);
	void Add(vec3f &pos, vec3f &dir, int alive_for);
	void Update();
	void Render();
	static void RegisterDepthTex(texture_ref depth);
    void ChangeColor(vec3f &color);
};
extern int max_particle_lifetime;

#endif

