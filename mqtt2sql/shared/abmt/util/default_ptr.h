/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */
#ifndef SHARED_ABMT_UTIL_DEFAULT_PTR_H_
#define SHARED_ABMT_UTIL_DEFAULT_PTR_H_

#include <memory>

namespace abmt{

/**
 * A std::shared_ptr with default initialization of T.
 */
template<typename T>
struct default_ptr: public std::shared_ptr<T>{
	default_ptr(): std::shared_ptr<T>(new T){

	}

	default_ptr(T* p): std::shared_ptr<T>(p){

	}

	template<typename T2>
	explicit default_ptr(T2* p): std::shared_ptr<T>(p){

	}

	template<typename T2>
	default_ptr(const std::shared_ptr<T2>& p): std::shared_ptr<T>(p){

	}
};

} // namespace abmt

#endif /* SHARED_ABMT_UTIL_DEFAULT_PTR_H_ */
