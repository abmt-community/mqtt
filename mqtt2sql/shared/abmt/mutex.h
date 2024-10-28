/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef UTIL_INC_MUTEX_H_
#define UTIL_INC_MUTEX_H_


namespace abmt{

class scope_lock;

class mutex{
public:
	void* mtx;
	mutex();
	void lock();
	void unlock();
	scope_lock get_scope_lock();
	~mutex();
};

class scope_lock{
public:
	mutex& m;
	scope_lock(mutex& m);
	~scope_lock();
};

} // namespace abmt

#endif /* UTIL_INC_MUTEX_H_ */
