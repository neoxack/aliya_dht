#pragma once

#include <stdint.h>
#include <time.h>
#include <boost/asio.hpp>

#include "../constants.h"
#include "../packet.h"


namespace aliya_dht {
	using boost::asio::ip::udp;

	class neighbors_message
	{
	public: 

		neighbors_message(){}

		neighbors_message(double node_id, const udp::endpoint &endpoint)
		{
			message.node_id = node_id;
			message.address_type = endpoint.address().is_v4() ? V4 : V6;
			message.addr = endpoint.address();		
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

		#pragma pack(1)
		struct neighbor
		{
			double node_id;
			uint32_t address_type;
			unsigned short port;
			boost::asio::ip::address addr;
		};

		#pragma pack(1)
		struct data
		{
			uint32_t neighbors_count;
			struct neighbor neighbors[0];
		} message;



		enum {
			V4 = 0x1908383a,
			V6 = 0xf7065916
		};

	};

}


