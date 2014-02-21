#pragma once

#include <stdint.h>
#include <boost/asio.hpp>

#include "../constants.h"
#include "../packet.h"


namespace aliya_dht {

	using boost::asio::ip::udp;

	class find_message
	{
	public: 

		find_message(){}

		find_message(double id, double node_id, uint32_t type, const udp::endpoint &endpoint)
		{
			message.id = id;
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
			message.type = type;
			message.port = endpoint.port();			
		}

		void serialize(std::array<char, aliya_dht::buf_size> &buf) const
		{
			header header;
			header.type = packet_type::FIND;
			header.timestamp = time(0);
			header.size = size();
			header.serialize(buf.data());
			std::memcpy(buf.data()+sizeof(header), &message, sizeof(message));
		}

		bool parse(const std::array<char, aliya_dht::buf_size> &buf, const header &head)
		{
			if(head.type != packet_type::FIND) return false;
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
			double id;
			double node_id;
			uint32_t type;
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

		enum {
			RESPONSE = 0x3e7b0bfb,
			REQUEST = 0x3b978f9f
		};

	};

}