/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */


#ifndef UTIL_MATH_VEC_H_
#define UTIL_MATH_VEC_H_

#include <initializer_list>
#include <cstring>
#include <cstdint>
#include <assert.h>
#include <cmath>
#include <abmt/serialize.h>

namespace abmt{

#pragma pack (push, 1)

template<unsigned int M, unsigned int N, typename T>
class mat;

template<unsigned int N, typename T = double, typename Enable = void>
class vec_base;

class vec_alias_select; // helper type thats used to select the correct base with the right size and aliasses

template<unsigned int N, typename T >
class vec_base<N, T, typename std::enable_if<(N==1), vec_alias_select>::type>{
public:
	union{
		T data[N] = {};
		struct{
			T x;
		};
		struct{
			T r;
		};
	};
};

template<unsigned int N, typename T >
class vec_base<N, T, typename std::enable_if<(N==2), vec_alias_select>::type>{
public:
	union{
		T data[N] = {};
		struct{
			T x;
			T y;
		};
		struct{
			T r;
			T g;
		};
		struct{
			T real; // or use r instead
			T i;
		};
	};

};

template<unsigned int N, typename T >
class vec_base<N, T, typename std::enable_if<(N==3), vec_alias_select>::type>{
public:
	union{
		T data[N] = {};
		struct{
			T x;
			T y;
			T z;
		};
		struct{
			T r;
			T g;
			T b;
		};
		struct{
			T real; // or use r instead
			T i;
			T j;
		};
	};
};

template<unsigned int N, typename T >
class vec_base<N, T, typename std::enable_if<(N>3), vec_alias_select>::type>{
public:
	union{
			T data[N] = {};
			struct{
				T x;
				T y;
				T z;
				T w;
			};
			struct{
				T r;
				T g;
				T b;
				T a;
			};
			struct{
				T real; // or use r instead
				T i;
				T j;
				T k;
			};
		};
};

/**
 * Vector class for mathematical operations.
 * You can access the elements via the vec.data[]-array or via aliasses like vec.x, vec.y etc.
 */
template<unsigned int N,typename T = double>
class vec: public vec_base<N, T, vec_alias_select>{
public:

	vec(){};

	vec(T val){
		for(unsigned int i = 0;i<N;++i){
			this->data[i] = val;
		}
	}

	vec(std::initializer_list<T> lst) {
		unsigned int i = 0;
		for(auto e:lst){
			this->data[i] = e;
			i++;
			if(i >= N){
				break;
			}
		}
	}

	template<typename T2>
	inline vec(const vec<N,T2>& src){
		for(unsigned int i = 0;i<N;++i){
			this->data[i] = src.data[i];
		}
	}

	size_t size(){
		return N;
	}

	inline vec<N,T>& operator=(const vec<N,T>& src){
		for(unsigned int i = 0;i<N;++i){
			this->data[i] = src.data[i];
		}
		return *this;
	}

	vec<N,T>& operator+=(const vec<N,T>& rhs){
		for(unsigned int i = 0;i<N;++i){
			this->data[i] += rhs.data[i];
		}
		return *this;
	}

	vec<N,T>& operator-=(const vec<N,T>& rhs){
		for(unsigned int i = 0;i<N;++i){
			this->data[i] -= rhs.data[i];
		}
		return *this;
	}

	vec<N,T>& operator*=(const vec<N,T>& rhs){
		for(unsigned int i = 0;i<N;++i){
			this->data[i] *= rhs.data[i];
		}
		return *this;
	}

	vec<N,T>& operator/=(const vec<N,T>& rhs){
		for(unsigned int i = 0;i<N;++i){
			this->data[i] /= rhs.data[i];
		}
		return *this;
	}

	vec<N,T> operator+( const vec<N,T>& rhs ) const {
		vec<N,T> result(*this);
		result += rhs;
		return result;
	}

	vec<N,T> operator-( const vec<N,T>& rhs ) const {
		vec<N,T> result(*this);
		result -= rhs;
		return result;
	}

	vec<N,T> operator*( const vec<N,T>& rhs ) const {
		vec<N,T> result(*this);
		result *= rhs;
		return result;
	}

	vec<N,T> operator/( const vec<N,T>& rhs ) const {
		vec<N,T> result(*this);
		result /= rhs;
		return result;
	}

	vec<N,T>& operator+=(const T& v){
		for(unsigned int i = 0;i<N;++i){
			this->data[i] += v;
		}
		return *this;
	}

	vec<N,T>& operator-=(const T& v){
		for(unsigned int i = 0;i<N;++i){
			this->data[i] -= v;
		}
		return *this;
	}

	vec<N,T>& operator*=(const T& v){
		for(unsigned int i = 0;i<N;++i){
			this->data[i] *= v;
		}
		return *this;
	}

	vec<N,T>& operator/=(const T& v){
		for(unsigned int i = 0;i<N;++i){
			this->data[i] /= v;
		}
		return *this;
	}

	vec<N,T> operator+( const T& v ) const {
		vec<N,T> result(*this);
		result += v;
		return result;
	}

	vec<N,T> operator-( const T& v ) const {
		vec<N,T> result(*this);
		result -= v;
		return result;
	}

	vec<N,T> operator*( const T& v ) const {
		vec<N,T> result(*this);
		result *= v;
		return result;
	}

	vec<N,T> operator/( const T& v ) const {
		vec<N,T> result(*this);
		result /= v;
		return result;
	}

	vec<N,T> operator+() const {
		vec<N,T> result(*this);
		return result;
	}

	vec<N,T> operator-() const {
		vec<N,T> result(*this);
		for(unsigned int i = 0;i<N;++i){
			result.data[i] = -(this->data[i]);
		}
		return result;
	}

	vec<N,T>& operator++()
	{
		for(unsigned int i = 0;i<N;++i){
			++(this->data[i]);
		}
		return *this;
	}

	vec<N,T> operator++(int)
	{
		vec<N,T> tmp(*this);
		operator++();
		return tmp;
	}

	vec<N,T>& operator--()
	{
		for(unsigned int i = 0;i<N;++i){
			--(this->data[i]);
		}
		return *this;
	}

	vec<N,T> operator--(int)
	{
		vec<N,T> tmp(*this);
		operator--();
		return tmp;
	}

	bool operator==( const vec<N,T>& rhs ) const {
		bool res = true;
		for(unsigned int i = 0;i<N;++i){
			if(rhs.data[i] != this->data[i]){
				res = false;
				break;
			}
		}
		return res;
	}

	T& operator[]( unsigned int idx ) {
		if(idx < N){
			return this->data[idx];
		}
		abmt::die("Index out of range!!!");
	}

	T operator[]( unsigned int idx ) const {
		if(idx < N){
			return this->data[idx];
		}
		abmt::die("Index out of range!!!");
	}

	T quad_len() const {
		T res = 0;
		for(unsigned int i = 0;i<N;++i){
			res += this->data[i]*this->data[i];
		}
		return res;
	}

	/// Alias for quad_len;
	T qlen() const {
			return quad_len();
		}

	T len() const {
		return std::sqrt(quad_len());
	}

	T sum(){
		T res = 0;
		for(unsigned int i = 0;i<N;++i){
			res += this->data[i];
		}
		return res;
	}

	vec<N,T> norm(){
		vec<N,T> res(*this);
		res =  res / res.len();
		return res;
	}

	vec<N,T> cross(vec<N,T> b){
			vec<N,T> res;
			if(N == 3){
				res.data[0] = this->data[1] * b.data[2] - this->data[2] * b.data[1];
				res.data[1] = this->data[2] * b.data[0] - this->data[0] * b.data[2];
				res.data[2] = this->data[0] * b.data[1] - this->data[1] * b.data[0];
			}else{
				assert(false);
			}
			return res;
	}

	T dot(vec<N,T> b) const {
		return ((*this) * b).sum();
	}

	size_t num_cols(){
		return N;
	}

	size_t num_rows(){
		return 1;
	}

	mat<N,1,T> m();
	mat<1,N,T> t();

};

using vec2 = vec<2,double>;
using vec3 = vec<3,double>;
using vec4 = vec<4,double>;

using vec2f = vec<2,float>;
using vec3f = vec<3,float>;
using vec4f = vec<4,float>;

using vec2i = vec<2,int32_t>;
using vec3i = vec<3,int32_t>;
using vec4i = vec<4,int32_t>;

using col3 = vec<3,uint8_t>; // color rgb
using col4 = vec<4,uint8_t>; // color rgba

template<> abmt::serialize::type abmt::serialize::define_type(vec2* target);
template<> abmt::serialize::type abmt::serialize::define_type(vec3* target);
template<> abmt::serialize::type abmt::serialize::define_type(vec4* target);

template<> abmt::serialize::type abmt::serialize::define_type(vec2f* target);
template<> abmt::serialize::type abmt::serialize::define_type(vec3f* target);
template<> abmt::serialize::type abmt::serialize::define_type(vec4f* target);

template<> abmt::serialize::type abmt::serialize::define_type(vec2i* target);
template<> abmt::serialize::type abmt::serialize::define_type(vec3i* target);
template<> abmt::serialize::type abmt::serialize::define_type(vec4i* target);

#pragma pack (pop)

} // namespace abmt

#endif /* UTIL_MATH_VEC_H_ */
