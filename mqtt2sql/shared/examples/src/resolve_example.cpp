/*
 * resolve_example.cpp
 *
 *  Created on: 23.08.2021
 *      Author: hva
 */


#include <abmt/io/eio.h>
#include <iostream>

using namespace std;

int main(){
	auto res = abmt::io::resolve("abmt.io");
	for(auto ip: res){
		cout << "res: " << ip.str() << endl;
	}
}

