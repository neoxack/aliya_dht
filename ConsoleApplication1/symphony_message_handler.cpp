#include "symphony_message_handler.h"
#include "messages\connect_message.h"
#include "messages\join_message.h"
#include "node.h"


aliya_dht::symphony_message_handler::symphony_message_handler(aliya_dht::node * nod) :_node(nod)
{

}

void aliya_dht::symphony_message_handler::set_net_service(aliya_dht::async_udp_client_server<symphony_message_handler> * net_service)
{
	_net_service = net_service;
}

void aliya_dht::symphony_message_handler::handle(const udp::endpoint &sender_endpoint, const std::array<char, aliya_dht::buf_size> &buffer, size_t bytes_recvd)
{
	header head;
	head.deserialize(buffer.data());

	std::cout << "receive message from " << sender_endpoint.address().to_string()<< ":" << sender_endpoint.port() << std::endl;
	switch(head.type)
	{
	case packet_type::CONNECT:
		{
			connect_message message;
			message.parse(buffer, head);
			handle(message, sender_endpoint);
			break;
		}
	case packet_type::JOIN:
		{
			join_message message;
			message.parse(buffer, head);
			handle(message, sender_endpoint);
			break;
		}
	}
}



void aliya_dht::symphony_message_handler::handle(const connect_message &message, const udp::endpoint &sender_endpoint)
{
	if(message.message.result == connect_message::STARTING)
	{
		connect_message new_message;
		new_message.message.result = connect_message::ACCEPT;
		_net_service->async_send(new_message, sender_endpoint);
	}
	if(message.message.result == connect_message::ACCEPT)
	{
		std::cout << " message: connect accepted" << std::endl;

		join_message join_mes(_node->get_id(), _node->get_endpoint());
		_net_service->async_send(join_mes, sender_endpoint);
	}
	//std::cout << " message: connect, param: " << message.message.result << std::endl;
}


void aliya_dht::symphony_message_handler::handle(const join_message &message, const udp::endpoint &sender_endpoint)
{
	std::cout << " message: join" << std::endl;

	//std::cout << " message: connect, param: " << message.message.result << std::endl;
}