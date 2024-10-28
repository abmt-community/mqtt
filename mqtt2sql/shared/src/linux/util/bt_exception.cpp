/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/util/bt_exception.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

using namespace std;
using namespace abmt::util;

bt_exception::bt_exception( const std::string& error_msg ):runtime_error(error_msg){
	read_bt();
}

bt_exception::bt_exception( const char* error_msg ):runtime_error(error_msg){
	read_bt();
}

void bt_exception::read_bt(){
	num_frames = backtrace (bt_ptrs, sizeof(bt_ptrs)/sizeof(void*));
}

void bt_exception::print_backtrace(){
	char**	strings = backtrace_symbols (bt_ptrs, num_frames);
	if(num_frames < 2){
		std::cout << "No Backtrace..." << std::endl;
		return;
	}
	std::cout << "Backtrace: " << std::endl;
	for (size_t i = 2; i < num_frames; i++){ // start at 2 because the constructor and read_bt is included..
		std::stringstream s;

		std::string str = strings[i];
		auto filename_end_pos = str.find('(');
		string filename = str.substr(0, filename_end_pos);
		auto adr_start = str.find('+',filename_end_pos);
		auto adr_end   = str.find(')',adr_start);
		if(adr_start == std::string::npos){
			adr_start = str.find('[',filename_end_pos);
			adr_end   = str.find(']',adr_start);
		}
		string adr = str.substr(adr_start + 1, adr_end - adr_start -1);
		int adr_int = 0;
		if(adr != ""){
			try{
				adr_int = std::stol(adr,nullptr,16);
			}catch(...){
				cout << "[" << i - 1 <<"] error decoding backtrace address" << endl;
				continue;
			}
		}
		adr_int--; // something is wrong here, but it works
		// cout << "t: [" << i - 1 << "] " << filename << " " << adr << endl;
		// cout << "t: [" << i -1 << "] " << str << endl;
		s << "echo -n \"[" << i -1 << "] "<< filename <<": \"; addr2line -fpCe " << filename << " " << std::hex << adr_int << endl;
		(void)! system(s.str().c_str());
	}
	std::cout << "Note: Line numbers are only displayed when compiled with debug symbols. Optimization can hide parts of the backtrace (automatic inlining)." << std::endl;
	free (strings);
}

