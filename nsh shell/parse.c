#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <ctype.h>
#include <errno.h>

#include "parse.h"

void error(char *msg)
{
    /* simple error printing message
     */
    fprintf(stderr, "%s\n", msg);
}

////////////////////////////////////////////
//
// Memory Functions


void ensureLength(int used, int* size, int type, void** obj)
{
    /*  realloc memory if the potential increased used amount
        equals the maximum size
     */

    size_t t;
    switch (type)
    {
        case 0: // char*
            t = sizeof(char *);
            break;
        case 1: // char
            t = sizeof(char);
            break;
        case 2: // command
            t = sizeof(command);
            break;
    }

    if ((used + 1) == *size)
    {
        *size *= 2;
        if ((*obj = realloc(*obj, *size * t)) == NULL)
            error("realloc failed");
    }
}


int fgetline(char** buf, FILE* file)
{
    /*  get whole line stored into *buf, default file* is stdin
     */

    int size = 256;
    int len  = 0;

    char* line = malloc(size * sizeof(char));

    if (line == NULL || feof(file))
    {
        *buf = line;
        return -1;
    }

    char c = 0;
    while ((c = fgetc(file)))
    {
        if (c == '\n' || c == EOF)
            break;

        ensureLength(len, &size, 1, (void*) &line);
        *(line + len++) = c;
    }

    *(line + len) = 0;
    *buf = line;

    return len;
}


int safeFree(void** ptr)
{
    /*  function that helps avoid double free
     */

    if (*ptr != NULL)
    {
        free(*ptr);
        *ptr = NULL;
    }
    return 0;
}


void clean(commandLine** cmdLine)
{
    /*  clean all alloc'd memory in a cmdLine object
     */

    if (*cmdLine == NULL)
        return;

    commandLine* cLine = *cmdLine;

    if (cLine->used == 0)
        cLine->used = 1;

    for (int i = 0; i < cLine->used; ++i)
        safeFree((void**) &cLine->cmds[i].argv);

    safeFree((void**) &cLine->cmds);
    safeFree((void**) &cLine->line);
    safeFree((void**) cmdLine);
}


command* Command(command* cmd)
{
    /*  constructor for command object
     */

    cmd->in         = NULL;
    cmd->out        = NULL;
    
    cmd->pipe       = false;
    cmd->cat        = false;

    cmd->argc       = 0;
    cmd->size       = 16;
    cmd->argv       = malloc(cmd->size * sizeof(char*));

    return cmd;
}


commandLine* CommandLine(char* line, int size)
{
    /*  constructor for command line object 
     */
    commandLine* ret    = malloc(sizeof(commandLine));

    ret->used           = 0;
    ret->size           = 16;

    ret->bg             = false;
    ret->line           = line;
    ret->line_sz        = size;
    ret->cmds           = malloc(sizeof(command) * ret->size);

    Command(&ret->cmds[0]);

    return ret;
}

////////////////////////////////////////////
//
// Parse Functions


bool isShellSymb(char c)
{
    return c == '|' || c == '<' 
        || c == '>' || c == '&';
}


bool isDelimiter(char c)
{
    return isspace(c) || isShellSymb(c);
}


char* nextWord(commandLine* cmdLine, int* i)
{
    /*  returns pointer next argument value in a line
     */

    while(isspace(cmdLine->line[*i]))
        ++(*i);
    if (*i > cmdLine->line_sz || cmdLine->line[*i] == 0)
        return NULL;
    return &(cmdLine->line[*i]);  
}


void terminateArg(char* line, int* i)
{
    /* finds end of argument in a line str and terminates it
     */

    while (line[*i] != 0 && !isDelimiter(line[*i]))
        ++(*i);
    if (isShellSymb(line[*i]))
    {
        --(*i);
        return;
    }
    line[*i] = 0;
}


void terminateArgv(commandLine* cmdLine)
{
    /*  terminates array of commands to ensure
        that the exec array has no issues in executing
        commands
     */

    command* cmd = &cmdLine->cmds[cmdLine->used];

    cmd->argv[cmd->argc] = NULL;
}


command* parsePipe(commandLine* cmdLine, int* cursor)
{
    /*  initializes new command object and adds it to the
        command array in a CommandLine object
     */

    command* cmd = &cmdLine->cmds[cmdLine->used];
    
    cmd->pipe = true;
    terminateArgv(cmdLine);
    cmdLine->line[*cursor] = 0;
    ensureLength(cmdLine->used, &cmdLine->size, 
        2, (void *) &cmdLine->cmds);
    
    return Command(&cmdLine->cmds[++cmdLine->used]);
}


char* parseInFile(commandLine* cmdLine, int* cursor)
{
    /*  find the start of a file name and terminates the end of.
        returns pointer to start of file name.

        acounts for file names with spaces (within single quotes)
     */

    cmdLine->line[(*cursor)++] = 0;

    char* startIn = nextWord(cmdLine, cursor);

    if (*startIn == '\'')
    {
        ++startIn;
        cmdLine->line[(*cursor)++] = 0;
        while (cmdLine->line[(*cursor)] != '\'') ++(*cursor);
        cmdLine->line[(*cursor)] = 0;
    }

    return startIn;
}


char* parseOutFile(commandLine* cmdLine, int* cursor)
{
    /*  find the start of a file name and terminates the end of.
        returns pointer to start of file name.

        acounts for file names with spaces (within single quotes)
     */

    cmdLine->line[(*cursor)++] = 0;

    if (cmdLine->line[*cursor] == '>')
    {
        ++(*cursor);
        cmdLine->cmds[cmdLine->used].cat = true;
    }


    char* startOut = nextWord(cmdLine, cursor);

    if (*startOut == '\'')
    {
        ++startOut;
        cmdLine->line[(*cursor)++] = 0;
        while (cmdLine->line[(*cursor)] != '\'') ++(*cursor);
        cmdLine->line[(*cursor)] = 0;
    }
    
    return startOut;
}


char* parseQuotes(commandLine* cmdLine, int* cursor)
{
    /*  terminates first occurence of single quote,
        looks for end of argument by searching for 
        next single quote
    */

    cmdLine->line[(*cursor)++] = 0;
    char* ret = parseArgv(cmdLine, &(*cursor));
    while (cmdLine->line[(*cursor)] != '\'') ++(*cursor);
    cmdLine->line[(*cursor)] = 0;

    return ret;
}


int parseBackg(commandLine* cmdLine, int* cursor)
{
    /*  looks for the background symbol and sets the flag
        for background tasks in the cmdLine object
     */

    cmdLine->line[*cursor] = 0;
    
    if (cmdLine->line[*cursor + 1] != 0)
    {
        error("nsh: syntax error at & token\n");
        if (cmdLine->cmds[0].argc > 0)
            ++cmdLine->used; // for preventing memory leaks

        return -1;
    }

    cmdLine->bg = true;
    return 0;
}


char* parseArgv(commandLine* cmdLine, int* cursor)
{
    /* stores argument into cmdLine->cmd[i] object
     */

    command* cmd = &cmdLine->cmds[cmdLine->used];
    
    ensureLength(cmd->argc, &cmd->size, 
        0, (void *) &cmd->argv);
    
    return &cmdLine->line[*cursor];   
}


int parse(commandLine* cmdLine)
{
    /* complete parser */

    char*    line   = cmdLine->line;   
    command* cmd    = &cmdLine->cmds[0];

    for (int i = 0; i < cmdLine->line_sz; ++i)
    {
        if (nextWord(cmdLine, &i) == NULL)
            break;

        switch(line[i])
        {
            case '|':
                cmd = parsePipe(cmdLine, &i);
                continue;
            case '<':
                cmd->in = parseInFile(cmdLine, &i);
                break;
            case '>':
                cmd->out = parseOutFile(cmdLine, &i);
                break;
            case '\'':
                cmd->argv[cmd->argc++] = parseQuotes(cmdLine, &i);
                break;
            case '#':
                line[i] = 0;
                goto end_loop; 
            case '&':
                if (parseBackg(cmdLine, &i) == -1)
                    return -1;
                break;
            default:
                cmd->argv[cmd->argc++] = parseArgv(cmdLine, &i);
                break;
        }
        terminateArg(line, &i);
    }

    end_loop:

    terminateArgv(cmdLine);

    if (cmdLine->cmds[0].argc > 0)
        ++cmdLine->used;

    return 0;
}