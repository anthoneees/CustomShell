# CustomShell
3rd Project from low level programming class, it is a custom shell 
Uses linked lists to run a simple shell that has select commands and can run executables
Can handle inputs from both file input and CLA
Has the available methods:
- cd [directory] - changes directory to given directory
- history - gives all previous commands called
- history[offset] - calls the history command at the given index and runs it again
- history -c - deletes all previous history commands and free all the memory deleted in the linked list
- exit - exits the program and frees all memory in the heap
- if given directory is entered it will assume it is an executable and run the exe 
