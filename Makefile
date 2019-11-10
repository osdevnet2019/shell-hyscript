all: bug_shell

bug_shell: bug_shell.o
	cc -g -o bug_shell.o -o bug_shell

bug_shell.o: bug_shell.c
	cc -g -c bug_shell.c

clean:
	rm -rf *o bug_shell