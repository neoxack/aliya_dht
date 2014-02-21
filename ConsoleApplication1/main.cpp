
#include "stdafx.h"
#include "node.h"
#include <iostream>
#include <conio.h>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		std::array<aliya_dht::node, 20> nodes;
		for(size_t i = 0; i< nodes.size(); i++)
		{
			nodes[i].start ("127.0.0.1", 9000+i);
			cout << nodes[i].to_string() << endl;
		}
		
	
		_getch();

		for(size_t i = 0; i< nodes.size(); i++)
		{
			nodes[i].join_to_dht("127.0.0.1", 9001);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		_getch();

		for(size_t i = 0; i< nodes.size(); i++)
		{
			nodes[1].find_node(to_string(i),[](aliya_dht::node_entry entry, int error){ if(error == aliya_dht::NOT_FIND) cout <<"timeout"<< endl; cout << entry.node_id << endl;});
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}

		_getch();
			
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}


	
	return 0;
}

