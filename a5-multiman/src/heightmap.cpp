#include "heightmap.h"


heightmap::heightmap(const char *filename, int k){
	img = load_image3f(filename, &width, &height);


	//render_mode 0 stupid
	//render_mode 1 better
	//render_mode 2 best
	render_mode = 2;

	chunk_k = k;
	nLoD = k + 1;
	chunk_size = pow(2, chunk_k)+2;

	//shader = find_shader("pos-only");//+norm+tc");
	shader = find_shader("terrain");


	// soo stupid
	stupid_chunk_size = pow(2, chunk_k)+1;
	stupid_actual_height = height-1 + stupid_chunk_size - (height-1) % (stupid_chunk_size-1);
	stupid_actual_width = width-1 + stupid_chunk_size - (width-1) % (stupid_chunk_size-1);
	// Generating stupid vertex buffer
	for (int i = 0; i < stupid_actual_height; i++){
		for (int k = 0; k < stupid_actual_width; k++){
			Vertex v = Vertex(vec3f(i, HEIGHT_FACTOR*sample_height(i, k), k), sample_normal(i, k), vec2f(i, k));
			stupid_vertex_buffer.push_back(v);
		}
	}

	// Buffer for render_mode 0 (stupid mode)
	for (int i = 0; i < stupid_chunk_size-1; i++){
		for (int k = 0; k < stupid_chunk_size; k++){
			stupid_index_buffer.push_back(i*stupid_actual_width+k);
			stupid_index_buffer.push_back((i+1)*stupid_actual_width+k);
			if (k == stupid_chunk_size-1){
				stupid_index_buffer.push_back((i+1)*stupid_actual_width+k);
				stupid_index_buffer.push_back((i+1)*stupid_actual_width+k-stupid_chunk_size+1);
			}
		}	
	}
	bind_stupid_VAO(stupid_vertex_buffer, stupid_index_buffer);
	//DONE

	// Generating the real vertex buffer
	actual_height = height-1 + chunk_size - (height-1) % (chunk_size-1);
	actual_width = width-1 + chunk_size - (width-1) % (chunk_size-1);
	for (int i = 0; i < actual_height; i++){
		for (int k = 0; k < actual_width; k++){
			Vertex v = Vertex(vec3f(i, HEIGHT_FACTOR*sample_height(i, k), k), sample_normal(i, k), vec2f(i, k));
			vertex_buffer.push_back(v);
		}
	}
	// End


	// Generating index buffers
	// <= chunk_k because there are chunk_k + 1 different possible resolutions for a chunk
	//
	std::vector<std::vector<unsigned int> > index_buffer_LoD;
	std::vector<unsigned int> index_buffer;
	
	// middle buffer
	for (int res = 0; res <= chunk_k; res++){
		int unit = pow(2, res);
		for (int i = 0; i < chunk_size-2; i += unit){
			for (int k = 0; k < chunk_size-1; k += unit){
				index_buffer.push_back(i*actual_width+k);
				index_buffer.push_back((i+unit)*actual_width+k);
				if (k == chunk_size-2){
					index_buffer.push_back((i+unit)*actual_width+k);
					index_buffer.push_back((i+unit)*actual_width+k-chunk_size+2);
				}
			}	
		}
		index_buffer_LoD.push_back(index_buffer);
		index_buffer.clear();
	}
	// always add the buffer to global buffer and clear afterwards
	index_buffer_LoD_type.push_back(index_buffer_LoD);
	index_buffer_LoD.clear();
	//DONE
	
	// Generating the seam buffers
	
	// Buffer for top_right corner
	index_buffer.push_back((chunk_size-2) * actual_width + chunk_size-2);
	index_buffer.push_back((chunk_size-1) * actual_width + chunk_size-2);
	index_buffer.push_back((chunk_size-2) * actual_width + chunk_size-1);
	index_buffer.push_back((chunk_size-1) * actual_width + chunk_size-1);

	index_buffer_LoD.push_back(index_buffer);

	index_buffer_LoD_type.push_back(index_buffer_LoD);
	index_buffer_LoD.clear();
	index_buffer.clear();
	//DONE
	

	// seam(>) means that next_LoD > current_LoD
	// Buffers for right seam(>)
	for (int res = 0; res < chunk_k; res++){
		int unit = pow(2, res);
		for (int i = 0; i < chunk_size-2; i += 2*unit){
			index_buffer.push_back(i*actual_width + chunk_size-2);
			index_buffer.push_back(i*actual_width + chunk_size-1);
			index_buffer.push_back((i+unit)*actual_width + chunk_size-2);
			index_buffer.push_back((i+2*unit)*actual_width + chunk_size-1);
			index_buffer.push_back((i+2*unit)*actual_width + chunk_size-2);
		}
		index_buffer_LoD.push_back(index_buffer);
		index_buffer.clear();
	}
	index_buffer_LoD_type.push_back(index_buffer_LoD);
	index_buffer_LoD.clear();
	//DONE
	
	// Buffers for right seam(<)
	for (int res = 1; res <= chunk_k; res++){
		int unit = pow(2, res);
		for (int i = 0; i < chunk_size-2; i += unit){
			index_buffer.push_back(i*actual_width + chunk_size-1);
			index_buffer.push_back(i*actual_width + chunk_size-2);
			index_buffer.push_back((i+unit/2)*actual_width + chunk_size-1);
			index_buffer.push_back((i+unit)*actual_width + chunk_size-2);
			index_buffer.push_back((i+unit)*actual_width + chunk_size-1);
		}
		index_buffer_LoD.push_back(index_buffer);
		index_buffer.clear();
	}
	
	index_buffer_LoD_type.push_back(index_buffer_LoD);
	index_buffer_LoD.clear();
	//DONE
	
	// Buffer for right seam(=)
	for (int res = 0; res <= chunk_k; res++){
		int unit = pow(2, res);
		for (int i = 0; i < chunk_size-2; i += unit){
			index_buffer.push_back(i*actual_width + chunk_size-2);
			index_buffer.push_back(i*actual_width + chunk_size-1);
			index_buffer.push_back((i+unit)*actual_width + chunk_size-2);
			index_buffer.push_back((i+unit)*actual_width + chunk_size-1);
			index_buffer.push_back((i+unit)*actual_width + chunk_size-2);
		}
		index_buffer_LoD.push_back(index_buffer);
		index_buffer.clear();
	}
	index_buffer_LoD_type.push_back(index_buffer_LoD);
	index_buffer_LoD.clear();
	//DONE

	// Buffers for top seam(>)
	for (int res = 0; res < chunk_k; res++){
		int unit = pow(2, res);
		for (int k = 0; k < chunk_size-2; k += 2*unit){
			index_buffer.push_back((chunk_size-2)*actual_width + k);
			index_buffer.push_back((chunk_size-1)*actual_width + k);
			index_buffer.push_back((chunk_size-2)*actual_width + k+unit);
			index_buffer.push_back((chunk_size-1)*actual_width + k+2*unit);
			index_buffer.push_back((chunk_size-2)*actual_width + k+2*unit);
		}
		index_buffer_LoD.push_back(index_buffer);
		index_buffer.clear();
	}

	index_buffer_LoD_type.push_back(index_buffer_LoD);
	index_buffer_LoD.clear();
	//DONE

	// Buffers for top seam(<)
	for (int res = 1; res <= chunk_k; res++){
		int unit = pow(2, res);
		for (int k = 0; k < chunk_size-2; k += unit){
			index_buffer.push_back((chunk_size-1)*actual_width + k);
			index_buffer.push_back((chunk_size-2)*actual_width + k);
			index_buffer.push_back((chunk_size-1)*actual_width + k+unit/2);
			index_buffer.push_back((chunk_size-2)*actual_width + k+unit);
			index_buffer.push_back((chunk_size-1)*actual_width + k+unit);
		}
		index_buffer_LoD.push_back(index_buffer);
		index_buffer.clear();
	}

	index_buffer_LoD_type.push_back(index_buffer_LoD);
	index_buffer_LoD.clear();
	//DONE

	// Buffer for top seam(=)
	for (int res = 0; res <= chunk_k; res++){
		int unit = pow(2, res);
		for (int k = 0; k < chunk_size-2; k += unit){
			index_buffer.push_back((chunk_size-2)*actual_width + k);
			index_buffer.push_back((chunk_size-1)*actual_width + k);
			index_buffer.push_back((chunk_size-2)*actual_width + k + unit);
			index_buffer.push_back((chunk_size-1)*actual_width + k + unit);
		}
		index_buffer_LoD.push_back(index_buffer);
		index_buffer.clear();
	}
	index_buffer_LoD_type.push_back(index_buffer_LoD);
	index_buffer_LoD.clear();
	//DONE


	// End

	bindVAO(vertex_buffer, index_buffer_LoD_type);
}


float heightmap::sample_height(int x, int y){
	if ((x < 0) || (y < 0) || (x >= height) || (y >= width)) return 0;
	return img[x*width+y].x;
}


float heightmap::get_max_surrounding_height(int xPos, int yPos, int dist){
	float  maximum = 0.0f;
	for (int i = 0; i <= dist; i++){
		for (int k = 0; k <= dist; k++){
			maximum = std::max(maximum, HEIGHT_FACTOR*sample_height(xPos+i, yPos+k));	
		}
	}	
	for (int i = 0; i <= dist; i++){
		for (int k = 0; k <= dist; k++){
			maximum = std::max(maximum, HEIGHT_FACTOR*sample_height(xPos-i, yPos+k));	
		}
	}	
	for (int i = 0; i <= dist; i++){
		for (int k = 0; k <= dist; k++){
			maximum = std::max(maximum, HEIGHT_FACTOR*sample_height(xPos+i, yPos-k));	
		}
	}	
	for (int i = 0; i <= dist; i++){
		for (int k = 0; k <= dist; k++){
			maximum = std::max(maximum, HEIGHT_FACTOR*sample_height(xPos-i, yPos-k));	
		}
	}	
	return maximum;
}

vec3f heightmap::sample_normal(int x, int y){
	//cross_vec3f(out, v1, v2)
	float cur_height = sample_height(x, y);
	vec3f v1 = vec3f(1,  HEIGHT_FACTOR*(sample_height(x+1, y)-cur_height), 0);
	vec3f v2 = vec3f(1,  HEIGHT_FACTOR*(sample_height(x+1, y+1)-cur_height), 1);
	vec3f v3 = vec3f(0,  HEIGHT_FACTOR*(sample_height(x, y+1)-cur_height), 1);
	vec3f v4 = vec3f(-1,  HEIGHT_FACTOR*(sample_height(x-1, y+1)-cur_height), 1);
	vec3f v5 = vec3f(-1,  HEIGHT_FACTOR*(sample_height(x-1, y)-cur_height), 0);
	vec3f v6 = vec3f(-1,  HEIGHT_FACTOR*(sample_height(x-1, y-1)-cur_height), -1);
	vec3f v7 = vec3f(0,  HEIGHT_FACTOR*(sample_height(x, y-1)-cur_height), -1);
	vec3f v8 = vec3f(1,  HEIGHT_FACTOR*(sample_height(x+1, y-1)-cur_height), -1);

	// 4 3 2
	// 5 x 1
	// 6 7 8
	normalize_vec3f(&v1);
	normalize_vec3f(&v2);
	normalize_vec3f(&v3);
	normalize_vec3f(&v4);
	normalize_vec3f(&v5);
	normalize_vec3f(&v6);
	normalize_vec3f(&v7);
	normalize_vec3f(&v8);
	vec3f n1, n2, n3, n4, n5, n6, n7, n8;
	cross_vec3f(&n1, &v1, &v2);
	cross_vec3f(&n2, &v2, &v3);
	cross_vec3f(&n3, &v3, &v4);
	cross_vec3f(&n4, &v4, &v5);
	cross_vec3f(&n5, &v5, &v6);
	cross_vec3f(&n6, &v6, &v7);
	cross_vec3f(&n7, &v7, &v8);
	cross_vec3f(&n8, &v8, &v1);
	vec3f n = (n1 + n2 + n3 + n4 + n5 + n6 + n7 + n8);
	n = n/8.;
	n.y = n.y/8.;
	normalize_vec3f(&n);
	return -n;
}

void heightmap::bind_stupid_VAO(const std::vector<Vertex> &vertexdata, const std::vector<unsigned int> &indices){

	glGenVertexArrays(1, &this->stupid_vao);
	glBindVertexArray(this->stupid_vao);
	
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertexdata.size() * sizeof(Vertex), vertexdata.data(), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tc));
	
	stupid_ibo = new GLuint[1];	
	glGenBuffers(1, stupid_ibo);

	// bind stupid buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stupid_ibo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void heightmap::bindVAO(const std::vector<Vertex> &vertexdata, const std::vector<std::vector<std::vector<unsigned int> > > &indices) {

	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);
	
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertexdata.size() * sizeof(Vertex), vertexdata.data(), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tc));

	// Create and bind a BO for index data
	ibo = new GLuint* [8];
	ibo[0] = new GLuint[nLoD];
	glGenBuffers(nLoD, ibo[0]);

	ibo[1] = new GLuint[1];
	glGenBuffers(1, ibo[1]);

	ibo[2] = new GLuint[nLoD-1];
	glGenBuffers(nLoD-1, ibo[2]);

	ibo[3] = new GLuint[nLoD-1];
	glGenBuffers(nLoD-1, ibo[3]);

	ibo[4] = new GLuint[nLoD];
	glGenBuffers(nLoD, ibo[4]);

	ibo[5] = new GLuint[nLoD-1];
	glGenBuffers(nLoD-1, ibo[5]);

	ibo[6] = new GLuint[nLoD-1];
	glGenBuffers(nLoD-1, ibo[6]);

	ibo[7] = new GLuint[nLoD];
	glGenBuffers(nLoD, ibo[7]);
	
	// copy data into the buffer object
	//
	for (int i = 0; i < nLoD; i++){
		//bind middle
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[0][i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[0][i].size() * sizeof(unsigned int), indices[0][i].data(), GL_STATIC_DRAW);
		
		//bind r_seam(=)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[4][i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[4][i].size() * sizeof(unsigned int), indices[4][i].data(), GL_STATIC_DRAW);
		
		//bind t_seam(=)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[7][i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[7][i].size() * sizeof(unsigned int), indices[7][i].data(), GL_STATIC_DRAW);
	}
	for (int i = 0; i < nLoD-1; i++){
		//bind r_seam(>)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[2][i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[2][i].size() * sizeof(unsigned int), indices[2][i].data(), GL_STATIC_DRAW);
		
		//bind r_seam(<)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[3][i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[3][i].size() * sizeof(unsigned int), indices[3][i].data(), GL_STATIC_DRAW);
		
		//bind t_seam(>)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[5][i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[5][i].size() * sizeof(unsigned int), indices[5][i].data(), GL_STATIC_DRAW);
		
		//bind t_seam(<)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[6][i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[6][i].size() * sizeof(unsigned int), indices[6][i].data(), GL_STATIC_DRAW);
	}

	// bind top_right corner
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[1][0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[1][0].size() * sizeof(unsigned int), indices[1][0].data(), GL_STATIC_DRAW);

	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


int heightmap::get_LoD(int x, int y){

	// Get the position of the camera
	vec3f cam_pos;
	extract_pos_vec3f_of_matrix(&cam_pos, lookat_matrix_of_cam(current_camera()));

	vec3f vertex_dist1 = vec3f(x, sample_height(x, y), y) - cam_pos;
	vec3f vertex_dist2 = vec3f(x+chunk_size, sample_height(x+chunk_size, y), y) - cam_pos;
	vec3f vertex_dist3 = vec3f(x, sample_height(x, y+chunk_size), y+chunk_size) - cam_pos;
	vec3f vertex_dist4 = vec3f(x+chunk_size, sample_height(x+chunk_size, y+chunk_size), y+chunk_size) - cam_pos;

	float dist1 = length_of_vec3f(&vertex_dist1);
	float dist2 = length_of_vec3f(&vertex_dist2);
	float dist3 = length_of_vec3f(&vertex_dist3);
	float dist4 = length_of_vec3f(&vertex_dist4); 
	

	// MAGIC_LOW defines the lower bound up to which the resolution should be maximal
	// MAGIC_HIGH defines the upper bound, below that the resolution will be minimal
	const float MAGIC_LOW = 5.f;

	// magic_step == k * chunk_size --> so MAGIC_HIGH must be k * (nLoD * chunk_size + MAGIC_LOW)
	const float MAGIC_HIGH = 2*(nLoD * chunk_size + MAGIC_LOW);

	float min_dist = std::min(dist1, std::min(dist2, std::min(dist3, dist4)));
	float magic_step = (MAGIC_HIGH-MAGIC_LOW)/nLoD;


	int ret_idx = (int)((min_dist - MAGIC_LOW)/magic_step);
	//ret_idx could be >=nLoD or negative at the moment but should have range [0, nLoD-1]
	if (ret_idx >= nLoD) ret_idx = nLoD -1;
	if (ret_idx < 0) ret_idx = 0;
	return ret_idx;
}

/*
 *Frustum culling for the ufo camera
 */
bool heightmap::check_invisible(int x, int y) {
	float near, far;
	vec2f near_size, far_size;
	vec3f cam_pos, cam_dir, cam_up, cam_right, cam_x, cam_y;

	vec3f middle;
	//frustum corners
	vec3f p_np, p_fp, p_ntr, p_ntl, p_nbr, p_nbl, p_ftr, p_ftl, p_fbr, p_fbl;
	//frustum normals
	vec3f nn, nf, nt, nb, nr, nl;
	vec3f v1, v2;
	float radius;
	float dist[6];

	camera_ref cam = find_camera("ufocam");
	matrix4x4f *lookat = lookat_matrix_of_cam(cam);
	extract_pos_vec3f_of_matrix(&cam_pos, lookat);
	extract_up_vec3f_of_matrix(&cam_up, lookat);
	extract_right_vec3f_of_matrix(&cam_right, lookat);
	extract_dir_vec3f_of_matrix(&cam_dir, lookat);
	normalize_vec3f(&cam_dir);
	normalize_vec3f(&cam_right);
	normalize_vec3f(&cam_up);
	near = camera_near(cam);
	far = camera_far(cam);
	camera_near_plane_size(cam, &near_size);
	camera_far_plane_size(cam, &far_size);

	middle = vec3f(x + 0.5 * chunk_size, 0, y + 0.5 * chunk_size);
	radius = sqrt(2 * pow(0.5 * chunk_size, 2) + pow(HEIGHT_FACTOR, 2));

	//centers of near and far
	p_np = cam_pos + vec3f(near * cam_dir.x, near * cam_dir.y, near * cam_dir.z);
	p_fp = cam_pos + vec3f(far * cam_dir.x, far * cam_dir.y, far * cam_dir.z);

	//near top right
	mul_vec3f_by_scalar(&cam_x, &cam_right, 0.5 * near_size.x);
	mul_vec3f_by_scalar(&cam_y, &cam_up, 0.5 * near_size.y);
	p_ntr = p_np + cam_x + cam_y;
	//near top left
	mul_vec3f_by_scalar(&cam_x, &cam_right, -0.5 * near_size.x);
	mul_vec3f_by_scalar(&cam_y, &cam_up, 0.5 * near_size.y);
	p_ntl = p_np + cam_x + cam_y;
	//near bottom right
	mul_vec3f_by_scalar(&cam_x, &cam_right, 0.5 * near_size.x);
	mul_vec3f_by_scalar(&cam_y, &cam_up, -0.5 * near_size.y);
	p_nbr = p_np + cam_x + cam_y;
	//near bottom left
	mul_vec3f_by_scalar(&cam_x, &cam_right, -0.5 * near_size.x);
	mul_vec3f_by_scalar(&cam_y, &cam_up, -0.5 * near_size.y);
	p_nbl = p_np + cam_x + cam_y;
	//far top right
	mul_vec3f_by_scalar(&cam_x, &cam_right, 0.5 * far_size.x);
	mul_vec3f_by_scalar(&cam_y, &cam_up, 0.5 * far_size.y);
	p_ftr = p_fp + cam_x + cam_y;
	//far top left
	mul_vec3f_by_scalar(&cam_x, &cam_right, -0.5 * far_size.x);
	mul_vec3f_by_scalar(&cam_y, &cam_up, 0.5 * far_size.y);
	p_ftl = p_fp + cam_x + cam_y;
	//far bottom right
	mul_vec3f_by_scalar(&cam_x, &cam_right, 0.5 * far_size.x);
	mul_vec3f_by_scalar(&cam_y, &cam_up, -0.5 * far_size.y);
	p_fbr = p_fp + cam_x + cam_y;
	//far bottom left
	mul_vec3f_by_scalar(&cam_x, &cam_right, -0.5 * far_size.x);
	mul_vec3f_by_scalar(&cam_y, &cam_up, -0.5 * far_size.y);
	p_fbl = p_fp + cam_x + cam_y;

	//near plane
	nn = cam_dir;
	v1 = middle - p_np;
	dist[0] = dot_vec3f(&nn, &v1);

	//far plane
	nf = -cam_dir;
	v1 = middle - p_fp;
	dist[1] = dot_vec3f(&nf, &v1);

	//top plane
	v1 = p_ntl - p_ntr;
	v2 = p_ftr - p_ntr;
	cross_vec3f(&nt, &v1, &v2);
	normalize_vec3f(&nt);
	v1 = middle - p_ntr;
	dist[2] = dot_vec3f(&nt, &v1);

	//bottom plane
	v1 = p_fbr - p_nbr;
	v2 = p_nbl - p_nbr;
	cross_vec3f(&nb, &v1, &v2);
	normalize_vec3f(&nb);
	v1 = middle - p_nbr;
	dist[3] = dot_vec3f(&nb, &v1);

	//right plane
	v1 = p_ftr - p_ntr;
	v2 = p_nbr - p_ntr;
	cross_vec3f(&nr, &v1, &v2);
	normalize_vec3f(&nr);
	v1 = middle - p_ntr;
	dist[4] = dot_vec3f(&nr, &v1);

	//left plane
	v1 = p_nbl - p_ntl;
	v2 = p_ftl - p_ntl;
	cross_vec3f(&nl, &v1, &v2);
	normalize_vec3f(&nl);
	v1 = middle - p_ntl;
	dist[5] = dot_vec3f(&nl, &v1);

	for(int i = 0; i < 6; i++)
		if(dist[i] < -8 * radius)
			return true;

	return false;
}

void heightmap::draw(){
	bind_shader(shader);
	int loc;
	loc = glGetUniformLocation(gl_shader_object(shader), "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);
	loc = glGetUniformLocation(gl_shader_object(shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

	// If a Modelmatrix would be required
	//loc = glGetUniformLocation(gl_shader_object(shader), "model");
	///glUniformMatrix4fv(loc, 1, GL_FALSE, make_unit_matrix4x4f(...));
	
	texture_ref water = find_texture("terrain_0");
	texture_ref grass = find_texture("terrain_1");
	texture_ref stone = find_texture("terrain_2");
	texture_ref snow = find_texture("terrain_3");
	
	bind_texture(water, 0);	
	loc = glGetUniformLocation(gl_shader_object(shader), "water");
	glUniform1i(loc, 0);

	bind_texture(grass, 1);	
	loc = glGetUniformLocation(gl_shader_object(shader), "grass");
	glUniform1i(loc, 1);

	bind_texture(stone, 2);	
	loc = glGetUniformLocation(gl_shader_object(shader), "stone");
	glUniform1i(loc, 2);

	bind_texture(snow, 3);	
	loc = glGetUniformLocation(gl_shader_object(shader), "snow");
	glUniform1i(loc, 3);

	loc = glGetUniformLocation(gl_shader_object(shader), "map_height");
	glUniform1f(loc, HEIGHT_FACTOR);

	vec3f cam_pos;
	extract_pos_vec3f_of_matrix(&cam_pos, lookat_matrix_of_cam(current_camera()));
	loc = glGetUniformLocation(gl_shader_object(shader), "eye_pos");
	glUniform3f(loc, cam_pos.x, cam_pos.y, cam_pos.z);

	setup_dir_light(shader);

	glPointSize(2);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	

	//stupid render mode appears to be really fast
	//I'm scared now
	if (render_mode == 0){
		glBindVertexArray(this->stupid_vao);
		for (int i = 0; i <= stupid_actual_height-stupid_chunk_size; i += stupid_chunk_size-1){
			for (int k = 0; k <= stupid_actual_width-stupid_chunk_size; k += stupid_chunk_size-1){
				//the stupid version with highest resolution
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stupid_ibo[0]);
				glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, stupid_index_buffer.size(), GL_UNSIGNED_INT, (void*)0, (GLint) (i*stupid_actual_width+k));
			}
		}
		glBindVertexArray(0);
	
	}else if (render_mode == 1 || render_mode == 2){
		glBindVertexArray(this->vao);
		for (int i = 0; i <= actual_height-chunk_size; i += chunk_size-1){
			for (int k = 0; k <= actual_width-chunk_size; k += chunk_size-1){
				if(check_invisible(i, k) && render_mode == 2)
					continue;
				//pos is (i, k) so linear (i*actual_width + k)
				int current_LoD = get_LoD(i, k);
				int top_LoD = get_LoD(i+chunk_size-1, k);
				int right_LoD = get_LoD(i, k+chunk_size-1);
				int right_idx = -1;
				int top_idx = -1;


				// draw base
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[0][current_LoD]);
				glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, index_buffer_LoD_type[0][current_LoD].size(), GL_UNSIGNED_INT, (void*)0, (GLint) (i*actual_width+k));

				// draw top_right corner
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[1][0]);
				glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, index_buffer_LoD_type[1][0].size(), GL_UNSIGNED_INT, (void*)0, (GLint) (i*actual_width+k));

				// the greater LoD is the worse the resolution 		
				switch(current_LoD - right_LoD){
					case -1:
						right_idx = 2;
						break;
					case 0:
						right_idx = 4;
						break;
					case 1:
						right_idx = 3;
						break;
				}
				switch(current_LoD - top_LoD){
					case -1:
						top_idx = 5;
						break;
					case 0:
						top_idx = 7;
						break;
					case 1:
						top_idx = 6;
						break;
				}

				// draw right seam
				if (right_idx == 2){
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[right_idx][current_LoD]);
					glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, index_buffer_LoD_type[right_idx][current_LoD].size(), GL_UNSIGNED_INT, (void*)0, (GLint) (i*actual_width+k));
				}else if (right_idx == 3){
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[right_idx][current_LoD-1]);
					glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, index_buffer_LoD_type[right_idx][current_LoD-1].size(), GL_UNSIGNED_INT, (void*)0, (GLint) (i*actual_width+k));

				}else if (right_idx == 4){
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[right_idx][current_LoD]);
					glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, index_buffer_LoD_type[right_idx][current_LoD].size(), GL_UNSIGNED_INT, (void*)0, (GLint) (i*actual_width+k));
				}else{
					std::cout << "this should not happen" << std::endl;
				}

				// draw top seam
				if (top_idx == 5){
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[top_idx][current_LoD]);
					glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, index_buffer_LoD_type[top_idx][current_LoD].size(), GL_UNSIGNED_INT, (void*)0, (GLint) (i*actual_width+k));
				}else if (top_idx == 6){
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[top_idx][current_LoD-1]);
					glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, index_buffer_LoD_type[top_idx][current_LoD-1].size(), GL_UNSIGNED_INT, (void*)0, (GLint) (i*actual_width+k));
				}else if (top_idx == 7){
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[top_idx][current_LoD]);
					glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, index_buffer_LoD_type[top_idx][current_LoD].size(), GL_UNSIGNED_INT, (void*)0, (GLint) (i*actual_width+k));
				}else{
					std::cout << "this->should not happen" << std::endl;
				}
			}
		}
		glBindVertexArray(0);
	}else{
		std::cout << "SUCKS" << std::endl;
	}

	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

	unbind_texture(water);
	unbind_texture(grass);
	unbind_texture(stone);
	unbind_texture(snow);
	unbind_shader(shader);

}
	


// Der Idealismus waechst mit der Entfernung vom Problem.
