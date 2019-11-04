#include <stdio.h>
#include <malloc.h>

char * read()
{
    return 0;
}

char **parse(char * line)
{

}

int execute(char ** args)
{

    return 0;
}



void run(void)
{
    /*reading the commands from user
     * 1. Reading command from user
     * 2. Parsing the commands
     * 3. Executing parsed commands
     * */

    char * line;
    char ** args;
    int status;

    do
    {
        printf("%s","supperBug@root > ");
        line = read();
        args = parse(line);
        status = execute(args);
        free(line);
        free(args);
    }
    while (status >=0);


}


int main(int argc, char** argv)
{
    //loading the configuration

    //run the program

    //cleanup the program

    printf("Hello, World!\n");
    return 0;
}