/**
 * Author: Hendrik van Arragon, 2022
 * SPDX-License-Identifier: MIT
 */

#include <abmt/util/str_utils.h>
#include <regex>

namespace abmt{
namespace util{

std::string str_to_upper(std::string s){
	transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return toupper(c); });
    return s;
}

std::string str_to_lower(std::string s){
	transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return tolower(c); });
    return s;
}

std::string str_replace(std::string str,std::string search, std::string replace){
	size_t pos = 0;
	while((pos = str.find(search, pos)) != std::string::npos) {
		 str.replace(pos, search.length(), replace);
		 pos += replace.length();
	}
	return str;
}

std::string str_trim(std::string s){
	s.erase(0, s.find_first_not_of(" \n\r\t"));
	s.erase(s.find_last_not_of(" \n\r\t")+1);
	return s;
}

std::vector<std::string> str_split(std::string target, std::string token){
	std::vector<std::string> lines;
	size_t pos = 0;
	while((pos = target.find(token)) != std::string::npos ){
		lines.push_back(target.substr(0, pos));
		target.erase(0, pos + token.size());
	}
	lines.push_back(target); // rest
	return lines;
}

std::vector<std::string> str_split_and_trim(std::string target, std::string token){
	std::vector<std::string> res = str_split(target,token);
	for(size_t i = 0; i < res.size(); i++){
		res[i] = str_trim(res[i]);
	}
	return res;
}

std::string str_replace_date(std::string str, abmt::date d){
	auto replace = [&](std::string search, int val){
		std::string add_zero = "";
		if(val < 10){
			add_zero = "0";
		}
		str = str_replace(str,search,add_zero + std::to_string(val));
	};

	str = str_replace(str,"#Y", std::to_string(d.year));
	str = str_replace(str,"#y", std::to_string(d.year - 2000)); // the future gereration has to change this to 2050, 2100..

	replace("#M",d.month);
	replace("#D",d.day);
	replace("#h",d.hour);
	replace("#m",d.min);

	std::string add_sec = "";
	if(d.sec < 10){
		add_sec = "0";
	}
	str = str_replace(str,"#s.s",add_sec + std::to_string(d.sec));
	str = str_replace(str,"#s",  add_sec + std::to_string((int)d.sec));
	return str;
}

abmt::date  str_read_date(std::string str,std::string format){
	abmt::date res = abmt::date::zero();

	if(str.length() < format.length()){
		return res;
	}
#ifdef __cpp_exceptions
	try{
#endif
		size_t pos = format.find("#Y");
		if(pos != std::string::npos) {
			res.year = std::stoi(str.substr(pos,4));
		}else{
			pos = format.find("#y");
			if(pos != std::string::npos) {
				res.year = std::stoi(str.substr(pos,2)) + 2000; // the future gereration has to change this to 2050, 2100..
			}
		}

		pos = format.find("#M");
		if(pos != std::string::npos) {
			res.month = std::stoi(str.substr(pos,2));
		}

		pos = format.find("#D");
		if(pos != std::string::npos) {
			res.day = std::stoi(str.substr(pos,2));
		}

		pos = format.find("#h");
		if(pos != std::string::npos) {
			res.hour = std::stoi(str.substr(pos,2));
		}

		pos = format.find("#m");
		if(pos != std::string::npos) {
			 res.min = std::stoi(str.substr(pos,2));
		}

		pos = format.find("#s");
		if(pos != std::string::npos) {
			 res.sec = std::stod(str.substr(pos));
		}
#ifdef __cpp_exceptions
	}catch(...){

	}
#endif

	return res;
}

abmt::date  str_read_date(std::string str){
	abmt::date res = abmt::date::zero();
	// find time
	auto colon1_pos = str.find(":");
	auto colon2_pos = str.find(":",colon1_pos+1);
#ifdef __cpp_exceptions
	try{
#endif
		bool time_found = false;
		if(colon1_pos != std::string::npos && colon2_pos != std::string::npos){
			res.hour = std::stoi(str.substr(colon1_pos-2,2));
			res.min = std::stoi(str.substr(colon1_pos+1,colon2_pos-colon1_pos));
			res.sec = std::stod(str.substr(colon2_pos+1));
			time_found = true;
		}else if(colon1_pos != std::string::npos && colon2_pos == std::string::npos){
			res.hour = std::stoi(str.substr(colon1_pos-2,2));
			res.min = std::stoi(str.substr(colon1_pos+1,2));
			time_found = true;
		}

		// find date
		bool found_date = false;
		auto minus1_pos = str.find("-");
		auto minus2_pos = str.find("-",minus1_pos+1);
		auto dot1_pos = str.find(".");
		auto dot2_pos = str.find(".",dot1_pos+1);
		auto slash1_pos = str.find("/");
		auto slash2_pos = str.find("/",slash1_pos+1);
		if(minus1_pos != std::string::npos && minus2_pos != std::string::npos){
			int year_pos = minus1_pos-4;
			if(year_pos < 0){
				year_pos = 0;
			}
			res.year = std::stoi(str.substr(year_pos,4));
			res.month = std::stoi(str.substr(minus1_pos+1,minus2_pos-minus1_pos));
			res.day = std::stoi(str.substr(minus2_pos+1));
			found_date = true;
		}else if(dot1_pos != std::string::npos && dot2_pos != std::string::npos){
			int day_pos = dot1_pos-2;
			if(day_pos < 0 ){
				day_pos = 0;
			}
			res.day = std::stoi(str.substr(day_pos));
			res.month = std::stoi(str.substr(dot1_pos+1,dot2_pos-dot1_pos));
			res.year = std::stoi(str.substr(dot2_pos+1));
			found_date = true;
		}else if(slash1_pos != std::string::npos && slash2_pos != std::string::npos){
			int mon_pos = slash1_pos-2;
			if(mon_pos < 0 ){
				mon_pos = 0;
			}
			res.month = std::stoi(str.substr(mon_pos));
			res.day = std::stoi(str.substr(slash1_pos+1,slash2_pos-slash1_pos));
			res.year = std::stoi(str.substr(slash2_pos+1));
			found_date = true;
		}

		if(time_found == false && found_date == false){
			if(str.length() > 8){
				return str_read_date(str,"#Y#M#D#h#m#s");
			}else if(str.length() > 6){
				return str_read_date(str,"#Y#M#D");
			}else{
				return str_read_date(str,"#y#M#D");
			}

		}
#ifdef __cpp_exceptions
	}catch(...){

	}
#endif

	if(res.year < 100){
		res.year += 2000; // the future gereration has to change this to 2050, 2100..
	}
	return res;
}

} //namespace util
} //namespace abmt
