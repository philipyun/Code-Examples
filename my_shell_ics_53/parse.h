void parse_space_pipe(char** line, char* start, char* buffer);


void parse_space_argv(char** line, char** start, char** cmd[], int* index);


void parse_operators(char** line, char** start, char** cmd[], int* index);


int end_parse(char* line, char* start, char** cmd[], int* index, int is_argv);


int parse_by_redirects(char* buffer, char* line, char* argv[]);


int parse_by_pipes(char* buffer, char* line, char* cmd[]);
