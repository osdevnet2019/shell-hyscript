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
    boolean is_home_on_path = TRUE;
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
            is_home_on_path = FALSE;
        }
    }

    if (is_home_on_path == TRUE)
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



int skipping_esccapes(const char *line, int i_line)
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

    pid_t pid;
    int status;
    pid = fork();
    if (pid == -1){

        // pid == -1 means error occured
        printf("can't fork, error occured\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0){

        execvp(args[0],args);


    }
    else{
        // a positive number is returned for the pid of
        // parent process
        // getppid() returns process id of parent of
        // calling process


        // the parent process calls waitpid() on the child
        // waitpid() system call suspends execution of
        // calling process until a child specified by pid
        // argument has changed state
        // see wait() man page for all the flags or options
        // used here
        if (waitpid(pid, &status, 0) > 0)
        {

            if (WIFEXITED(status) && !WEXITSTATUS(status))// NOLINT(hicpp-signed-bitwise)
                printf(" ");

            else if (WIFEXITED(status) && WEXITSTATUS(status))  // NOLINT(hicpp-signed-bitwise)
            {

                    if (WEXITSTATUS(status) == 127)// NOLINT(hicpp-signed-bitwise)
                    {

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

            printf("waitpid() failed\n");
        }

    }

}


//counting the length of string
int lenstr(const char * arr)
{
    int i = 0;
    char stop = '\000';
    while (1)
    {
        if (arr[i] == stop)
            return i;
        i++;
    }

    exit(0); //can't figure out why but after returning i program continue to execution
             //this is temporary solution
}

//string concatination
char * concatstr(const char *a, const char *b, int a_size, int b_size)
{

    int i_a = 0;

    int c_size = a_size + b_size;

    char * c =( char *)  malloc( c_size*sizeof(char));

    for (i_a = 0; i_a < a_size; ++i_a)
        c[i_a] = a[i_a];

    for (int j = 0; j < b_size; ++j)
        c[i_a++] = b[j];

    return c;
}

void parse(char * line, char  * args[])
{
    int i_line = 0;
    int i_args = 0;
    int i_tmp_str = 0;

    char *tmp_str=(char *) malloc(BUFSIZE * sizeof(char));

    while (1)
    {
        if (line[i_line] == '\n')
        {
            int tmp_str_size = lenstr(tmp_str);
            args[i_args] = concatstr( args[i_args], tmp_str, 0, tmp_str_size);
            break;
        }

        if(line[i_line] == 32) //the 32 is 'space' code
        {
            //1 will be added in the end of the while loop
            //-1 is assure us to get right index
            i_line = skipping_esccapes(line, i_line);
            i_tmp_str = 0;
            int tmp_str_size = lenstr(tmp_str);
            args[i_args] = concatstr(args[i_args],tmp_str,0, tmp_str_size);
            i_args++;
            free(tmp_str);
            tmp_str=(char *) malloc(BUFSIZE * sizeof(char));

        }

        tmp_str[i_tmp_str++] = line [i_line++];
    }
}

void run(void)
{
    /*reading the commands from user
     * 1. Reading command from user
     * 2. Parsing the commands
     * 3. Executing parsed commands
     * */

    char * line = 0;
    int status = 0;

    do
    {
        you_are_here(getuid());
        line = read_input();


        int size = 0;
        //for path determination, replacing home with ~
        for (int i = 0; i < BUFSIZE; ++i)
        {

            if (line[i]==10) // 10 = /n
                break;
            size ++;
        }

        char * args [BUFSIZE];

        for (int j = 0; j < BUFSIZE; ++j)
            args[j] = 0;

        parse(line,args);
        //set follow-fork-mode child set detach-on-fork off
        status = execute(args);
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

    return 0;
}