#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>



char* block = "\033[07m \033[m";


void clear()
{
    char* cl[] = {"clear"};
    execvp(cl[0], cl);
}


void init_board(int w, int h, int board[w][h])
{
    srand(time(NULL));
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            board[i][j] = rand() % 2;
}


void print_array(int w, int h, int board[w][h])
{
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
            printf("%d", board[i][j]);
        printf("\n");
    }
}


void print_board(int w, int h, int board[w][h])
{
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
            (board[i][j] == 1) ? printf("%s", block) : printf(" "); 
        printf("\n");
    }
}


int is_in_board(int x, int y, int w, int h)
{
    return !((x < 0) || (y < 0) || (x >= w) || (y >= h));
}


int neighbors(int x, int y, int w, int h, int board[w][h])
{
    int nghbrs = 0;
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            if (i == 0 && j == 0)
                continue;
            if (is_in_board(x + i, y + j, w, h))
            {
                if (board[x + i][y + j] == 1)
                    ++nghbrs;
            }
        }
    }
    return nghbrs;
}


void copy_board(int w, int h, int dest[w][h], int src[w][h])
{
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            dest[i][j] = src[i][j];
}


void update(int w, int h, int board[w][h])
{
    int new_board[w][h];
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            if (board[i][j] == 0 && neighbors(i,j,w,h,board) == 3)
                new_board[i][j] = 1;
            else if ((board[i][j]) == 1)
            {
                int cmp = neighbors(i,j,w,h,board);
                if (cmp < 2 || cmp > 3)
                    new_board[i][j] = 0;
                else if (cmp == 2 || cmp == 3)
                    new_board[i][j] = 1;
            }
            else
                new_board[i][j] = board[i][j];
        }
    }
    copy_board(w, h, board, new_board);
}


int main(int argc, char* argv[])
{
    int w = 80, h = 22;
    if (argc == 3)
        w = atoi(argv[1]), h = atoi(argv[2]);
    int board[w][h];
    init_board(w, h, board);
    for(;;)
    {
        system("clear");
        print_board(w, h, board);
        update(w, h, board);
        sleep(1);
    }
    return 0;
}
