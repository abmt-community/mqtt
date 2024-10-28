/**
 * Author: Hendrik van Arragon, 2022
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_RT_OBSERVER_ADAPTER_H_
#define SHARED_ABMT_RT_OBSERVER_ADAPTER_H_

#include <abmt/rt/com.h>
#include <abmt/io/protocols/s2p.h>
#include <abmt/io/eio.h>
#include <abmt/version.h>
#include <abmt/os.h>

namespace abmt{
namespace rt{

class observer_adapter{
public:

	/**
	 * Interface:
	 */

	abmt::io::s2p s2p_connection;

	virtual void on_model_online(abmt::blob& blk){

	};

	virtual void on_ack_signal_def(abmt::blob& p){

	};

	virtual void on_ack_parameter_def(abmt::blob& p){

	};

	virtual void on_ack_set_daq_list(){

	};

	virtual void on_ack_set_paq_list(){

	};

	virtual void on_ack_set_data(){

	};

	virtual void on_daq_data(abmt::blob& blk){

	};

	virtual void on_paq_data(abmt::blob& blk){

	};

	virtual void on_log(std::string s){
		abmt::log(s);
	};

	virtual void on_log_err(std::string s){
		abmt::log("error: " + s);
	};

	virtual void send_hello(){
		uint16_t version = ABMT_VERSION;
		send(cmd::hello, &version, sizeof(version));
	}

	virtual void send_request_signal_def(){
		send(cmd::request_signal_def,0,0);
	}

	virtual void send_request_parameter_def(){
		send(cmd::request_parameter_def,0,0);
	}

	void send_daq_list(abmt::blob& lst){
		send(cmd::set_daq_list,lst.data,lst.size);
	}

	void send_paq_list(abmt::blob& lst){
		send(cmd::set_paq_list,lst.data,lst.size);
	}

	void send_parameter(abmt::blob& data){
		send(cmd::set_prameter,data.data,data.size);
	}

	void send_command(std::string command){
		send(cmd::command,command.c_str(),command.size());
	}

	/**
	 * Implementation
	 */
	observer_adapter(){
		s2p_connection.on_new_pack = [this](size_t id, void* data, size_t size){
			abmt::blob blk(data,size);
			handle_data(id,blk);
		};
	}

	void set_connection(abmt::io::tcp_ptr tcp){
		s2p_connection.set_sink(tcp);
		s2p_connection.set_source(tcp);
		tcp->on_close = [this]{
			on_disconnect();
		};
	}

	virtual void on_disconnect(){
		s2p_connection.set_sink(io::sink());
		s2p_connection.set_source(io::source());
	}


	void handle_data(size_t id, abmt::blob& blk){
		cmd c = (cmd)id;
		switch(c){
		case cmd::model_online:
			on_model_online(blk);
			break;
		case cmd::ack_signal_def:
			on_ack_signal_def(blk);
			break;
		case cmd::ack_parameter_def:
			on_ack_parameter_def(blk);
			break;
		case cmd::ack_set_daq_list:
			on_ack_set_daq_list();
			break;
		case cmd::ack_set_paq_list:
			on_ack_set_paq_list();
			break;
		case cmd::daq_data:
			on_daq_data(blk);
			break;
		case cmd::paq_data:
			on_paq_data(blk);
			break;
		case cmd::log:
			on_log(blk.str());
			break;
		case cmd::log_err:
			on_log_err(blk.str());
			break;
		default:
			return;
		}
	}

	virtual void send(cmd id, const void* data, uint32_t size){
		s2p_connection.send((uint8_t)id,data,size);
	}

	virtual void send(cmd id, abmt::blob& blk){
		s2p_connection.send((uint8_t)id,blk.data,blk.size);
	}

	virtual ~observer_adapter(){

	};

}; // observer_adapter

} // namespace rt
} // namespace abmt


#endif /* SHARED_ABMT_RT_OBSERVER_ADAPTER_H_ */
