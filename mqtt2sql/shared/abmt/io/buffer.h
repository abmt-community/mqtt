/*
 * io_buffer.h
 *
 *  Created on: 23.11.2021
 *      Author: hva
 */

#ifndef IO_BUFFER_H_
#define IO_BUFFER_H_


#include <stddef.h>
#include <functional>
#include <abmt/blob.h>

namespace abmt{
namespace io{

/// When the send function is called, data is pushed into the buffer and on_new_data
/// is called. The default on_new_data returns zero, so everything stays in the buffer.
/// Use as sink: the source calls send and the default on_new_data function keeps everything
/// the buffer.
/// Use as source: The source overrides the on_new_data function. When you call
/// the buffers send(ptr,size) function, new data is put into the buffer, and the complete
/// buffer given to on_new_date of the source. The used data is then poped out of the buffer.

struct buffer{
	uint8_t* data = 0;
	size_t bytes_used = 0;
	size_t size = 0;

	std::function<size_t(abmt::blob&)> on_new_data = [this](abmt::blob& b)->size_t{
		return 0;
	};

	buffer(size_t init_size = 256);

	buffer(const buffer&) = delete; // pointer ownership

	/// copies data to buffer. Does not call on_new_data
	void push_back(const void* data, size_t data_size);

	void pop_front(size_t nbytes);

	/// Copies data to buffer and calls send();
	void send(const void* data, size_t size);

	/// Calles on_new_data and pops accepted bytes.
	void send();

	/// Sets the buffer size to new value. Old data is copied.
	void set_size(size_t s);

	/// Sets the size to bytes_used.
	void shrink();

	/// Sets bytes_used to 0. Does not shrink the buffer size.
	void flush();

	~buffer();
};

}; // namespace io
}; // namespace abmt

#endif /* IO_BUFFER_H_ */
