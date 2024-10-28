/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */
#include <abmt/serialize.h>
#include <cstring>
#include <deque>
#include <cmath>
using namespace abmt::serialize;
using namespace std;

// helper:
// for i in bool char uint8_t int8_t uint16_t int16_t uint32_t int32_t uint64_t int64_t float double; do echo "type::type(${i}& o){"; echo -e "\ttype = type_t::${i^^};" | sed -e "s/_T//"; echo -e "\tobj = &o;"; echo -e "\tinfo.size = sizeof(o);"; echo -e "}"; echo ""; done
//

bool abmt::serialize::type_id_is_value(type_id id){
	if( id > type_id::UNDEFINED && id < type_id::OBJECT ){
		return true;
	}
	return false;
}
bool abmt::serialize::type_id_is_num(type_id id){
	if( id >= type_id::UINT8 && id < type_id::DOUBLE ){
		return true;
	}
	return false;
}
bool abmt::serialize::type_id_is_obj(type_id id){
	if( id == type_id::OBJECT || id == type_id::MAN_CTRL ){
		return true;
	}
	return false;
}
bool abmt::serialize::type_id_is_binary(type_id id){
	if( id == type_id::RAW_DATA || id == type_id::STRING || id == type_id::SCENE_2D || id == type_id::SCENE_3D || id == type_id::JSON_STRING || id == type_id::SERIALIZED_STRING ){
		return true;
	}
	return false;
}

type::type(){
	id = type_id::UNDEFINED;
	obj = 0;
	serialize_info.size = 0;
}

type::type(bool& o){
	id = type_id::BOOL;
	obj = &o;
	serialize_info.size = sizeof(o);
}

type::type(float& o){
	id = type_id::FLOAT;
	obj = &o;
	serialize_info.size = sizeof(o);
}

type::type(double& o){
	id = type_id::DOUBLE;
	obj = &o;
	serialize_info.size = sizeof(o);
}

type::type(std::string& o){
	id = type_id::STRING;
	obj = &o;
	serialize_info.rexp = {
			[](void* obj) -> size_t { return (*(std::string*)obj).size(); },
			[](void* obj, void* data, size_t size) { (*(std::string*)obj).copy((char*)data,size); },
			[](void* obj, void* data, size_t size) { (*(std::string*)obj) = std::string((char*)data,size); }
	};
}

#ifdef __cpp_lib_string_view
type::type(std::string_view& o){
	id = type_id::STRING;
	obj = &o;
	serialize_info.rexp = {
			[](void* obj) -> size_t { return (*(std::string_view*)obj).size(); },
			[](void* obj, void* data, size_t size) { (*(std::string_view*)obj).copy((char*)data,size); },
			[](void* obj, void* data, size_t size) { return; }
	};
}
#endif

type::type(member_list lst){
	id = type_id::OBJECT;
	obj = 0;
	serialize_info.attr = lst;
}
type::type(abmt::blob& blk){
	id = type_id::RAW_DATA;
	obj = &blk;
	serialize_info.rexp = {
			[](void* obj) -> size_t { return (*(abmt::blob*)obj).size; },
			[](void* obj, void* data, size_t size) { (*(abmt::blob*)obj).get_mem(data, size, 0); },
			[](void* obj, void* data, size_t size) { (*(abmt::blob*)obj).set_mem(data, size, 0); }
	};
}

type::type(abmt::blob_shared& blk){
	id = type_id::RAW_DATA;
	obj = &blk;
	serialize_info.rexp = {
			[](void* obj) -> size_t { return (*(abmt::blob_shared*)obj).size; },
			[](void* obj, void* data, size_t size) { (*(abmt::blob_shared*)obj).get_mem(data, size, 0); },
			[](void* obj, void* data, size_t size) { (*(abmt::blob_shared*)obj).realloc(data, size); }
	};
}

type::type(blob_type r){
	id = type_id::RAW_DATA;
	obj = &r;
	serialize_info.rexp = r;
}

type::type(string_type s){
	id = type_id::STRING;
	obj = &s;
	serialize_info.rexp = s;
}

type::type(scene_2d_type s){
	id = type_id::SCENE_2D;
	obj = &s;
	serialize_info.rexp = s;
}

type::type(scene_3d_type s){
	id = type_id::SCENE_3D;
	obj = &s;
	serialize_info.rexp = s;
}

double abmt::serialize::type::to_double(void* data_ptr, size_t data_len){
	double res = 0; // nan is not supportet by every plattform
	if(id == type_id::BOOL) {
		if(sizeof(uint8_t) > data_len) return res;
		res = (double) *(bool*)data_ptr;
	} else if(id == type_id::CHAR) {
		if(sizeof(uint8_t) > data_len) return res;
		res = (double) *(char*)data_ptr;
	} else if(id == type_id::UINT8) {
		if(sizeof(uint8_t) > data_len) return res;
		res = (double) *(uint8_t*)data_ptr;
	} else if(id == type_id::INT8) {
		if(sizeof(int8_t) > data_len) return res;
		res = (double) *(int8_t*)data_ptr;
	} else if(id == type_id::UINT16) {
		if(sizeof(uint16_t) > data_len) return res;
		res = (double) *(uint16_t*)data_ptr;
	} else if(id == type_id::INT16) {
		if(sizeof(int16_t) > data_len) return res;
		res = (double) *(int16_t*)data_ptr;
	} else if(id == type_id::UINT32) {
		if(sizeof(uint32_t) > data_len) return res;
		res = (double) *(uint32_t*)data_ptr;
	} else if(id == type_id::INT32) {
		if(sizeof(int32_t) > data_len) return res;
		res = (double) *(int32_t*)data_ptr;
	} else if(id == type_id::UINT64) {
		if(sizeof(uint64_t) > data_len) return res;
		res = (double) *(uint64_t*)data_ptr;
	} else if(id == type_id::INT64) {
		if(sizeof(int64_t) > data_len) return res;
		res = (double) *(int64_t*)data_ptr;
	} else if(id == type_id::FLOAT) {
		if(sizeof(float) > data_len) return res;
		res = (double) *(float*)data_ptr;
	} else if(id == type_id::DOUBLE) {
		if(sizeof(double) > data_len) return res;
		res = (double) *(double*)data_ptr;
	}
	return res;
}

void abmt::serialize::type::from_double(double d){
	if(id == type_id::BOOL) {
		bool new_val = d;
		copy(&new_val,sizeof(new_val));
	} else if(id == type_id::CHAR) {
		char new_val = d;
		copy(&new_val,sizeof(new_val));
	} else if(id == type_id::UINT8) {
		uint8_t new_val = d;
		copy(&new_val,sizeof(new_val));
	} else if(id == type_id::INT8) {
		int8_t new_val = d;
		copy(&new_val,sizeof(new_val));
	} else if(id == type_id::UINT16) {
		uint16_t new_val = d;
		copy(&new_val,sizeof(new_val));
	} else if(id == type_id::INT16) {
		int16_t new_val = d;
		copy(&new_val,sizeof(new_val));
	} else if(id == type_id::UINT32) {
		uint32_t new_val = d;
		copy(&new_val,sizeof(new_val));
	} else if(id == type_id::INT32) {
		int32_t new_val = d;
		copy(&new_val,sizeof(new_val));
	} else if(id == type_id::UINT64) {
		uint64_t new_val = d;
		copy(&new_val,sizeof(new_val));
	} else if(id == type_id::INT64) {
		int64_t new_val = d;
		copy(&new_val,sizeof(new_val));
	} else if(id == type_id::FLOAT) {
		float new_val = d;
		copy(&new_val,sizeof(new_val));
	} else if(id == type_id::DOUBLE) {
		copy(&d,sizeof(d));
	}
}

const member* member_list::begin(){
	return list;
}

const member* member_list::end(){
	return list + length;
}

scene_2d_type::scene_2d_type(blob_type bt):blob_type(bt){

}

scene_3d_type::scene_3d_type(blob_type bt):blob_type(bt){

}

size_t type::get_def_size(){
	size_t res = sizeof(type_id);
	if(type_id_is_obj(id)){
		res += 2; // name_len = 0 + OBJECT_END
		for(auto m:serialize_info.attr){
			res += 1; // 1 = name_len_byte
			auto name_len = strlen(m.name);
			if(name_len > 255){
				name_len = 255;
			}
			res += name_len;
			res += m.t.get_def_size();
		}
	}
	return res;
}

size_t type::get_def(void* def_buff, size_t def_size){
	if(def_size == 0) return 0;
	uint8_t* c_buf = (uint8_t*)def_buff;
	*c_buf = (uint8_t) id;
	size_t bytes_written_to_def = 1;
	if(type_id_is_obj(id)){
		if(def_size < 1){
			// not enough space for OBJECT_END
			*c_buf = (uint8_t) type_id::UNDEFINED;
			return 1;
		}
		for(auto m:serialize_info.attr){
			uint8_t* len_byte = c_buf + bytes_written_to_def;
			auto name_len = strlen(m.name);
			if(name_len > 255){
				name_len = 255;
			}

			// first 2 => len_byte + type_byte; second 2 => must be left over for name_len=0 and OBJECT_END
			if( (def_size - 2 - name_len) < 2 ){
				// not enough space for name
				*c_buf = (uint8_t) type_id::UNDEFINED;
				return 1;
			}

			*len_byte = (uint8_t) name_len;
			bytes_written_to_def += 1;
			uint8_t* name_str = c_buf + bytes_written_to_def;
			memcpy(name_str, m.name, name_len);
			bytes_written_to_def += name_len;

			uint8_t* member_type_pos = c_buf    + bytes_written_to_def;
			size_t   new_def_size    = def_size - bytes_written_to_def;
			size_t   type_size       = m.t.get_def(member_type_pos, new_def_size);
			bytes_written_to_def += type_size;
		}

		uint8_t* name_len_zero = c_buf + bytes_written_to_def;
		*name_len_zero = 0;
		uint8_t* obj_end = c_buf + bytes_written_to_def +1;
		*obj_end = (uint8_t) type_id::OBJECT_END;
		bytes_written_to_def += 2;
	}
	return bytes_written_to_def;
}

size_t type::get_size(){
	if( type_id_is_value(id) ){
		return serialize_info.size;
	}else if( type_id_is_obj(id) ){
		size_t res = 0;
		for(auto m:serialize_info.attr){
			auto& member_type = m.t;
			member_type.obj = ((char*)obj + m.offset);
			res += member_type.get_size();
		}
		return res;
	}else if( type_id_is_binary(id) ){
		return serialize_info.rexp.get_size(obj) + 4; // 4 = len_info (uint32_t)
	}
	return 0;
}

void type::serialize(void* data, size_t data_size){
	copy(data,data_size, true);
}

void type::deserialize(void* data, size_t data_size){
	copy(data,data_size);
}

void type::deserialize(void* data, size_t data_size, void* def_buff, size_t def_size){
	char* def_ptr  = (char*)def_buff;
	char* data_ptr = (char*)data;
	std::deque<type> obj_stack;
	type current_obj = *this;

	string member_name;
	bool current_obj_valid = true;
	while(def_size > 0){
		member_name = "";
		if(obj_stack.empty() == false){
			uint8_t len = *(uint8_t*)def_ptr;
			++def_ptr; --def_size;
			current_obj = abmt::serialize::type();
			current_obj_valid = false;
			if(len > def_size) break; // error
			if(len > 0){
				member_name = string(def_ptr, def_ptr + len);
				def_ptr += len;
				def_size -= len;
				auto last_obj = obj_stack.back();
				if(last_obj.id == abmt::serialize::type_id::OBJECT){
					for(auto member:last_obj.serialize_info.attr){
						if(string(member.name) == member_name){
							current_obj = member.t;
							current_obj.obj = (char*) last_obj.obj + member.offset;
							current_obj_valid = true;
						}
					}
				}
			}
		}
		abmt::serialize::type_id type = *(abmt::serialize::type_id*)def_ptr;
		++def_ptr; --def_size;

		if( type == serialize::type_id::OBJECT || type == serialize::type_id::MAN_CTRL ){
			obj_stack.push_back(current_obj);
		}else if( type == abmt::serialize::type_id::OBJECT_END ){
			obj_stack.pop_back();
			if(obj_stack.empty()){
				current_obj = *this;
			}else{
				current_obj = obj_stack.back();
			}
		}else if(type_id_is_value(type)){
			if(current_obj_valid && type == current_obj.id){
				auto curr_size = current_obj.get_size();
				current_obj.deserialize(data_ptr, curr_size);
				data_size -= curr_size;
				data_ptr  += curr_size;
			}else{
				// convert over double
				if(type == abmt::serialize::type_id::BOOL) {
					if(sizeof(uint8_t) > data_size) break;
					if(current_obj_valid) current_obj.from_double(*(uint8_t*)data_ptr);
					data_size -= sizeof(uint8_t);
					data_ptr  += sizeof(uint8_t);
				}else if(type == abmt::serialize::type_id::CHAR) {
					if(sizeof(char) > data_size) break;
					char c = *(char*)data_ptr;
					if(current_obj_valid) current_obj.from_double(c);
					data_size -= sizeof(char);
					data_ptr  += sizeof(char);
				} else if(type == abmt::serialize::type_id::UINT8) {
					if(sizeof(uint8_t) > data_size) break;
					if(current_obj_valid) current_obj.from_double(*(uint8_t*)data_ptr);
					data_size -= sizeof(uint8_t);
					data_ptr  += sizeof(uint8_t);
				} else if(type == abmt::serialize::type_id::INT8) {
					if(sizeof(int8_t) > data_size) break;
					if(current_obj_valid) current_obj.from_double(*(int8_t*)data_ptr);
					data_size -= sizeof(int8_t);
					data_ptr  += sizeof(int8_t);
				} else if(type == abmt::serialize::type_id::UINT16) {
					if(sizeof(uint16_t) > data_size) break;
					if(current_obj_valid) current_obj.from_double(*(uint16_t*)data_ptr);
					data_size -= sizeof(uint16_t);
					data_ptr  += sizeof(uint16_t);
				} else if(type == abmt::serialize::type_id::INT16) {
					if(sizeof(int16_t) > data_size) break;
					if(current_obj_valid) current_obj.from_double(*(int16_t*)data_ptr);
					data_size -= sizeof(int16_t);
					data_ptr  += sizeof(int16_t);
				} else if(type == abmt::serialize::type_id::UINT32) {
					if(sizeof(uint32_t) > data_size) break;
					if(current_obj_valid) current_obj.from_double(*(uint32_t*)data_ptr);
					data_size -= sizeof(uint32_t);
					data_ptr  += sizeof(uint32_t);
				} else if(type == abmt::serialize::type_id::INT32) {
					if(sizeof(int32_t) > data_size) break;
					if(current_obj_valid) current_obj.from_double(*(int32_t*)data_ptr);
					data_size -= sizeof(int32_t);
					data_ptr  += sizeof(int32_t);
				} else if(type == abmt::serialize::type_id::UINT64) {
					if(sizeof(uint64_t) > data_size) break;
					if(current_obj_valid) current_obj.from_double(*(uint64_t*)data_ptr);
					data_size -= sizeof(uint64_t);
					data_ptr  += sizeof(uint64_t);
				} else if(type == abmt::serialize::type_id::INT64) {
					if(sizeof(int64_t) > data_size) break;
					if(current_obj_valid) current_obj.from_double(*(int64_t*)data_ptr);
					data_size -= sizeof(int64_t);
					data_ptr  += sizeof(int64_t);
				} else if(type == abmt::serialize::type_id::FLOAT) {
					if(sizeof(float) > data_size) break;
					if(current_obj_valid) current_obj.from_double(*(float*)data_ptr);
					data_size -= sizeof(float);
					data_ptr  += sizeof(float);
				} else if(type == abmt::serialize::type_id::DOUBLE) {
					if(sizeof(double) > data_size) break;
					if(current_obj_valid) current_obj.from_double(*(double*)data_ptr);
					data_size -= sizeof(double);
					data_ptr  += sizeof(double);
				}else{
					abmt::die("deserialize: unknown type");
				}
			} // type != id;
		}else if(type_id_is_binary(type)) {
			if(data_size < 4 ) break;
			uint32_t s = *(uint32_t*)data_ptr;
			if(current_obj_valid && type == current_obj.id){
				current_obj.copy(data_ptr, s + 4); //"raw data";
			}
			data_size -= 4 + s;
			data_ptr  += 4 + s;
		}else{
			if(type != abmt::serialize::type_id::UNDEFINED){
				abmt::die("deserialize: unknown type");
			}
		}
	}// while
}


size_t type::copy(void* data, size_t data_size, bool serialize){
	char* data_ptr = (char*) data;
	if( type_id_is_value(id) ){
		size_t size_needed = serialize_info.size;
		if(data_size >= size_needed){
			void* dst = data_ptr;
			void* src = obj;
			if(serialize == false){
				auto tmp = src;
				src = dst;
				dst = tmp;
			}
			memcpy(dst,src,size_needed);
			return size_needed;
		}
	}else if( type_id_is_obj(id) ){
		size_t data_pos = 0;
		char* obj_byte_ptr = (char*)obj;
		for(auto m:serialize_info.attr){
			// copy m.t is important for threadsafty; m.exp does not know the obj, and you can not write it because
			// another obj might be serialized at the same time. type-copy-constructor does not work :(
			auto member_type = m.t;
			member_type.obj = (obj_byte_ptr + m.offset);
			size_t bytes_copied = member_type.copy(data_ptr + data_pos, data_size - data_pos, serialize);
			data_pos += bytes_copied;
		}
		return data_pos;
	}else if( type_id_is_binary(id) ){
		if(serialize){
			uint32_t s = serialize_info.rexp.get_size(obj);
			if(data_size >= s + 4){
				*(uint32_t*)data_ptr = s;
				serialize_info.rexp.obj_to_data(obj,data_ptr + 4, data_size - 4);
				return s + 4;
			}
		}else{
			if( data_size < 4 ) return false;
			uint32_t s = *(uint32_t*) data_ptr;
			if( data_size >= s + 4 ){
				serialize_info.rexp.data_to_obj(obj,data_ptr + 4, s);
				return s + 4;
			}
		}
	}
	return 0;
}
