/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <abmt/io/eio.h>
#include <abmt/os.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <exception>
#include <cerrno>
#include <cstring>
#include <clocale>
#include <unistd.h>
#include <fcntl.h>
#include <execinfo.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <netdb.h>
#include <sstream>

using namespace std;

namespace abmt::io{

event_list::event_list(){
	list_fd = epoll_create1(0); // epoll_create1 = moderner; todo: O_CLOEXEC nochmal püfen
}

event_list::~event_list(){
	if(list_fd > 0){
		close(list_fd);
	}
}

void event_list::add(event& e){
	epoll_ctl(list_fd, EPOLL_CTL_ADD, e.fd, &(e.ev));
}
void event_list::remove(event& e){
	epoll_ctl(list_fd, EPOLL_CTL_DEL, e.fd, &(e.ev));
	if(e.fd > 0){
		close(e.fd);
		//cout << "close remove: " << e.fd << endl;
		e.fd = 0;
	}
	break_loop = true;
}
void event_list::modify(event& e){
	epoll_ctl(list_fd, EPOLL_CTL_MOD, e.fd, &(e.ev));
}

void event_list::wait(uint64_t timeout_ms){
	epoll_event events[5];
	int num = epoll_wait(list_fd, events, ((sizeof events) / (sizeof events[0])), timeout_ms);
	for (int i = 0; i < num; i++) {
		event* e = (event*) events[i].data.ptr;
		if( events[i].events & EPOLLHUP ) {
			remove(*e);
			e->epoll_hup();
		}
		if(break_loop){
			break_loop = false;
			break;
		}
		if( events[i].events & EPOLLIN ) {
			e->epoll_in();
		}
		if(break_loop){
			break_loop = false;
			break;
		}
		if( events[i].events & EPOLLOUT ) {
			e->epoll_out();
		}
	}
}

event::event(event_list& lst): lst(lst){
	fd = -1;
	ev.data.ptr = this;
	ev.events = EPOLLIN;
	epoll_in  = [this]{ abmt::die("event: event.on_event_in not overwriten..."); };
	epoll_out = [this]{ abmt::die("event: event.on_event_out not overwriten..."); };
	epoll_hup = [this]{ abmt::die("event: event.on_event_hup not overwriten..."); };
}

event::~event(){
	lst.remove(*this);
	if(fd > 0){
		close(fd);
		fd = 0;
		//cout << "close destruct: " << fd << endl;
	}
}

timer::timer(event_list& lst, size_t interval_ms, std::function<void()> oe):event(lst){
	on_expiraton = oe;
	epoll_in = [this]{
		uint64_t exp_count;
		(void)! ::read(this->fd,&exp_count,sizeof(exp_count)); // es muss gelesen werden, damit epoll_in gelöscht wird
		if(exp_count > 1){
			// one or more expirations missed....
		}
		on_expiraton();
	};
	fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	set_interval_ms(interval_ms);
	lst.add(*this);
}

uint64_t timer::get_interval_ms(){
	return interval_ns;
}

void timer::set_interval_ms(uint64_t ms){
	interval_ns = ms*1000*1000;
	itimerspec val;
	val.it_value.tv_sec = interval_ns/(1000UL*1000UL*1000UL);
	val.it_value.tv_nsec = interval_ns - val.it_value.tv_sec*1000UL*1000UL*1000UL;
	val.it_interval.tv_sec = interval_ns/(1000UL*1000UL*1000UL);
	val.it_interval.tv_nsec = interval_ns - val.it_value.tv_sec*1000UL*1000UL*1000UL;
	timerfd_settime(fd, 0, &val, 0);
}

void timer::stop(){
	set_interval_ms(0);
}


ip_addr::ip_addr(in_addr adr):adr(adr){

};

ip_addr::ip_addr(in_addr_t ip){
	adr.s_addr = ip;
};


ip_addr::ip_addr(std::string str){
	auto res = resolve(str);
	if(res.size() == 0){
		abmt::die(string("error resolving '") + str + "': "  + std::strerror(errno));
	}
	*this = res[0];
	/*
	int ret = inet_pton(AF_INET, str.c_str(), &adr);
	if (ret == 0){
		abmt::die("wrong ip_address format");
	}else if (ret < 0){
		abmt::die(string("ip_address conversion: ") + std::strerror(errno));
	}
	*/
};

ip_addr::ip_addr(const char* s):ip_addr(string(s)){

}

std::string ip_addr::str(){
	char buff[100];
	inet_ntop(AF_INET, &adr.s_addr, buff, sizeof(buff));
	return buff;
}

std::vector<abmt::io::ip_addr> resolve(std::string name){
	std::vector<abmt::io::ip_addr> return_result;
	if(name == ""){
		name = "localhost";
	}
	if( name == "any" || name == "INADDR_ANY" || name == "inaddr_any"){
		return_result.push_back({INADDR_ANY});
		return return_result;
	}
	addrinfo* result;
	addrinfo request_def = {};
	request_def.ai_family = AF_INET;
	request_def.ai_protocol = SOCK_STREAM;
	int res = getaddrinfo(name.c_str(), NULL, &request_def, &result);
	if(res != 0){
		return return_result;
	}
	for(auto r = result; r != NULL; r = r->ai_next){
		auto adr = (struct sockaddr_in *)r->ai_addr;
		return_result.push_back({adr->sin_addr});
	}
	freeaddrinfo(result);
	return return_result;
}

output_buffer::output_buffer(const void* src_data_to_copy, size_t data_size){
	data = new char[data_size];
	size = data_size;
	offset = 0;
	memcpy(data,src_data_to_copy,size);
}


output_buffer::~output_buffer(){
	delete[] data;
}

io_base::io_base(event_list& lst_tmp, size_t s):event(lst_tmp){
	in_buffer = new char[s];
	in_buffer_size = s;
	epoll_in = [this]{

		if(in_buffer == 0){
			abmt::die("io_base: no_input_buffer set, but data received!");
		}else{

			int num_bytes_read = ::read(fd,(char*)in_buffer+bytes_read,in_buffer_size-bytes_read);

			if( num_bytes_read < 0 ) {
				abmt::die( string("io_base rcv_error:")  + std::strerror(errno) );
			}

			if(num_bytes_read == 0){
				lst.remove(*this);
				epoll_hup();
				return;
			}

			bytes_read += num_bytes_read;

			abmt::blob b(in_buffer, bytes_read);
			size_t bytes_to_pop = on_new_data(b);
			bytes_read -= bytes_to_pop;
			memmove((char*)in_buffer,(char*)in_buffer+bytes_to_pop,bytes_read);

			if(in_buffer_size == bytes_read){
				io_base::set_buffer_size(in_buffer_size*1.2);
			}

		} // else buffer == null
	};
}

void io_base::set_buffer_size(size_t s){
	auto new_in_buffer = new char[s];
	auto size = bytes_read;
	if(s < size){
		size = s;
	}
	memcpy(new_in_buffer,in_buffer, size);
	delete[] in_buffer;
	in_buffer = new_in_buffer;
	in_buffer_size = s;
}

void io_base::send(abmt::blob& b){
	send(b.data, b.size);
}

void io_base::send(abmt::blob_shared& b){
	send(b.data, b.size);
}

io_base::~io_base(){
	delete[] in_buffer;
}

udp::udp(event_list& lst, end_point dst):io_base(lst),dst(dst){
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if( fd < 0 ) {
		abmt::die("error creating udp socket");
	}
	lst.add(*this);
}

udp::udp(event_list& lst, end_point dst, end_point src):udp(lst,dst){
	// Setting src-port
	int on = 1;
	int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if( ret != 0 ) {
		abmt::die("reuse address faild");
	}
	sockaddr_in target = src;
	ret = ::bind(fd, (sockaddr*) &target, sizeof(target));
	if( ret < 0 ) {
		abmt::die("error binding socket");
	}
}

void udp::send_to(const void* data, size_t len, end_point dst){
	sockaddr_in target = dst;
	char str[100] = {};
	inet_ntop(AF_INET, &(target.sin_addr), str, INET_ADDRSTRLEN);
	int ret = ::sendto(fd, data, len, 0, (const sockaddr*)&target, sizeof(target));
	if( ret == -1 ) {
		abmt::die(string("send_error:")  + std::strerror(errno) );
	}
}

void udp::send(const void* data, size_t len){
	send_to(data,len,dst);
}



tcp_ptr tcp::connect_to(event_list& lst, end_point dst){
	tcp_ptr res(new tcp(lst));
	res->fd = socket(AF_INET, SOCK_STREAM, 0);
	res->setup_fd();
	res->connect(dst);
	return res;
}

tcp_ptr tcp::connect(event_list& lst, end_point src, end_point dst){
	tcp_ptr res(new tcp(lst));
	res->fd = socket(AF_INET, SOCK_STREAM, 0);
	res->setup_fd();
	res->bind(src);
	res->connect(dst);
	return res;
}

tcp::tcp(event_list& lst):io_base(lst){
	on_open = []{};
	on_ready_to_send = []{};
	on_new_data = [](abmt::blob& b)->size_t{ return b.size;};
	on_close = []{};

	epoll_hup = [this]{
		connected = false;
		on_close();
	};

	epoll_out = [this]{
		if(connected == false){
			on_open();
			connected = true;
		}

		if(out_data_que.size() > 0){
			while(out_data_que.size() > 0){
				int ret = ::send(fd, out_data_que.front().data + out_data_que.front().offset, out_data_que.front().size - out_data_que.front().offset, MSG_NOSIGNAL);
				if( ret < 0 ) {
					if(errno == EAGAIN){
						break;
					}else{
						close();
						//abmt::die(string("tcp::send: ")  + std::strerror(errno) );
					}
				}
				if(ret != (int) (out_data_que.front().size - out_data_que.front().offset)){
					out_data_que.front().offset += ret;
					break;
				}else{
					out_data_que.pop_front();
				}
			}
			ev.events =  EPOLLIN | EPOLLOUT;
			this->lst.modify(*this);
			return;
		}

		ev.events = EPOLLIN;
		this->lst.modify(*this);

		on_ready_to_send();
	};// epoll out
}

void tcp::send(const void* data, size_t len){
	if(out_data_que.size() > 0){
		out_data_que.emplace_back(data,len);
		return;
	}
	int ret = ::send(fd, data, len, MSG_NOSIGNAL); // MSG_NOSIGNAL no error on closed socket
	if( ret < 0 ) {
		if(errno == EAGAIN){
			out_data_que.emplace_back(data,len);
		}else{
			if(errno == 104){ // reset by peer
				close();
			}else if(errno == 32){ // broken pipe
				close();
			}else if(errno == 88){
				// socked closed
			}else{
				abmt::die(string("error tcp::send [") + to_string(errno) + "]: "  + std::strerror(errno) );
			}
		}
	}
	if(ret > 0 && ret < (int)len){
		const char* data_char = (const char*) data;
		out_data_que.emplace_back((data_char + ret),len-ret);
	}
	ev.events =  EPOLLIN| EPOLLOUT;
	lst.modify(*this);
}

void tcp::setup_fd(){
	fcntl(fd, F_SETFL, O_NONBLOCK);
	int set = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *) &set, sizeof(set));
	//setsockopt(fd, IPPROTO_TCP, TCP_QUICKACK, (void *) &set, sizeof(set));

	ev.events = EPOLLIN | EPOLLOUT;
	lst.add(*this);
}

void tcp::bind(end_point e){

	int on = 1;
	int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if( ret != 0 ) {
		abmt::die(string("tcp::bind: error setting reuse_addr")  + std::strerror(errno) );
	}
	sockaddr_in target = e;
	ret = ::bind(fd, (struct sockaddr *) &target, sizeof(target));
	if( ret != 0 ) {
		abmt::die(string("tcp::bind: bind_error: ")  + std::strerror(errno) );
	}
}

void tcp::connect(end_point e){
	sockaddr_in target = e;
	int ret = ::connect(fd, (sockaddr*) &target, sizeof(target));
	if( ret < 0 && errno != EINPROGRESS ) {
		abmt::die(string("tcp::connect: ")  + std::strerror(errno) );
	}
}

void tcp::close(){
	on_close();
	lst.remove(*this);
	connected = false;
}


tcp_server::tcp_server(event_list& lst_tmp, end_point p):event(lst_tmp){
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if( fd < 0 ) {
		abmt::die(string("tcp_server::connect: ")  + std::strerror(errno) );
	}

	fcntl(fd, F_SETFL, O_NONBLOCK);


	sockaddr_in target = p;
	int on = 1;
	int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if( ret != 0 ) {
		abmt::die(string("tcp_server::connect: Address-reuse failed: ")  + std::strerror(errno) );
	}
	ret = ::bind(fd, (struct sockaddr *) &target, sizeof(target));
	if( ret != 0 ) {
		abmt::die(string("tcp_server::connect: Error binding to port ") + to_string(p.port) + ": " + std::strerror(errno) );

	}
	ret = listen(fd, 100);
	if( ret != 0 ) {
		abmt::die(string("tcp_server::connect: Error after listen... ") + std::strerror(errno) );
	}

	epoll_in  = [this]{
		struct sockaddr_in in_addr;
		socklen_t in_len = sizeof(in_addr);
		int new_fd = 0;
		while(new_fd != -1) {
			new_fd = ::accept(this->fd,(sockaddr*)&in_addr,&in_len);
			if(new_fd != -1) {
				tcp_ptr res(new tcp(lst));
				res->fd = new_fd;
				res->setup_fd();
				res->connected = true;
				on_new_connection(res);
			} else {
				if(errno == EAGAIN || errno == EWOULDBLOCK) {
					// ok
				} else {
					abmt::die(string("tcp_server::epoll_in: ")  + std::strerror(errno) );
				}
			}
		}
	};

	lst.add(*this);
}

serial::serial(event_list& lst, std::string device, int baudrate, bool raw):io_base(lst){
	if(device != ""){
		open(device,baudrate, raw);
	}
}

void serial::open(std::string device,int baudrate, bool raw){
	if(fd != -1){
		lst.remove(*this);
		::close(fd);
	}
	fd = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd == -1){
		abmt::die(string("unable to open device " ) + device + ": " + std::strerror(errno) );
	}
	set_baudrate(baudrate);
	if(raw){
		enable_raw_mode();
	}
	epoll_out = [this]{
		if(out_data_que.size() > 0){
			while(out_data_que.size() > 0){
				int ret = write(fd, out_data_que.front().data + out_data_que.front().offset, out_data_que.front().size - out_data_que.front().offset);
				if( ret < 0 ) {
					if(errno == EAGAIN){
						break;
					}
				}
				if(ret != (int) (out_data_que.front().size - out_data_que.front().offset)){
					out_data_que.front().offset += ret;
					break;
				}else{
					out_data_que.pop_front();
				}
			}
			ev.events =  EPOLLIN | EPOLLOUT;
			this->lst.modify(*this);
			return;
		}

		ev.events = EPOLLIN;
		this->lst.modify(*this);
	}; // epoll out

	lst.add(*this);
}

void serial::set_baudrate(int baudrate){
    termios tio;
    int ret;
    ret = ioctl(fd, TCGETS, &tio);
    if(ret != 0){
    	abmt::die(string("unable to set baudrate (TCGETS): " ) + ": " + std::strerror(errno) );
    }
    cfsetspeed(&tio, baudrate);
    ret = ioctl(fd, TCSETS, &tio);
    if(ret != 0){
		abmt::die(string("unable to set baudrate (TCSETS): " ) + ": " + std::strerror(errno) );
	}
}

void serial::enable_raw_mode(){
    termios tio;
    int ret;
    ret = ioctl(fd, TCGETS, &tio);
    if(ret != 0){
    	abmt::die(string("unable to set baudrate (TCGETS): " ) + ": " + std::strerror(errno) );
    }
    tio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tio.c_oflag &= ~OPOST;
    tio.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tio.c_cflag &= ~(CSIZE | PARENB);
    tio.c_cflag |= CS8;
    ret = ioctl(fd, TCSETS, &tio);
    if(ret != 0){
		abmt::die(string("unable to set baudrate (TCSETS): " ) + ": " + std::strerror(errno) );
	}
}

void serial::send(const void* data, size_t len){
	if(out_data_que.size() > 0){
		out_data_que.emplace_back(data,len);
		return;
	}
	int ret = write(fd,data,len);
	if( ret < 0 ) {
		if(errno == EAGAIN){
			out_data_que.emplace_back(data,len);
		}else{
			abmt::die(string("error serial::send [") + to_string(errno) + "]: "  + std::strerror(errno) );
		}
	}
	if(ret > 0 && ret < (int)len){
		const char* data_char = (const char*) data;
		out_data_que.emplace_back((data_char + ret),len-ret);
	}
	ev.events =  EPOLLIN| EPOLLOUT;
	lst.modify(*this);
}


} // namespace eio
