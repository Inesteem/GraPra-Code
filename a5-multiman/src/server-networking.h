#ifndef __SERVER_NETWORKING_H
#define __SERVER_NETWORKING_H

#include "messages.h"
#include "wall-timer.h"
#include "server-logic.h"

using namespace std;

void send_message(int player, msg::message *m);
void broadcast(msg::message *m);

class GameStage;
class server_message_reader : public message_reader {
    int player_id; // the messages are sent by this player
    GameStage *m_gameStage;

public:
    server_message_reader(GameStage *gameStage, message_reader::socket *socket, int player) : m_gameStage(gameStage), message_reader(), player_id(player) {
        setSocket(socket);
    }

    void handle_message(msg::spawn_troup_client *m);
    void handle_message(msg::building_upgrade_house *m);
    void handle_message(msg::building_upgrade_turret *m);
    void handle_message(msg::client_settings *m);
    void handle_message(msg::client_left *m);
};

namespace client_connections {
    extern unsigned int sockets;
    extern boost::asio::ip::tcp::socket **socket;
    extern server_message_reader **reader;
}

struct quit_signal {
    int status;
    quit_signal(int s) : status(s) {}
};
void quit(int status);

extern wall_time_timer since_last_broadcast;


void broadcast(msg::message *m);
#endif
