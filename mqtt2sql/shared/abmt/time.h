/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef UTIL_INC_CLOCK_H_
#define UTIL_INC_CLOCK_H_

#include <cstdint>
#include <string>
#include <chrono>

namespace abmt{

class time{
public:

	volatile int64_t ns_since_1970;

	time(int64_t ns=0);

	template<typename T>
	time(std::chrono::duration<T> d){
		ns_since_1970 = std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
	}

	template<typename T, typename U>
	time(std::chrono::duration<T,U> d){
		ns_since_1970 = std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
	}

	/// must be defined in runtime.
	static time now();

	static time ns(int64_t t);
	static time us(int64_t t);
	static time ms(int64_t t);
	static time sec(int64_t t);
	static time min(int64_t t);
	static time hour(int64_t t);
	static time day(int64_t t);
	static time week(int64_t t);
	static time year(int64_t t);

	int64_t ns();
	int64_t us();
	int64_t ms();
	int64_t sec();
	int64_t min();
	int64_t hour();
	int64_t day();
	int64_t week();
	int64_t year();

	double sec2();

	time delta_to_now();

	bool operator==(const time &t);
	bool operator!=(const time &t);
	bool operator< (const time &t);
	bool operator> (const time &t);
	bool operator<=(const time &t);
	bool operator>=(const time &t);

	time& operator+=(const time &t);
	time& operator-=(const time &t);
	time& operator*=(const time &t);

	time& operator/=(const time &t);
	time operator+(const time &t) const;
	time operator-(const time &t) const;
	time operator*(const time &t) const;
	time operator/(const time &t) const;

	template<typename T>
	time operator*(const T& n) const{
		time res(*this);
		res.ns_since_1970 = res.ns_since_1970 * n;
		return res;
	}

	template<typename T>
	time operator/(const T& n){
		time res(*this);
		res.ns_since_1970 = res.ns_since_1970 / n;
		return res;
	}
};

template<typename T>
time operator*(const T& n, const time &rhs){
	time res = rhs;
	res.ns_since_1970 = n * res.ns_since_1970;
	return res;
}

struct date{
	size_t year;
	size_t month;
	size_t day;
	size_t hour;
	size_t min;
	float  sec;

	date(time t = abmt::time::now());
	static date zero();
	operator time();
	bool is_zero();
};

} // namespace

#endif /* UTIL_INC_CLOCK_H_ */
