#include "fork.h"
#include <ctype.h>
/*
 * Anthony Snider G01327101
 * CS 262, Lab Section 212
 * Project 3 Simple Shell
 */
/*
 *method that inserts user input or file redirection into the linked list included in fork.h
 *takes cmdPoint(index of command), argLen(amount of arguments), arglist(list of arguments), and head(linked list) as arguments
 *0 indicates that no errors were returned while 1 indicates than an error happened
 */
int insertNode(int cmdPoint, int argLen, char **argList, Command *head)
{
	int i, j, argHolder, comHolder;					  // i and j will be used for for loops, argHolder and comHolder are used for memory allocation purposes
	Command *newCommand = calloc(1, sizeof(Command)); // allocate memory for the new node
	if (!newCommand)								  // check if there were any errors allocating memory
	{
		fprintf(stderr, "memory allocation error\n");
		return 1;
	}
	comHolder = strlen(argList[0]) + 1;					   // set comHolder to the length of the argument + 1 to avoid filling up allocated memory
	newCommand->command = calloc(comHolder, sizeof(char)); // allocate memory to command
	if (!(newCommand->command))							   // check if there were any errors allocating memory
	{
		free(newCommand);
		fprintf(stderr, "memory allocation error\n");
		return 1;
	}
	newCommand->arguments = calloc(129, sizeof(char *)); // allocate memory for amount of arguments plus 1
	if (!(newCommand->arguments))						 // check if there were any errors allocating memory and free any memory allocated so far
	{
		free(newCommand->command);
		free(newCommand);
		fprintf(stderr, "memory allocation error\n");
		return 1;
	}
	strcpy(newCommand->command, argList[0]); // copy first index of argument list to the command value
	for (i = 0; i < argLen; i++)			 // loop through all arguments
	{
		argHolder = strlen(argList[i]) + 1;							// set argHolder to length of arguments+1
		newCommand->arguments[i] = calloc(argHolder, sizeof(char)); // allocate memory
		if (!(newCommand->arguments[i]))							// check if there were any errors allocating memory and free any memory allocated so far
		{
			for (j = 0; j < i; j++)
			{
				free(newCommand->arguments[j]);
			}
			free(newCommand->arguments);
			free(newCommand->command);
			free(newCommand);
			fprintf(stderr, "memory allocation error\n");
			return 1;
		}
		strcpy(newCommand->arguments[i], argList[i]); // copy argument list at index i to arguments[i] command at index i
	}
	newCommand->arguments[i] = NULL;	  // set last value of arguments to NULL for executable method later
	newCommand->index = cmdPoint;		  // set index to its current index which is being tracked in main with
	newCommand->argumentslength = argLen; // set argumentLength to arglen

	if (head->next == NULL) // checks if linked list is empty
	{
		head->next = newCommand; // if its empty set head->next to the newCommand node
		return 0;				 // leave method
	}
	else
	{
		newCommand->next = head->next; // if there are other commands set the front of the new node to where head points to
		head->next = newCommand;	   // set the front of head to the back of newCommand
		return 0;
	}
}
/*
 *used only in history to print out all the nodes in the linked list
 *takes in head and count as parameters
 *head is a reference to the linked list and count is the index of the node that is being searched for to print out
 */
void traverseCommand(Command *head, int count)
{
	Command *temp;		 // temp variable that will be used to traverse through the list
	temp = head->next;	 // head is NULL so we need to set temp to the next node
	while (temp != NULL) // while temp still has nodes to search
	{
		if (temp->index == count) // if the index of temps matches the number that we are looking to print
		{
			printf("%d: ", count);							// formatting of the node
			for (int j = 0; j < temp->argumentslength; j++) // loop through arguments
			{
				printf("%s", temp->arguments[j]);	// print the argument
				if (j != temp->argumentslength - 1) // if its not the last index print a space since the tester has no space at the end of the last argument
				{
					printf(" ");
				}
			}
			printf("\n"); // newline
			return;		  // exit the method
		}
		temp = temp->next; // increment temp
	}
}
/*
 *method used to delete the linked list and free all memory in the list
 *takes the head as a reference to the linked list and a flag to know how much we are deleting
 *flag 1 is used in exit and if we reach an EOF to free all memory before we leave our program
 *flag 0 is used in history -c to not fully delete the linked list and keep head so that we can refill the linked list
 */
void deleteHead(Command *head, int flag)
{
	Command *temp; // going to be used to traverse to the linked list
	int i;		   // used for loop
	if (flag == 0) // flag is 0 when history -c is called and we dont want to delete all of the linked list
	{
		temp = head->next;	 // move temp so we dont delete head and can refill it later
		while (temp != NULL) // while temp still has nodes to free
		{
			head->next = temp->next;					// set the next node to temp->next
			temp->next = NULL;							// set point after temp to null
			free(temp->command);						// free command
			for (i = 0; i < temp->argumentslength; i++) // loop through arguments and free each argument**
			{
				free(temp->arguments[i]);
			}
			free(temp->arguments); // free argument*
			free(temp);			   // free temp variable itself
			temp = head->next;	   // increment temp
		}
	}
	if (flag == 1) // if deleteing for exit or EOF
	{
		while (head != NULL) // while head still has nodes
		{
			temp = head;								// set temp to head
			free(temp->command);						// free command
			for (i = 0; i < temp->argumentslength; i++) // loops through arguments and free each argument**
			{
				free(temp->arguments[i]);
			}
			free(temp->arguments); // free argument*
			head = head->next;	   // increment head
			free(temp);			   // free temp itself
		}
	}
}
/*
 *used to read user input or file input and return a single string that contains what we will use later to execute what we want
 *only takes in head so that it is freed in case anything goes wrong while being in this method
 *returns a single string containing the inputs
 */
char *readLine(Command *head)
{
	int size = 1000, count = 0, current;	 // size is an aribtrary large number that will be used for memory allocation, count will be used to keep track of amount of characters so we can realloc if needed, current will hold the current characer
	char *buff = calloc(size, sizeof(char)); // buff will be the string that is returned at the end which is set to some arbitrary number but will be checked after each character is added to make sure memory never runs out
	if (!buff)								 // check if there were any errors allocating memory
	{
		fprintf(stderr, "memory allocation error\n");
		return NULL;
	}
	while (1) // infinite loop as we will make sure to exit when needed
	{
		current = getchar(); // set current to the current character in the stream
		if (current == '\n') // if the user hit enter
		{
			if (count == 0) // if they hit enter without entering any other character
			{
				free(buff);	 // free buff
				return NULL; // return NULL
			}
			buff[count] = '\0'; // set the last character to a null character
			return buff;		// return buff
		}
		else if (current == EOF) // will only be passed in input file redirection when end of file is reached
		{
			if (count == 0) // if the stream is at an EOF
			{
				deleteHead(head, 1); // free linked list
				free(buff);			 // free buff
				exit(0);			 // exit
			}
			buff[count] = '\0'; // set last index in buff to null character
			return buff;		// return
		}
		else // else theres more to read
		{
			buff[count] = current; // select string index to current character
		}
		count++;		   // increment count to keep track of memory
		if (count >= size) // if memory is run out
		{
			size *= 2;					// double size
			buff = realloc(buff, size); // realloc
			if (!buff)					// check if there were any errors allocating memory
			{
				fprintf(stderr, "memory allocation error\n");
				return NULL;
			}
		}
	}
}
/*
 *used to separate string from readLine into different indexes in a 2d character array
 *takes in single string which will be split
 *returns a 2d character array
 */
char **parseLine(char *buff)
{
	char *temp, **args = calloc(129, sizeof(char *)); // temp which will be used for strtok splitting, and args which will be returned at end of method, callocing to 129 since if 128 is input program will exit
	int count = 0;									  // count will be used to keep track of amount of arguments inputted
	if (!args)										  // check if there were any errors allocating memory
	{
		fprintf(stderr, "memory allocation error\n");
		return NULL;
	}
	temp = strtok(buff, " "); // strtok which separates the arguments for us
	while (temp != NULL)	  // while temp has more arguments
	{
		args[count] = temp; // set args at index count to where temp is at currently
		count++;			// increment count
		if (count > 128)	// if more than 128 arguments including the command/path is input then free args, print an error, and exit the method
		{
			free(args);
			fprintf(stderr, "error: too many arguments\n");
			return NULL;
		}
		temp = strtok(NULL, " "); // increments the while loop
	}
	args[count] = NULL; // set last argument to NULL
	return args;		// return args
}
/*
 *used to change directory takes in head which holds the command of the most recent node
 */
void changeDir(Command *head)
{
	char currentDir[500];						  // will hold the current directory
	getcwd(currentDir, sizeof(currentDir));		  // set the current directory to cwd
	strcat(currentDir, head->next->arguments[1]); // concatenate the current directory to the entered argument
	chdir(currentDir);							  // method that changes directory
	if (chdir(head->next->arguments[1]) == -1)	  // if there was an error print the error statement givenn
	{
		fprintf(stderr, "error: %s\n", strerror(errno));
	}
}
/*
 *where the shell is actually implemented
 *creates infinite loop and runs executable if path is given or a command if a valid one is given
 */
int main()
{
	Command *head = calloc(1, sizeof(Command));			// head is the front node that we will link other nodes to
	char *buff, **args, numTest;						// buff will be the string taken from readLine, args will be the 2d array from parse, and cwd is the string containing the current directory, numTest is used to check if an argument is an integer
	int count, cmdCount = 0, ret, i, j, flag, offIndex; // count keeps track of argument amount, cmdCount keep track of how many commands have been added, ret is the exe method return value, flag is used to check if the argument in history is a number, offIndex will hold the string casted to an integer
	if (!head)											// check if there were any errors allocating memory
	{
		fprintf(stderr, "memory allocation error\n");
		exit(1); // exit program because we ran into memory allocation error before loop even started
	}
	head->next = NULL; // set the head->next to null since it represents the end of the linked list
	while (1)		   // infinite loop that the program will execute
	{
		flag = 1;
		i = 0;
		count = 0;			   // reset all values to their base numbers (flag,i, and count)
		printf("262$");		   // shell print line
		fflush(stdout);		   /// flush the stream to make sure print statement executes
		buff = readLine(head); // set buff to the string returned from readLine
		if (buff == NULL)	   // check to make sure readLine worked correctly
		{
			continue;
		}
		args = parseLine(buff); // sets args to the 2d array returned from parseLine
		if (args == NULL)		// make sure parseLine worked correctly
		{
			free(buff);
			continue;
		}
		while (args[i] != NULL) // count the amount of arguments so that we cant keep track of it later
		{
			count++;
			i++;
		}
		if (strcmp(args[0], "history") != 0) // since we dont want to add history to the linked list only insert nodes that dont contain history
		{
			if (insertNode(cmdCount, count, args, head) == 0) // if there were no errors in insert node increment count
			{
				cmdCount++;
			}
			else // if there was an error just reset the loop
			{
				free(buff);
				free(args);
				continue;
			}
		}
		if (strcmp(args[0], "exit") == 0) // if the command is exit
		{
			if (count == 1)
			{						 // make sure no other arguments were added
				deleteHead(head, 1); // free all memory including args and buff
				free(args);
				free(buff);
				exit(0); // exit program
			}
			else
			{ // if other arguments were added restart the loop
				free(buff);
				free(args);
				continue;
			}
		}
		else if (strcmp(args[0], "cd") == 0) // if cd is the command entered
		{
			changeDir(head); // change directory method
			free(args);		 // free buff and args then continue to next iteration of the infinite loop
			free(buff);
			continue;
		}
		if (strcmp(args[0], "history") == 0) // if command given is history we have to check for if no arguments were entered, if -c was entered, or if a number was entered
		{
			if (count == 1) // if there were no extra arguments given
			{
				for (i = 0; i < cmdCount; i++) // loop through the amount of commands we've added
				{
					traverseCommand(head, i); // print out the index that we are looking for
				}
				free(args); // free args and buff then continue
				free(buff);
				continue;
			}
			else if (count == 2) // if there are other arguments then its either -c or a number
			{
				for (i = 0; i < strlen(args[1]); i++) // loop through every character in the first argument
				{
					numTest = args[1][i];	   // set numTest to the character
					if (isdigit(numTest) == 0) // if its not a digit set flag to 0
					{
						flag = 0;
					}
				}
				if (strcmp(args[1], "-c") == 0) // if the argument is -c
				{
					deleteHead(head, 0); // delete list with the 0 num to make sure we dont delete head
					cmdCount = 0;		 // set command counter back to 0 since we have no commands
					free(args);			 // free args and buff then continue
					free(buff);
					continue;
				}
				else if (flag == 1) // if there were all digits in the argument string
				{
					offIndex = atoi(args[1]);						   // since we know the argument entered is an integer, atoi will convert it to an int type
					Command *temp, *dupe = calloc(1, sizeof(Command)); // dupe is the duplicated command that will be entered to the front of the linked list
					if (!dupe)										   // check if there were any errors allocating memory
					{
						free(args);
						free(buff);
						fprintf(stderr, "memory allocation error\n");
						continue;
					}
					temp = head->next;		  // since head is null set temp to head-> next
					if (offIndex >= cmdCount) // make sure the integer numbered is an index of a command in the linked list and if its not restart the loop
					{
						free(args);
						free(buff);
						free(dupe);
						continue;
					}
					while (temp != NULL) // while temp isnt null
					{
						if (temp->index == offIndex) // if the number entered is found in the index of one of the commands
						{
							dupe->arguments = calloc(temp->argumentslength + 1, sizeof(char *)); // allocate memory to everything that needs to be allocated
							if (!(dupe->arguments))												 // check if there were any errors allocating memory
							{
								free(dupe);
								free(args);
								free(buff);
								fprintf(stderr, "memory allocation error\n");
								continue;
							}
							for (i = 0; i < temp->argumentslength; i++) // loop through all arguments
							{
								dupe->arguments[i] = calloc(strlen(temp->arguments[i]) + 1, sizeof(char)); // allocate memory
								if (!(dupe->arguments[i]))												   // check if there were any errors allocating memory and free any memory
								{
									for (j = 0; j < i; j++)
									{
										free(dupe->arguments[j]);
									}
									free(dupe->arguments);
									free(dupe);
									free(args);
									free(buff);
									fprintf(stderr, "memory allocation error\n");
									continue;
								}
								strcpy(dupe->arguments[i], temp->arguments[i]); // copy each argument into the new Command node
							}
							dupe->index = cmdCount;											 // set the index to the latest count
							dupe->argumentslength = temp->argumentslength;					 // set the argument length to the same
							dupe->command = calloc(strlen(temp->command) + 1, sizeof(char)); // allocate memory
							if (!(dupe->command))											 // check if there were any errors allocating memory
							{
								for (i = 0; i < dupe->argumentslength; i++)
								{
									free(dupe->arguments[i]);
								}
								free(dupe->arguments);
								free(dupe);
								free(args);
								free(buff);
								fprintf(stderr, "memory allocation error\n");
								continue;
							}
							strcpy(dupe->command, temp->command); // copy the command from temp to dupe
						}
						temp = temp->next; // increment temp
					}
					dupe->next = head->next;			  // link the front of dupe to where head points to
					head->next = dupe;					  // set head to the back of dupe
					cmdCount++;							  // incremenet command counter since we just added something to the linked list
					if (strcmp(dupe->command, "cd") == 0) // since exit leaves the program and history is not added to the linked list the only cases we have to check for are cd and references to executables or a random input
					{
						changeDir(head); // change directory
						free(args);		 // free args,buff and continue to the next iteration of the infinite loop
						free(buff);
						continue;
					}
					else if (strstr(dupe->command, "/bin/") != NULL) // if it contains /bin/ its a reference to an executable
					{
						ret = call_exe(dupe); // run the command again
						if (ret == 2)
						{
							fprintf(stderr, "error: %s\n", strerror(ret)); // error print statement
						}
						free(args); // free args,buff and continue to the next iteration of the infinite loop
						free(buff);
						continue;
					}
					else // else its a random input
					{
						free(args); // free args,buff and continue to the next iteration of the infinite loop
						free(buff);
						continue;
					}
				}
				else // else it wasnt a valid argument (not -c or a number or none) so go to the next iteration of the loop
				{
					free(args);
					free(buff);
					continue;
				}
			}
		}
		else if (strstr(args[0], "/bin/") != NULL) // check if its a directory(was told to assume /bin/ denotes a direcotry)
		{
			ret = call_exe(head->next); // ret is the return number from call_exe
			if (ret == 2)				// if return is 2 then print the error statement given
			{
				fprintf(stderr, "error: %s\n", strerror(ret));
			}
			free(args); // free args and buff then continue
			free(buff);
			continue;
		}
		else // if its not a command, or executable free memory and just iterate back through the loop
		{
			free(buff);
			free(args);
			continue;
		}
	}
}
