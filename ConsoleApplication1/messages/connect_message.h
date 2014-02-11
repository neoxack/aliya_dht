#pragma once

#include <stdint.h>
#include <array>

#include "../constants.h"
#include "../packet.h"

namespace aliya_dht {

	class connect_message
	{
	public:

		void serialize(std::array<char, aliya_dht::buf_size> &buf) const
		{
			header header;
			header.type = packet_type::CONNECT;
			header.timestamp = time(0);
			header.size = size();
			header.serialize(buf.data());
			std::memcpy(buf.data()+sizeof(header), &message, sizeof(message));
		}

		bool parse(const std::array<char, aliya_dht::buf_size> &buf, const header &head)
		{
			if(head.type != packet_type::CONNECT) return false;
			std::memcpy(&message, buf.data()+sizeof(header), sizeof(message));
			return true;
		}

		size_t size() const
		{
			return sizeof(message) + sizeof(header);
		}

		#pragma pack(1)
		struct data
		{
			uint32_t result;
		} message;

		enum
		{
			STARTING  = 0x25cbfc4f,
			ACCEPT = 0xb320ed34,
			DENY = 0x9bb3932
		};

};

}