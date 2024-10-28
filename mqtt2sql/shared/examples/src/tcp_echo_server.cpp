/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/io/eio.h>
#include <iostream>

using namespace std;

// test with '$ nc localhost 12345'

std::vector<abmt::io::tcp_ptr> connections;

void update_connections(){
	for (auto it = connections.begin() ; it != connections.end(); ) {
	  if ( (*it)->connected == false ) {
	    it = connections.erase(it);
	  } else {
	    ++it;
	  }
	}
}

int main(){
	abmt::io::event_list e;
	abmt::io::tcp_server server_test(e,{12345});

	abmt::io::tcp_ptr last_con;

	int i = 0;
	server_test.on_new_connection = [&](abmt::io::tcp_ptr p){
		cout << "new connection" << endl;
		p->on_close = update_connections;
		p->on_new_data = [p](abmt::blob& b)->size_t{
			p->send(b.data,b.size);
			cout << "rcv: " << b.str() << endl;
			return b.size;
		};
		connections.push_back(p);
	};

	while(true){
		e.wait();
	}

	return 0;
}


