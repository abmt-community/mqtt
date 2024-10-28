/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef UTIL_INC_SIMPLE_JSON_H_
#define UTIL_INC_SIMPLE_JSON_H_

#include <initializer_list>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <ostream>

namespace abmt{
class json;
struct json_hash_entry;

namespace json_details{

	class iterator; // forward decl

	struct iterator_output{
		json&     value;
		std::string  key;
		size_t    index;
		iterator& it;
		iterator_output(iterator& it, json& v, std::string k, size_t i);
		void erase();
	};

	class iterator{
	public:
		json& src;
		size_t obj_itr = 0;
		size_t arr_itr = 0;
		size_t index = 0;

		size_t num_erased = 0;

		iterator(json& src);
		iterator operator++();
	    iterator_output operator*();
		bool operator!=(iterator& lhs);
	};

	std::string escaped_str(std::string str);
	std::string un_escaped_str(const std::string& str);

} // namspace json_details

class json{
public:
	enum types {null_type, boolean, num, str_type, obj, arr};
	union type_data{
		bool boolean;
		double number;
		std::string* str;
		std::vector<json>* arr;
		std::vector<json_hash_entry>* obj;
	};

	types type = types::null_type;
	type_data v = {};

public:
	json();
	~json();


	json(bool val);
	json(std::string val);
	json(const char* val);
	json(double n);
	template <typename T,
	          typename = typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type>
	json(T n){
		type = num;
		v.number = n;
	}
	json(std::initializer_list<json> list);

	template<typename T>
	json(const std::vector<T>& src){
		type = types::arr;
		v.arr = new std::vector<json>;
		for(auto i:src){
			v.arr->push_back(i);
		}
	}

	template<typename T>
	json(const std::vector<std::shared_ptr<T>>& src){
		type = types::arr;
		v.arr = new std::vector<json>;
		for(auto i:src){
			v.arr->push_back(*i);
		}
	}

	template<typename T>
	json(const std::map<std::string, T>& src){
		type = types::obj;
		v.obj = new std::vector<json_hash_entry>;
		for(auto i:src){
			v.obj->operator [](i.first) = i.second;
		}
	}

	template<typename T>
	json(const std::map<std::string, std::shared_ptr<T>>& src){
		type = types::obj;
		v.obj = new std::vector<json_hash_entry>;
		for(auto i:src){
			v.obj->operator [](i.first) = *(i.second);
		}
	}

	static json array(std::initializer_list<json> list = {});
	static json object();
	static json string(std::string s = "");
	static json number(double n = 0);
	static json null();


	/// resets to type no init
	void destruct();

	/// returns *this
	json& merge(json obj);
	/// returns *this
	json& swap(json& partner);

	json(const json& copy);
	json& operator=(const json& rhs);

	json(json&& rhs);
	json& operator=(json&& rhs);

	bool is_object();
	bool is_array();
	bool is_num();
	bool is_bool();
	bool is_string();
	bool is_null();

    template <typename T,
              typename = typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, char>::value>::type>
	operator T(){
    	switch (type){
			case null_type: return 0;
			case boolean: return v.boolean;
			case num: return v.number;
			case str_type: return std::stof(*v.str);
			default: return 0;
		}
    }
    operator std::string();

	std::string str();

	json& operator[](json idx);
	json& operator[](const char* idx);
	json& operator[](std::string idx);
	json& operator[](int idx);

	size_t count(std::string idx);
	bool key_exsists(std::string idx);
	/// same as key_exsists()
	bool exsists(std::string idx);
	size_t empty();
	size_t len();
	size_t length();

	/// returns *this
	json& clear();
	/// returns *this
	json& erase(size_t idx);
	/// returns *this
	json& erase(std::string key);
	/// returns *this
	json& set(std::string key, const json& val);
	/// returns *this
	json& push_back(const json& val);
	/// returns *this
	json& push_back(json&& val);
	/// returns *this
	json& pop_back();

	json& front();
	json& back();
	/// intend = intention per object / array. "" leads no intention and no new line per attribute.
	std::string dump(std::string intend = "  ");
	/// more compact multi-line dump
	std::string dump_compact(int width = 50);
	static json parse(void* data_ptr, size_t data_len);
	static json parse(const std::string string);

	/// Used for serialization. Same as dump with in_on_line set
	std::string to_json_str();
	/// Used for serialization. Same as this_obj = json::parse(s)
	void from_json_str(std::string s);

	bool operator==(const json& rhs);
	bool operator!=(const json& rhs);

	json operator+(const json& rhs);
	json operator-(const json& rhs);
	json operator*(const json& rhs);
	json operator/(const json& rhs);

	json& operator+=(const json& rhs);
	json& operator-=(const json& rhs);
	json& operator*=(const json& rhs);
	json& operator/=(const json& rhs);

	/// Removes " \n\r\t" at the beginning and the end of the string
	json& trim();

	json_details::iterator begin();
	json_details::iterator end();

}; // value

struct json_hash_entry{
	std::string key;
	json value;
};


std::ostream& operator<< (std::ostream& stream, json& v);


} // namespace abmt


#endif /* UTIL_INC_SIMPLE_JSON_H_ */
