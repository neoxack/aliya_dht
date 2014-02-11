#pragma once

#include "async_udp_server.h"

namespace aliya_dht
{
	struct net_operation
	{
		std::array<char, aliya_dht::buf_size> buf;
	};

}
