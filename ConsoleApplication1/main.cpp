
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
		aliya_dht::node node_b;
		node_b.start("127.0.0.1", 9001);
		aliya_dht::node node_a;
		node_a.start("127.0.0.1", 9000);
		aliya_dht::node node_c;
		node_c.start("127.0.0.1", 9003);
		
		cout << node_a.to_string() << endl;
		cout << node_b.to_string() << endl;
		cout << node_c.to_string() << endl;
		cout << sizeof(ip::address) << endl;
		cout << sizeof(ip::address_v4) << endl;
		cout << sizeof(ip::address_v6) << endl;


		_getch();
		for(int i = 0; i< 10; i++){

			node_a.join_to_dht("127.0.0.1", 9001);
			
			_getch();
		}

		_getch();
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}


	
	return 0;
}

