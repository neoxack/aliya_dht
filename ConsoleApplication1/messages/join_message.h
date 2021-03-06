#pragma once

#include <stdint.h>
#include <boost/asio.hpp>

#include "../constants.h"
#include "../packet.h"


namespace aliya_dht {

	using boost::asio::ip::udp;

	class join_message
	{
	public: 

		join_message(){}

		join_message(double node_id, const udp::endpoint &endpoint)
		{
			message.node_id = node_id;
			if(endpoint.address().is_v4())
			{
				message.address_type = V4;
				message.addr_v4 = endpoint.address().to_v4().to_bytes();
			}
			else
			{
				message.address_type = V6;
				message.addr_v6 = endpoint.address().to_v6().to_bytes();
			}
			message.port = endpoint.port();			
		}

		void serialize(std::array<char, aliya_dht::buf_size> &buf) const
		{
			header header;
			header.type = packet_type::JOIN;
			header.timestamp = time(0);
			header.size = size();
			header.serialize(buf.data());
			std::memcpy(buf.data()+sizeof(header), &message, sizeof(message));
		}

		bool parse(const std::array<char, aliya_dht::buf_size> &buf, const header &head)
		{
			if(head.type != packet_type::JOIN) return false;
			std::memcpy(&message, buf.data()+sizeof(header), sizeof(message));
			return true;
		}

		size_t size() const
		{
			return sizeof(message) + sizeof(header);
		}

		boost::asio::ip::udp::endpoint get_endpoint() const
		{
			if(message.address_type == V4)
			{
				boost::asio::ip::address_v4 adr_v4(message.addr_v4);
				return boost::asio::ip::udp::endpoint(adr_v4, message.port);
			}
			else
			{
				boost::asio::ip::address_v6 adr_v6(message.addr_v6);
				return boost::asio::ip::udp::endpoint(adr_v6, message.port);
			}

		}

		#pragma pack(1)
		struct data
		{
			double node_id;
			uint32_t address_type;
			unsigned short port;
			union {
				boost::asio::ip::address_v6::bytes_type addr_v6;
				boost::asio::ip::address_v4::bytes_type addr_v4;
			};					
		} message;

		enum {
			V4 = 0x1908383a,
			V6 = 0xf7065916
		};

	};

}