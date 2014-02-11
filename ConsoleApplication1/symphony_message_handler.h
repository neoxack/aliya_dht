#pragma once

#include <iostream>
#include "async_udp_server.h"


namespace aliya_dht {

class connect_message;
class join_message;
class node;
class symphony_message_handler
{
public:
	

	symphony_message_handler(aliya_dht::node * nod);

	void set_net_service(aliya_dht::async_udp_client_server<symphony_message_handler> * net_service);

	void handle(const boost::asio::ip::udp::endpoint &sender_endpoint, const std::array<char, aliya_dht::buf_size> &buffer,  size_t bytes_recvd);
	~symphony_message_handler(){}

private:
	

	void handle (const connect_message &message, const udp::endpoint &sender_endpoint);
	void handle (const join_message &message, const udp::endpoint &sender_endpoint);

	symphony_message_handler( const symphony_message_handler& );
	const symphony_message_handler& operator=( const symphony_message_handler& );

	aliya_dht::node *_node;
	aliya_dht::async_udp_client_server<symphony_message_handler> * _net_service;
};

}