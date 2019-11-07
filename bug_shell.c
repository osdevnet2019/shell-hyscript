#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <jmorecfg.h>
#include <string.h>
#include <zconf.h>
#include <wait.h>
#include <pwd.h>

size_t BUFSIZE = 1024; // be careful with this variable before changing its value



const char *getUserName()
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw)
    {
        return pw->pw_name;
    }

    return "";
}


void you_are_here(uid_t usr_type) {

    char cwd[2048];
    printf("%s", getUserName());
    char home_pattern[] = {"/home/"}; //size = 7
    char updated_path [2048];
    //chdir("/path/to/change/directory/to");
    getcwd(cwd, sizeof(cwd));
    boolean l = TRUE;
    int username_length = 7; // to fix username count
    for (int i = 0; i < 2048; ++i)
    {
        if (cwd[i]==0)
            break;

        if (i < 6 && home_pattern[i] == cwd[i])
        {
            username_length++;
        }

        else if(i < 6 && home_pattern[i] != cwd[i])
        {
            l = FALSE;
        }
    }

    if (l == TRUE)
    {
        updated_path[0] = '~';
        for (int j = 0; j < 2048; ++j)
        {
            if (cwd[j]==0)
                break;
            if ( j > username_length)
                updated_path[j-username_length] = cwd[j];
        }
    }



    if (usr_type == 0) {
        printf("@bug_shell:%s# > ",updated_path);
    }
    else {
        printf("@bug_shell:%s$ > ", updated_path);
    }
}



int skipping_esccapes(char *line, int i_line)
{
       while (line[i_line]==' ')
        {
            i_line++;
        }
    return i_line;
}


char *read_input()
{
    
    char *buffer = malloc(BUFSIZE * sizeof(char));

    if (!buffer)
    {
        fprintf(stderr,"Allocation error");
        exit(1);
    }

    getline(&buffer,&BUFSIZE,stdin);

    return buffer;
}

int execute(char * args[])
{

    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == -1){

        // pid == -1 means error occured
        printf("can't fork, error occured\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0){

        char * ls_args[] = {"ls","-1", NULL};
        int o = execvp(ls_args[0],ls_args);


    }
    else{
        // a positive number is returned for the pid of
        // parent process
        // getppid() returns process id of parent of
        // calling process
        printf("parent process, pid = %u\n",getppid());

        // the parent process calls waitpid() on the child
        // waitpid() system call suspends execution of
        // calling process until a child specified by pid
        // argument has changed state
        // see wait() man page for all the flags or options
        // used here
        if (waitpid(pid, &status, 0) > 0) {

            if (WIFEXITED(status) && !WEXITSTATUS(status))
                printf("program execution successfull\n");

            else if (WIFEXITED(status) && WEXITSTATUS(status)) {
                if (WEXITSTATUS(status) == 127) {

                    // execv failed
                    printf("execv failed\n");
                }
                else
                    printf("program terminated normally,"
                           " but returned a non-zero status\n");
            }
            else
                printf("program didn't terminate normally\n");
        }
        else {
            // waitpid() failed
            printf("waitpid() failed\n");
        }

    }

}

void run(void)
{
    /*reading the commands from user
     * 1. Reading command from user
     * 2. Parsing the commands
     * 3. Executing parsed commands
     * */

    char * line;
    int status;

    do
    {
        char *p = getenv("ls");
        printf("%s",p);
        you_are_here(getuid());
        line = read_input();


        int size = 0;

        for (int i = 0; i < 1000000; ++i)
        {

            if (line[i]==10)
                break;
            size ++;
        }
        char * arguments [++size];
        for (int j = 0; j < size; ++j)
            arguments[j] = &line[j];
        arguments[--size] = NULL;

        //set follow-fork-mode child set detach-on-fork off
        status = execute(arguments);
        free(line);
    }
    while (status >=0);


}


int main(int argc, char** argv)
{
    /* loading the configuration
     * run the program
     * cleanup the program
     * */


    run();

    printf("Hello, World!\n");
    return 0;
}