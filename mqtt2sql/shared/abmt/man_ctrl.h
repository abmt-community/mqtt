/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_MAN_CTRL_H_
#define SHARED_ABMT_MAN_CTRL_H_

/**
 * Warning: experimental feature
 */

#include <cstdint>
#include <abmt/serialize.h>

namespace abmt{

struct man_ctrl_state{
    uint8_t key1 = 0;
	uint8_t key2 = 0;
	uint8_t key3 = 0;
	uint8_t key4 = 0;
	uint8_t key5 = 0;
	uint8_t key6 = 0;
	uint8_t key7 = 0;
	uint8_t key8 = 0;
	bool alt   = false;
	bool ctrl  = false;
	bool shift = false;

	void print();
	bool key_pressed(uint8_t key);
	bool no_key_pressed();
};

template<> abmt::serialize::type abmt::serialize::define_type(man_ctrl_state* ptr);

} // namespace abmt

#endif /* SHARED_ABMT_MAN_CTRL_H_ */
