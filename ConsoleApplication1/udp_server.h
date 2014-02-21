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
	class udp_server
	{
	public:

		udp_server(boost::asio::io_service& io_service, const udp::endpoint &endpoint, Handler *handler):
			_socket(io_service, endpoint), 
			_handler(handler), 
			_my_endpoint(endpoint)
		{
			_socket.set_option(udp::socket::reuse_address(true));
		}

		~udp_server(void)
		{
		}

		void do_receive()
		{
			std::thread t1([&]
			{
				for(;;)
				{
					boost::system::error_code error;
					size_t bytes_recvd = _socket.receive_from(boost::asio::buffer(_buf, buf_size), _from_endpoint, 0, error);
					if(!error)
						_handler->handle(_from_endpoint, _buf, bytes_recvd);
				}
			});
			t1.detach();
		}

		template<typename Message>
		void send(const Message &message, const udp::endpoint &endpoint)
		{
			message.serialize(_send_buf);
			_to_endpoint = endpoint;
			boost::system::error_code error;
			size_t bytes_sent = _socket.send_to(boost::asio::buffer(_send_buf, message.size()), _to_endpoint, 0, error);
			on_send(error, bytes_sent);
		}

	private:

		void on_send(const boost::system::error_code& error, size_t bytes_sent)
		{
			//std::cout << "sent message to " << _to_endpoint.address().to_string()<< ":" << _to_endpoint.port() << std::endl;
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