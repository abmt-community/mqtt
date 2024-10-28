/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_RT_RASTER_H_
#define SHARED_ABMT_RT_RASTER_H_

#include <cstdint>
#include <abmt/time.h>

namespace abmt{
namespace rt{

class raster{
public:
	const char* name;
	bool is_sync = false;
	abmt::time interval;
	uint64_t n_ticks = 0;

	virtual void init(){

	}

	virtual void init_tick(){

	}

	virtual void tick(){

	}

	/**
	 * Returns the time to wait until next poll.
	 * When 0 is returned the raster should be executed.
	 */
	virtual abmt::time poll(){
		return abmt::time::sec(1);
	}

	virtual void final(){

	}

	virtual ~raster(){

	};
};



} // namespace runtime
} // namespace abmt


#endif /* INC_RASTER_H_ */
