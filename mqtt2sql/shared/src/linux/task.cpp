/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/task.h>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <fcntl.h>


using namespace std;
using namespace abmt;

void* task::start_task(void* t){
	//pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	task* tsk = (task	*)t;
	tsk->tsk();
	tsk->running = false;
	tsk->on_exit();
	return 0;
}


task::task(std::function<void()> tsk, bool start_now):tsk(tsk){
	if(start_now){
		start();
	}
}

void task::start(){
	if(started == false){
		if(pthread_create(&thread, NULL, &start_task, this)){
			std::cout << "Error starting thread!!!!" << std::endl;
		}else{
			started = true;
			running = true;
		}
	}else{
		cout << "thread already started, you can not start it twice" << endl;
	}
}

bool task::is_running(){
	return running;
}
void task::kill(){
	if(running && started){
		if(pthread_cancel(thread)){
			cout << "error canceling thread" << endl;
		}else{
			running = false;
			cout << "killed" << endl;
		}
	}

}
void task::yield(){
	sched_yield();
}

void task::send_sig(int sig){
	pthread_kill(thread, sig);
}

task::~task(){
	if(running){
		cout << "Thread killed by destructor!!!!! This may lead to memoryleaks..." << endl;
		kill();
		//usleep(10*1000);
	}
}

command_task::command_task(string _command, std::vector<std::string> _args, std::string path_to_execute_in):command(_command),args(_args){
	/**
	 * Todo:
	 * - argv als execve befüllen und als parameter
	 * - ein und ausgabe umleiten
	 * - vllt prioritäten einbauen
	 * - rechere mit processgroups
	 */

	if(std::filesystem::exists(command) == false){
		bool command_found_in_path = false;
		const char* env_path = std::getenv("PATH");
		if(env_path){

			std::stringstream ss(env_path);
			std::string path_element;

			while(std::getline(ss, path_element, ':'))
			{
				if(std::filesystem::exists(path_element + "/" + command)){
					command_found_in_path = true;
					command = path_element + "/" + command;
					break;
				}
			}
		}

		if(command_found_in_path == false){
			cerr << "command_task: command not found (" << command << ")" << endl;
			return;
		}
	}

	if(args.size() > 20){
		cerr << "Error: For command_task only 20 argumends are allowed" << endl;
	}

	//argv = new char* [22]; // one for the command and on for NULL at end

	argv[0] = (char*)command.c_str();
	int i = 1;
	for(auto& str:args){
		(void)(str); // supress unused warning
		argv[i] = (char*) (args[i-1].c_str());
		++i;
	}
	argv[i] = NULL;
	/*
	char* env[] =
	{
		"HOME=/",
		"PATH=/bin:/usr/bin"
		0
	};
	*/

	(void)! pipe(input_pipe_fd);
	(void)! pipe(output_pipe_fd);

	pid = vfork();
	if(pid < 0){
		cout << "fork error !!!" << endl;
		exit(-1);
	}else if( pid == 0){
		// child
		if(path_to_execute_in != ""){
			(void)! chdir(path_to_execute_in.c_str());
		}
		close(output_pipe_fd[0]);
		close(input_pipe_fd[1]);
		dup2(output_pipe_fd[1], 1);
		dup2(output_pipe_fd[1], 2);
		dup2(input_pipe_fd[0], 0);
		//close(output_pipe_fd[0]);


		setsid(); // all subprocesses will be killed when this process is killed
		execve(command.c_str(),argv,environ);
	}else{
		// parrent
		close(output_pipe_fd[1]);
		fcntl(output_pipe_fd[0], F_SETFL, O_NONBLOCK);
		close(input_pipe_fd[0]);
		fcntl(input_pipe_fd[1], F_SETFL, O_NONBLOCK);
	}
}

size_t command_task::read_output(char* buffer, size_t buffer_size){
	int res = read(output_pipe_fd[0], buffer, buffer_size);
	if(res > 0){
		return res;
	}
	return 0;
}

void command_task::write_input(char* buffer, size_t buffer_size){
	write(input_pipe_fd[1], buffer, buffer_size);	
}

bool command_task::is_running(){
	if(waitpid(pid,&exit_status,WNOHANG) == 0){
		return true;
	}else{
		return false;
	}
}

void command_task::kill(){
	::kill(pid *-1, SIGTERM);
}

command_task::~command_task(){
	if(is_running()){
		kill();
	}
	close(output_pipe_fd[0]);
}
