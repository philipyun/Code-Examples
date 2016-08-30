//Philip Yun 65035200

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct
{
    char  permissions[11];
    int   hard_count;
    char  user[33];
    char  group[17];
    int   size;
    char  tm[15];
    char  file_name[256];
} entry;

int a_opt = 0;
int A_opt = 0;
int L_opt = 0;

int hard_max = 0;
int user_max = 0;
int group_max = 0;
int size_max = 0;

// Output Formatting helper Functions

void reset()
{
    hard_max = 0;
    user_max = 0;
    group_max = 0;
    size_max = 0;
}

int int_length(int x)
{
    int length = 0;
    for (int i = x; i != 0; i /= 10)
        ++length;
    return length;
}


void set_max(entry* ent)
{
    int user_len = strlen(ent->user);
    int group_len = strlen(ent->group);
    int size_len = int_length(ent->size);
    int hard_len = int_length(ent->hard_count);

    if (user_len > user_max)
        user_max = user_len;
    if (group_len > group_max)
        group_max = group_len;
    if (size_len > size_max)
        size_max = size_len;
    if (hard_len > hard_max)
        hard_max = hard_len;
}


int count_files(char* path)
{
    int count = 0;
    struct stat st;

    if (stat(path, &st) == -1)
        perror("stat");

    if (S_ISDIR(st.st_mode))
    {
        DIR* dir;
        struct dirent* direntp;

        if (!(dir = opendir(path)))
            perror("opendir");

        while (direntp = readdir(dir))
            ++count;

        if (closedir(dir) == -1)
            perror("closedir");

        return count;
    }
    return 1;
}

// Permission Helper Functions

int user_type(int type)
{
    switch (type)
    {
        case 0:
            return 1;
        case 1:
            return 10;
        case 2:
            return 100;
    }
    return 1;
}


void user_permissions(char permissions[], struct stat st, int type)
{
    int usr = user_type(type);
    int offset = type * 3;

    permissions[1 + offset] = (st.st_mode & (400 / usr)) ? 'r' : '-';
    permissions[2 + offset] = (st.st_mode & (200 / usr)) ? 'w' : '-';
    permissions[3 + offset] = (st.st_mode & (100 / usr)) ? 'x' : '-';
}


char* get_permissions(char* permissions, struct stat st)
{
    if (S_ISDIR(st.st_mode))
        permissions[0] = 'd';
    else if (S_ISBLK(st.st_mode))
        permissions[0] = 'b';
    else if (S_ISCHR(st.st_mode))
        permissions[0] = 'c';
    else if (S_ISFIFO(st.st_mode))
        permissions[0] = 'p';
    else if (S_ISLNK(st.st_mode))
        permissions[0] = 'l';
    else
        permissions[0] = '-';

    user_permissions(permissions, st, 0);
    user_permissions(permissions, st, 1);
    user_permissions(permissions, st, 2);
    permissions[10] = 0;

    return permissions;
}


int is_hidden(struct dirent* direntp)
{
    return direntp->d_name[0] == '.';
}

// File ID helper functions

char* user_name(uid_t uid)
{
    struct passwd* pw;
    pw = getpwuid(uid);

    return pw->pw_name;
}


char* group_name(gid_t gid)
{
    struct group* gr;
    gr = getgrgid(gid);

    return gr->gr_name;
}

// Entry Data helper functions

void build_path(char* new_path, char* root_path, char* next)
{
    strcpy(new_path, root_path);
    if (new_path[(strlen(new_path) - 1)] != '/')
        strcat(new_path, "/");
    strcat(new_path, next);
}


void copy_time(char* dst, char* src)
{
    if (src == NULL)
        perror("ctime");

    int count = 0;
    for (int i = 4; i < 16; ++i, ++count)
        dst[count] = src[i];
    dst[count] = 0;
}


void set_attr(entry* ent, struct stat* st, char* file_name, char* path)
{
    ent->size = st->st_size;
    ent->hard_count = st->st_nlink;

    char f_time[13];
    copy_time(f_time, ctime(&st->st_mtime));

    strcpy(ent->tm, f_time);
    strcpy(ent->file_name, file_name);

    if (L_opt == 0 && S_ISLNK(st->st_mode))
    {
        char point[1000];
        if (readlink(path, point, 99) == -1)
            perror("readlink");
        strcat(ent->file_name, " -> ");
        strcat(ent->file_name, point);
    }

    strcpy(ent->user, user_name(st->st_uid));
    strcpy(ent->group, group_name(st->st_gid));

    get_permissions(ent->permissions, *st);
}

// Main task processer function

void store_info(char* file_name, char* path, struct stat* st, entry* ent)
{
    if (L_opt == 1)
    {
        if (stat(path, st) == -1)
        {
            lstat(path, st);
            if (S_ISLNK(st->st_mode))
                fprintf(stderr, "lss error: %s points to nowhere\n\n", path);
            else
                perror("stat");
            stat(path, st);
        }
    }
    else
        if (lstat(path, st) == -1)
            perror("lstat");

    set_attr(ent, st, file_name, path);
    set_max(ent);
}


void store_entries(char* path, entry entries[])
{
    int count = 0;
    DIR* dir;
    struct dirent* direntp;
    struct stat st;

    if (stat(path, &st) == -1)
        perror("stat");

    if (S_ISDIR(st.st_mode))
    {
        if (!(dir = opendir(path)))
            perror("opendir");

        while (direntp = readdir(dir))
        {
            if (A_opt == 1 && a_opt == 0 && is_hidden(direntp))
                continue;

            char new_path[strlen(path) + strlen(direntp->d_name) + 2];
            build_path(new_path, path, direntp->d_name);

            store_info(direntp->d_name, new_path, &st, &entries[count++]);
        }
        if (closedir(dir) == -1)
            perror("closedir");
    }
    else
        store_info(path, path, &st, &entries[count]);
}

// Qsort helper

int compare(const void* a, const void* b)
{
    entry* EA = (entry*)a;
    entry* EB = (entry*)b;

    return (EB->size - EA->size);
}

// Main functions and input validation

void ls(char* file)
{
    int f_num = count_files(file);
    if (a_opt == 0 && A_opt == 1)
        f_num -= 2;

    entry entries[f_num];

    store_entries(file, entries);

    char ls_format[100];
    sprintf(ls_format, 
        "%%s %%%dd %%%ds %%%ds %%%dd %%s %%s\n",
        hard_max,
        user_max,
        group_max,
        size_max);

    qsort(entries, f_num, sizeof(entry), compare);

    for (int i = 0; i < f_num; ++i)
        printf(ls_format, 
            entries[i].permissions,
            entries[i].hard_count,
            entries[i].user,
            entries[i].group,
            entries[i].size,
            entries[i].tm,
            entries[i].file_name);
}


void set_options(int argc, char* argv[])
{
    int c;
    while ((c = getopt(argc, argv, "aAL")) != -1)
    {
        switch (c)
        {
            case 'a':
                a_opt = 1;
                break;
            case 'A':
                A_opt = 1;
                break;
            case 'L':
                L_opt = 1;
                break;
            default:
                fprintf(stderr, "Error: unsupported option\n");
                exit(1);
                break;
        }
    }
}


int main(int argc, char* argv[])
{
    int count = 1;
    set_options(argc, argv);
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
            ++count;
        else
        {
            printf("%s:\n", argv[i]);
            ls(argv[i]);
            if (i != argc - 1)
            {
                printf("\n");
                reset();
            }
        }
    }
    if (count == argc)
        ls(".");

    return 0;
}