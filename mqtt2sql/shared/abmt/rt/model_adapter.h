/**
 * Author: Hendrik van Arragon, 2022
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_RT_MODEL_ADAPTER_H_
#define SHARED_ABMT_RT_MODEL_ADAPTER_H_

#include <abmt/rt/com.h>
#include <abmt/rt/model.h>
#include <abmt/io/protocols/s2p.h>

namespace abmt{
namespace rt{

class model_adapter{
public:

	/**
	 * Interface
	 */

	abmt::io::s2p connection;
	abmt::rt::model* mdl = 0;
	abmt::mutex send_mtx;

	virtual void on_hello(uint16_t version){

	};

	virtual void on_request_signal_def(){

	};

	virtual void on_request_parameter_def(){

	};

	virtual void on_set_daq_list(abmt::blob& data){
		send(cmd::ack_set_daq_list,0,0);
	};

	virtual void on_set_paq_list(abmt::blob& data){
		send(cmd::ack_set_paq_list,0,0);
	};

	virtual void on_set_parameter(abmt::blob& data){

	};

	virtual void on_command(std::string command){

	};


	void send_model_online(){
		static uint32_t cnt = 0;
		send(cmd::model_online,&cnt,sizeof(cnt));
		cnt++;
	}

	/**
	 * Implementation
	 */

	model_adapter(){
		// empty...
		connection.on_new_pack = [this](size_t id, void* data, size_t size){
			abmt::blob blk(data,size);
			handle_data(id,blk);
		};
	}

	void set_model(abmt::rt::model* m){
		mdl = m;
	}

	void handle_data(size_t id, abmt::blob& blk){
		cmd c = (cmd)id;
		switch(c){
		case cmd::hello:
			on_hello(blk.get<uint16_t>(0));
			break;
		case cmd::request_signal_def:
			on_request_signal_def();
			break;
		case cmd::request_parameter_def:
			on_request_parameter_def();
			break;
		case cmd::set_daq_list:
			on_set_daq_list(blk);
			break;
		case cmd::set_paq_list:
			on_set_paq_list(blk);
			break;
		case cmd::set_prameter:
			on_set_parameter(blk);
			break;
		case cmd::command:
			on_command(blk.str());
			break;
		default:
			return;
		}
	}

	virtual void send(cmd id, const void* data = 0, uint32_t size = 0){
		auto lock = send_mtx.get_scope_lock();
		connection.send((uint8_t)id,data,size);
	}

	virtual void send(cmd id, abmt::blob& blk){
		auto lock = send_mtx.get_scope_lock();
		connection.send((uint8_t)id,blk.data,blk.size);
	}

	virtual void log(std::string msg){
		size_t len = msg.length();
		if(len > 240){
			msg[237]='.';
			msg[238]='.';
			msg[239]='.';
			len = 240;
		}
		send(cmd::log,msg.c_str(),len);
	}

	virtual void log_err(std::string msg){
		size_t len = msg.length();
		if(len > 240){
			msg[237]='.';
			msg[238]='.';
			msg[239]='.';
			len = 240;
		}
		send(cmd::log_err,msg.c_str(),len);
	}

	virtual ~model_adapter(){

	};

}; // class model_adapter

using model_adatper = model_adapter;

} // namespace rt
} // namespace abmt

#endif /* SHARED_ABMT_RT_MODEL_ADAPTER_H_ */
