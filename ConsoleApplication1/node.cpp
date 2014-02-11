#include "node.h"
#include "symphony_message_handler.h"
#include "messages\connect_message.h"
#include "async_udp_server.h"

using namespace boost::asio;


void aliya_dht::node::init_udp_server()
{
	_message_handler = std::unique_ptr<symphony_message_handler>(new symphony_message_handler(this));
	_server = std::unique_ptr<async_udp_client_server<symphony_message_handler>>(new async_udp_client_server<symphony_message_handler>(_service, _endpoint, _message_handler.get()));
	_message_handler->set_net_service(_server.get());
	_server->do_receive();
	std::thread t([&]{_service.run();});
	t.detach();
}

void aliya_dht::node::join_to_dht(const std::string &ip_adr, unsigned short port)
{
	ip::address adr = ip::address::from_string(ip_adr);
	ip::udp::endpoint endpoint = ip::udp::endpoint(adr, port);
	connect_message mess;
	mess.message.result = connect_message::STARTING;

	_server->async_send(mess, endpoint);
}
