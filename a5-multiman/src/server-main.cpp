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

#include "server-logic.h"

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

GameStage *gameStage;

void initGame() {
	gameStage = new GameStage();

	unsigned int x, y;
	vec3f *mapData = load_image3f("./render-data/images/level0.png", &x, &y);

	msg::init_game ig = make_message<msg::init_game>();
	ig.mapX = x;
	ig.mapY = y;
	broadcast(&ig);

	for(unsigned int r = 0; r < y; r++) {
		for(unsigned int c = 0; c < x; c++) {
			vec3f color = mapData[r*x + c];
			if(color.x > 0.9f) {
				cout << "Building at (" << r << "," << c << ")" << endl;
				gameStage->spawnHouse(r, c);

				msg::spawn_house sh = make_message<msg::spawn_house>();
				sh.x = r;
				sh.y = c;
				broadcast(&sh);
			} else if(color.y > 0.9f) {
				cout << "Tree at (" << r << "," << c << ")" << endl;
				gameStage->spawnTree(r, c);
			}
		}
	}

	msg::init_done id = make_message<msg::init_done>();
	broadcast(&id);
}

void updateGame()
{

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
		}

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
				updateGame();
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

