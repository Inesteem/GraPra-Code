#ifndef __CLIENTSIDE_NETWORKING_H__ 
#define __CLIENTSIDE_NETWORKING_H__ 

#include "messages.h"
#include "wall-timer.h"
#include <string>
#include "gameobject.h"
#include "simple_heightmap.h"

class client_message_reader : public message_reader {
private:
    ObjHandler *m_objHandler;
    simple_heightmap *m_sh;

public:
    client_message_reader(ObjHandler *objHandler, simple_heightmap *sh );

	void send_message(msg::message &msg);
	void networking_prologue();

protected:
	virtual void handle_message(msg::init_game *m);
	virtual void handle_message(msg::spawn_house *m);
	virtual void handle_message(msg::init_done *m);


};


#endif

