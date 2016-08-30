#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>

pthread_t threads[4000];
int thread_count = 0;

struct args
{
    char pattern[1024];
    char file_name[1024];
    int file_index;
};



int error(char* message)
{
    perror(message);
    return errno;
}


int directory_is_this_or_prev(struct dirent* direntp)
{
    int this = strcmp(direntp->d_name, ".");
    int prev = strcmp(direntp->d_name, "..");

    return (this == 0 || prev == 0);
}


void build_path(char* new_path, char* root_path, char* next)
{
    strcpy(new_path, root_path);
    if (new_path[(strlen(new_path) - 1)] != '/')
        strcat(new_path, "/");
    strcat(new_path, next);
}


void print_number(int i)
{
    printf("%d ", i);
}


void print_string(char *s)
{
    printf("%s", s);
}


void print_match(char * file_name, int file_index, int line_number, char * line)
{
    // Ensure text for match comes out on one line (so lines are atomic)
    // Matches from several files may be intermixed with each other (if parallel).
    // Use the following to start, but it will be wrong if your parallelism is working.
    
    //
    pthread_mutex_lock(&threads[thread_count]);
    print_number(file_index);
    print_string(file_name);
    print_string(":");
    print_number(line_number);
    print_string("  ");
    print_string(line);
    pthread_mutex_unlock(&threads[thread_count]);
}


void* add_all_matching_lines(void * args)
{
    struct args* real_args = args;

    int line_number = 1;
    FILE* file = fopen(real_args->file_name, "r");
    char line[1024];

    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, real_args->pattern))
            print_match(real_args->file_name, real_args->file_index, line_number, line);
        strcpy(line,"");
        line_number++;
    }

    free(args);
    pthread_exit(NULL);
    return 0;
}


void search(char * pattern, int file_index, char * file_name)
{
    DIR* dirp = opendir(file_name);
    struct dirent* direntp;

    if (!dirp)
    {
        error("error opening directory");
        return;
    }
    while (direntp = readdir(dirp))
    {
        if (directory_is_this_or_prev(direntp))
            continue;
        else
        {
            char new_path[1024];
            build_path(new_path, file_name, direntp->d_name);
            struct stat new_path_info;
            stat(new_path, &new_path_info);

            if (S_ISDIR(new_path_info.st_mode))
                search(pattern, file_index, new_path);
            else if (S_ISREG(new_path_info.st_mode))
            {
                //printf("name %s\n", new_path);

                struct args* targs = malloc(sizeof(struct args));
                
                strcpy(targs->file_name, new_path);
                targs->file_index = file_index;
                strcpy(targs->pattern, pattern);

                pthread_create(&threads[thread_count], NULL, add_all_matching_lines, targs);
                pthread_join(threads[thread_count++], NULL);
            }
        }
    }
}


int main( int argc, char *argv[] ) // a parallel version of grep -r
{
    int file_index = 1;
    if ( argc < 3 )
        error("Usage: search pattern file");
    for (int i = 2; i < argc; ++i)
        search(argv[1], file_index++, argv[i]);

    
    return 0;
}
