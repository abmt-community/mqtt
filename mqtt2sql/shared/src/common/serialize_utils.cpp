/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <cstdint>
#include <iostream>
#include <deque>
#include <abmt/serialize_utils.h>
#include <abmt/blob.h>
#include <abmt/const_array.h>

using namespace std;

using json = abmt::json;

namespace abmt{
namespace serialize{

std::string base64_encode(const std::string &in) {
	std::string out;
	abmt::const_array base_str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


    int rest = 0;
    int byte = 0;
    int idx = 0;
    for (uint8_t c : in) {
        if(byte == 0){
        	idx = c >> 2;
        	rest = c & 0x03;
        	byte = 1;
        }else if( byte == 1 ){
        	idx = (rest << 4) | (c >> 4);
        	rest = c & 0x0F;
        	byte = 2;
        }else if( byte == 2 ){
        	idx = (rest << 2) | (c >> 6);
        	out.push_back(base_str[idx]);
        	idx = c & 0x3F;
        	byte = 0;
        }
        out.push_back(base_str[idx]);
    }
    if(byte == 1){
    	idx = rest << 4;
    	out.push_back(base_str[idx]);
    	out.push_back('=');
    	out.push_back('=');
    }
    if(byte == 2){
    	idx = rest << 2;
    	out.push_back(base_str[idx]);
    	out.push_back('=');
    }
    return out;
}

std::string base64_decode(const std::string &in) {
    std::string out;
    auto get_val = [](uint8_t c)->uint8_t{
    	if(c >= 'A' && c <= 'Z'){
    		return c - 'A';
    	}
    	if(c >= 'a' && c <= 'z'){
			return c - 'a' + 26;
		}
    	if(c >= '0' && c <= '9'){
			return c - '0' + 52;
		}
    	if(c == '+'){
    		return 26*2 + 10; // 62
    	}
    	if(c == '/'){
			return 26*2 + 10 +1; //63
		}
    	if(c == '='){
    		return 64;
    	}
    	return 65;
    };

    uint8_t rest = 0;
    int byte = 0;
    for (uint8_t c : in) {

    	uint8_t val = get_val(c);
    	if(val >= 64){
    		break;
    	}
    	if(byte == 0){
    		rest = val << 2;
    		byte = 1;
    	}else if(byte == 1){
    		out.push_back(rest | (val >> 4));
    		rest = val << 4;
    		byte = 2;
    	}else if(byte == 2){
    		out.push_back( rest | (val >> 2) );
    		rest = val << 6;
    		byte = 3;
    	}else if(byte == 3){
    		out.push_back(rest | val);
    		byte = 0;
    	}

    }

    return out;
}

void print_raw_def(char* buffer){
	uint32_t size = *(uint32_t*)buffer;
	//cout << "size of def: " << size << endl;
	buffer += sizeof(size);
	size_t intend = 0;
	while(size > 0){
		uint8_t len = *(uint8_t*)buffer;
		++buffer;
		size_t bytes_needed = len + 2; // 2 + len_byte + type_byte;
		if(size < bytes_needed){
			return;
		}
		size -= bytes_needed;

		string name = string(buffer, buffer+len);
		buffer += len;
		uint8_t type = *(uint8_t*)buffer;

		++ buffer;
		if(type == (uint8_t)abmt::serialize::type_id::OBJECT_END){
			--intend;
		}
		for(size_t i = 0; i < intend; ++i){
			cout << "  ";
		}
		if(type != (uint8_t)abmt::serialize::type_id::OBJECT_END){
			cout << name << ": " << (int)type;
		}
		if(type == (uint8_t)abmt::serialize::type_id::OBJECT || type == (uint8_t)abmt::serialize::type_id::MAN_CTRL){
			cout <<  " {";
			++intend;
		}else if(type == (uint8_t)abmt::serialize::type_id::OBJECT_END){
			cout << "}";
		}
		cout << endl;
	}
}

abmt::json to_json(abmt::serialize::type src){
	size_t def_size = src.get_def_size();
	char* def = (char*) malloc(def_size);
	src.get_def(def,def_size);

	size_t data_size = src.get_size();
	void* data = malloc(data_size);
	src.serialize(data, data_size);

	if(data_size == 0){
		return 0;
	}
	json res;
	char* def_ptr  = (char*)def;
	char* data_ptr = (char*)data;
	std::deque<json*> obj_stack;
	json* working_obj = &res;

	std::string name = "";
	while(def_size > 0){
		name = "";
		if(obj_stack.empty() == false){
			// we are reading a member (or obj_end)
			uint8_t len = *(uint8_t*)def_ptr;
			++def_ptr; --def_size;
			if(len > def_size) break; // error
			if(len > 0){
				name = string(def_ptr, def_ptr + len);
				def_ptr += len;
				def_size -= len;
				working_obj = &(obj_stack.back()->operator[](name));
			}
		}

		abmt::serialize::type_id type = *(abmt::serialize::type_id*)def_ptr;
		++def_ptr; --def_size;

		if( type == serialize::type_id::OBJECT || type == serialize::type_id::MAN_CTRL ){
			obj_stack.push_back(working_obj);
		}else if( type == abmt::serialize::type_id::OBJECT_END ){
			obj_stack.pop_back();
			if(obj_stack.empty()){
				working_obj = &res;
			}else{
				working_obj = obj_stack.back();
			}
		}else if(type == abmt::serialize::type_id::BOOL) {
			if(sizeof(uint8_t) > data_size) break;
			uint8_t val = *(uint8_t*)data_ptr;
			if(val){
				*working_obj = true;
 			}else{
 				*working_obj = false;
 			}
			data_size -= sizeof(uint8_t);
			data_ptr  += sizeof(uint8_t);
		}else if(type == abmt::serialize::type_id::CHAR) {
			if(sizeof(char) > data_size) break;
			char c = *(char*)data_ptr;
			*working_obj = string(1,c);
			data_size -= sizeof(char);
			data_ptr  += sizeof(char);
		} else if(type == abmt::serialize::type_id::UINT8) {
			if(sizeof(uint8_t) > data_size) break;
			*working_obj = (double) *(uint8_t*)data_ptr;
			data_size -= sizeof(uint8_t);
			data_ptr  += sizeof(uint8_t);
		} else if(type == abmt::serialize::type_id::INT8) {
			if(sizeof(int8_t) > data_size) break;
			*working_obj = (double) *(int8_t*)data_ptr;
			data_size -= sizeof(int8_t);
			data_ptr  += sizeof(int8_t);
		} else if(type == abmt::serialize::type_id::UINT16) {
			if(sizeof(uint16_t) > data_size) break;
			*working_obj = (double) *(uint16_t*)data_ptr;
			data_size -= sizeof(uint16_t);
			data_ptr  += sizeof(uint16_t);
		} else if(type == abmt::serialize::type_id::INT16) {
			if(sizeof(int16_t) > data_size) break;
			*working_obj = (double) *(int16_t*)data_ptr;
			data_size -= sizeof(int16_t);
			data_ptr  += sizeof(int16_t);
		} else if(type == abmt::serialize::type_id::UINT32) {
			if(sizeof(uint32_t) > data_size) break;
			*working_obj = (double) *(uint32_t*)data_ptr;
			data_size -= sizeof(uint32_t);
			data_ptr  += sizeof(uint32_t);
		} else if(type == abmt::serialize::type_id::INT32) {
			if(sizeof(int32_t) > data_size) break;
			*working_obj = (double) *(int32_t*)data_ptr;
			data_size -= sizeof(int32_t);
			data_ptr  += sizeof(int32_t);
		} else if(type == abmt::serialize::type_id::UINT64) {
			if(sizeof(uint64_t) > data_size) break;
			*working_obj = (double) *(uint64_t*)data_ptr;
			data_size -= sizeof(uint64_t);
			data_ptr  += sizeof(uint64_t);
		} else if(type == abmt::serialize::type_id::INT64) {
			if(sizeof(int64_t) > data_size) break;
			*working_obj = (double) *(int64_t*)data_ptr;
			data_size -= sizeof(int64_t);
			data_ptr  += sizeof(int64_t);
		} else if(type == abmt::serialize::type_id::FLOAT) {
			if(sizeof(float) > data_size) break;
			*working_obj = (double) *(float*)data_ptr;
			data_size -= sizeof(float);
			data_ptr  += sizeof(float);
		} else if(type == abmt::serialize::type_id::DOUBLE) {
			if(sizeof(double) > data_size) break;
			*working_obj = (double) *(double*)data_ptr;
			data_size -= sizeof(double);
			data_ptr  += sizeof(double);
		} else if(type == abmt::serialize::type_id::RAW_DATA || type == abmt::serialize::type_id::SCENE_2D || type == abmt::serialize::type_id::SCENE_3D) {
			if(4 > data_size) break;
			uint32_t s = *(uint32_t*)data_ptr;
			*working_obj = base64_encode(string(data_ptr + 4,s)); //"raw data";
			data_size -= 4 + s;
			data_ptr  += 4 + s;
		} else if(type == abmt::serialize::type_id::STRING || type == abmt::serialize::type_id::JSON_STRING || type == abmt::serialize::type_id::SERIALIZED_STRING) {
			if(4 > data_size) break;
			uint32_t s = *(uint32_t*)data_ptr;
			*working_obj = string(data_ptr + 4,s);
			data_size -= 4 + s;
			data_ptr  += 4 + s;
		} else if(type == abmt::serialize::type_id::UNDEFINED) {
			// do nothing; no data is reserved for UNDEFINED objects
		} else {
			cout << "to_json: type error" << (int)type << endl; // todo remove and check!
			break; // error
		}
	} // while(def_size > 0)

	free(def);
	free(data);

	return res;
}


void from_json(abmt::json& src, abmt::serialize::type& dst){
	size_t def_size = dst.get_def_size();
	char* def = (char*) malloc(def_size);
	dst.get_def(def,def_size);

	std::deque<json*> obj_stack;
	json* working_obj = &src;

	std::deque<std::string> base64_decode_buffer;

	char* def_ptr = def;
	// calculate data_size
	size_t data_size = 0;
	std::string name;
	while(def_size > 0){

		name = "";
		if(obj_stack.empty() == false){
			// we are reading a member (or obj_end)
			uint8_t len = *(uint8_t*)def_ptr;
			++def_ptr; --def_size;
			if(len > def_size) break; // error
			if(len > 0){
				name = string(def_ptr, def_ptr + len);
				def_ptr += len;
				def_size -= len;
				working_obj = &(obj_stack.back()->operator[](name));
			}
		}

		abmt::serialize::type_id type = *(abmt::serialize::type_id*)def_ptr;
		++def_ptr; --def_size;

		if( type == serialize::type_id::OBJECT || type == serialize::type_id::MAN_CTRL ){
			obj_stack.push_back(working_obj);
		}else if( type == abmt::serialize::type_id::OBJECT_END ){
			obj_stack.pop_back();
			if(obj_stack.empty()){
				working_obj = &src;
			}else{
				working_obj = obj_stack.back();
			}
		}else if(type == abmt::serialize::type_id::BOOL) {
			data_size += sizeof(uint8_t);
		}else if(type == abmt::serialize::type_id::CHAR) {
			data_size += sizeof(char);
		} else if(type == abmt::serialize::type_id::UINT8) {
			data_size += sizeof(uint8_t);
		} else if(type == abmt::serialize::type_id::INT8) {
			data_size += sizeof(int8_t);
		} else if(type == abmt::serialize::type_id::UINT16) {
			data_size += sizeof(uint16_t);
		} else if(type == abmt::serialize::type_id::INT16) {
			data_size += sizeof(int16_t);
		} else if(type == abmt::serialize::type_id::UINT32) {
			data_size += sizeof(uint32_t);
		} else if(type == abmt::serialize::type_id::INT32) {
			data_size += sizeof(int32_t);
		} else if(type == abmt::serialize::type_id::UINT64) {
			data_size += sizeof(uint64_t);
		} else if(type == abmt::serialize::type_id::INT64) {
			data_size += sizeof(int64_t);
		} else if(type == abmt::serialize::type_id::FLOAT) {
			data_size += sizeof(float);
		} else if(type == abmt::serialize::type_id::DOUBLE) {
			data_size += sizeof(double);
		} else if(type == abmt::serialize::type_id::RAW_DATA || type == abmt::serialize::type_id::SCENE_2D || type == abmt::serialize::type_id::SCENE_3D) {
			std::string data = base64_decode(*working_obj);
			base64_decode_buffer.push_back(data);
			data_size += 4 + data.size();
		} else if(type == abmt::serialize::type_id::STRING || type == abmt::serialize::type_id::JSON_STRING || type == abmt::serialize::type_id::SERIALIZED_STRING) {
			std::string str = *working_obj;
			data_size += 4 + str.size();
		} else if(type == abmt::serialize::type_id::UNDEFINED) {
			// do nothing; no data is reserved for UNDEFINED objects
		} else {
			cout << "from_json 1: type error" << (int) type << endl; // todo remove and check!
			return; // error
		}
	} // while(def_size > 0)

	void*  data_mem = malloc(data_size);
	size_t data_mem_size = data_size;

	working_obj = &src;
	size_t data_pos = 0;
	abmt::blob data(data_mem, data_size);

	def_size = dst.get_def_size();
	def_ptr = def;

	while(def_size > 0){
		name = "";
		if(obj_stack.empty() == false){
			// we are reading a member (or obj_end)
			uint8_t len = *(uint8_t*)def_ptr;
			++def_ptr; --def_size;
			if(len > def_size) break; // error
			if(len > 0){
				name = string(def_ptr, def_ptr + len);
				def_ptr += len;
				def_size -= len;
				working_obj = &(obj_stack.back()->operator[](name));
			}
		}

		abmt::serialize::type_id type = *(abmt::serialize::type_id*)def_ptr;
		++def_ptr; --def_size;

		if( type == serialize::type_id::OBJECT || type == serialize::type_id::MAN_CTRL ){
			obj_stack.push_back(working_obj);
		}else if( type == abmt::serialize::type_id::OBJECT_END ){
			obj_stack.pop_back();
			if(obj_stack.empty()){
				working_obj = &src;
			}else{
				working_obj = obj_stack.back();
			}
		}else if(type == abmt::serialize::type_id::BOOL) {
			if(sizeof(uint8_t) > data_size) break;
			if(*working_obj){
				data.set((uint8_t) true, data_pos);
 			}else{
 				data.set((uint8_t) false, data_pos);
 			}
			data_size -= sizeof(uint8_t);
			data_pos  += sizeof(uint8_t);
		}else if(type == abmt::serialize::type_id::CHAR) {
			if(sizeof(char) > data_size) break;
			data.set((char) (working_obj->str())[0], (data_pos));
			data_size -= sizeof(char);
			data_pos  += sizeof(char);
		} else if(type == abmt::serialize::type_id::UINT8) {
			if(sizeof(uint8_t) > data_size) break;
			data.set((uint8_t) *working_obj, (data_pos));
			data_size -= sizeof(uint8_t);
			data_pos  += sizeof(uint8_t);
		} else if(type == abmt::serialize::type_id::INT8) {
			if(sizeof(int8_t) > data_size) break;
			data.set((int8_t) *working_obj, (data_pos));
			data_size -= sizeof(int8_t);
			data_pos  += sizeof(int8_t);
		} else if(type == abmt::serialize::type_id::UINT16) {
			if(sizeof(uint16_t) > data_size) break;
			data.set((uint16_t) *working_obj, (data_pos));
			data_size -= sizeof(uint16_t);
			data_pos  += sizeof(uint16_t);
		} else if(type == abmt::serialize::type_id::INT16) {
			if(sizeof(int16_t) > data_size) break;
			data.set((int16_t) *working_obj, (data_pos));
			data_size -= sizeof(int16_t);
			data_pos  += sizeof(int16_t);
		} else if(type == abmt::serialize::type_id::UINT32) {
			if(sizeof(uint32_t) > data_size) break;
			data.set((uint32_t) *working_obj, (data_pos));
			data_size -= sizeof(uint32_t);
			data_pos  += sizeof(uint32_t);
		} else if(type == abmt::serialize::type_id::INT32) {
			if(sizeof(int32_t) > data_size) break;
			data.set((int32_t) *working_obj, (data_pos));
			data_size -= sizeof(int32_t);
			data_pos  += sizeof(int32_t);
		} else if(type == abmt::serialize::type_id::UINT64) {
			if(sizeof(uint64_t) > data_size) break;
			data.set((uint64_t) *working_obj, (data_pos));
			data_size -= sizeof(uint64_t);
			data_pos  += sizeof(uint64_t);
		} else if(type == abmt::serialize::type_id::INT64) {
			if(sizeof(int64_t) > data_size) break;
			data.set((int64_t) *working_obj, (data_pos));
			data_size -= sizeof(int64_t);
			data_pos  += sizeof(int64_t);
		} else if(type == abmt::serialize::type_id::FLOAT) {
			if(sizeof(float) > data_size) break;
			data.set((float) *working_obj, (data_pos));
			data_size -= sizeof(float);
			data_pos  += sizeof(float);
		} else if(type == abmt::serialize::type_id::DOUBLE) {
			if(sizeof(double) > data_size) break;
			data.set((double) *working_obj, (data_pos));
			data_size -= sizeof(double);
			data_pos  += sizeof(double);
		} else if(type == abmt::serialize::type_id::RAW_DATA || type == abmt::serialize::type_id::SCENE_2D || type == abmt::serialize::type_id::SCENE_3D) {
			if(4 > data_size) break;
			std::string str = base64_decode_buffer.front();
			base64_decode_buffer.pop_front();
			uint32_t s = str.size();
			data.set((uint32_t) s, data_pos);
			data.set_mem(str.c_str(),s,data_pos +4);
			data_size -= 4 + s;
			data_pos  += 4 + s;
		} else if(type == abmt::serialize::type_id::STRING || type == abmt::serialize::type_id::JSON_STRING || type == abmt::serialize::type_id::SERIALIZED_STRING) {
			if(4 > data_size) break;
			std::string str = working_obj->str();
			uint32_t s = str.size();
			data.set((uint32_t) s, data_pos);
			data.set_mem(str.c_str(),s,data_pos+4);
			data_size -= 4 + s;
			data_pos  += 4 + s;
		} else if(type == abmt::serialize::type_id::UNDEFINED) {
			// do nothing; no data is reserved for UNDEFINED objects
		} else {
			cout << "from_json 2: type error" << (int) type << endl; // todo remove and check!
			break; // error
		}
	} // while(def_size > 0)

	dst.deserialize(data_mem, data_mem_size);

	free(data_mem);
}

} // namespace serialize
} // namespace abmt
