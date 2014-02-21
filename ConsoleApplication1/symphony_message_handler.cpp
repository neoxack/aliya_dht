#include "symphony_message_handler.h"
#include "messages\connect_message.h"
#include "messages\join_message.h"
#include "messages\neighbors_message.h"
#include "messages\find_message.h"
#include "node.h"


aliya_dht::symphony_message_handler::symphony_message_handler(aliya_dht::node * nod) :_node(nod)
{

}

void aliya_dht::symphony_message_handler::set_net_service(aliya_dht::udp_server<symphony_message_handler> * net_service)
{
	_net_service = net_service;
}

void aliya_dht::symphony_message_handler::handle(const udp::endpoint &sender_endpoint, const std::array<char, aliya_dht::buf_size> &buffer, size_t bytes_recvd)
{
	header head;
	head.deserialize(buffer.data());

	//std::cout << "receive message from " << sender_endpoint.address().to_string()<< ":" << sender_endpoint.port() << std::endl;
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
	case packet_type::NEIGHBORS:
		{
			neighbors_message message;
			message.parse(buffer, head);
			handle(message, sender_endpoint);
			break;
		}
	case packet_type::FIND:
		{
			find_message message;
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
		_net_service->send(new_message, sender_endpoint);
	}
	if(message.message.result == connect_message::ACCEPT)
	{
		std::cout << " message: connect accepted" << std::endl;

		join_message join_mes(_node->get_id(), _node->get_endpoint());
		_net_service->send(join_mes, sender_endpoint);
	}
	//std::cout << " message: connect, param: " << message.message.result << std::endl;
}


void aliya_dht::symphony_message_handler::handle(const join_message &message, const udp::endpoint &sender_endpoint)
{
	std::cout << " message: join" << std::endl;
	node_entry responsible_node = _node->find_responsible(message.message.node_id);
	if(responsible_node.node_id == _node->get_id()) //i'm responsible
	{
		std::vector<node_entry> entries;
		entries.push_back(_node->entry());
		node_entry prev = _node->get_prev_node();
		node_entry next = _node->get_next_node();
		if(prev.node_id != _node->get_id()) entries.push_back(prev);
		if(next.node_id != _node->get_id()) entries.push_back(next);

		node_entry temp_node;
		temp_node.node_id = message.message.node_id;
		temp_node.endpoint = message.get_endpoint();
		_node->add_neighbor(temp_node);

		neighbors_message n_message(entries);
		_net_service->send(n_message, message.get_endpoint());
	}
	else
	{
		_net_service->send(message, responsible_node.endpoint);
	}

	//std::cout << " message: connect, param: " << message.message.result << std::endl;
}

void aliya_dht::symphony_message_handler::handle(const neighbors_message &message, const udp::endpoint &sender_endpoint)
{
	std::cout << " message: neighbors, count: " << message.message.neighbors_count << std::endl;

	for(uint32_t i = 0; i< message.message.neighbors_count; i++)
	{
		node_entry temp_node;
		temp_node.node_id = message.message.neighbors[i].node_id;
		temp_node.endpoint = message.get_endpoint(message.message.neighbors[i]);
		if(temp_node.node_id < _node->get_id())
		{
			if(_node->get_prev_node().node_id == _node->get_id() || temp_node.node_id > _node->get_prev_node().node_id)
				_node->set_prev_node(temp_node);
		}

		if(temp_node.node_id > _node->get_id())
		{
			if(_node->get_next_node().node_id == _node->get_id() || temp_node.node_id < _node->get_next_node().node_id)
				_node->set_next_node(temp_node);
		}
	}

	if(message.message.neighbors_count > 1)
	{
		std::vector<node_entry> entries;
		for(uint32_t i = 0; i< message.message.neighbors_count; i++)
		{
			entries.clear();
			entries.push_back(_node->entry());
			neighbors_message n_message(entries);
			_net_service->send(n_message, message.get_endpoint(message.message.neighbors[i]));
		}
	}
	
	
}

void aliya_dht::symphony_message_handler::handle(const find_message &message, const udp::endpoint &sender_endpoint)
{
	if(message.message.type == find_message::REQUEST) 
	{
		node_entry responsible_node = _node->find_responsible(message.message.id);
		if(responsible_node.node_id == _node->_entry.node_id) //i'm responsible
		{
			find_message f_message(_node->_entry.node_id, find_message::RESPONSE, _node->_entry.endpoint);
			_net_service->send(f_message, message.get_endpoint());
		}
		else
		{
			_net_service->send(message, responsible_node.endpoint);
		}
		return;
	}
	if(message.message.type == find_message::RESPONSE) 
	{
		//_node->_find_timer.cancel();
		node_entry temp_node;
		temp_node.node_id = message.message.id;
		temp_node.endpoint = message.get_endpoint();
		_node->_find_callback(temp_node, aliya_dht::FIND_OK);
		return;
	}
}
