#include "fork.h"

// this function performs the process control aspect of the project. it accepts a single argument being the command to execute which is a reference to an executable

// the arguments variable of the struct must be NULL terminated and it the argument at the 0th index must also point to the reference to the executable i.e the command

int call_exe(Command * command){
	int pid = fork();
        if(pid == 0){
	        execvp(command->command, command->arguments);
		exit(errno);
		}
        else {
		int status;
		wait(&status);
		if(WIFEXITED(status)){
			return WEXITSTATUS(status); // returns 0 if execvp succeeded and 2 if execvp failed 
		}

		return -1;
	}
}
