/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/io/eio.h>
#include <iostream>

using namespace std;

int main(){
	abmt::io::event_list e;
	abmt::io::timer t(e,1000);
	abmt::io::tcp_server server_test(e,{5001});

	abmt::io::tcp_ptr last_con;

	int i = 0;
	server_test.on_new_connection = [&](abmt::io::tcp_ptr p){
		cout << "new connection" << endl;
		p->send("hi!\n");
		last_con = p;
	};

	t.on_expiraton = [&]{
		if(last_con && last_con->connected){
			last_con->send("send!\n");
		}
	};
	while(i <= 5){
		e.wait();
	}

	return 0;
}


