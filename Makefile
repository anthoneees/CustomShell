#Anthony Snider
#CS 262, Lab Section 212
#Project 3 Makefile
cs262sh: shell.o fork.o
	gcc -o cs262sh shell.o fork.o -g -Wall
shell.o: shell.c fork.h
	gcc -c shell.c
fork.o: fork.c fork.h
	gcc -c fork.c
clean:
	rm cs262sh
	rm *.o
