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

#include "server-networking.h"
#include "server-logic.h"

using namespace std;
//using namespace gamelogic;

// NOTE: in opengl +y is up, in our array representation +y is down.

GameStage *gameStage;

void initGame() {
	unsigned int x, y;
    vec3f *mapData = load_image3f("./render-data/images/smalllvl.png", &x, &y);

    gameStage->m_mapX = x;
    gameStage->m_mapY = y;

	msg::init_game ig = make_message<msg::init_game>();
	ig.mapX = x;
	ig.mapY = y;
	ig.id = 0;
	broadcast(&ig);

    gameStage->m_map = new bool*[y];

	for(unsigned int r = 0; r < y; r++) {
        gameStage->m_map[r] = new bool[x];
		for(unsigned int c = 0; c < x; c++) {
            gameStage->m_map[r][c] = true;
            vec3f color = mapData[(y-1-r)*x + c];
			if(color.x > 0.9f) {
                gameStage->m_map[r][c] = false;
				cout << "Building at (" << r << "," << c << ")" << endl;
                Building *b = gameStage->spawnHouse(r, c);

				msg::spawn_house sh = make_message<msg::spawn_house>();
				sh.x = r;
				sh.y = c;
                sh.id = b->m_id;
				broadcast(&sh);
				
				msg::new_houseowner nho = make_message<msg::building_owner_changed>();
				nho.buildingId = b->m_id;
				nho.oldOwner = -1;
				//TODO
                nho.newOwner = 0;
				broadcast(&nho);				
			
    struct building_owner_changed : public message {
        building_owner_changed() : message(code::building_owner_changed) {}
        uint8_t buildingId;
        uint8_t oldOwner;
        uint8_t newOwner;			
				
				
			} else if(color.y > 0.9f) {
                gameStage->m_map[r][c] = false;
				cout << "Tree at (" << r << "," << c << ")" << endl;

				msg::spawn_tree st = make_message<msg::spawn_tree>();
				st.x = r;
				st.y = c;
                broadcast(&st);
			}
		}
	}

    for(unsigned int r = 0; r < y; r++) {
        cout << endl;
        for(unsigned int c = 0; c < x; c++) {
            cout << (gameStage->m_map[r][c] ? "1 " : "0 ");
        }
    }

	msg::init_done id = make_message<msg::init_done>();
    broadcast(&id);
}

void updateGame()
{
    gameStage->Update();
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
        gameStage = new GameStage();

		boost::asio::io_service io_service;
		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 2214));

		for (int i = 0; i < client_connections::sockets; ++i) {
			cout << "waiting for connection " << i << endl;
			client_connections::socket[i] = new tcp::socket(io_service);
			acceptor.accept(*client_connections::socket[i]);
			cout << "got it" << endl;
            client_connections::reader[i] = new server_message_reader(gameStage, client_connections::socket[i], i);
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
                //broadccurrentast(&ping);
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

