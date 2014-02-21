#pragma once

#include <stdint.h>
#include <time.h>
#include <boost/asio.hpp>

#include "../constants.h"
#include "../packet.h"
#include "../node.h"


namespace aliya_dht {
	using boost::asio::ip::udp;

	class neighbors_message
	{
	public: 

		neighbors_message()
		{}

		neighbors_message(const std::vector<node_entry> &entries)
		{
			message.neighbors_count = entries.size();
			size_t i = 0;
			for(const auto &entry: entries)
			{
				neighbor current_neighbor;
				current_neighbor.node_id = entry.node_id;
				current_neighbor.port = entry.endpoint.port();
				if(entry.endpoint.address().is_v4())
				{
					current_neighbor.address_type = V4;
					current_neighbor.addr_v4 = entry.endpoint.address().to_v4().to_bytes();
				}
				else
				{
					current_neighbor.address_type = V6;
					current_neighbor.addr_v6 = entry.endpoint.address().to_v6().to_bytes();
				}

				message.neighbors[i] = current_neighbor;
				i++;
			}
		}

		void serialize(std::array<char, aliya_dht::buf_size> &buf) const
		{
			header header;
			header.type = packet_type::NEIGHBORS;
			header.timestamp = time(0);
			header.size = size();
			header.serialize(buf.data());

			std::memcpy(buf.data()+sizeof(header), &message, sizeof(message.neighbors_count) + sizeof(neighbor)*message.neighbors_count);
		}

		bool parse(const std::array<char, aliya_dht::buf_size> &buf, const header &head)
		{
			if(head.type != packet_type::NEIGHBORS) return false;
			std::memcpy(&message, buf.data()+sizeof(header), head.size - sizeof(header));
			return true;
		}

		size_t size() const
		{
			return sizeof(header) + sizeof(message.neighbors_count) + sizeof(neighbor)*message.neighbors_count;
		}

		

		#pragma pack(1)
		struct neighbor
		{
			double node_id;
			uint32_t address_type;
			unsigned short port;
			union {
				boost::asio::ip::address_v6::bytes_type addr_v6;
				boost::asio::ip::address_v4::bytes_type addr_v4;
			};	
		};
		typedef struct neighbor neighbor;

		#pragma pack(1)
		struct data
		{
			uint32_t neighbors_count;
			neighbor neighbors[aliya_dht::max_neighbors + 1];
		} message;

		enum {
			V4 = 0x1908383a,
			V6 = 0xf7065916
		};

		boost::asio::ip::udp::endpoint get_endpoint(const neighbor& neighbor) const
		{
			if(neighbor.address_type == V4)
			{
				boost::asio::ip::address_v4 adr_v4(neighbor.addr_v4);
				return boost::asio::ip::udp::endpoint(adr_v4, neighbor.port);
			}
			else
			{
				boost::asio::ip::address_v6 adr_v6(neighbor.addr_v6);
				return boost::asio::ip::udp::endpoint(adr_v6, neighbor.port);
			}
		}


	};

}


