/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/io/eio.h>
#include <iostream>

using namespace std;

int main(){
	abmt::io::event_list e;
	abmt::io::timer t(e,1*abmt::io::sec);
	abmt::io::tcp_ptr tcp_test = abmt::io::tcp::connect_to(e,{"127.0.0.1",5001});

	int i = 0;

	tcp_test->on_open = []{
		cout << "on_open" << endl;
	};

	tcp_test->on_ready_to_send = []{
		cout << "on_ready_to_send" << endl;
	};

	tcp_test->on_close = [&]{
		cout << "on_close" << endl;
		i = 6;
	};

	tcp_test->on_new_data = [&](abmt::blob& b)->size_t{
		cout << "rcv " << b.size << "; value: " << b.str() << endl;
		return b.size;
	};



	t.on_expiraton = [&]{
		if(tcp_test->connected == true){
			tcp_test->send("send...\n");
		}
	};
	while(i <= 5){
		e.wait();
	}
}
