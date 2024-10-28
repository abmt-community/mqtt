/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef UTIL_INC_SHARED_DATA_H_
#define UTIL_INC_SHARED_DATA_H_

#include <abmt/mutex.h>
#include <abmt/serialize.h>
#include <functional>

namespace abmt{

template<typename T, typename MUTEX = abmt::mutex>
class shared_data{
public:
	T data;
	MUTEX lock;
	shared_data(T initial_value){
		data = initial_value;
	}

	shared_data(){

	}

	T get(){
		lock.lock();
		T copy = data;
		lock.unlock();
	   return copy;
	}

	void set(T new_value){
		lock.lock();;
		data = new_value;
		lock.unlock();
	}
};

} // namespace abmt



#endif /* UTIL_INC_SHARED_DATA_H_ */
