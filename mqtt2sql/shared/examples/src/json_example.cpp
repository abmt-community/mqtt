/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */
#include <abmt/json.h>
#include <iostream>

int main(){
	// Create objects
	abmt::json create_1;
	create_1["key1"] = "val1";
	create_1["key2"] = 1;
	create_1["key3"] = {{"key1","value1"}, {"key2","value2"}};
	create_1["key4"] = {"value1", "value2", 3.14, "array_create_test"};

	auto create_2 = abmt::json::object();
	auto create_3 = abmt::json::array({1,2,3,4,"value5"});
	auto create_4 = abmt::json::parse("{\"key\": \"value\"}");

	 //output
	std::cout << "Dump:" << std::endl << "-----"  << std::endl;
	std::cout << create_1.dump() << std::endl << std::endl;

	// Iterating
	std::cout << "Iterate 1:" << std::endl << "----------" << std::endl;
	for(auto itr: create_1){
		std::cout << "key: " << itr.key << " value:" << itr.value << std::endl;
		if(itr.key == "key2"){
			itr.erase();
		}
	}
	std::cout << "Dump after erase:" << create_1 << std::endl;


	//c++17 style
	std::cout << "Iterate 2:" << std::endl << "----------" << std::endl;
	for(auto [value, key, index, it]: create_1){
		if(key == "key1"){
			std::cout << "key1: " << value << " has index " << index << std::endl;
		}
	}

	//get values

	std::cout << "Assign 1" << std::endl << "----------" << std::endl;
	double to_double   = create_1["key4"][2];
	int    to_int      = create_1["key4"][2];
	std::string to_str = create_1["key1"];
	to_str = create_1["key1"].str(); // String assignment
	std::cout << to_double << " " << to_int << " " << to_str << std::endl;

	// Unfortunately string has an ambiguous operator= that nobody needs.
	// So the following doesn't work, and you need to call the str()-method.
	// std::string to_str2;
	// to_str2 = create_1["key1"];

	return 0;
}
