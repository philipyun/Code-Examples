#include <stdio.h>
#include <string.h>

void print_array(char* array[])
{
    while (*array)
    {
        if (*array == NULL)
        {
            printf("NULL\n");
        }
        printf("%s\n", *array++);
    }
}


void print_array_n(char* array[], int n)
{
    printf("Array Size %d\n", n);
    for (int i = 0; i < n; ++i)
    {
        if (*array == NULL)
            printf("NULL\n");
        else
            printf("%s\n", *array++);
    }
}


void clear_array(char* array[], int n)
{
    for (int i = 0; i < n; ++i)
        array[i] = NULL;
}


void copy_array(char* src[], char* dst[], int n)
{
    for (int i = 0; i < n; ++i)
        dst[i] = src[i];
}