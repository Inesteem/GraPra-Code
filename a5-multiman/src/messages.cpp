#include "messages.h"

#include <iostream>

using namespace std;

message_reader::message_reader() : got_eof(false), msg_size(0) { 
	 curr_input = buffer; 
}

void message_reader::read_and_handle() {
	// check if data is available
	boost::system::error_code av_error;
	size_t av = sock->available(av_error);
	if (av_error) throw boost::system::system_error(av_error);
	if (av == 0) return;
	// copy data to our buffer
	boost::system::error_code error;
	size_t len = boost::asio::read(*sock, boost::asio::buffer(curr_input, buffer_size-(curr_input-buffer)), boost::asio::transfer_at_least(av), error);
	if (error == boost::asio::error::eof) got_eof = true;
	else if (error)                       throw boost::system::system_error(error);
	curr_input += len;
	// manage received data
	if (msg_size != 0)
		read_message_part();
	while (msg_size == 0 && curr_input - buffer > 0) {
		msg_size = buffer[0];
		read_message_part();
	}
}
	
void message_reader::read_message_part() {
	size_t read = curr_input - buffer;
	if (read >= msg_size && read > 0) {
// 		cout << "read message of size " << msg_size << endl;
		handle_message();
		for (int i = msg_size; i < read; ++i)
			buffer[i-msg_size] = buffer[i];
		curr_input = buffer + (read - msg_size);
		msg_size = 0;
	}
}
	
void message_reader::handle_message() {
	switch (buffer[1]) {
        case msg::code::init_game:                  handle_message((msg::init_game*)buffer); break;
        case msg::code::init_done:                  handle_message((msg::init_done*)buffer); break;
        case msg::code::spawn_house:                handle_message((msg::spawn_house*)buffer); break;
        case msg::code::spawn_tree:                 handle_message((msg::spawn_tree*)buffer); break;
        case msg::code::spawn_troup_server:         handle_message((msg::spawn_troup_server*)buffer); break;
        case msg::code::spawn_troup_client:         handle_message((msg::spawn_troup_client*)buffer); break;
        case msg::code::next_troup_destination:     handle_message((msg::next_troup_destination*)buffer); break;
        case msg::code::troup_arrived:              handle_message((msg::troup_arrived*)buffer); break;
        case msg::code::building_owner_changed:     handle_message((msg::building_owner_changed*)buffer); break;
        case msg::code::building_unit_generated:    handle_message((msg::building_unit_generated*)buffer); break;
        case msg::code::building_upgrade:    		handle_message((msg::building_upgrade*)buffer); break;
        case msg::code::building_upgrade_house:     handle_message((msg::building_upgrade_house*)buffer); break;
        case msg::code::building_upgrade_turret:    handle_message((msg::building_upgrade_turret*)buffer); break;
        case msg::code::game_over:                  handle_message((msg::game_over*)buffer); break;
        case msg::code::client_settings:            handle_message((msg::client_settings*)buffer); break;
        case msg::code::spawn_random_stuff:         handle_message((msg::spawn_random_stuff*)buffer); break;
        case msg::code::new_player:                 handle_message((msg::new_player*)buffer); break;






		default:
			cerr << "got a message with unknown code " << (int)buffer[1] << endl;
			exit(-1);
	}
}
void message_reader::warn(msg::message *msg) {
	cout << "got a message of type " << (int)msg->message_type << " for which there is no handler defined!" << endl;
}

/* vim: set foldmethod=marker: */

