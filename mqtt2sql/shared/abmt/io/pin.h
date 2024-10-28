/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_IO_PIN_H_
#define SHARED_ABMT_IO_PIN_H_

#include <memory>
#include <abmt/util/default_ptr.h>

namespace abmt{
namespace io{

struct opin{
	virtual void set(bool val = true){

	}
	virtual void clear(){
		set(false);
	}
	virtual ~opin() = default;
};
using opin_ptr = abmt::default_ptr<opin>;

struct ipin{
	virtual bool get(){
		return false;
	}
	virtual ~ipin() = default;
};
using ipin_ptr = abmt::default_ptr<ipin>;


} // namespace io
} // namespace abmt

#endif /* SHARED_ABMT_IO_PIN_H_ */
