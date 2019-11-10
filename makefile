all: compile
	echo "done"

compile: bug_shell.c
	cc -o bug_shell bug_shell.c
clean: 
	rm bug_shell
