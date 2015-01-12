#include <iostream>
#include <cstdlib>
#include <vector>
#include <boost/asio.hpp>

#include <png++/png.hpp>

#include <libcgl/impex.h>

// #include <libmcm-0.0.1/vectors.h>

//#include "server-cmdline.h"
#include "messages.h"
//#include "gamelogic.h"
#include "wall-timer.h"

using namespace std;
//using namespace gamelogic;

// NOTE: in opengl +y is up, in our array representation +y is down.

void broadcast(msg::message *m);

class server_message_reader : public message_reader {
	int player_id; // the messages are sent by this player
public:
	server_message_reader(message_reader::socket *socket, int player) : message_reader(), player_id(player) {
		setSocket(socket);
	}
};

namespace client_connections {
	unsigned int sockets = 0;
	boost::asio::ip::tcp::socket **socket;
	server_message_reader **reader;
}

struct quit_signal {
	int status;
	quit_signal(int s) : status(s) {}
};
void quit(int status) { throw quit_signal(status); }

wall_time_timer since_last_broadcast;


void broadcast(msg::message *m) {
	int i = 0;
	try {
		for (i = 0; i < client_connections::sockets; ++i)
			boost::asio::write(*client_connections::socket[i], boost::asio::buffer(m, m->message_size), boost::asio::transfer_all());
	}
	catch (boost::system::system_error &err) {
		cerr << "Error sending message to client of player " << i << ": " << err.what() << "\nExiting." << endl;
		quit(-1);
	}
	since_last_broadcast.restart();
}

class Building
{
public: 
	int x, y;

	Building(int x, int y) : x(x), y(y) {}
};

std::vector<Building> buildings;

void initGame() {
	unsigned int x, y;
    vec3f *mapData = load_image3f("./render-data/images/smalllvl.png", &x, &y);

	msg::init_game ig = make_message<msg::init_game>();
	ig.mapX = x;
	ig.mapY = y;
	broadcast(&ig);

	for(unsigned int r = 0; r < y; r++) {
		for(unsigned int c = 0; c < x; c++) {
			vec3f color = mapData[r*x + c];
            if(color.x > 0.8f){// && color.y == 0.0f && color.z == 0.0f) {
				cout << "Building at (" << r << "," << c << ")" << endl;
				Building b(r,c);
				buildings.push_back(b);
			}
		}
	}

	for(std::vector<Building>::iterator it = buildings.begin(); it != buildings.end(); it++ ) {
		msg::spawn_house sh = make_message<msg::spawn_house>();
		sh.x = (*it).x;
		sh.y = (*it).y;
		broadcast(&sh);
	}

	msg::init_done id = make_message<msg::init_done>();
	broadcast(&id);
}

int main(int argc, char **argv)
{	
	using namespace boost::asio::ip;

	//parse_cmdline(argc, argv);
	
	client_connections::sockets = 1;//cmdline.players;
	client_connections::socket = new tcp::socket*[client_connections::sockets];
	client_connections::reader = new server_message_reader*[client_connections::sockets];
			
	try
	{
		boost::asio::io_service io_service;
		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 2214));

		for (int i = 0; i < client_connections::sockets; ++i) {
			cout << "waiting for connection " << i << endl;
			client_connections::socket[i] = new tcp::socket(io_service);
			acceptor.accept(*client_connections::socket[i]);
			cout << "got it" << endl;
			client_connections::reader[i] = new server_message_reader(client_connections::socket[i], i);

			/*msg::connected msg = make_message<msg::connected>();
			msg.your_id = i;
			msg.players = cmdline.players;

			boost::system::error_code ignored_error;
			boost::asio::write(*client_connections::socket[i], boost::asio::buffer(&msg, msg.message_size), boost::asio::transfer_all(), ignored_error);*/
		}

/*
		png::image<png::rgb_pixel> image(cmdline.map);
		int w = image.get_width();
		int h = image.get_height();
		std::vector<Box> boxes;
		
		for (size_t y = 0; y < h; ++y)
			for (size_t x = 0; x < w; ++x) {
				png::rgb_pixel px = image.get_pixel(x, h-y-1);
				msg::box_type::t type = msg::box_type::none;
				if (px.red <= 10 && px.green <= 10 && px.blue <= 10)	type = msg::box_type::stone;
				if (px.red >= 100 && px.red <= 150 && px.green >= 100 && px.green <= 150 && px.blue >= 100 && px.blue <= 150) type = msg::box_type::wood;
				if (type != msg::box_type::none)
					boxes.push_back(Box(x, y, type));
			}
		
		board = new Board(w, h);
		msg::board_info bi = make_message<msg::board_info>();
		bi.w = w;
		bi.h = h;
		bi.boxes = boxes.size();
		broadcast(&bi);

		for (int i = 0; i < boxes.size(); ++i) {
			msg::spawn_box sb = make_message<msg::spawn_box>();
			sb.x = boxes[i].x;
			sb.y = boxes[i].y;
			sb.box_type = (int)boxes[i].type;
			board->CreateBox(sb.x, sb.y, sb.box_type);
			broadcast(&sb);
		}
		
		players = cmdline.players;
		player = new Player*[players];
		for (int i = 0; i < players; ++i) {
			player[i] = new Player(i);
			msg::initial_player_data_position pp = make_message<msg::initial_player_data_position>();
			pp.player = i;
			Player::Pos pos = board->FindPosition();
			pp.x = pos.x;
			pp.y = pos.y;
			broadcast(&pp);
			player[i]->Position(pp.x,pp.y);
			player[i]->Damage(0,-1); // send healt init message.
		}


		msg::start_game sg = make_message<msg::start_game>();
		broadcast(&sg);*/

		initGame();

		bool game_over = false;

		// main loop
		wall_time_timer wtt, info_timer;
		while (1) {
			if (wtt.look() < 100) { //wall_time_timer::msec(settings::timeslicethickness)) {
				sleep(0); // just schedule away...
				continue;
			}
			wtt.start();
			for (int i = 0; i < client_connections::sockets; ++i) {
				client_connections::reader[i]->read_and_handle();
				if (client_connections::reader[i]->eof()) {
					cerr << "Player " << i << " disconnected. Exiting." << endl;
					quit(0);
				}
			}

			if (!game_over)
			{
				/*
				for (int i = 0; i < players; ++i)
					player[i]->Tick();

				int clear = 0;
				for (deque<Bomb*>::iterator it = bombs.begin(); it != bombs.end(); ++it)
					if ((*it)->Tick())
						++clear;
				while (clear-- > 0)      // NOTE: this does no longer work when bombs may have different timeouts!
					bombs.pop_front();

				for (int i = 0; i < players; ++i) {
					if (player[i]->frags >= cmdline.frag_limit) {
						game_over = true;
						msg::game_over go = make_message<msg::game_over>();
						broadcast(&go);
					}
				}*/
			}

			if (info_timer.look() > wall_time_timer::sec(3)) {
				 //board->Print();
				 info_timer.restart();
			}
			if (since_last_broadcast.look() > wall_time_timer::sec(5)) {
				//msg::keep_alive ping = make_message<msg::keep_alive>();
				//broadcast(&ping);
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	catch (quit_signal &sig) {
		exit(sig.status);
	}


	return 0;
}

