#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <unordered_map>

#include "random_utils.h"
#include "udp_server.h"
#include "symphony_message_handler.h"

namespace aliya_dht 
{
	using namespace boost::asio;

	class symphony_message_handler;

	typedef struct node_entry
	{
		double node_id;
		ip::udp::endpoint endpoint;
	} node_entry;

	class node
	{
	public:

		node():_find_timer(_timer_service)
		{
			_entry.node_id = random_utils::rand();
		}

		void start(const ip::udp::endpoint &endpoint)
		{
			_entry.endpoint = endpoint;
			_prev_node = _entry;
			_next_node = _entry;
			init_udp_server();
		}

		void start(const std::string &ip_adr, unsigned short port)
		{
			ip::address adr = ip::address::from_string(ip_adr);
			ip::udp::endpoint endpoint = ip::udp::endpoint(adr, port);
			start(endpoint);
		}

		void join_to_dht(const std::string &ip_adr, unsigned short port);

		~node()
		{
			_timer_service.stop();
		}

		std::string to_string() const
		{
			std::stringstream result;
			result<< _entry.endpoint.address().to_string() << ":" << _entry.endpoint.port() << " id: < " << std::to_string(_entry.node_id) << " >";
			return result.str();
		}

		double get_id() const
		{
			return _entry.node_id;
		}
		const ip::udp::endpoint& get_endpoint() const
		{
			return _entry.endpoint;
		}

		node_entry find_responsible(double id) const
		{
			if(id < _entry.node_id)
			{
				if(id > _prev_node.node_id) return _entry;
				else return _prev_node;
			}
			else
			{
				return _next_node;
			}
		}

		node_entry get_prev_node() const
		{
			return _prev_node;
		}

		node_entry get_next_node() const
		{
			return _next_node;
		}

		void set_prev_node(const node_entry &entry)
		{
			_prev_node.endpoint = entry.endpoint;
			_prev_node.node_id = entry.node_id;
		}

		void set_next_node(const node_entry &entry)
		{
			_next_node.endpoint = entry.endpoint;
			_next_node.node_id = entry.node_id;
		}

		void add_neighbor(const node_entry &entry)
		{
			if(entry.node_id > _entry.node_id)
			{
				set_next_node(entry);
			}
			else
			{
				set_prev_node(entry);
			}
		}

		node_entry entry() const
		{
			return _entry;
		}

		void put(const std::string &key, const std::string &value);
		std::string get(const std::string &key);
		void find_node(const std::string &key, const std::function<void(node_entry, int)> &callback);

	private:
		node_entry _entry;
		node_entry _prev_node;
		node_entry _next_node;
		std::vector<node_entry> _long_nodes;

		io_service _service;
		io_service _timer_service;
		std::unique_ptr<udp_server<symphony_message_handler>> _server;
		std::unique_ptr<symphony_message_handler> _message_handler;
		boost::asio::deadline_timer _find_timer;

		std::unordered_map<double, std::function<void(node_entry, int)>> _find_nodes_map;
		void init_udp_server();
		std::thread                   _timer_thread;

		void on_timeout(const boost::system::error_code& e)
		{
			if(!e)
			{
				_find_callback(_entry, aliya_dht::NOT_FIND);
			}
			
		}

		friend class symphony_message_handler;

	};

}