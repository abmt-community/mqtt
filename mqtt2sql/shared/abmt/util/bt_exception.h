/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef UTIL_INC_BT_EXCEPTION_H_
#define UTIL_INC_BT_EXCEPTION_H_

#include <stdexcept>

namespace abmt{
namespace util{

/// use abmt::die(msg) instead of throwing bt_exception
class bt_exception: public std::runtime_error{
public:
	void* bt_ptrs[40] = {0};
	size_t num_frames = 0;

	bt_exception( const std::string& error_msg );

	bt_exception( const char* error_msg );
	void read_bt();
	void print_backtrace();

};

}; // namespace util
}; // namespace abmt

#endif /* UTIL_INC_BT_EXCEPTION_H_ */
