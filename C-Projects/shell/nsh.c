#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>

#include "parse.h"

////////////////////////////////////////////
//
// System Call Helpers

void childSig(int signum)
{
    /*  signal handler used to automatically check 
        up on dead processes especially jobs that 
        are sent to the background.

        essentially only used for background jobs so 
        resources are cleaned up, since non background 
        jobs will clean up immediately 
     */

    int status;
    waitpid(-1, &status, WNOHANG);
}


void intSig(int signum)
{
    /*  signal handler for SIGINTS that restart the 
        shell prompt and prevent closing of the shell
     */ 

    printf("\n? ");
    fflush(stdout);
}


void safePipe(int* pipefds)
{
    /*  a rewrite of the pipe() system call that 
        does error checking for you
     */

    if (pipe(pipefds) == -1)
        error("cannot create pipe");
}


int safeOpen(char* file, int mode, bool append)
{
    /*  a rewrite of open() sys call that does 
        error checking for you.

        mode = 0 --> read
        mode = 1 --> write
        append = true --> append, not truncate

        by default all files are given 0666 
        permissions
     */

    int O_MODE;

    if (mode == 0)
        O_MODE = O_RDONLY;
    else
    {
        O_MODE = O_CREAT|O_WRONLY|O_TRUNC;
        if (append)
            O_MODE = O_CREAT|O_WRONLY|O_APPEND;
    }

    int fd = open(file, O_MODE, 0666);

    if (fd == -1)
        error("open() error");

    return fd;
}


void initPipe(int in, int out)
{
    /*  given an input and output file descriptor
        this function will duplicate the appropriate
        file descriptors to the correct pipes
     */

    if (in != 0)
        dup2(in, 0);
    if (out != 1)
        dup2(out, 1);
}


void closePipe(int in, int out)
{
    /*  close all pipes passed to function,
        unless the pipes are stdin/stdout
     */

    if (in != 0) 
        close(in);
    if (out != 1) 
        close(out);
}

////////////////////////////////////////////
//
// Change Directory

void cd(commandLine* cmdLine)
{
    /*  cd command
        checks for appropriate errors,
            file doesnt exist
            file is not directory
            file does not have correct permissions
     */

    char home[50] = "/home/";
    getlogin_r(&home[6], 37);
    if (cmdLine->cmds[0].argc >= 2)
    {
        if (chdir(cmdLine->cmds[0].argv[1]) == -1);
        {
            switch (errno)
            {
                case ENOENT:
                    fprintf(stderr, 
                        "nsh: cd: %s: No such file or directory\n", 
                        cmdLine->cmds[0].argv[1]);
                    break;
                case ENOTDIR:
                    fprintf(stderr, 
                        "nsh: cd: %s: Not a directory\n", 
                        cmdLine->cmds[0].argv[1]);
                    break;
                case EACCES:
                    fprintf(stderr, "nsh: cd: %s: Permisson denied\n", 
                        cmdLine->cmds[0].argv[1]);
                    break;
            }
        }
    }
    else if (cmdLine->cmds[0].argc == 1)
        chdir(home);
}


////////////////////////////////////////////
//
// Process Handling

void childWork(command* cmd, int* in, int* out)
{
    /*  childWork sets appropriate file descriptors 
        given pipes, or input/output files

        childWork will then exec the command and exit 
        the currently running process
     */

    if (cmd->in)
        *in = safeOpen(cmd->in, 0, 0);
    if (cmd->out)
        *out = safeOpen(cmd->out, 1, cmd->cat);
    
    initPipe(*in, *out);
    if (execvp(cmd->argv[0], cmd->argv) == -1)
        fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
    exit(0);
}


int spawnProcess(command* cmd, int* in, int* out)
{
    /*  handles forks.  Parent will close pipes and 
        the child will initiate pipes and exec command
     */

    int child;
    switch ((child = fork()))
    {
        case -1:
            fprintf(stderr, "nsh: fork() failed\n");
            break;
        case 0:
            childWork(cmd, in, out);
            break;
        default:
            closePipe(*in, *out);
            break;
    }
    return child;
}


void exec(commandLine* cmdLine)
{
    /*  exec command line and process all possible pipes 
        or file redirection
     */

    int in = 0, out = 1, pipefds[2];
    int used = cmdLine->used;

    if (used == 1 && strcmp(cmdLine->cmds[0].argv[0], "cd") == 0)
        cd(cmdLine);

    else
    {
        for (int i = 0; i < used; ++i)
        {
            command* cmd = &cmdLine->cmds[i];
            
            if (used > 1)
            {
                safePipe(pipefds);
                out = (i == cmdLine->used - 1) ? 1 : pipefds[1];
            }
            
            spawnProcess(cmd, &in, &out);
            in = pipefds[0];

        }
        if (used > 1)
            closePipe(in, out);
    }
    fflush(stdout);
}


int processCmdLine(commandLine* cmdLine)
{
    /*  parse cmdLine then execute it.
        wait for the children if necessary
     */

    parse(cmdLine);
    exec(cmdLine);

    if (!cmdLine->bg)
        for (int i = 0; i < cmdLine->used; ++i)
            wait(NULL);

    return 0;
}


int processFilePtr(FILE* file)
{
    /*  process shell commands based on a file ptr.
        default file ptr is stdin.

        if the script is called as nsh <scriptname>,
        then the file* passed is the file* of 
        <scriptname>
     */

    char* line = NULL;  
    int   size = fgetline(&line, file);


    if (size == 0 && !feof(file))
    {
        safeFree((void**) &line);
        return 1;
    }
    if (strcmp(line, "exit") == 0)
        return safeFree((void**) &line);

    if (feof(file) && size <= 0)
        return safeFree((void**) &line);


    commandLine* cmdLine = CommandLine(line, size);
    
    if (size > 0)
        processCmdLine(cmdLine);

    clean(&cmdLine);
    return 1;
}


int processFile(char* filename)
{
    /*  given a filename, create a file* and then pass that
        to processFilePtr until EOF occurs
     */

    FILE* file = fopen(filename, "r");
    int i = 0;
    while (!feof(file) && processFilePtr(file));
    fclose(file);

    return 0;
}


int interacitveShell()
{
    // sig int should be handled in interactive mode
    signal(SIGINT, intSig);


    printf("? ");
    while (processFilePtr(stdin))
        printf("? ");


    printf("exit\n");

    return 0;
}


int main(int argc, char** argv)
{
    // sigchild should always be handled
    signal(SIGCHLD, childSig);

    if (argc == 1)
        interacitveShell();
    else
        for (int i = 1; i < argc; ++i)
            processFile(argv[i]);

    return 0;
}