/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_RT_PARAMETER_H_
#define SHARED_ABMT_RT_PARAMETER_H_

#include <abmt/serialize.h>
#include <abmt/mutex.h>

namespace abmt{
namespace rt{

class parameter_base{
public:

	virtual void lock() = 0;
	virtual void unlock() = 0;
	virtual abmt::serialize::type get_type() = 0;
	virtual void* get_data_ptr() = 0; // don't use unless you know what you are doing; lock before use; unlock after use...

	virtual ~parameter_base(){};
};

template<typename T, typename MUTEX = abmt::mutex>
class parameter: public parameter_base{
public:
	T data;
	MUTEX mtx;

	bool dont_save = false;

	parameter(T initial_value){
		data = initial_value;
	}

	parameter(){
		data = {};
	}

	T get(){
		auto l = mtx.get_scope_lock();
		return data;
	}

	void set(T new_value){
		auto l = mtx.get_scope_lock();
		data = new_value;
	}

	virtual void lock(){
		mtx.lock();
	}
	virtual void unlock(){
		mtx.unlock();
	}

	virtual abmt::serialize::type get_type(){
		return data; // calls abmt::serialize::type-constructor
	}

	// don't use unless you know what you are doing; lock before use; unlock after use...
	virtual void* get_data_ptr(){
		return &data;
	}
};


} // namespace rt
} // namespace abmt

#endif /* SHARED_ABMT_RT_PARAMETER_H_ */
