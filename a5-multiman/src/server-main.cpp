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

void blockMap(int r, int c, int maxX, int maxY) {
    if(r < 0 || c < 0 || r >= maxY || c >= maxX) return;
    gameStage->m_map[r][c] = false;
}

void initGame(string &levelName) {
	unsigned int x, y;
    string levelPath = "./render-data/images/" + levelName + ".png";
    vec3f *mapData = load_image3f(levelPath.c_str(), &x, &y);

    gameStage->m_mapX = x;
    gameStage->m_mapY = y;

    for (unsigned int i = 0; i < client_connections::sockets; ++i) {
        msg::init_game ig = make_message<msg::init_game>();
        ig.mapX = x;
        ig.mapY = y;
        ig.id = i;
        strcpy(ig.levelName, levelName.c_str());
        send_message(i, &ig);
    }

    gameStage->m_map = new bool*[y];

    int buildingPlayerIndex = 0;

    for(unsigned int r = 0; r < y; r++) {
        gameStage->m_map[r] = new bool[x];
        for(unsigned int c = 0; c < x; c++) {
            gameStage->m_map[r][c] = true;
        }
    }

	for(unsigned int r = 0; r < y; r++) {
		for(unsigned int c = 0; c < x; c++) {
            vec3f color = mapData[(y - 1 - r) * x + c];
			if(color.x > 0.9f) {
                //gameStage->m_map[r][c] = false;
                //cout << "Building at (" << r << "," << c << ")" << endl;
                Building *b = gameStage->spawnHouse(c, r);

				msg::spawn_house sh = make_message<msg::spawn_house>();
                sh.x = c;
                sh.y = r;
                sh.id = b->m_id;
                sh.unitCount = 20;
                broadcast(&sh);
				
				msg::building_owner_changed nho = make_message<msg::building_owner_changed>();
				nho.buildingId = b->m_id;
				nho.oldOwner = -1;
                nho.newUnitCount = 20;
                nho.newOwner = (buildingPlayerIndex < client_connections::sockets) ? buildingPlayerIndex : -1;
                b->m_player = (buildingPlayerIndex < client_connections::sockets) ? buildingPlayerIndex : -1;
                buildingPlayerIndex++;
                broadcast(&nho);

                b->m_unitCount = b->m_player == -1 ? 10 : 20;
                msg::building_unit_generated bug = make_message<msg::building_unit_generated>();
                bug.buildingId = b->m_id;
                bug.newUnitCount = b->m_unitCount;
                broadcast(&bug);
							
			} else if(color.y > 0.4f) {
                blockMap(r,c-1, x, y);
                blockMap(r,c, x, y);
                blockMap(r,c+1, x, y);
                blockMap(r+1,c-1, x, y);
                blockMap(r+1,c, x, y);
                blockMap(r+1,c+1, x, y);
                blockMap(r-1,c-1, x, y);
                blockMap(r-1,c, x, y);
                blockMap(r-1,c+1, x, y);

                //cout << "Tree at (" << r << "," << c << ")" << endl;

				msg::spawn_tree st = make_message<msg::spawn_tree>();
                st.x = c;
                st.y = r;
                st.type = 1;
				if(color.y > 0.9f)
					st.type = 0;
					
                broadcast(&st);
			} 
		}
    }

    if(buildingPlayerIndex < client_connections::sockets) {
        cout << "Not enough buildings on level to give every player a home :(" << endl;
    }

    for(unsigned int r = 0; r < y; r++) {
        cout << endl;
        for(unsigned int c = 0; c < x; c++) {
            cout << (gameStage->m_map[r][c] ? " " : "T");
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

    if(argc != 3) {
        cout << "Usage: ./multiman_server <num players> <levelname>" << endl;
        exit(0);
    }

    int numPlayers = atoi(argv[1]);

    if(numPlayers < 1) {
        cout << "Usage: ./multiman_server <num players> <levelname>" << endl;
        exit(0);
    }

    string levelName = string(argv[2]);
    if(levelName.length() > LVL_MAX_LENGTH) {
        cout << "Invalid level name, must be shorter than" << LVL_MAX_LENGTH << " characters." << endl;
        exit(0);
    }

    cout << "Number of players: " << numPlayers << endl;
    cout << "Level: " << levelName << endl;
	
    client_connections::sockets = numPlayers;
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

            // TODO send message to single client with player id
		}

        initGame(levelName);

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

