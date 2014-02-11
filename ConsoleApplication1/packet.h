#pragma once

#include <stdint.h>
#include <cstring>

namespace aliya_dht {

	enum  packet_type
	{
		CONNECT = 0x74cff91f,
		JOIN = 0x2861d8d7,
		NEIGHBORS = 0x4e9f6278,
		QUERY_CONNECT = 0x3e65c83d,
		ACCEPT_CONNECT = 0x33bcf5c0
	};

	#pragma pack(1)
	struct header
	{
		uint32_t type;
		uint32_t size;
		time_t timestamp;
	
		void serialize(char * data) const
		{
			std::memcpy(data, this, sizeof(header));
		}

		void deserialize(const char * const data) 
		{
			std::memcpy(this, data, sizeof(header));
		}
	};

	typedef struct header header;

}