/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_IO_RCV_RSP_H_
#define SHARED_ABMT_IO_RCV_RSP_H_
#include <string>
#include <abmt/util/pimpl.h>
#include <abmt/blob.h>
#include <abmt/util/default_ptr.h>

namespace abmt{
namespace io{

struct rcv_rsp{
	/// can be set and used by the receiver
	abmt::util::pimpl session;
	/// Data is hold during conneciton. Needs to be cleared by receiver. 
	abmt::blob_shared data;

	virtual void send(void* data, size_t size){

	}

	virtual void send(std::string s){
		send((void*)s.c_str(), s.size());
	}

	virtual void close(){

	}

	virtual ~rcv_rsp(){

	}
};

using rcv_rsp_ptr = abmt::default_ptr<rcv_rsp>;

} // namespace io
} // namespace abmt

#endif /* SHARED_ABMT_IO_RCV_RSP_H_ */
