#include <boost/crc.hpp>

#include "node.h"
#include "symphony_message_handler.h"
#include "messages\connect_message.h"
#include "messages\find_message.h"
#include "udp_server.h"

using namespace boost::asio;


void aliya_dht::node::init_udp_server()
{
	_message_handler = std::unique_ptr<symphony_message_handler>(new symphony_message_handler(this));
	_server = std::unique_ptr<udp_server<symphony_message_handler>>(new udp_server<symphony_message_handler>(_service, _entry.endpoint, _message_handler.get()));
	_message_handler->set_net_service(_server.get());
	_server->do_receive();
	//std::thread t1([&]{_service.run();});
}

void aliya_dht::node::join_to_dht(const std::string &ip_adr, unsigned short port)
{
	ip::address adr = ip::address::from_string(ip_adr);
	ip::udp::endpoint endpoint = ip::udp::endpoint(adr, port);
	connect_message mess;
	mess.message.result = connect_message::STARTING;

	_server->send(mess, endpoint);
}

double get_key_id(const std::string &key)
{
	boost::crc_32_type result;
	result.process_bytes(key.data(), key.length());
	uint32_t checksum = result.checksum();
	return (double)((long double)checksum / UINT32_MAX);
}

void aliya_dht::node::put(const std::string &key, const std::string &value)
{
	double id = get_key_id(key);
}

std::string aliya_dht::node::get(const std::string &key)
{
	double id = get_key_id(key);
	return "not found";
}

void aliya_dht::node::find_node(const std::string &key, const std::function<void(node_entry, int)> &callback)
{
	double id = get_key_id(key);
	
	node_entry responsible_node = find_responsible(id);
	if(responsible_node.node_id == _entry.node_id)
	{
		callback(_entry, aliya_dht::FIND_OK);
	}
	else
	{
		_find_nodes_map[id] = callback;
		//_find_timer.expires_from_now(boost::posix_time::seconds(3));	
		//_find_timer.async_wait(boost::bind(&aliya_dht::node::on_timeout, this, boost::asio::placeholders::error));		
		find_message message(id, find_message::REQUEST, _entry.endpoint);
		_server->send(message, responsible_node.endpoint);
	}
}

