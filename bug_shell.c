#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <pwd.h>

size_t BUFSIZE = 1024; // be careful with this variable before changing its value
int isInBackground = 0;


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
    char home_pattern[] = {"/home/"}; //size = 7
    char updated_path [2048];
    //chdir("/path/to/change/directory/to");
    getcwd(cwd, sizeof(cwd));
    int is_home_on_path = 1;
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
            is_home_on_path = 0;
        }
    }

    if (is_home_on_path == 1)
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
        printf("%s@bug_shell:%s# > ",getUserName(),updated_path);
    }
    else {
        printf("%s@bug_shell:%s$ > ",getUserName(),updated_path);
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


void read_input(char *buffer)
{
    if (!buffer)
    {
        fprintf(stderr,"Allocation error");
        exit(1);
    }
    getline(&buffer,&BUFSIZE,stdin);

}

#pragma ide diagnostic ignored "hicpp-signed-bitwise"
int execute(char * args[])
{

    pid_t pid = fork();
    int status = 0;

    if (pid == -1){

        // pid == -1 means error occured
        printf("can't fork, error occured\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0 && isInBackground == 0){

        execvp(args[0],args);


    }
    else if (pid == 0 && isInBackground == 1){
        //don' do anything cz this is child process
    } else
    {


        if (isInBackground == 0) {
            if (waitpid(pid, &status, 0) < 1)
                printf("waitpid() failed\n");
        }
        else
        {
            if (waitpid(pid, &status, 0) > 0)
            {

                if (WIFEXITED(status) && WEXITSTATUS(status))  // NOLINT(hicpp-signed-bitwise)
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
    isInBackground = 0; //resetting background process
    return 1;

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
            if(i_line > 0 && line[i_line-1] == 38)
                isInBackground = 1;
            int tmp_str_size = lenstr(tmp_str);
            args[i_args] = concatstr( args[i_args], tmp_str, 0, tmp_str_size);
            break;
        }

        if(line[i_line] == 32) //the 32 is a code of 'space'
        {
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

    int status = 1;
    while (status > 0)
    {
        you_are_here(getuid());
        char *line = malloc(BUFSIZE * sizeof(char));

        read_input(line);

        char * args [BUFSIZE];
        for (int j = 0; j < BUFSIZE; ++j)
            args[j] = 0;

        int size = 0;

        //for path determination, replacing home with ~
        for (int i = 0; i < BUFSIZE; ++i)
        {
            if (line[i]==10) // 10 = /n
                break;
            size ++;
        }

        parse(line,args);
        //set follow-fork-mode child set detach-on-fork off
        status = execute(args);
        free(line);//0x7fff33941160

    }



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