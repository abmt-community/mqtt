/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef INC_MODEL_H_
#define INC_MODEL_H_

#include <abmt/rt/raster.h>
#include <abmt/rt/parameter.h>
#include <abmt/const_array.h>
#include <abmt/serialize.h>

namespace abmt{
namespace rt{

struct signal_exporter{
	const char* name;
	abmt::serialize::type type;
	unsigned int raster_index;
};

struct parameter_list_element{
	const char* name;
	abmt::rt::parameter_base* parameter;
};

class model{
public:
	abmt::const_array<raster*>  rasters;
	abmt::const_array<signal_exporter> signals;
	abmt::const_array<parameter_list_element> parameters;

	virtual ~model(){};
};

/**
 * This function is (optionally) generated by the model-compiler.
 * The main reason for this function is to avoid including the
 * generated header in the runtime-sources. Furthermore this helps
 * to keep the runtime model-independent and saves compile-time.
 */
abmt::rt::model* get_model();

} // namespace runtime
} // namespace abmt

#endif /* INC_MODEL_H_ */
