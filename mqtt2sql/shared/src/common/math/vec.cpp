/*
 * vec.cpp
 *
 *  Created on: 20.09.2021
 *      Author: hva
 */
#include <abmt/math/vec.h>

template<> abmt::serialize::type abmt::serialize::define_type(vec2* target){
	static const abmt::serialize::member list[] = {
		ABMT_DEF_PTR_MEMBER(target, x),
		ABMT_DEF_PTR_MEMBER(target, y),
	};
	return list;
}

template<> abmt::serialize::type abmt::serialize::define_type(vec3* target){
	static const abmt::serialize::member list[] = {
		ABMT_DEF_PTR_MEMBER(target, x),
		ABMT_DEF_PTR_MEMBER(target, y),
		ABMT_DEF_PTR_MEMBER(target, z),
	};
	return list;
}

template<> abmt::serialize::type abmt::serialize::define_type(vec4* target){
	static const abmt::serialize::member list[] = {
		ABMT_DEF_PTR_MEMBER(target, x),
		ABMT_DEF_PTR_MEMBER(target, y),
		ABMT_DEF_PTR_MEMBER(target, z),
		ABMT_DEF_PTR_MEMBER(target, w),
	};
	return list;
}


template<> abmt::serialize::type abmt::serialize::define_type(vec2f* target){
	static const abmt::serialize::member list[] = {
		ABMT_DEF_PTR_MEMBER(target, x),
		ABMT_DEF_PTR_MEMBER(target, y),
	};
	return list;
}

template<> abmt::serialize::type abmt::serialize::define_type(vec3f* target){
	static const abmt::serialize::member list[] = {
		ABMT_DEF_PTR_MEMBER(target, x),
		ABMT_DEF_PTR_MEMBER(target, y),
		ABMT_DEF_PTR_MEMBER(target, z),
	};
	return list;
}

template<> abmt::serialize::type abmt::serialize::define_type(vec4f* target){
	static const abmt::serialize::member list[] = {
		ABMT_DEF_PTR_MEMBER(target, x),
		ABMT_DEF_PTR_MEMBER(target, y),
		ABMT_DEF_PTR_MEMBER(target, z),
		ABMT_DEF_PTR_MEMBER(target, w),
	};
	return list;
}


template<> abmt::serialize::type abmt::serialize::define_type(vec2i* target){
	static const abmt::serialize::member list[] = {
		ABMT_DEF_PTR_MEMBER(target, x),
		ABMT_DEF_PTR_MEMBER(target, y),
	};
	return list;
}

template<> abmt::serialize::type abmt::serialize::define_type(vec3i* target){
	static const abmt::serialize::member list[] = {
		ABMT_DEF_PTR_MEMBER(target, x),
		ABMT_DEF_PTR_MEMBER(target, y),
		ABMT_DEF_PTR_MEMBER(target, z),
	};
	return list;
}

template<> abmt::serialize::type abmt::serialize::define_type(vec4i* target){
	static const abmt::serialize::member list[] = {
		ABMT_DEF_PTR_MEMBER(target, x),
		ABMT_DEF_PTR_MEMBER(target, y),
		ABMT_DEF_PTR_MEMBER(target, z),
		ABMT_DEF_PTR_MEMBER(target, w),
	};
	return list;
}


