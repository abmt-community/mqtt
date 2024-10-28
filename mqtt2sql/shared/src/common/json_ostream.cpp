/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/json.h>
namespace abmt{
using namespace json_details;

std::ostream& operator<< (std::ostream& stream, json& v) {
	switch (v.type){
		case json::num:
			stream << v.v.number;
			break;
		case json::boolean:
			stream << v.v.boolean;
			break;
		case json::str_type:
			stream << *(v.v.str);
			break;
		case json::obj:
			stream << v.dump() << std::endl;
			break;
		case json::arr:
			stream << v.dump() << std::endl;
			break;
		default: break;
		}

	return stream;
}

}// namespace abmt
