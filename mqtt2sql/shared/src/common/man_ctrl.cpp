/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */
#include <abmt/man_ctrl.h>
#include <iostream>

using namespace std;
using namespace abmt;

void man_ctrl_state::print(){
    return;
    cout << "keys: ";
    if(alt){
        cout << "Alt ";
    }
    if(ctrl){
        cout << "Ctrl ";
    }
    if(shift){
        cout << "Shift ";
    }
    if(key1 != 0){
        cout << (int) key1 << " ";
    }
    if(key2 != 0){
        cout << (int) key2 << " ";
    }
    if(key3 != 0){
        cout << (int) key3 << " ";
    }
    if(key4 != 0){
        cout << (int) key4 << " ";
    }
    if(key5 != 0){
        cout << (int) key5 << " ";
    }
    if(key6 != 0){
        cout << (int) key6 << " ";
    }
    if(key7 != 0){
        cout << (int) key7 << " ";
    }
    if(key8 != 0){
        cout << (int) key8 << " ";
    }

    cout << endl;
}

bool man_ctrl_state::key_pressed(uint8_t key){
    if( key1 == key || key2 == key || key3 == key || key4 == key ||
        key5 == key || key6 == key || key7 == key || key8 == key
    ){
        return true;
    }
    return false;
}

bool man_ctrl_state::no_key_pressed(){
    if( key1 == 0 && key2 == 0 && key3 == 0 && key4 == 0 &&
        key5 == 0 && key6 == 0 && key7 == 0 && key8 == 0 &&
        alt == false && ctrl == false && shift == false
    ){
        return true;
    }
    return false;
}

template<> abmt::serialize::type abmt::serialize::define_type(man_ctrl_state* ptr){
	static const serialize::member map[] = {
			{"key1", ptr->key1},
			{"key2", ptr->key2},
			{"key3", ptr->key3},
			{"key4", ptr->key4},
			{"key5", ptr->key5},
			{"key6", ptr->key6},
			{"key7", ptr->key7},
			{"key8", ptr->key8},
			{"alt", ptr->alt},
			{"ctrl", ptr->ctrl},
			{"shift", ptr->shift}
	 };

	 auto t = abmt::serialize::type(map);
	 t.id = abmt::serialize::type_id::MAN_CTRL;
	 return t;
}

