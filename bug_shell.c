#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <pwd.h>

size_t BUFSIZE = 1024; // be careful with this variable before changing its value
int isInBackground = 0;
char updated_path [2048];

/*=======================================================================================================
 * implementation of default library functions
 *=======================================================================================================
 */
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

static void handler(siginfo_t *si)
{
    printf("Got SIGSEGV at address: 0x%lx\n",(long) si->si_addr);
    //exit(EXIT_FAILURE);
}

void cpstr(char *to, char *from)
{
    int to_size = lenstr(to);
    int from_size = lenstr(from);

    for (int i = 0; i < to_size; ++i)
    {
        if(i < from_size)
            to[i]=from[i];
        else
            to[i] = 0;
    }

}

char * mergestr(char *to, char *from)
{
    int a_size = lenstr(to);
    int b_size = lenstr(from);
    int total = a_size+b_size;

    char * result = (char *) alloca(a_size+b_size);
    for (int j = 0; j <= a_size; ++j)
    {
        result[j] = to[j];
    }
    result[a_size]='/';

    for (int i = a_size; i < total; ++i)
    {
        result[i+1]=from[i-a_size];
    }
    return result;

}


int cmpstr(char * a, char *b)
{
    int a_size = lenstr(a);
    int b_size = lenstr(b);

    if (a_size != b_size)
        return 0;
    else
    {
        for (int i = 0; i < b_size; ++i)
        {
            if(a[i]!=b[i])
                return 0;
        }

        return 1;
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
/*
 * =================================================================================================================
 * end of the library functions implementation
 * ==================================================================================================================
 */

/*=================================================================================================================
 * Shell starts here
 *=================================================================================================================
 */


char *getUserName()
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw)
    {
        return pw->pw_name;
    }

    return "";
}

char * get_home_string()
{
    char * username = getUserName();
    char * home_pattern = "/home";

    return mergestr(home_pattern, username);
}


int get_home_path_length()
{
    int home_path_length = lenstr(get_home_string()); // home pattern size
    return home_path_length;
}


int is_home_on_path()
{
    int is_home_on_path = 1;
    int home_path_length = get_home_path_length();
    char *home_pattern = get_home_string();

    for (int i = 0; i < home_path_length; ++i)
    {
        if (home_pattern[i] != updated_path[i])
            is_home_on_path = 0;
    }
    return is_home_on_path;
}


void home_to_tilda() {



        //getcwd(updated_path, sizeof(updated_path));
        int size_cwd = lenstr(updated_path);
        int home_path_length = get_home_path_length(); // home pattern size

        if (size_cwd > 1)
        {
            char new_path [BUFSIZE];

            new_path[0] = '~';
            new_path[1] = '/';
            int j = home_path_length;
            for (; j < 2048; ++j)
            {

                if (updated_path[j]==0)
                    break;
                if(j >= home_path_length)
                    new_path[j-home_path_length+1] = updated_path[j];
            }
            cpstr(updated_path,new_path);
        }


}


//printing the current path
void update_path(char * update_the_path)
{
    if(update_the_path[0]=='/')
        cpstr(updated_path,update_the_path);
    else
    {
        if( cmpstr(update_the_path,".."))
        {
            int i = lenstr(updated_path);
            if(!cmpstr(update_the_path,"~"))
            {
                for (; i >= 0; --i)
                {
                    if (updated_path[i] != '/')
                        updated_path[i] = 0;
                    else
                    {
                        updated_path[i] = 0;
                        break;
                    }
                }
            } else
                printf("bubu");
        }

        else if(update_the_path[0]!='~')
        {
            int s_a = lenstr(updated_path);
            int s_b = lenstr(update_the_path);
            char * result = mergestr(updated_path, update_the_path);
            cpstr(updated_path,result);
        }
        else
        {
            cpstr(updated_path, "/");
            cpstr(updated_path,update_the_path);
        }

    }
}

void print_current_path()
{
    if(is_home_on_path())
      home_to_tilda();

    uid_t usr_type = getuid();
    if (usr_type == 0) {
        printf("%s@bug_shell:%s# > ",getUserName(),updated_path);
    }
    else {
        printf("%s@bug_shell:%s$ > ",getUserName(),updated_path);
    }
}

//change directory
void cd(char *path)
{
   int status = chdir(path);
   if(status > -1)
       update_path(path);
   else
       printf("bug_shell: cd: %s: No such file or directory\n",path);
}


//simillar to printing path can't explai


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
        //don't do anything cz this is child process
    }

    else
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
    int is_command = 0;
    while (status >= 0)
    {
        print_current_path();
        char *line = malloc(BUFSIZE * sizeof(char));
        //home_path();
        read_input(line);

        char * args [BUFSIZE];
        for (int j = 0; j < BUFSIZE; ++j)
            args[j] = 0;

        parse(line,args);
        //set follow-fork-mode child set detach-on-fork off
        if (cmpstr(args[0],"cd"))
        {
            cd(args[1]);
            is_command = 1;
        }

        else
        {
            is_command = 0;
            status = execute(args);
        }
        free(line);//0x7fff33941160

    }



}

int main(int argc, char** argv)
{
    /* loading the configuration
     * run the program
     * cleanup the program
     * */
    getcwd(updated_path, sizeof(updated_path));
    run();

    return 0;
}