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
	int i = 0;
	t.on_expiraton = [&]{
		cout << "timer" << i << endl;
		++i;
		if(i > 5){
			t.stop();
		}
	};
	while(i <= 5){
		e.wait();
	}
}


