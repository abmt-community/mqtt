/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_STR_UTILS_H_
#define SHARED_ABMT_STR_UTILS_H_

#include <abmt/time.h>

#include <string>
#include <vector>

namespace abmt{
namespace util{

std::string str_to_upper(std::string s);
std::string str_to_lower(std::string s);

std::string str_replace(std::string str,std::string search, std::string replace);

/// Removes leading and tailing " \n\r\t"
std::string str_trim(std::string s);
std::vector<std::string> str_split(std::string target, std::string token = ",");
std::vector<std::string> str_split_and_trim(std::string target, std::string token = ",");

/// Replaces #Y #y #M #D #h #m #s #s.s in str with date (#Y -> 2020, #y -> 20).
std::string str_replace_date(std::string str = "#Y-#M-#D #h:#m:#s", abmt::date d = abmt::time::now());

/// "format" can contain #Y #y #M #D #h #m #s. For seconds can have subseconds.
abmt::date  str_read_date(std::string str,std::string format);

/// Tries to guess the format of str. Returns date.is_zero() on parse fail.
abmt::date  str_read_date(std::string str);

} //namespace util
} //namespace abmt

#endif
