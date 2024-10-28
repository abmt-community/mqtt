/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_BLOB_MBU_H_
#define SHARED_ABMT_BLOB_MBU_H_

#include <type_traits>
#include <cstdlib>
#include <cstring>
#include <string>
#include <memory>
#include <abmt/os.h>
#include <abmt/const_array.h>

namespace abmt{

/**
 * Helper for conversion form a block of memory(datapointer + size) to another type.
 */

class blob;
class blob_shared;

class blob{
public:
	char* data;
	size_t size;

	blob(){
		data = 0;
		size = 0;
	}

	blob(void* ptr, size_t s):data((char*)ptr),size(s){

	}

	// when len = 0 then the subblock contains the rest of the block;
	blob sub_blob(size_t offset, size_t len = 0){
		if(len == 0){
			if(offset > size){
				abmt::die("Offset can not be bigger then size.");
			}
			len = size-offset;
		}
		if(offset+len > size){
			abmt::die("unable to create subblock, because of sizelimit");
		}
		return blob(((char*)data + offset), len);
	}

	void set_mem(const void* src, size_t len, size_t offset){
		if(len+offset > size){
			abmt::die("abmt::block to small to copy data from src...");
		}
		memcpy((char*) data + offset,src,len);
	}

	void get_mem(void* dst, size_t len, size_t offset){
		if(len+offset > size){
			abmt::die("abmt::block to small to copy data to src...");
		}
		memcpy(dst,(char*) data + offset,len);
	}

	template<typename T>
	inline T get(size_t offset){
		if(sizeof(T) + offset > size) abmt::die("Unable to convert memview. Size error");
		T res;
		// it is done that way because of the arm architecture and it's stupid alingment rules
		memcpy(&res,(data + offset),sizeof(T));
		return res;
	}

	template<typename T>
	inline void set(T&& obj, size_t offset){
		if(sizeof(T) + offset > size) abmt::die("Unable to convert memview. Size error");
		memcpy(data + offset, &obj, sizeof(T));
	}

	/// Like get without size check.
	template<typename T>
	inline T get2(size_t offset){
		T res;
		// it is done that way because of the arm architecture and it's stupid alingment rules
		memcpy(&res, data + offset, sizeof(T));
		return res;
	}

	/// Like set without size check.
	template<typename T>
	inline void set2(T&& obj, size_t offset){
		memcpy(data + offset, &obj, sizeof(T));
	}

	std::string str(size_t offset = 0, size_t len = 0){
		if(len == 0){
			if(offset > size){
				abmt::die("Offset can not be bigger then size.");
			}
			len = size-offset;
		}
		if(offset+len > size){
			abmt::die("unable to create object, because of size limit");
		}
		return std::string((char*)data +offset , len);
	}

	/// Compares only the pointer and the size. Not the content!
	bool operator==(blob& rhs){
		if(data == rhs.data && size == rhs.size){
			return true;
		}
		return false;
	}

	operator std::string(){
		return str();
	}

	virtual ~blob(){

	}
};

/// Is derived from blob. So all blob functions can also be called.
class blob_shared: public blob{
public:
	std::shared_ptr<char[]> shared_ptr;

	blob_shared(size_t s = 0):blob(0,s), shared_ptr(new char[s]){
		data = shared_ptr.get();
	}

	/// copies s to the newly allocated buffer
	blob_shared(std::string s){
		realloc((void*) s.c_str(), s.size());
	}

	blob_shared(blob& b){
		realloc((void*)b.data, b.size);
	}

	blob_shared(const blob_shared&) = default;

	/// allocates a new buffer initialized with 0
	void realloc(size_t s = 0){
		shared_ptr.reset(new char[s]);
		data = shared_ptr.get();
		size = s;
		memset(data, 0, size);
	}

	void realloc(void* src_data_ptr_to_copy_from, size_t s){
		shared_ptr.reset(new char[s]);
		data = shared_ptr.get();
		size = s;
		if(src_data_ptr_to_copy_from == 0){
			memset(data, 0, size);
		}else{
			set_mem(src_data_ptr_to_copy_from, s, 0);
		}
	}

	void clear(){
		realloc(0);
	}

	/// resets the internal shared_ptr to new_data_ptr and takes over ownership
	void reset(void* new_data_ptr = 0, size_t s = 0){
		shared_ptr.reset((char*)new_data_ptr);
		data = (char*) new_data_ptr;
		size = s;
	}

	blob_shared operator+(blob b){
		size_t new_size = size + b.size;
		blob_shared res(new_size);
		memcpy(res.data, data, size);
		memcpy(res.data + size, b.data, b.size);
		return res;
	}

	blob_shared& operator+=(blob b){
		size_t new_size = size + b.size;
		std::shared_ptr<char[]> new_ptr(new char[new_size]);
		memcpy(new_ptr.get(), data, size);
		memcpy(new_ptr.get() + size, b.data, b.size);

		shared_ptr = new_ptr;
		data = shared_ptr.get();
		size = new_size;
		return *this;
	}
};

} // namespace abmt

#endif /* _MBU_H_ */
