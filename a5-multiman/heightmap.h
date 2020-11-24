#ifndef __HEIGHTMAP_H__
#define __HEIGHTMAP_H__

#include <libmcm-0.0.1/vectors.h>
#include <libmcm-0.0.1/camera-matrices.h>
#include <gl-version.h>
#include <glut.h>
#include <cgl.h>
#include <mesh.h>
#include <shader.h>
#include <camera.h>
#include <texture.h>

#include <cmath>
#include <vector>
#include <algorithm>
#include "rendering.h"

#include <impex.h>

struct Vertex {
	Vertex(vec3f pos, vec3f norm, vec2f texcoord) {
		position[0] = pos.x;
		position[1] = pos.y;
		position[2] = pos.z;

		normal[0] = norm.x;
		normal[1] = norm.y;
		normal[2] = norm.z;

		tc[0] = texcoord.x;
		tc[1] = texcoord.y;
	};

	GLfloat position[3];
	GLfloat normal[3];
	GLfloat tc[2];
};

class heightmap {
protected: 
	vec3f *img;
	vec3f *blur_img;
	vec3f *blur2_img;
	unsigned int width;
	unsigned int height;
	shader_ref shader;
	
	//0 is basic chunking
	//1 is stitched chunking with LoD
	//2 is stitched chunking with LoD and culling
	int render_mode;

	// information about the map_size and chunk_size
	int chunk_k;
	int nLoD; // this is actually only chunk_k+1 but needed really often
	int chunk_size;
	int actual_width;
	int actual_height;

	// for stupid render_mode	
	int stupid_chunk_size;
	int stupid_actual_width;
	int stupid_actual_height;
	std::vector<Vertex> stupid_vertex_buffer;
	std::vector<unsigned int> stupid_index_buffer;
	GLuint stupid_vao;
	GLuint *stupid_ibo;
	
	// index buffer
	// case indices are
	// buffer for middle	 			[0] #nLoD   buffer
	// buffer for top_right corner 			[1] #1      buffer
	// buffer for right_seam(>) 			[2] #nLoD-1 buffer
	// buffer for right_seam(<) 			[3] #nLoD-1 buffer
	// buffer for right_seam(=)			[4] #nLoD   buffer
	// buffer for top_seam(>)  			[5] #nLoD-1 buffer
	// buffer for top_seam(<) 			[6] #nLoD-1 buffer
	// buffer for top_seam(=)			[7] #nLoD   buffer
	std::vector<std::vector<std::vector<unsigned int> > > index_buffer_LoD_type;
	// index_buffer_LoD_type[type][LoD][buffer];
	
	std::vector<Vertex> vertex_buffer;
	
	// For gl bindings
	GLuint vao;
	GLuint **ibo;

	void calc_blur_img(int range);

public:
	const int HEIGHT_FACTOR = 25;
	heightmap(const char *filename, int k);	
	~heightmap(){
		free(img);
		free(blur_img);
		free(blur2_img);
	}

	float sample_height(int x, int y);
//	float sample_blur_height(int x, int y);
	vec3f sample_normal(int x, int y);
	float get_max_surrounding_height(int xPos, int yPos, int dist);

	unsigned int map_width() const { return width;}
	unsigned int map_height() const { return height;}

	int get_LoD(int x, int y);
	bool check_invisible(int x, int y);

	void bindVAO(const std::vector<Vertex> &vertexdata, const std::vector<std::vector<std::vector<unsigned int> > > &indices);
	void bind_stupid_VAO(const std::vector<Vertex> &vertexdata, const std::vector<unsigned int> &indices);
	void draw();

	void set_render_mode(int render_mode){
		this->render_mode = render_mode;
	}
};

#endif
