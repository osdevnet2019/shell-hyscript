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

struct pairs
{
    int i;
    int j;
};

struct pairs parse(char * line, char * argss[])
{
    //alocation of arguments array
    char **args = (char **)malloc(BUFSIZE * sizeof(char *));

    for (int i=0; i<BUFSIZE; i++)
    {
        args[i] = (char *) malloc(BUFSIZE * sizeof(char));

    }


    int i_line = 0;

    //indecies of arguments
    int i_args = 0;
    int j_args = 0;
    do
    {
        if ((int)line[i_line] == 32)
        {
            i_args++;
            i_line = skipping_esccapes(line, i_line);
            j_args=0;
            args[i_args][j_args++] = line[i_line];


        } else
        {
            args[i_args][j_args] = line[i_line];
            j_args++;


        }

       char h = line[i_line];

        i_line++;

    }
    while (line[i_line] != 0);
    boolean isOver=FALSE;
    for (int i = 0; i <= i_args; i++)
    {
        for (int j = 0; j < j_args; j++)
        {
            if (isOver)
                argss[i][j]= 0;
            if (args[i][j]=='\n')
                isOver = TRUE;
            else
               argss[i][j] = args[i][j];

        }
    }
    free(args);
    struct pairs p;
    argss[i_args][++j_args] = '\0';
    p.i = i_args;
    p.j = j_args;
    return p;
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
    char *ar[] ={"ls","-l",NULL};
    pid = fork();
    if (pid == -1){

        // pid == -1 means error occured
        printf("can't fork, error occured\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0){

        int o = execvp(args[0],args);


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
        //for path determination, replacing home with ~
        for (int i = 0; i < 1000000; ++i)
        {

            if (line[i]==10) // 10 = /n
                break;
            size ++;
        }

        char * arguments [++size];
        for (int j = 0; j < size; ++j)
            arguments[j] = &line[j];
        arguments[--size] = NULL;

        char  *args [1000];

        for (int i=0; i<1000; i++)
        {
            args[i] = (char *) alloca(1000 * sizeof(char));

        }

        struct pairs pp = parse(line,args);
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

    printf("Hello, World!\n");
    return 0;
}