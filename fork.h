#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>

#ifndef FORK_H
#define FORK_H

typedef struct Command{
	char * command; // this variables contains the command 
	char ** arguments; // this variable contains the command line input where arguments[0] would be the command and arguments[1] would argument 1, etc. AFter the last argument, you must insert the NULL pointer!	
	struct Command * next; // this variables represent the next command in the list	
	int index; // this variable represen the order in which the command was entered
	int argumentslength; // this variable indicates the length of the arguments array
}Command;

int call_exe(Command *);

#endif
