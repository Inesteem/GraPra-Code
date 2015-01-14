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
	void networking_prologue();

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
};


#endif

