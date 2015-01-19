#include "server-networking.h"

namespace client_connections {
    unsigned int sockets = 0;
    boost::asio::ip::tcp::socket **socket;
    server_message_reader **reader;
}

wall_time_timer since_last_broadcast;

void broadcast(msg::message *m)
{
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

void quit(int status)
{
    throw quit_signal(status);
}

void server_message_reader::handle_message(msg::spawn_troup_client *m) {
    cout << "Spawning troup..." << endl;
    m_gameStage->spawnTroup(m->sourceId, m->destinationId, m->unitCount);
}

void server_message_reader::handle_message(msg::building_upgrade_client *m) {
    cout << "Upgrading Building " << m->buildingId << endl;
    m_gameStage->upgrade_building(m->buildingId, m->state);
}
