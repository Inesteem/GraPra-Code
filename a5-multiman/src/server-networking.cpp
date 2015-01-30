#include "server-networking.h"

vector<int> left_player;

namespace client_connections {
    unsigned int sockets = 0;
    boost::asio::ip::tcp::socket **socket;
    server_message_reader **reader;
}

void send_message(int player, msg::message *m) {
	
	for(int i = 0; i < left_player.size();i++){
		if(player == left_player[i])
			return;
	}

    try {
        boost::asio::write(*client_connections::socket[player], boost::asio::buffer(m, m->message_size), boost::asio::transfer_all());
    }
    catch (boost::system::system_error &err) {
        //cerr << "Error sending message to client of player " << player << ": " << err.what() << "\nExiting." << endl;
        //quit(-1);
    }
}

wall_time_timer since_last_broadcast;

void broadcast(msg::message *m)
{
    int i = 0;
    for (i = 0; i < client_connections::sockets; ++i) {
        send_message(i, m);
    }

    since_last_broadcast.restart();
}


void quit(int status)
{
    throw quit_signal(status);
}

void server_message_reader::handle_message(msg::spawn_troup_client *m) {
    cout << "Spawning troup..." << endl;
    m_gameStage->addArmy(m->sourceId, m->destinationId, m->unitCount);
}

void server_message_reader::handle_message(msg::building_upgrade_house *m) {
    cout << "Upgrading building house " << (int)m->buildingId << endl;
    m_gameStage->upgrade_building_house((int)m->buildingId);
}

void server_message_reader::handle_message(msg::building_upgrade_turret *m) {
    cout << "Upgrading building turret " << m->buildingId << endl;
    m_gameStage->upgrade_building_turret(m->buildingId);
}

void server_message_reader::handle_message(msg::client_settings *m) {
    cout << "Got settings from player " << m->playerId << endl;
    m_gameStage->handleClientSettings(m->playerId, m->colorId, m->frac);
}

void server_message_reader::handle_message(msg::client_left *m) {
	left_player.push_back(m->playerId);
    cout << "Player " << m->playerId << "left" << endl;
    m_gameStage->lostClient(m->playerId);
}
