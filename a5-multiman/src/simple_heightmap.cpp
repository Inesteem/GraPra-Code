
#include "simple_heightmap.h"
#include "rendering.h"
#include <vector>
simple_heightmap::simple_heightmap()
    {


}


void simple_heightmap::init( const std::string filename, int width, int height){

	this->filename = filename;
    m_mesh = make_mesh("mesh_heightmap", 2);
    vec3f* colors(load_image3f(filename.c_str(), &m_width, &m_height));

    m_heights = vector<vec3f>(colors, colors + m_width*m_height);
    m_g_height = height;
    m_g_width = width;
   // cout << m_width << " " << m_height << endl;
//    if(width != m_width || height != m_height){
//        cerr << filename << " width or height doesnt match received width or height!" << endl;
//    }
    m_gamefield = vector<char>(m_g_width*m_g_height);
    for(int i = 0; i < m_gamefield.size(); i++){
        m_gamefield[i] = 'n';
    }

    vector<vec3f> pos = vector<vec3f>(m_g_width*m_g_height);
    vector<vec3f> norm = vector<vec3f>(m_width*m_height);

//    m_heights = vector<float>(m_width*m_height);
//    for(int i = 0; i < m_height; ++i){
//        for(int j = 0; j < m_width; ++j){

//            m_heights[i + j *m_height] = colors[i + j *m_height].x * render_settings::height_factor;


//        }
//    }
    for(int i = 0; i < m_g_height; ++i){
        for(int j = 0; j < m_g_width; ++j){
            pos[i + j * m_g_height] = vec3f(j/(float)m_g_width,0,i/(float)m_g_height);
//            pos[i + j *m_g_height].y =  m_heights[i*(m_width/m_g_width)  + j*(m_height/m_g_height) *m_g_height].x ;

//            m_heights[i + j *m_height] = colors[i + j *m_height].x * render_settings::height_factor;


        }
    }
    make_unit_matrix4x4f(&m_model);
    m_model.col_major[0 *4 + 0] = render_settings::tile_size_x * m_g_width;
    m_model.col_major[1 *4 + 1] = render_settings::height_factor;
    m_model.col_major[2 *4 + 2] = render_settings::tile_size_y * m_g_height;
    m_model.col_major[3 *4 + 0] = -render_settings::tile_size_x/2;
    m_model.col_major[3 *4 + 1] = 0;
    m_model.col_major[3 *4 + 2] = -render_settings::tile_size_y/2;

    tex_params_t p = default_tex_params();
    height_map = make_texture_ub(("height_map"), filename.c_str(), GL_TEXTURE_2D, &p);

//    for(int i = 0; i < m_height; ++i){
//        for(int j = 0; j < m_width; ++j){
//            norm[i + j * m_height] = sample_normal(i,j);
//        }
//    }
    
    
    m_shader = find_shader("heightmap_shader");
//    m_shader = find_shader("terrain");

    std::vector<unsigned int> index;
//    for(int i = 0; i < m_height-1; ++i){
//        for(int j = 0; j < m_width; ++j){
//            if(i%2 == 0){
//            index.push_back(i + j*m_height);
//            index.push_back((i+1)  + j*m_height);
//            } else {
//                index.push_back((i+1) + (m_width -j -1) * m_height);
//                index.push_back(i+ (m_width-j-1)*m_height)  ;

//            }

//        }
//    }

    for(int i = 0; i < m_g_height-1; ++i){
        for(int j = 0; j < m_g_width-1; ++j){

            index.push_back(i + j*m_g_height);
            index.push_back((i+1)  + j*m_g_height);
            index.push_back(i+1 + (j+1)*m_g_height);

//            index.push_back(i+1 + (j+1)*m_g_height);
            index.push_back(i + (j+1)*m_g_height);
//            index.push_back(i + (j)*m_g_height);

        }
    }

    bind_mesh_to_gl(m_mesh);
    add_vertex_buffer_to_mesh(m_mesh, "in_pos", GL_FLOAT, m_width*m_height, 3, (float*) pos.data() , GL_STATIC_DRAW);
  //  add_vertex_buffer_to_mesh(m_mesh, "in_tc", GL_INT, m_width*m_height, 1, nullptr , GL_STATIC_DRAW);
    add_vertex_buffer_to_mesh(m_mesh, "in_norm", GL_FLOAT, m_width*m_height, 3,(float*) norm.data(), GL_STATIC_DRAW );
    add_index_buffer_to_mesh(m_mesh, index.size(), (unsigned int *) index.data(), GL_STATIC_DRAW);
    unbind_mesh_from_gl(m_mesh);


	init_done = true;

}

float simple_heightmap::get_height(float x, float y){

//    cout << x << " " << y << endl;

    if (x < 0 || y < 0 || x > m_width || y > m_height ) return 0;
    float new_x = x*m_height/m_g_height;
    float new_y = y*m_width/m_g_width;
    vec2i pos1 ((int) new_x, (int) new_y);
//    vec2i pos2 ((int) x + 1, (int) y);
//    vec2i pos3 ((int) x, (int) y + 1 );
//    vec2i pos4 ((int) x + 1 , (int) y + 1);
    float height;
    try{

    height = m_heights.at(pos1.y + pos1.x * m_height).x * render_settings::height_factor;

    }
    catch(...){
    height = 0;
    }

//    height = (x - (float)((int) x))*m_heights[pos1.y + pos1.x * m_height] + (1.0f -(x - (float)((int) x)))* m_heights[pos2.y + pos2.x * m_height];
//    height += (y - (float)((int) y))*m_heights[pos1.y + pos1.x * m_height] + (1.0f -(y - (float)((int) y)))* m_heights[pos3.y + pos3.x * m_height];
//    height += (y - (float)((int) y))*m_heights[pos2.y + pos2.x * m_height] + (1.0f -(y - (float)((int) y)))* m_heights[pos4.y + pos4.x * m_height];
//    height += (x - (float)((int) x))*m_heights[pos3.y + pos3.x * m_height] + (1.0f -(x - (float)((int) x)))* m_heights[pos4.y + pos4.x * m_height];

//    height /= 4;

    return height;
}



void simple_heightmap::draw(){

    bind_shader(m_shader);

    int loc = glGetUniformLocation(gl_shader_object(m_shader), "proj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

    loc = glGetUniformLocation(gl_shader_object(m_shader), "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);

    loc = glGetUniformLocation(gl_shader_object(m_shader), "model");
    glUniformMatrix4fv(loc, 1, GL_FALSE, m_model.col_major);

    bind_texture(grass, 0);
    loc = glGetUniformLocation(gl_shader_object(m_shader), "grass");
    glUniform1i(loc, 0);
    bind_texture(stone, 1);
    loc = glGetUniformLocation(gl_shader_object(m_shader), "stone");
    glUniform1i(loc, 1);
    bind_texture(water, 2);
    loc = glGetUniformLocation(gl_shader_object(m_shader), "water");
    glUniform1i(loc, 2);
    bind_texture(snow, 3);
    loc = glGetUniformLocation(gl_shader_object(m_shader), "snow");
    glUniform1i(loc, 3);
    bind_texture(height_map, 4);
    loc = glGetUniformLocation(gl_shader_object(m_shader), "height_map");
    glUniform1i(loc, 4);


    loc = glGetUniformLocation(gl_shader_object(m_shader), "height_factor");
    glUniform1f(loc, render_settings::height_factor);

    loc = glGetUniformLocation(gl_shader_object(m_shader), "tex_res");
    glUniform2f(loc, m_width,m_height);
    loc = glGetUniformLocation(gl_shader_object(m_shader), "CamPos");
    glUniform3f(loc,gl_view_matrix_of_cam(current_camera())->col_major[3 *4 + 0],
                gl_view_matrix_of_cam(current_camera())->col_major[3 *4 + 1],
                gl_view_matrix_of_cam(current_camera())->col_major[3 *4 + 2]);


    loc = glGetUniformLocation(gl_shader_object(m_shader), "light_dir");
    glUniform3f(loc, 0.7, 1.2,0.3);
    loc = glGetUniformLocation(gl_shader_object(m_shader), "light_col");
    glUniform3f(loc, 1,1,1);

//	loc = glGetUniformLocation(gl_shader_object(m_shader), "map_height");
//	glUniform1f(loc, render_settings::height_factor);

//	vec3f cam_pos;
//	extract_pos_vec3f_of_matrix(&cam_pos, lookat_matrix_of_cam(current_camera()));
//	loc = glGetUniformLocation(gl_shader_object(m_shader), "eye_pos");
//	glUniform3f(loc, cam_pos.x, cam_pos.y, cam_pos.z);

//	setup_dir_light(m_shader);


	if(render_planes){

		bind_mesh_to_gl(m_mesh_2);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		draw_mesh_as(m_mesh_2,GL_PATCHES);
		unbind_mesh_from_gl(m_mesh_2);
	} else {
		bind_mesh_to_gl(m_mesh);
        glPatchParameteri(GL_PATCH_VERTICES, 4);
		draw_mesh_as(m_mesh,GL_PATCHES);
		unbind_mesh_from_gl(m_mesh);
	}
    unbind_shader(m_shader);

    unbind_texture(grass);
    unbind_texture(stone);
    unbind_texture(water);
    unbind_texture(snow);
    unbind_texture(height_map);






}



vec3f simple_heightmap::sample_normal(int x, int y){
	//cross_vec3f(out, v1, v2)
	float cur_height = get_height(x, y);
	vec3f v1 = vec3f(render_settings::tile_size_x, 	get_height(x+1, y)-cur_height, 	0);
	vec3f v2 = vec3f(render_settings::tile_size_x,  	get_height(x+1, y+1)-cur_height,render_settings::tile_size_y);
	vec3f v3 = vec3f(0,  	get_height(x, y+1)-cur_height, 	1);
	vec3f v4 = vec3f(-render_settings::tile_size_x,  	get_height(x-1, y+1)-cur_height,render_settings::tile_size_y);
	vec3f v5 = vec3f(-render_settings::tile_size_x,  	get_height(x-1, y)-cur_height, 	0);
	vec3f v6 = vec3f(-render_settings::tile_size_x,  	get_height(x-1, y-1)-cur_height,-render_settings::tile_size_y);
	vec3f v7 = vec3f(0,  	get_height(x, y-1)-cur_height, 	-1);
	vec3f v8 = vec3f(render_settings::tile_size_x,  	get_height(x+1, y-1)-cur_height,-render_settings::tile_size_y);

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


//void simple_heightmap::re_init(vector<vec3f> *planes){


//    m_mesh_2 = make_mesh("mesh_heightmap_2", 2);
//    vec3f* colors(load_image3f(filename.c_str(), &m_width, &m_height));

//    vector<vec3f> pos = vector<vec3f>(m_width*m_height);
//    vector<vec3f> norm = vector<vec3f>(m_width*m_height);

//    m_heights = vector<float>(m_width*m_height);
//    for(int i = 0; i < m_height; ++i){
//        for(int j = 0; j < m_width; ++j){
//            pos[i + j * m_height] = vec3f(j*render_settings::tile_size_x-render_settings::tile_size_x/2,0,i*render_settings::tile_size_y-render_settings::tile_size_y/2);
//            pos[i + j *m_height].y =  colors[i + j *m_height].x * render_settings::height_factor;
//            m_heights[i + j *m_height] = colors[i + j *m_height].x * render_settings::height_factor;
   
//            vec2f pos_1 = vec2f(j*render_settings::tile_size_x-render_settings::tile_size_x/2,i*render_settings::tile_size_y-render_settings::tile_size_y/2);
	
//            for(vector<vec3f>::iterator it = planes->begin(); it != planes->end(); ++it) {
//                vec3f vec = *it;
//            //	vec2f pos_2 = vec2f(vec.x, vec.z);
//            //	vec2f dist = pos_2 - pos_1;
//            //	float distance = length_of_vec2f(&dist);
//        //		if(distance <= 5){
//            //		pos[i + j *m_height].y = 100;
//            //		m_heights[i + j *m_height] = 100;
//            //		cout << "done" << endl;
				
//            //	}
//                if((vec.x >= j-1 && vec.x <= j+1) && (vec.z >= i-1 && vec.z <= i+1)){
////				if(((vec.z == i-1 || vec.z == i+1) && (vec.x == j))||((vec.x == j-1 || vec.x == j+1) && (vec.z == i))){
//            //		pos[i + j *m_height].y = colors[(int)vec.z + (int)vec.x *m_height].x * render_settings::height_factor;
//                    pos[i + j *m_height].y = vec.y;
//            //		m_heights[i + j *m_height] = colors[(int)vec.z + (int)vec.x *m_height].x * render_settings::height_factor;
//                    m_heights[i + j *m_height] = vec.y;
//                }
					
//            }
            
            


//        }
//    }
    
//    for(int i = 0; i < m_height-1; ++i){
//        for(int j = 0; j < m_width; ++j){
//            norm.push_back(sample_normal(i,j));
//        }
//    }
    
//    std::vector<unsigned int> index;


//    for(int i = 0; i < m_height-1; ++i){
//        for(int j = 0; j < m_width-1; ++j){

//            index.push_back(i + j*m_height);
//            index.push_back((i+1)  + j*m_height);
//            index.push_back(i+1 + (j+1)*m_height);

//            index.push_back(i+1 + (j+1)*m_height);
//            index.push_back(i + (j+1)*m_height);
//            index.push_back(i + (j)*m_height);

//        }
//    }

//    bind_mesh_to_gl(m_mesh_2);
//    add_vertex_buffer_to_mesh(m_mesh_2, "in_pos", GL_FLOAT, m_width*m_height, 3, (float*) pos.data() , GL_STATIC_DRAW);
//    add_vertex_buffer_to_mesh(m_mesh_2, "in_norm", GL_FLOAT, m_width*m_height, 3,(float*) norm.data(), GL_STATIC_DRAW );
//    add_index_buffer_to_mesh(m_mesh_2, index.size(), (unsigned int *) index.data(), GL_STATIC_DRAW);
//    unbind_mesh_from_gl(m_mesh_2);
//    render_planes = true;
//    cout << "changed_mesh" << endl;
//}


