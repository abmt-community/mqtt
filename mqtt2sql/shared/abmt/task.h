/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef UTIL_INC_TASK_H_
#define UTIL_INC_TASK_H_


#include <functional>
#include <pthread.h>
#include <sys/wait.h>
#include <string>
#include <vector>

namespace abmt{
/**
 * Achtung!!! nicht benutzen wenn gekillt werden soll, und das Programm danach weiterlaufen muss.
 * Hintergrund: Es kann nicht festgestellt werden welche Ressourcen allokiert wurden. Somit können
 * diese auch nicht freigegeben werden....
 */
class task{

private:
	pthread_t thread = {};
	std::function<void()> tsk;
	bool started = false;
	bool running = false;

public:

	std::function<void()> on_exit = []{}; // will run in thread_context; not after kill!

	task(std::function<void()> tsk, bool start_now = true);

	void start();
	bool is_running();
	void kill();
	void yield();
	void send_sig(int sig);

	virtual ~task();

private:
	static void* start_task(void* t);

};


/**
 * Damit der Subprozess auch mithilfe des Destruktors
 * gekillt werden kann, muss das programm ordentlich
 * beendet werden. Ohne Signalhandler bricht STRG+C
 * Das Programm ohne Destruktoraufruf ab...
 */

class command_task{
public:

	pid_t pid = {};
	int exit_status = 0; // contains the exitstatus after execution

	int input_pipe_fd[2];
	int output_pipe_fd[2];

	command_task(std::string command, std::vector<std::string>args = {}, std::string path_to_execute_in = ""); // maxium 20 arguments...

	bool is_running();
	void kill();

	size_t read_output(char* buffer, size_t buffer_size); // returns number of bytes read to buffer
    void   write_input(char* buffer, size_t buffer_size);

	~command_task();

private:
	std::string command;
	std::vector<std::string> args;
	char* argv[22];
};

} // namespace abmt

#endif /* UTIL_INC_TASK_H_ */
