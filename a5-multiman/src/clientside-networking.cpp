#include "clientside-networking.h"

//#include "cmdline.h"

#include <iostream>
//#include <GL/freeglut.h>
//#include <cairo/cairo.h>

using namespace std;

boost::asio::ip::tcp::socket *server_connection = 0;
extern bool game_is_running;

void client_message_reader::send_message(msg::message &msg) {
	try {
		boost::asio::write(*server_connection, boost::asio::buffer(&msg, msg.message_size), boost::asio::transfer_all());
	}
	catch (boost::system::system_error &err) {
		cerr << "Error sending message to the server: " << err.what() << "\nExiting." << endl;
		//quit(-1);
		exit(EXIT_FAILURE);
	}
}

/*
void drawStartScreen(bool loadingDone, bool failure, const string &addText) {
	static bool init = true;
	
	static mesh_ref mesh;
	static matrix4x4f model;
	static texture_ref texture;
	static camera_ref cam;
	
	static const unsigned int fontSize = 100;
	
	static cairo_surface_t *cairo_surface = NULL;
	static unsigned char *cairo_surface_data = NULL;
	
	static cairo_t *cairo = NULL;
	
	static int numDots = 0;
	
	if (init) {
		numDots = 0;
		
		vec3f pos[4] = { {0,0,-10}, {1,0,-10}, {1,1,-10}, {0,1,-10} };
		vec2f tc[4] = { {0,1}, {1,1}, {1,0}, {0,0} };
		unsigned int idx[6] = { 0, 1, 2, 2, 3, 0 };
		
		mesh = make_mesh("quad_start", 2);
		
		bind_mesh_to_gl(mesh);
		add_vertex_buffer_to_mesh(mesh, "in_pos", GL_FLOAT, 4, 3, (float *) pos, GL_STATIC_DRAW);
		add_vertex_buffer_to_mesh(mesh, "in_tc", GL_FLOAT, 4, 2, (float *) tc, GL_STATIC_DRAW);
		add_index_buffer_to_mesh(mesh, 6, idx, GL_STATIC_DRAW);
		unbind_mesh_from_gl(mesh);
		
		const unsigned int width = msg::player_name::max_string_length * fontSize;
		const unsigned int height = fontSize;
		
		tex_params_t p = default_fbo_tex_params();
		p.wrap_s = GL_CLAMP_TO_EDGE;
		p.wrap_t = GL_CLAMP_TO_EDGE;
		texture = make_empty_texture("loading", width, height, GL_TEXTURE_2D, GL_RGBA8, GL_UNSIGNED_BYTE, GL_RGBA, &p);
		
		cairo = create_cairo_context(texture_width(texture), texture_height(texture), 4, &cairo_surface, &cairo_surface_data);
		cairo_set_font_size(cairo, fontSize);
		cairo_select_font_face(cairo, "DejaVu Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		
		vec3f cam_pos = {0,0,0}, cam_dir = {0,0,-1}, cam_up = {0,1,0};
		cam = make_orthographic_cam((char *) "startscreen_cam", &cam_pos, &cam_dir, &cam_up, 50, 0, 50, 0, 0.01, 1000);
		
		
		const float scale = 2.0;
		
		make_unit_matrix4x4f(&model);
		model.row_col(0,0) = scale * (float)texture_width(texture) / (float)texture_height(texture);
		model.row_col(1,1) *= scale;
		model.row_col(0,3) = 5;
		model.row_col(1,3) = 10;
		
		init = false;
	}
	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	cairo_move_to(cairo, 0, fontSize);
	cairo_set_source_rgb(cairo, 0, 0, 0);
	cairo_paint(cairo);
	
	
	string text = "LOADING";
	
	if (!failure) {
		cairo_set_source_rgb(cairo, 1, 1, 1);
		
		for (int i = 0; i < min(numDots, 20); ++i) {
			text += ".";
		}
		
		if (loadingDone) {
			cairo_set_source_rgb(cairo, 0, 1, 0);
			text += " done";
		}
	} else {
		cairo_set_source_rgb(cairo, 0, 0, 1);
		text += " FAILED";
	}
	
	text += addText;
	
	cairo_show_text(cairo, text.c_str());
	
	bind_texture(texture, 0);
	unsigned char *data = cairo_image_surface_get_data(cairo_surface);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture_width(texture), texture_height(texture), GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	camera_ref old_cam = current_camera();
	use_camera(cam);
	
	shader_ref shader = find_shader("startscreen");
	bind_shader(shader);
	
	int loc = glGetUniformLocation(gl_shader_object(shader), "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection_matrix_of_cam(current_camera())->col_major);

	loc = glGetUniformLocation(gl_shader_object(shader), "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, gl_view_matrix_of_cam(current_camera())->col_major);
	
	vec3f textColor = { 1, 1, 1 };
	if (loadingDone) textColor = { 0, 1, 0 };
	if (failure) textColor = { 1, 0, 0 };
	
	
	loc = glGetUniformLocation(gl_shader_object(shader), "color");
	glUniform3fv(loc, 1, (float*) &textColor);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
			

	loc = glGetUniformLocation(gl_shader_object(shader), "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, model.col_major);
	
	bind_texture(texture, 0);
	loc = glGetUniformLocation(gl_shader_object(shader), "tex");
	glUniform1i(loc, 0);

	
	bind_mesh_to_gl(mesh);
	draw_mesh(mesh);
	unbind_mesh_from_gl(mesh);
	
	unbind_texture(texture);
	unbind_shader(shader);
	
	if (!loadingDone) ++numDots;
	
	glutSwapBuffers();
	
	use_camera(old_cam);
}
*/

void client_message_reader::networking_prologue() {
	using boost::asio::ip::tcp;
	
	//drawStartScreen();

	try {
		boost::asio::io_service io_service;

		cout << "connecting..." << endl;
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(/*cmdline.server_host*/ "localhost", "2214");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::resolver::iterator end;
		
		//drawStartScreen();

		server_connection = new tcp::socket(io_service);
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end) {
			server_connection->close();
			server_connection->connect(*endpoint_iterator++, error);
		}
		if (error)
			throw boost::system::system_error(error);

		cout << "connected." << endl;
		
		//drawStartScreen();

		/*msg::player_name pn = make_message<msg::player_name>();
		strncpy(pn.name, cmdline.player_name.c_str(), msg::player_name::max_string_length);
		pn.name[msg::player_name::max_string_length-1] = '\0';
		pn.id = 0;
		send_message(pn);*/
		
		//drawStartScreen();

		setSocket(server_connection);
		/*
		while (!reader->eof()) {
			//drawStartScreen();
			
			reader->read_and_handle();
			if (reader->prologue_over()) {
				//wall_time_timer timer;
				//while (timer.look() < 1337) { drawStartScreen(true); }
				
				break;
			}
		}*/
	}
	
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		
		string addText = " (" + string(e.what()) + ")";

		//wall_time_timer timer;
		//while (timer.look() < 1337) { drawStartScreen(true, true, addText); }
		
		exit(EXIT_FAILURE);
	}
}

client_message_reader::client_message_reader(Game *game) : message_reader(), game(game) {
	//setup_done = false;
}

/*bool client_message_reader::prologue_over() {
	return setup_done;
}*/

void client_message_reader::handle_message(msg::init_game *m)
{
	std::cout << "Initilizing map with, x = " << m->mapX << ", y = " << m->mapY << std::endl;
    game->init("./render-data/images/smalllvl_height.png",m->mapX,m->mapY);
}

void client_message_reader::handle_message(msg::spawn_house *m)
{
   game->add_building("building_lot",1,m->x,m->y);
	std::cout << "Spawning house at (" << m->x << "," << m->y << ")" << std::endl;
}

void client_message_reader::handle_message(msg::spawn_tree *m)
{
    game->add_tree(m->x, m->y);
	std::cout << "Spawning tree at (" << m->x << "," << m->y << ")" << std::endl;
}

void client_message_reader::handle_message(msg::init_done *m)
{	
	cout << "Initializeing done, starting game..." << endl;
}

void handle_message(msg::spawn_troup_server *m)
{
    cout << "Spawning new troup..." << endl;
}

void handle_message(msg::next_troup_destination *m)
{
    cout << "Got new troup destination..." << endl;
}

void handle_message(msg::troup_arrived *m)
{
    cout << "Troup arrived at building..." << endl;
}

void handle_message(msg::building_owner_changed *m)
{
    cout << "Building owner changed..." << endl;
}

/*
void client_message_reader::handle_message(msg::connected *m) {
	cout << "you are connected. your id is " << (int)m->your_id << "/" << (int)m->players << endl;
	players = m->players;
	player_id = m->your_id;
	player = new Player*[players];

	for (int i = 0; i < players; ++i) {
		std::ostringstream oss;
		oss << "player-" << i;
		player[i] = new Player(oss.str(), character_prototype);
		player[i]->id = i;
	}
}

void client_message_reader::handle_message(msg::initial_player_data_position *m) {
	cout << "the initial player position for player " << (int)m->player << " is " << (int)m->x << ", " << (int)m->y << endl;
	player[m->player]->ForcePosition(m->x,m->y);
	if (m->player == player_id) {
		// Set initial camera position
		camera_ref cam = find_camera("playercam");
		matrix4x4f* mat = lookat_matrix_of_cam(cam);

		vec3f pos = {
			player[m->player]->trafo.row_col(0,3),
			player[m->player]->trafo.row_col(1,3) + 45.f,
			player[m->player]->trafo.row_col(2,3) + 45.f
		};

		vec3f dir = {
			player[m->player]->trafo.row_col(0,3) - pos.x,
			player[m->player]->trafo.row_col(1,3) - pos.y + 10.f,
			player[m->player]->trafo.row_col(2,3) - pos.z
		};

		vec3f up = {0.f, 1.f, 0.f};

		make_lookat_matrixf(mat, &pos, &dir, &up);

		recompute_gl_matrices_of_cam(cam);
	}
}

void client_message_reader::handle_message(msg::board_info *m) {
	cout << "the board is " << (int)m->w << " x " << (int)m->h << " with " << (int)m->boxes << " boxes." << endl;
	int tiles_x = m->w;
	int tiles_y = m->h;
	the_board = new Board(tiles_x, tiles_y);
}

void client_message_reader::handle_message(msg::spawn_box *m) {
	cout << "a box (type " << (int)m->box_type << ") spanws at " << (int)m->x << ", " << (int)m->y << endl;
	the_board->AddBox(m->x, m->y, m->box_type);
}

void client_message_reader::handle_message(msg::start_game *m) {
	cout << "game on!" << endl;
	setup_done = true;
}

void client_message_reader::handle_message(msg::force_player_position *m) {
	cout << "forced player pos to " << (int)m->x << "," << (int)m->y << endl;

	// Setting health to 100 and enforcing the position indicates a respawn
	if (player[m->player]->respawning) {
		player[m->player]->respawning = false;


		if (m->player == player_id) {
			// In case the client respawns the relative camera has to be corrected
			matrix4x4f trafo_old = player[m->player]->trafo;
			player[m->player]->ForcePosition(m->x,m->y);
			camera_ref cam = find_camera("playercam");
			matrix4x4f* mat = lookat_matrix_of_cam(cam);
			mat->row_col(0, 3) -= trafo_old.row_col(0, 3) - player[m->player]->trafo.row_col(0,3);
			mat->row_col(2, 3) -= trafo_old.row_col(2, 3) - player[m->player]->trafo.row_col(2,3);
			recompute_gl_matrices_of_cam(cam);
		} else {
			player[m->player]->ForcePosition(m->x,m->y);
		}
	} else {
		player[m->player]->ForcePosition(m->x,m->y);
	}
}

void client_message_reader::handle_message(msg::start_move *m) {
	cout << "start moving in direction " << (int)m->dir_x << "," << (int)m->dir_y << " for about " << (int)m->est_duration << " ms" << endl;
	player[m->player]->StartMoving(m->dir_x, m->dir_y, m->est_duration);
}

void client_message_reader::handle_message(msg::spawn_bomb *m) {
	cout << "bomb " << (int)m->id << " spawns at " << (int)m->x << ", " << (int)m->y << endl;
	Bomb *b = new Bomb(m->x, m->y, m->id);
	the_board->AddBomb(b);
}

void client_message_reader::handle_message(msg::bomb_explosion *m) {
	cout << "bomb " << (int)m->id << " explodes" << endl;
	the_board->Explosion(m->id, m->dir_codes);
}

void client_message_reader::handle_message(msg::player_health_info *m) {
	cout << "player" << (int)m->id << "'s health is " << (int)m->health << endl;
	player[m->id]->health = m->health;

	// Check for respawn
	if (player[m->id]->health == 100) {
		player[m->id]->respawning = true;
	}
}

void client_message_reader::handle_message(msg::player_name *m) {
	cout << "player" << (int)m->id << "'s name is " << m->name << endl;
	player[m->id]->name = m->name;
	player[m->id]->MakeNameTexture();
}

void client_message_reader::handle_message(msg::frags_update *m) {
	cout << "player" << (int)m->player << "'s new frag count is " << (int)m->frags << endl;
	player[m->player]->frags = m->frags;
}

void client_message_reader::handle_message(msg::game_over *m) {
	game_is_running = false;
}*/


client_message_reader *reader = 0;



using namespace std;


/* vim: set foldmethod=marker: */

