/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_OS_H_
#define SHARED_ABMT_OS_H_

#include <string>

namespace abmt{

[[noreturn]] void die(std::string msg);

/// Dies when condition is true.
void die_if(bool condition, std::string msg = "die_if: fatal error!");

void log(std::string msg);

[[deprecated("use abmt::log instead and place the word error in the log")]]
void log_err(std::string msg);

}; // namespace abmt

#endif /* SHARED_ABMT_OS_H_ */
