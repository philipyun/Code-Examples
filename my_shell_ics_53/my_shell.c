#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "parse.h"
#include "shell_array.h"

///////////////////////////////////////////////////////////////////////////////

void error(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "error number: %d\n", errno);
    exit(-1);
}


void wait_for_children()
{
    int pid;
    while (waitpid(-1, 0, WNOHANG) >= 0);
}


int open_for_reading(char *in_file)
{
    int fd = open(in_file, O_RDONLY);
    if (fd == -1)
        error("unable to open input file");
    return fd;
}


int open_for_writing(char *out_file)
{
    int fd = open(out_file, O_CREAT|O_WRONLY|O_TRUNC, 0666);
    if (fd == -1)
        error("unable to open input file");
    return fd;
}


void check_for_exit(char* argv[])
{
    if (argv && strcmp(argv[0], "exit") == 0)
    {
        printf("logout... my_shell terminated.\n");
        exit(0);
    }
}


void init_pipe(int in, int out)
{
    if (in != 0)
        dup2(in, 0);
    if (out != 1)
        dup2(out, 1);
}


void reconstruct_argv(char* cmd[], char* argv[])
{
    int index = 0;
    while (*cmd)
    {
        if (strcmp(*cmd, ">") == 0 || strcmp(*cmd, "<") == 0)
            break;
        argv[index++] = *cmd++;
    }
    argv[index] = NULL;
}


int set_in_file_redirect(int in, char* argv[], int size)
{
    for (int i = 0; i < size - 1; ++i)
    {
        if (strcmp(argv[i], "<") == 0)
            return open_for_reading(argv[i+1]);
    }
    return in;
}


int set_out_file_redirect(int out, char* argv[], int size)
{
    for (int i = 0; i < size - 1; ++i)
    {
        if (strcmp(argv[i], ">") == 0)
            return open_for_writing(argv[i+1]);
    }
    return out;
}


void redirect_pipe(int* in, int* out, char* argv[], int size)
{
    char* new_argv[1024] = {""};

    *in = set_in_file_redirect(*in, argv, size);
    *out = set_out_file_redirect(*out, argv, size);

    init_pipe(*in, *out);
    reconstruct_argv(argv, new_argv);

    if (execvp(new_argv[0], new_argv) == -1)
        printf("Error: exec failed\n");
}   


void process_handling(int in, int out, char* cmd)
{
    char* argv[1024] = {""};
    char line[1024] = "";
    int size = parse_by_redirects(cmd, line, argv);
    pid_t pid = 1;

    check_for_exit(argv);

    switch (pid = fork())
    {
        case -1:
            error("Forking failed");
        case 0:
            redirect_pipe(&in, &out, argv, size);
            break;
        default:
            if (in != 0) close(in);
            if (out != 1) close(out);
            break;
    }
}


void short_pipe(int pipefds[])
{
    if (pipe(pipefds) == -1) 
       error("cannot create pipe");
}


int sequence_pipes(char* cmd[], int size)
{
    int in = 0, pipefds[2];

    for (int i = 0; i < size; ++i)
    {
        if (i == size - 1 && strcmp(cmd[i], "&") == 0)
            return 1;

        short_pipe(pipefds);
        
        int out = (i == size - 1) ? 1 : pipefds[1];

        process_handling(in, out, cmd[i]);
        in = pipefds[0];
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////////

void main_sequence()
{
    char buffer[1024] = "";
    char line[2048] = "";
    char* cmd[1024] = {""};

    fgets(buffer, sizeof(buffer), stdin);
    int x = parse_by_pipes(buffer, line, cmd);
    int bg = sequence_pipes(cmd, x);

    if (bg == 0)
        wait_for_children();
}


int main()
{
    int count = 0;
    while (1)
    {
        char buffer[1024] = "";
        char line[2048] = "";
        char* cmd[1024] = {""};

        printf("%% ");
        main_sequence();
    }
    return 0;
}
