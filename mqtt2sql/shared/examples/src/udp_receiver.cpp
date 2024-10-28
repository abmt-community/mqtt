/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/io/eio.h>
#include <iostream>

using namespace std;

// sends to 5001 listens on 5000
// test with "$ nc -u -l -p 5001"
// or "$ nc -u localhost 5000" when you only want to send something

int main(){
	abmt::io::event_list e;
	abmt::io::timer t(e,1000);
	abmt::io::udp udp_test(e,{"127.0.0.1",5001}, {"127.0.0.1",5000});

	udp_test.on_new_data = [&](abmt::blob& b)->size_t{
		cout << "rcv " << b.size << "; value: " << b.str() << endl;
		return b.size;
	};

	int i = 0;
	t.on_expiraton = [&]{
		cout << "timer" << i << endl;
		udp_test.send("snd...\n");
		if(i > 5){
			//t.stop();
		}
	};
	while(i <= 5){
		e.wait();
	}
}


