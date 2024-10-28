/*
 * io_buffer.cpp
 *
 *  Created on: 26.08.2022
 *      Author: hva
 */

#include <abmt/os.h>
#include <abmt/io/buffer.h>

using namespace std;
using namespace abmt;


abmt::io::buffer::buffer(size_t init_size){
	data = new uint8_t[init_size];
	size = init_size;
}

/// copies data to buffer. Does not call on_new_data
void abmt::io::buffer::push_back(const void* data_ptr, size_t data_size){
	size_t bytes_to_copy = data_size;
	if(bytes_to_copy + bytes_used > size){
		set_size(bytes_to_copy + bytes_used);
	}
	memcpy(data+bytes_used,data_ptr,bytes_to_copy);
	bytes_used += bytes_to_copy;
}

void abmt::io::buffer::pop_front(size_t nbytes){
	if(nbytes != 0){
		if(bytes_used - nbytes == 0){
			bytes_used = 0;
			return;
		}
		memmove(data, data+nbytes, bytes_used - nbytes);
		bytes_used -= nbytes;
	}
}

/// Copies data to buffer and calls send();
void abmt::io::buffer::send(const void* data_ptr, size_t data_size){
	push_back(data_ptr,data_size);
	send();
}

// calles on_new_data and pops received bytes
void abmt::io::buffer::send(){
	abmt::blob b(data,bytes_used);
	size_t bytes_to_pop = on_new_data(b);
	pop_front(bytes_to_pop);
}

void abmt::io::buffer::set_size(size_t new_size){
	if(size == new_size){
		return;
	}
	size = new_size;
	uint8_t* new_data = new uint8_t[size];
	if(bytes_used > size){
		bytes_used = size;
	}
	if(data != 0){
		memcpy(new_data,data,bytes_used);
		delete[] data;
	}
	data = new_data;
}

void abmt::io::buffer::shrink(){
	set_size(bytes_used);
}

void abmt::io::buffer::flush(){
	bytes_used = 0;
}

abmt::io::buffer::~buffer(){
	if(data != 0){
		delete[] data;
	}
}

