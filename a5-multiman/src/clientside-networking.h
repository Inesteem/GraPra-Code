#ifndef __CLIENTSIDE_NETWORKING_H__ 
#define __CLIENTSIDE_NETWORKING_H__ 

#include "messages.h"
#include "wall-timer.h"
#include <string>
#include "game.h"

class client_message_reader : public message_reader {
private:
    Game *game;

public:
    client_message_reader(Game *game);

	void send_message(msg::message &msg);
    int networking_prologue(char *hostName);

    bool m_init_done;

protected:
	virtual void handle_message(msg::init_game *m);
	virtual void handle_message(msg::spawn_house *m);
	virtual void handle_message(msg::spawn_tree *m);
	virtual void handle_message(msg::init_done *m);
    virtual void handle_message(msg::spawn_troup_server *m);
    virtual void handle_message(msg::next_troup_destination *m);
    virtual void handle_message(msg::troup_arrived *m);
    virtual void handle_message(msg::building_owner_changed *m);
    virtual void handle_message(msg::building_unit_generated *m);
    virtual void handle_message(msg::building_upgrade *m);
    virtual void handle_message(msg::game_over *m);
    virtual void handle_message(msg::spawn_random_stuff *m);
    virtual void handle_message(msg::new_player *m);
};


#endif

