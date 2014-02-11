#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "random_utils.h"
#include "async_udp_server.h"
#include "symphony_message_handler.h"

namespace aliya_dht 
{
	using namespace boost::asio;

	class symphony_message_handler;

	class node
	{
	public:

		node() :_node_id(random_utils::rand())
		{
		}

		void start(const ip::udp::endpoint &endpoint)
		{
			_endpoint = endpoint;
			init_udp_server();
		}

		void start(const std::string &ip_adr, unsigned short port)
		{
			ip::address adr = ip::address::from_string(ip_adr);
			_endpoint = ip::udp::endpoint(adr, port);
			init_udp_server();
		}

		void join_to_dht(const std::string &ip_adr, unsigned short port);

		~node()
		{
		}

		std::string to_string() const
		{
			std::stringstream result;
			result<< _endpoint.address().to_string() << ":" << _endpoint.port() << " id: < " << std::to_string(_node_id) << " >";
			return result.str();
		}

		double get_id() const
		{
			return _node_id;
		}
		const ip::udp::endpoint& get_endpoint() const
		{
			return _endpoint;
		}

	private:

		double _node_id;
		ip::udp::endpoint _endpoint;

		typedef struct another_node
		{
			double node_id;
			ip::udp::endpoint endpoint;
		} another_node;

		another_node _prev_node;
		another_node _next_node;
		std::vector<another_node> _long_nodes;

		io_service _service;
		std::unique_ptr<async_udp_client_server<symphony_message_handler>> _server;
		std::unique_ptr<symphony_message_handler> _message_handler;

		void init_udp_server();


	};

}