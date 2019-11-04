#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <jmorecfg.h>
#include <string.h>

size_t BUFSIZE = 1024; // be careful with this variable before changing its value

int skipping_esccapes(char *line, int i_line)
{
       while (line[i_line]==' ')
        {
            i_line++;
        }
    return i_line;
}


char *read()
{
    
    char *buffer = malloc(BUFSIZE * sizeof(char));

    if (!buffer)
    {
        fprintf(stderr,"Allocation error");
        exit(1);
    }

    printf( "Enter a value :");
    getline(&buffer,&BUFSIZE,stdin);

    return buffer;
}

char **parse(char * line)
{
    //alocation of arguments array
    char **args = (char **)malloc(BUFSIZE * sizeof(char *));

    for (int i=0; i<BUFSIZE; i++)
        args[i] = (char *)malloc(BUFSIZE * sizeof(char));
    for (int j = 0; j < BUFSIZE; ++j)
    {
        for (int k = 0; k < BUFSIZE; ++k)
        {
            printf("%s", &args[j][k]);
        }
    }




    int l_size =(int) sizeof(line)/sizeof(line[0]); // NOLINT(bugprone-sizeof-expression)
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
//    printf("\n");
//    int i=0;
//    int j=0;
//
//    while ((int)args[i][j]!=10)
//    {
//        if((int)args[i][j]==0)
//        {
//           i++;
//           j=0;
//        } else
//        {
//            printf("%s", &args[i][j]);
//            j++;
//        }
//
//
//    }
//    printf("\n");
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
    /* loading the configuration
     * run the program
     * cleanup the program
     * */

   parse( read());

    printf("Hello, World!\n");
    return 0;
}