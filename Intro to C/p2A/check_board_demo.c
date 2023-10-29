#include <stdio.h>
#include <stdlib.h>
 
#define N 9
 
void print(int *pointer)
{
     for (int i = 0; i < N; i++)
      {
         for (int j = 0; j < N; j++)
            printf("%d ", *(pointer + i*N + j));
         printf("\n");
       }
}
 
int isSafe(int *grid, int row, int col, int num)
{
     
    for (int x = 0; x <= 8; x++)
        if (*(grid + row * N + x) == num)
            return 0;
    for (int x = 0; x <= 8; x++)
        if (*(grid + x * N + col) == num)
            return 0;

    int startRow = row - row % 3,
                 startCol = col - col % 3;
   
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (*(grid + (i + startRow)*N +(j + startCol)) == num)
                return 0;
 
    return 1;
}
 
int solveSudoku(int *grid, int row, int col)
{
    if (row == N - 1 && col == N)
        return 1;
    if (col == N)
    {
        row++;
        col = 0;
    }

    if (*(grid + row * N + col) > 0)
        return solveSudoku(grid, row, col + 1);
 
    for (int num = 1; num <= N; num++)
    {
        if (isSafe(grid, row, col, num)==1)
        {

            *(grid + row * N + col) = num;
            if (solveSudoku(grid, row, col + 1)==1)
                return 1;
        }
       
        *(grid + row * N + col) = 0;
    }
    return 0;
}
 
int main()
{
    int grid[N][N] = {  {7,0,4,0,0,0,8,0,0},
                        {0,9,0,0,8,7,6,0,0},
                        {0,0,0,0,4,0,7,0,1},
                        {3,0,0,2,0,0,0,6,7},
                        {0,2,0,6,0,3,0,4,0},
                        {6,4,0,0,0,9,0,0,5},
                        {1,0,6,0,3,0,0,0,0},
                        {0,0,9,1,6,0,0,8,0},
                        {0,0,2,0,0,0,4,0,6}};
    
    int *ptr = &grid[0][0];

    if (solveSudoku(ptr, 0, 0)==1)
        print(ptr);
    else
        printf("No solution exists");
    // for(int i = 0; i < N; i ++)
    //     printf("%d ", *(ptr + 4*9 + i));
    // printf("%d ", *(ptr + 4*9 + 5));
    // printf("%d", grid[4][5]);
    return 0;
}