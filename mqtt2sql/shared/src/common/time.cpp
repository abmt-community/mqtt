/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/time.h>
#include <ctime>

namespace abmt{

time::time(int64_t ns):ns_since_1970(ns){

}

time time::ns(int64_t t){
	return time(t);
}

time time::us(int64_t t){
	return time(t * 1000);
}

time time::ms(int64_t t){
	return time(t * 1000 * 1000);
}

time time::sec(int64_t t){
	return time(t * 1000LL * 1000 * 1000);
}

time time::min(int64_t t){
	return time(t * 1000LL * 1000 * 1000 * 60);
}

time time::hour(int64_t t){
	return time(t * 1000LL * 1000 * 1000 * 60 * 60);
}

time time::day(int64_t t){
	return time(t * 1000LL * 1000 * 1000 * 60 * 60 * 24);
}

time time::week(int64_t t){
	return time(t * 1000LL * 1000 * 1000 * 60 * 60 * 24 * 7);
}

time time::year(int64_t t){
	return time(t * 1000LL * 1000 * 1000 * 60 * 60 * 24 * 7 * 52);
}


int64_t time::ns(){
	return ns_since_1970;
}

int64_t time::us(){
	return ns_since_1970/(1000);
}

int64_t time::ms(){
	return ns_since_1970/(1000*1000);
}

int64_t time::sec(){
	return ns_since_1970/(1000*1000*1000);
}

int64_t time::min(){
	return ns_since_1970/(1000LL*1000*1000*60);
}

int64_t time::hour(){
	return ns_since_1970/(1000LL*1000*1000*60*60);
}

int64_t time::day(){
	return ns_since_1970/(1000LL*1000*1000*60*60*24);
}

int64_t time::week(){
	return ns_since_1970/(1000LL*1000*1000*60*60*24*7);
}

int64_t time::year(){
	return ns_since_1970/(1000LL*1000*1000*60*60*24*7*52);
}

double time::sec2(){
	return (double)ns_since_1970/(1000*1000*1000);
}

time time::delta_to_now(){
	return now() - *this;
}

bool time::operator==(const time &t) { return ns_since_1970 == t.ns_since_1970; }
bool time::operator!=(const time &t) { return ns_since_1970 != t.ns_since_1970; }
bool time::operator< (const time &t) { return ns_since_1970 <  t.ns_since_1970; }
bool time::operator> (const time &t) { return ns_since_1970 >  t.ns_since_1970; }
bool time::operator<=(const time &t) { return ns_since_1970 <= t.ns_since_1970; }
bool time::operator>=(const time &t) { return ns_since_1970 >= t.ns_since_1970; }

time& time::operator+=(const time &t){
	ns_since_1970 = ns_since_1970 + t.ns_since_1970;
	return *this;
}

time& time::operator-=(const time &t){
	ns_since_1970 = ns_since_1970 - t.ns_since_1970;
	return *this;
}

time& time::operator*=(const time &t){
	ns_since_1970 = ns_since_1970 * t.ns_since_1970;
	return *this;
}

time& time::operator/=(const time &t){
	ns_since_1970 = ns_since_1970 / t.ns_since_1970;
	return *this;
}

time time::operator+(const time &t) const{
	time res(*this);
	res += t;
	return res;
}
time time::operator-(const time &t) const{
	time res(*this);
	res -= t;
	return res;
}
time time::operator*(const time &t) const{
	time res(*this);
	res *= t;
	return res;
}
time time::operator/(const time &t) const{
	time res(*this);
	res /= t;
	return res;
}

date::date(time t){
	double t_in_sec = t.sec2();
	std::time_t std_t = t_in_sec;
	std::tm tm = *std::localtime(&std_t);
	year  = tm.tm_year + 1900;
	month = tm.tm_mon +1;
	day   = tm.tm_mday;
	hour  = tm.tm_hour;
	min   = tm.tm_min;
	sec   = tm.tm_sec + (t_in_sec - (int64_t)t_in_sec);
}

date date::zero(){
	date res;
	res.year  = 0;
	res.month = 0;
	res.day   = 0;
	res.hour  = 0;
	res.min   = 0;
	res.sec   = 0;
	return res;
}

date::operator time(){
	std::tm tm = {0};
	tm.tm_year  = year - 1900;
	tm.tm_mon   = month -1;
	tm.tm_mday  = day;
	tm.tm_hour  = hour;
	tm.tm_min   = min;
	tm.tm_sec   = sec;
	tm.tm_isdst = -1;

	std::time_t std_t = std::mktime(&tm) ;
	time res(std_t*1000ULL*1000*1000);
	res += time::ns((sec - (int64_t)sec)*1000ULL*1000*1000);
	return res;
}

bool date::is_zero(){
	return 	year == 0 && month == 0 && 	day == 0 && hour == 0 && min == 0 && sec == 0;
}

} // namespace
