/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/mutex.h>
#include <pthread.h>

using namespace abmt;

mutex::mutex(){
	mtx = new pthread_mutex_t;
	pthread_mutex_init((pthread_mutex_t*)mtx,NULL);
}
void mutex::lock(){
	 pthread_mutex_lock((pthread_mutex_t*)mtx);
}
void mutex::unlock(){
	 pthread_mutex_unlock((pthread_mutex_t*)mtx);
}

mutex::~mutex(){
	delete (pthread_mutex_t*)mtx;
}

scope_lock mutex::get_scope_lock(){
	return scope_lock(*this);
}

scope_lock::scope_lock(mutex& m):m(m){
	m.lock();
}
scope_lock::~scope_lock(){
	m.unlock();
}






