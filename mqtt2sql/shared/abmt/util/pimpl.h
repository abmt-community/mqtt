/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_PIMPL_H_
#define SHARED_ABMT_PIMPL_H_

#include <functional>
#include <typeinfo>
#include <abmt/os.h>

namespace abmt{
namespace util{

/**
 * Container for an unknown object. It stores a pointer to an object and its destructor.
 * Pimpl can be used to hide includes from the main model to speed up compile time.
 */
struct pimpl{
	void* p = 0;
	std::function<void()> destruct = []{};
	std::size_t tid = 0;

	pimpl() = default;

	template<typename T>
	pimpl(T* instance){
		set(instance);
	}

	// object can not be copied
	pimpl(const pimpl& p) = delete;

	/// Sets a new pointer with a new type. When it was set before the old object
	/// is deleted.
	template<typename T>
	void set(T* instance){
		if(p != 0){
			destruct();
		}
		destruct = [this]{
			delete (T*)p;
		};
		p = instance;
#ifdef __cpp_rtti
		tid = typeid(T).hash_code();
#else
		tid = 0;
#endif
	}

	template<typename T>
	T& get(){
		die_if(p == 0, "No object was set to pimpl!");
#ifdef __cpp_rtti
		die_if(tid != typeid(T).hash_code(), "The type that is requested is different than the type that was set to pimpl!");
#endif
		return *(T*)p;
	}

	/// template function
	template<typename T>
	bool is_type(){
#ifdef __cpp_rtti
		return tid == typeid(T).hash_code();
#else
		return true;
#endif
	}

	bool is_set(){
		return p != 0 && tid != 0;
	}

	/// when you know what you are doing and what your past me has done... (returns cast without type-check)
	template<typename T>
	T& get2(){
		return *(T*)p;
	}


	~pimpl(){
		if(p != 0){
			destruct();
		}
	}
};

} //namespace util
} //namespace abmt

#endif /* SHARED_ABMT_PIMPL_H_ */
