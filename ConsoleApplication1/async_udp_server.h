#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

#include "constants.h"
#include "packet.h"


namespace aliya_dht
{
	using boost::asio::ip::udp;
	
	template<typename Handler>
	class async_udp_client_server
	{
	public:

		async_udp_client_server(boost::asio::io_service& io_service, const udp::endpoint &endpoint, Handler *handler):_socket(io_service, endpoint), _handler(handler), _my_endpoint(endpoint)
		{
			_socket.set_option(udp::socket::reuse_address(true));
		}

		~async_udp_client_server(void)
		{
		}

		template<typename Message>
		void async_send(const Message &message, const udp::endpoint &endpoint)
		{
			message.serialize(_send_buf);
			_to_endpoint = endpoint;
			_socket.async_send_to(boost::asio::buffer(_send_buf, message.size()), _to_endpoint, boost::bind(&async_udp_client_server::on_send, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}

		void do_receive()
		{
			_socket.async_receive_from(boost::asio::buffer(_buf, buf_size), _from_endpoint, boost::bind(&async_udp_client_server::on_receive, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}

	private:

		void on_receive(const boost::system::error_code& error, size_t bytes_recvd)
		{
			if(!error)
			{
				_handler->handle(_from_endpoint, _buf, bytes_recvd);			
				do_receive();
			}
		}

		void on_send(const boost::system::error_code& error, size_t bytes_sent)
		{
			std::cout << "sent message to " << _to_endpoint.address().to_string()<< ":" << _to_endpoint.port() << std::endl;
		}

		Handler *_handler;
		udp::socket _socket;

		udp::endpoint _to_endpoint;
		udp::endpoint _from_endpoint;
		udp::endpoint _my_endpoint;

		std::array<char, aliya_dht::buf_size> _send_buf;
		std::array<char, aliya_dht::buf_size> _buf;
	
	};

}