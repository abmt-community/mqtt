/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_IO_I2C_H_
#define SHARED_ABMT_IO_I2C_H_

#include <cstdint>
#include <initializer_list>
#include <abmt/util/default_ptr.h>

namespace abmt{
namespace io{

struct i2c{

	/// returns true when everything went fine. This is the only function you have to implement
	/// to use this interface
    virtual bool write_read(uint16_t addr, void* write, uint16_t w_len, void* read, uint16_t r_len){
    	return false;
    }

    template<typename T1, typename T2>
    bool write_read(uint16_t adr, T1& w, T2& r){
        return write_read(adr,&w, sizeof(w), &r, sizeof(r));
    }

    template<typename T1>
    bool write_read(uint16_t adr, std::initializer_list<uint8_t> w, T1& r){
        return write_read(adr,(void*)w.begin(), w.size(), &r, sizeof(r));
    }

    template<typename T>
    bool write(uint16_t adr, T& w){
        return write_read(adr,&w, sizeof(w), 0, 0);
    }

    bool write(uint16_t adr, std::initializer_list<uint8_t> d){
        return write_read(adr,(void*)d.begin(), d.size(), 0, 0);
    }

    template<typename T>
    bool read(uint16_t adr, T& w){
        return write_read(adr,0, 0, &w, sizeof(w));
    }

    virtual ~i2c() {};
};
using i2c_ptr = abmt::default_ptr<i2c>;

using i2c_bus = i2c;
using i2c_bus_ptr = abmt::default_ptr<i2c_bus>;

} // namespace io
} // namespace abmt


#endif /* SHARED_ABMT_IO_I2C_H_ */
