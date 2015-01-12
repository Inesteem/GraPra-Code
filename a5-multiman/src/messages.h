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
			init_done/*
			initial_player_data_position,
			board_info,
			spawn_box,
			start_game,
			force_player_position,
			start_move,
			spawn_bomb,
			bomb_explosion,
			player_health_info,
			keep_alive,
			frags_update,
			game_over,*/
			

			/*
			key_updown,
			key_drop,
			player_name,*/
		};
	}

/*
	namespace key_code {
		enum {
			up,
			down,
			left,
			right,
		};
	}

	namespace box_type {
		enum t {
			none = 0,
			stone = 1,
			wood = 2,
		};
	}
*/

	struct message {
		uint8_t message_size, message_type;
		message(uint8_t type) : message_type(type) {}
	};

	// server -> client

/*
	struct connected : public message {
		connected() : message(code::connected) {}
		uint8_t your_id, players;
	} __attribute__((aligned(8)));

	struct initial_player_data_position : public message {
		initial_player_data_position() : message(code::initial_player_data_position) {}
		uint8_t player;
		uint8_t x, y;
	} __attribute__((aligned(8)));

	struct board_info : public message {
		board_info() : message(code::board_info) {}
		uint8_t w, h;
		uint16_t boxes;
	} __attribute__((aligned(8)));

	struct spawn_box : public message {
		spawn_box() : message(code::spawn_box) {}
		uint8_t box_type;
		uint8_t x,y;
	} __attribute__((aligned(8)));

	struct start_game : public message {
		start_game() : message(code::start_game) {}
		//
	} __attribute__((aligned(8)));

	struct force_player_position : public message {
		force_player_position() : message(code::force_player_position) {}
		uint8_t player;
		uint8_t x,y;
	} __attribute__((aligned(8)));

	struct start_move : public message {
		start_move() : message(code::start_move) {}
		uint8_t player;
		int8_t dir_x, dir_y;	// not uint!
		uint16_t est_duration;
	} __attribute__((aligned(8)));

	struct spawn_bomb : public message {
		spawn_bomb() : message(code::spawn_bomb) {}
		uint8_t x,y;
		uint16_t id;
		uint16_t est_duration;
	} __attribute__((aligned(8)));

	struct bomb_explosion : public message {
		bomb_explosion() : message(code::bomb_explosion) {}
		uint16_t id;
		uint16_t dir_codes;
	} __attribute__((aligned(8)));

	struct player_health_info : public message {
		player_health_info() : message(code::player_health_info) {}
		uint8_t id;
		uint8_t health;
	} __attribute__((aligned(8)));

	struct keep_alive : public message {
		keep_alive() : message(code::keep_alive) {}
	} __attribute__((aligned(8)));

	struct frags_update : public message {
		frags_update() : message(code::frags_update) {}
		uint8_t player;
		int8_t frags;
	} __attribute__((aligned(8)));

	struct game_over : public message {
		game_over() : message(code::game_over) {}
	} __attribute__((aligned(8)));
*/

	struct init_game : public message {
		init_game() : message(code::init_game) {}
		uint16_t mapX;
		uint16_t mapY;
	} __attribute__((aligned(8)));

	struct spawn_house : public message {
		spawn_house() : message(code::spawn_house) {}		
		uint16_t x;
		uint16_t y;
	} __attribute__((aligned(8)));

	struct init_done : public message {
		init_done() : message(code::init_done) {}
	} __attribute__((aligned(8)));


	// client -> server

/*

	struct key_updown : public message {
		key_updown() : message(code::key_updown) {}
		uint8_t k;
		uint8_t down; // down==1, up==0
	} __attribute__((aligned(8)));

	struct key_drop : public message {
		key_drop() : message(code::key_drop) {}
	} __attribute__((aligned(8)));

	struct player_name : public message {
		player_name() : message(code::player_name) {}
		enum { max_string_length = 26 };
		uint8_t id;	// not used for client->server, but for server->client.
		char name[max_string_length];
	} __attribute__((aligned(8)));
*/
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

	// server -> client
	virtual void handle_message(msg::init_game *m) { warn(m); }
	virtual void handle_message(msg::spawn_house *m) { warn(m); }
	virtual void handle_message(msg::init_done *m) { warn(m); }
	
	// client -> server

};


#endif

