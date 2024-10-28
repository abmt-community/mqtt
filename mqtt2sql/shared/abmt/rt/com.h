/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef SHARED_ABMT_RT_COM_H_
#define SHARED_ABMT_RT_COM_H_

#include <cstdint>

namespace abmt{
namespace rt{

/**
 * Concept
 * =======
 * The communication between abmt and a model-instance is done via adapters.
 * The modelside is handled with the model_adapter. The observer via the observer_adapter.
 * Both base-adapters map the incoming requests to their virtual interface-functions.
 * This abstraction is done to allow different dataacquisition-implementations.
 * Embedded systems have special needs here.
 *
 * Communication
 * -------------
 * Model- and observer-side receive the others side events with an on_-prefix. Sending requests
 * can be done with a send_-prefix method.
 *
 */


enum class cmd: uint8_t{
	// observer to model (requests)
	hello = 0x00,
	request_signal_def,
	request_parameter_def,
	set_daq_list,           // set list and list_number
	set_paq_list,
	set_prameter,
	command,


	// model to observer (responses)
	model_online = 0x10,
	ack_signal_def,
	ack_parameter_def,
	ack_set_daq_list,
	ack_set_paq_list,


	// continuous messages to observer
	daq_data  = 0x20,
	paq_data,

	//other
	log = 0x30,
	log_err

};

#pragma pack(push, 1)

struct daq_data_hdr{
	uint32_t list_idx;
	uint32_t size;
	int64_t  time;
};

struct paq_data_hdr{
	uint32_t list_idx;
	uint32_t size;
};

#pragma pack(pop)


} // namespace rt
} // namespace abmt


#endif /* RUNTIME_INTERFACE_MODEL_COM_H_ */
