#include <stdio.h>
#include <string.h>
#include "parse.h"


void parse_space_pipe(char** line, char* start, char* buffer)
{
    char c = *(buffer + 1);
    if (*line != start && *(*line - 1) != ' ' && c != '|' && c != '&' && c != ' ')
        *((*line)++) = ' ';  
}


void parse_space_argv(char** line, char** start, char** cmd[], int* index)
{
    *((*line)++) = 0;
    if (strcmp(*start, "") != 0)
        (*cmd)[(*index)++] = *start;
    *start = *line;
}


void parse_operators(char** line, char** start, char** cmd[], int* index)
{
    *((*line)++) = 0;
    if (strcmp(*start, "") != 0)
        (*cmd)[(*index)++] = *start;
    *start = *line;
}


int end_parse(char* line, char* start, char** cmd[], int* index, int is_argv)
{
    *line = 0;
    (*cmd)[(*index)++] = start;
    if (is_argv == 1)
        (*cmd)[(*index)++] = NULL;
    return *index;
}


int parse_by_redirects(char* buffer, char* line, char* argv[])
{
    char* start = line;
    int index = 0;
    for( ; *buffer != 0 && *buffer != '\n'; ++buffer)
    {
        switch (*buffer)
        {
            case '\n':
            case '\t':
            case ' ':
                parse_space_argv(&line, &start, &argv, &index);
                break;
            case '<':
            case '>':
                parse_operators(&line, &start, &argv, &index);
                *line++ = *buffer;
                parse_operators(&line, &start, &argv, &index);
                break;
            default:
                *line++ = *buffer;
                break;
        }
    }
    return end_parse(line, start, &argv, &index, 1);
}


int parse_by_pipes(char* buffer, char* line, char* cmd[])
{
    int index = 0;
    char* start = line;
    for (; *buffer != 0 && *buffer != '\n'; ++buffer)
    {
        switch (*buffer)
        {
            case '\t':
            case ' ':
                parse_space_pipe(&line, start, buffer);
                break;
            case '|':
                parse_operators(&line, &start, &cmd, &index);
                parse_operators(&line, &start, &cmd, &index);
                break;
            case '&':
                parse_operators(&line, &start, &cmd, &index);
                *line++ = *buffer;
                //parse_operators(&line, &start, &cmd, &index);
                break;
            default:
                *line++ = *buffer;
                break;
        }
    }
    return end_parse(line, start, &cmd, &index, 0);
}