#include <stdbool.h>

typedef struct 
{
    char**      argv;
    int         argc;

    char*       in;
    char*       out;

    bool        pipe;
    bool        cat;

    int         size;
} command;


typedef struct
{
    char*       line;
    int         line_sz;

    command*    cmds;
    int         used;
    int         size;

    bool        bg;
} commandLine;

void            error(char *msg);

/* commandLine is an object containing an array of commands.
   
   this implementation makes it easy to loop through
   lists of piped commands and sequence the pipes
   without a degradation of code readability
 */




////////////////////////////////////////////
//
// Memory Functions and Constructors


void            ensureLength(int used, int* size, int type, void** obj);
int             fgetline(char** buf, FILE* file);
int             safeFree(void** ptr);
void            clean(commandLine** cmdLine);
command*        Command(command* cmd);
commandLine*    CommandLine();


////////////////////////////////////////////
//
// Parse Functions


bool            isShellSymb(char c);
bool            isDelimiter(char c);

char*           nextWord(commandLine* cmdLine, int* i);
void            terminateArg(char* line, int* i);
void            terminateArgv(commandLine* cmdLine);

command*        parsePipe(commandLine* cmdLine, int* cursor);
char*           parseInFile(commandLine* cmdLine, int* cursor);
char*           parseOutFile(commandLine* cmdLine, int* cursor);
char*           parseQuotes(commandLine* cmdLine, int* cursor);
char*           parseArgv(commandLine* cmdLine, int* cursor);
int             parse(commandLine* cmdLine);