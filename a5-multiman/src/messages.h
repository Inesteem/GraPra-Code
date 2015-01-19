#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <stdint.h>
#include <boost/asio.hpp>

/* how to add a message:
 * - specify a message code
 * - defined the struct, don't forget the c'tor and the alignment declaration
 * - define a virtual function in the message_reader
 * - extend the case statement in handle_message()
 * oh yeah, and don't forget to create your message object with make_message,
 * the sizeof won't get in there by itself :)
 */

namespace msg {
	namespace code {
		enum {
			init_game = 1,
			spawn_house,
			spawn_tree,
            init_done,
            spawn_troup_server,
            spawn_troup_client,
            next_troup_destination,
            troup_arrived,
            building_owner_changed,
            building_unit_generated,
            building_upgrade,
            building_upgrade_client
		};
	}

	struct message {
		uint8_t message_size, message_type;
		message(uint8_t type) : message_type(type) {}
	};

	// server -> client

	struct init_game : public message {
		init_game() : message(code::init_game) {}
        uint8_t mapX;
        uint8_t mapY;
        uint8_t id;
	} __attribute__((aligned(8)));

	struct spawn_house : public message {
        spawn_house() : message(code::spawn_house) {}
        uint8_t id;
        uint8_t x;
        uint8_t y;
	} __attribute__((aligned(8)));

	struct spawn_tree : public message {
		spawn_tree() : message(code::spawn_tree) {}		
        uint8_t x;
        uint8_t y;
	} __attribute__((aligned(8)));

	struct init_done : public message {
		init_done() : message(code::init_done) {}
	} __attribute__((aligned(8)));

    struct spawn_troup_server : public message {
        spawn_troup_server() : message(code::spawn_troup_server) {}
        uint8_t playerId;
        uint8_t troupId;
        uint8_t sourceId;
        uint8_t destinationId;
        uint8_t unitCount;
    } __attribute__((aligned(8)));

    struct next_troup_destination : public message {
        next_troup_destination() : message(code::next_troup_destination) {}
        uint8_t mapX;
        uint8_t mapY;
        uint8_t troupId;
        uint16_t time;
    } __attribute__((aligned(8)));

    struct troup_arrived : public message {
        troup_arrived() : message(code::troup_arrived) {}
        uint8_t troupId;
    } __attribute__((aligned(8)));

    struct building_owner_changed : public message {
        building_owner_changed() : message(code::building_owner_changed) {}
        uint8_t buildingId;
        uint8_t oldOwner;
        uint8_t newOwner;
    } __attribute__((aligned(8)));

    struct building_upgrade : public message {
        building_upgrade() : message(code::building_upgrade) {}
        uint8_t buildingId;
        uint8_t state;
    } __attribute__((aligned(8)));

    struct building_unit_generated : public message {
        building_unit_generated() : message(code::building_unit_generated) {}
        uint8_t buildingId;
        uint8_t newUnitCount;
    } __attribute__((aligned(8)));

	// client -> server

    struct spawn_troup_client : public message {
        spawn_troup_client() : message(code::spawn_troup_client) {}
        uint8_t playerId;
        uint8_t sourceId;
        uint8_t destinationId;
        uint8_t unitCount;
    } __attribute__((aligned(8)));
    
    struct building_upgrade_client : public message {
        building_upgrade_client() : message(code::building_upgrade_client) {}
        uint8_t buildingId;
        uint8_t state;
    } __attribute__((aligned(8)));
}

template<typename M> M make_message() {
	M m;
	m.message_size = sizeof(M);
	return m;
}


class message_reader {
public:
	typedef boost::asio::ip::tcp::socket socket;
private:
	socket *sock;
	static const unsigned int buffer_size = 1024;
	char buffer[buffer_size];
	char *curr_input;
	bool got_eof;
	size_t msg_size;

public:
	message_reader();
	void read_and_handle();
	bool eof() { return got_eof; }
	void setSocket(socket *s) { sock = s; }

protected:
	void read_message_part();
	void handle_message();
	void warn(msg::message *msg);

	virtual void handle_message(msg::init_game *m) { warn(m); }
	virtual void handle_message(msg::spawn_house *m) { warn(m); }
	virtual void handle_message(msg::spawn_tree *m) { warn(m); }
	virtual void handle_message(msg::init_done *m) { warn(m); }
    virtual void handle_message(msg::spawn_troup_server *m) { warn(m); }
    virtual void handle_message(msg::spawn_troup_client *m) { warn(m); }
    virtual void handle_message(msg::next_troup_destination *m) { warn(m); }
    virtual void handle_message(msg::troup_arrived *m) { warn(m); }
    virtual void handle_message(msg::building_owner_changed *m) { warn(m); }
    virtual void handle_message(msg::building_unit_generated *m) { warn(m); }
    virtual void handle_message(msg::building_upgrade *m) { warn(m); }
    virtual void handle_message(msg::building_upgrade_client *m) { warn(m); }
};


#endif

