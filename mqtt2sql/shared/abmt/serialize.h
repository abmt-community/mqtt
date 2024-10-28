/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef UTIL_EXAMPLES_SERIALIZE2_H_
#define UTIL_EXAMPLES_SERIALIZE2_H_

#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
#include <abmt/blob.h>

// I'm not proud of this!
#define ABMT_DEF_MEMBER(X) {#X, &this->X, this}
#define ABMT_DEF_PTR_MEMBER(P, X) {#X, &P->X, P}

namespace abmt{
namespace serialize{
class type;

template<class C> type define_type(C* ptr = 0);

class member;

struct member_list{
	size_t length;
	const member* list;

	member_list() = default; // @suppress("Class members should be properly initialized")

	template<int N>
	member_list(const member(&ptr)[N] ){
		length = N;
		list = ptr;
	}

	const member* begin();
	const member* end();
};

struct blob_type{
	size_t (*get_size)    (void* obj);
	void   (*obj_to_data) (void* obj, void* data, size_t size);
	void   (*data_to_obj) (void* obj, void* data, size_t size);
};

struct string_type: public blob_type{

};

struct scene_2d_type: public blob_type{
	scene_2d_type(blob_type r);
};

struct scene_3d_type: public blob_type{
	scene_3d_type(blob_type r);
};

template <typename T>
class obj_has_define_type
{
    typedef char one[1];
    typedef char two[2];

    template <typename C> static one& test( decltype(&C::define_type) ) ;
    template <typename C> static two& test(...);

public:
    enum { value = sizeof(test<T>(0)) == sizeof(one) };
};

template <typename T>
class obj_serializes_to_json
{
    typedef char one[1];
    typedef char two[2];

    template <typename C> static one& test( decltype(&C::to_json_str)) ;
    template <typename C> static two& test(...);

public:
    enum { value = sizeof(test<T>(0)) == sizeof(one) };
};

template <typename T>
class obj_serializes_to_string
{
    typedef char one[1];
    typedef char two[2];

    template <typename C> static one& test( decltype(&C::to_string) ) ;
    template <typename C> static two& test(...);

public:
    enum { value = sizeof(test<T>(0)) == sizeof(one) };
};

enum class type_id: uint8_t{
	// default:
	UNDEFINED = 0,
	// base types
	BOOL,
	CHAR,
	UINT8,
	INT8,
	UINT16,
	INT16,
	UINT32,
	INT32,
	UINT64,
	INT64,
	FLOAT,
	DOUBLE,

	// special:
	OBJECT,
	STRING,
	RAW_DATA,
	SCENE_2D,
	SCENE_3D,
	JSON_STRING,       // object has to_json_str()/from_json_str() methods
	SERIALIZED_STRING, // object has to_string()/from_string() methods


	// object based special types
	MAN_CTRL = 100,

	// only used in definition / helper
	OBJECT_END = 255
};


bool type_id_is_value(type_id t);
bool type_id_is_num(type_id t);
bool type_id_is_obj(type_id t);
bool type_id_is_binary(type_id t);

/// Type is a container for something you want to serialize. It holds the information
/// how to serialize the object you created the type from, and a pointer to this object (void* obj).
class type{
public:

	type_id id;

	void* obj;

	union serialize_type_info{
		member_list attr;
		blob_type rexp;
		size_t size;
	};

	serialize_type_info serialize_info = {};

	type(); // undefined

	type(bool& o);

	template <typename T,
		          typename = typename std::enable_if<std::is_integral<T>::value
				  && !std::is_same<T, bool>::value
				  >::type>
	type(T& o){
		id = type_id::UNDEFINED;
		obj = 0;
		if(std::is_signed<T>::value){
			if(sizeof(o) == 1){
				id = type_id::INT8;
				obj = &o;
				serialize_info.size = sizeof(o);
			}
			if(sizeof(o) == 2){
				id = type_id::INT16;
				obj = &o;
				serialize_info.size = sizeof(o);
			}
			if(sizeof(o) == 4){
				id = type_id::INT32;
				obj = &o;
				serialize_info.size = sizeof(o);
			}
			if(sizeof(o) == 8){
				id = type_id::INT64;
				obj = &o;
				serialize_info.size = sizeof(o);
			}
		}else{
			if(sizeof(o) == 1){
				id = type_id::UINT8;
				obj = &o;
				serialize_info.size = sizeof(o);
			}
			if(sizeof(o) == 2){
				id = type_id::UINT16;
				obj = &o;
				serialize_info.size = sizeof(o);
			}
			if(sizeof(o) == 4){
				id = type_id::UINT32;
				obj = &o;
				serialize_info.size = sizeof(o);
			}
			if(sizeof(o) == 8){
				id = type_id::UINT64;
				obj = &o;
				serialize_info.size = sizeof(o);
			}
		}
	}

	type(float& o);
	type(double& o);
	type(std::string& o);
#ifdef __cpp_lib_string_view
	type(std::string_view& o);
#endif

	type(member_list lst);
	type(abmt::blob& blk);
	type(abmt::blob_shared& blk);

	template<int N>
	type(const member(&ptr)[N] ){
		id = type_id::OBJECT;
		serialize_info.attr.length = N;
		serialize_info.attr.list = ptr;
		obj = 0;
	}

	/// Construct type from a class that has a define_type method.
	template<class C>
	type(C& o, std::enable_if_t<obj_has_define_type<C>::value && !std::is_integral<C>::value>* = 0){
		*this = o.define_type();
		obj = &o;
	}

	/// Construct type from a class that has a to_- / from_json method.
	template<class C>
	type(C& o, std::enable_if_t<obj_serializes_to_json<C>::value && !std::is_integral<C>::value>* = 0){
		id = type_id::JSON_STRING;
		obj = &o;
		serialize_info.rexp = {
				[](void* obj) -> size_t { return (*(C*)obj).to_json_str().size(); },
				[](void* obj, void* data, size_t size) { (*(C*)obj).to_json_str().copy((char*)data,size); },
				[](void* obj, void* data, size_t size) { (*(C*)obj).from_json_str(std::string((char*)data,size)); }
		};
	}

	/// Construct type from a class that has a to_- / from_string method.
	template<class C>
	type(C& o, std::enable_if_t<obj_serializes_to_string<C>::value && !std::is_integral<C>::value>* = 0){
		id = type_id::SERIALIZED_STRING;
		obj = &o;
		serialize_info.rexp = {
				[](void* obj) -> size_t { return (*(C*)obj).to_string().size(); },
				[](void* obj, void* data, size_t size) { (*(C*)obj).to_string().copy((char*)data,size); },
				[](void* obj, void* data, size_t size) { (*(C*)obj).from_string(std::string((char*)data,size)); }
		};
	}

	/// When no other constructor works, this one is used. It utilizes define_type.
	/// When no implementation is found it defaults to "UNDEFINDED".
	template<class C>
	type(C& o, std::enable_if_t<!obj_has_define_type<C>::value && !std::is_integral<C>::value>* = 0,
			   std::enable_if_t<!obj_serializes_to_string<C>::value && !std::is_integral<C>::value>* = 0,
			   std::enable_if_t<!obj_serializes_to_json<C>::value && !std::is_integral<C>::value>* = 0

	){
		C* zero_ptr_for_offsets = 0;
		*this = define_type(zero_ptr_for_offsets);
		obj = &o;
	}



	//do not call directly: call via template above and create_type;
	type(blob_type r);
	type(string_type s);
	type(scene_2d_type s);
	type(scene_3d_type s);

	type(const abmt::serialize::type& e) = default;
	type(abmt::serialize::type& e) = default; // very important! copy-assingment would be overwriten by template

	template<typename T>
	type& operator=(T& rhs){ // "&" is important here! You dont want a copy here
		*this = type(rhs);
		return *this;
	}

	size_t get_def_size();
	size_t get_def(void* def_buff, size_t def_size); // returns bytes written to def_buff (used internally)


	size_t get_size();
	void   serialize(void* data, size_t data_size);
	void   deserialize(void* data, size_t data_size);
	void   deserialize(void* data, size_t data_size, void* def_buff, size_t def_size);

	double to_double(void* data_ptr, size_t data_len);
	void   from_double(double d);

private:
	// returns bytes copied from data to this.obj;
	size_t copy(void* data, size_t data_size, bool reverse = false);
};


struct member{
	const char* name;
	type t;
	size_t offset;

	/// Expects that member is referenced to zero so that the value of a pointer to member
	/// represents the offset to the start of the object that it belongs to.
	/// See examples [flex tape meme]...
	template<typename T>
	member(const char* name_str, T& member):name(name_str),t(type(member)){
		offset = (size_t)&member;
	}

	template<typename T>
	member(const char* name_str, T* member, void* obj):name(name_str),t(type(*member)){
		offset = (size_t)member - (size_t)obj;
	}

};

// default type: undefined
template<class C> type define_type(C* ptr){
	return type();
}

struct decoder{
	/// raw data. Can also be used to convert to string.
	abmt::blob data;
	abmt::blob def;

	decoder(const decoder&) = delete; /// copy is not allowed because of the ownership of the underling pointers. (You may have forgot to set the type in your cast expression.)

	/// Deserializes data and def to the type T.
	template<
			  typename T,
			  typename std::enable_if<
			  	   !std::is_same<T,abmt::serialize::type>::value
			    && !std::is_same<T,char>::value
				&& !std::is_same<T,const char*>::value
				&& !std::is_same<T,std::string&&>::value
				&& !std::is_same<T,const std::string&>::value
				&& !std::is_same<T,std::initializer_list<char>>::value
			  >::type* = nullptr
		    >
	operator T(){
		 T res = {};
		 if(data.data == 0){
			 // can happen when playback has no data to play yet
			 return res;
		 }
		 abmt::serialize::type decode_type(res);
		 decode_type.deserialize(data.data, data.size, def.data, def.size);
		 return res;
	}

	/// This method returns "(cast me)".
	/// If you don't want to use the cast, you can access the raw data via the data member.
	/// obj.data.str() creates a string out of the raw data.
	std::string to_string() { return "(cast me)"; }

	/// not used
	void from_string(std::string) { }

};

} // namespace serialize
} // namespace abmt

#endif /* UTIL_EXAMPLES_SERIALIZE2_H_ */
