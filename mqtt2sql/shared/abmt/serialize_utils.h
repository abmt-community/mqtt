/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_SRC_COMMON_SERIALIZE_UTILS_H_
#define SHARED_SRC_COMMON_SERIALIZE_UTILS_H_

#include <abmt/serialize.h>
#include <abmt/json.h>

namespace abmt{
namespace serialize{

// prints a definition to the console
void print_raw_def(char* buffer);

//simple_json::value to_json(char* def, size_t def_size, void* data, size_t data_size);

abmt::json to_json(abmt::serialize::type src);

void from_json(abmt::json& src, abmt::serialize::type& dst);

} // namespace serialize
} // namespace abmt

#endif /* SHARED_SRC_COMMON_SERIALIZE_UTILS_H_ */
