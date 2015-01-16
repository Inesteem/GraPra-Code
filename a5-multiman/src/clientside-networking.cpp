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

void client_message_reader::networking_prologue() {
	using boost::asio::ip::tcp;
	
	try {
		boost::asio::io_service io_service;

		cout << "connecting..." << endl;
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(/*cmdline.server_host*/ "localhost", "2214");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::resolver::iterator end;
		
		server_connection = new tcp::socket(io_service);
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end) {
			server_connection->close();
			server_connection->connect(*endpoint_iterator++, error);
		}
		if (error)
			throw boost::system::system_error(error);

		cout << "connected." << endl;
		
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

client_message_reader::client_message_reader(Game *game) : message_reader(), game(game) {}

void client_message_reader::handle_message(msg::init_game *m)
{
    std::cout << "Initilizing map with, x = " << (unsigned int) m->mapX << ", y = " << (unsigned int) m->mapY << std::endl;
    game->init("./render-data/images/smalllvl_height.png",m->mapX,m->mapY);
}

void client_message_reader::handle_message(msg::spawn_house *m)
{
    // TODO add id (m->id) to building!!
   game->add_building("building_lot",1,m->x,m->y, m->id);
    std::cout << "Spawning house at (" << (unsigned int) m->x << "," << (unsigned int) m->y << ")" << std::endl;
}

void client_message_reader::handle_message(msg::spawn_tree *m)
{
    game->add_tree(m->x, m->y);
    std::cout << "Spawning tree at (" << (unsigned int) m->x << "," << (unsigned int) m->y << ")" << std::endl;
}

void client_message_reader::handle_message(msg::init_done *m)
{	
	cout << "Initializeing done, starting game..." << endl;
}

void client_message_reader::handle_message(msg::spawn_troup_server *m) {

    cout << "Spawning new troup..." << endl;
    game->add_unit_group(m->sourceId, m->destinationId, m->unitCount, m->troupId);
}

void client_message_reader::handle_message(msg::next_troup_destination *m)
{
    cout << "Got new troup destination: (" << (unsigned int) m->mapX << " , " << (unsigned int) m->mapY << ")" << endl;
    game->update_unit_group(m->mapX, m->mapY, m->troupId, m->time);
}

void client_message_reader::handle_message(msg::troup_arrived *m)
{
    cout << "Troup arrived at building..." << endl;
	game->troup_arrived(m->troupId);
}

void client_message_reader::handle_message(msg::building_owner_changed *m)
{
    cout << "Building owner changed..." << endl;
}

void client_message_reader::handle_message(msg::building_unit_generated *m)
{
    cout << "New unit count " << (unsigned int) m->newUnitCount << " in building " << (unsigned int) m->buildingId << endl;
	game->update_building_unit_count( m->buildingId, m->newUnitCount);
}
