///////////////////////////////////////////////////////////////////////////////
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission, CS 354 Spring 2022, Deb Deppeler
////////////////////////////////////////////////////////////////////////////////
// Main File:        myMagicSquare.c
// This File:        myMagicSquare.c
// Other Files:      (name of all other files if any)
// Semester:         CS 354 Fall 2022
// Instructor:       deppeler
//
// Author:           Dian Jin
// Email:            djin34@wisc.edu
// CS Login:         dian
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   Fully acknowledge and credit all sources of help,
//                   including Peer Mentors, Instructors, and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   Avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of
//                   of any information you find.
////////////////////////////////////////////////////////////////////////////////
   

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure that represents a magic square
typedef struct {
    int size;           // dimension of the square
    int **magic_square; // pointer to heap allocated magic square
} MagicSquare;

/* TODO:
 * Prompts the user for the magic square's size, reads it,
 * checks if it's an odd number >= 3 (if not display the required
 * error message and exit), and returns the valid number.
 */
void Siamese(int row, int col, int **square, int num, int n);

int getSize() {
    int size;
    printf("Enter magic square's size (odd integer >=3)\n");
    scanf("%d", &size);
    if (size%2 == 0 || size < 3)
    {
        exit(1);
        printf("Magic square size must be odd.");
    }
    return size;   
} 
   
/* TODO:
 * Makes a magic square of size n using the alternate 
 * Siamese magic square algorithm from assignment and 
 * returns a pointer to the completed MagicSquare struct.
 *
 * n the number of rows and columns
 */

MagicSquare *generateMagicSquare(int n) {
    // init square
    int **square = (int**)calloc(n, sizeof(int*));
    for (int i = 0; i < n; i++)
        *(square+i)= (int*)calloc(n, sizeof(int));
    
    if(square == NULL)
        exit(1);

    Siamese(0, n/2, square, 1, n);

    MagicSquare *Magic_Square = malloc(sizeof(MagicSquare));
    if(Magic_Square == NULL)
        exit(1);

    Magic_Square ->magic_square = square;
    Magic_Square ->size = n;
    return Magic_Square;    
} 

/* TODO:  
 * Opens a new file (or overwrites the existing file)
 * and writes the square in the specified format.
 *
 * magic_square the magic square to write to a file
 * filename the name of the output file
 */
void fileOutputMagicSquare(MagicSquare *magic_square, char *filename) {
    int N = (*magic_square).size;
    int **square = (*magic_square).magic_square;
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "%d \n", N);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N - 1; j++)
            fprintf(fp, "%d,", *(*(square + i) + j));
        fprintf(fp, "%d\n", *(*(square + i) + N-1));
    }
}

/* TODO:
 * Generates a magic square of the user specified size and
 * output the quare to the output filename
 *
 * Add description of required CLAs here
 */
int main(int argc, char **argv) {
    // TODO: Check input arguments to get output filename
    if (argc != 2)
    {
        printf("you need two arguments.\n");
        exit(1);
    }
    
    // Open the file and check if it opened successfully.
    FILE *fp = fopen(*(argv + 1), "w");
    if (fp == NULL) {
        printf("Can't open file for writing.\n");
        exit(1);
    }
    // TODO: Get magic square's size from user
    int size = getSize();
    
    // TODO: Generate the magic square
    MagicSquare *square;
    square = generateMagicSquare(size);
    printf("%i", square->size);

    // TODO: Output the magic square
    fileOutputMagicSquare(square, argv[1]);
    for(int i = 0; i < square->size; i ++)
    {
        free(*(square -> magic_square + i));
    }
    free(square ->magic_square);
    free(square);
    square = NULL;
    // close the file
    if (fclose(fp) != 0) {
        printf("Error while closing the file.\n");
        exit(1);
    } 
    return 0;
} 


//     F22 deppeler myMagicSquare.c      

/* TODO:
 * this is a helper method that preferem the algorithm
 *
 * n the number of rows and columns
 * row the row of the 2d array
 * col the column of the 2d array
 * square the 2d array pointer
 * num the num that will put in the specific location of this array
 */
void Siamese(int row, int col, int **square, int num, int n){
    *(*(square + row) + col) = num;
    // case 1: row is 0, col is not last
    if( row == 0 && col != n - 1)
    {
        if (*(*(square + n - 1) + col + 1) == 0)
        {
            Siamese(n - 1, col + 1, square, num + 1, n);
        }
        else
        {
            if(*(*(square + row + 1) + col) == 0)
                Siamese(row + 1, col, square, num + 1, n);
            else{}
        }

    }
    // case 2: row is 0, col is last
    if( row == 0 && col == n -1)
    {
        if (*(*(square + n - 1) + 0) == 0)
        {
            Siamese(n - 1, 0, square, num + 1, n);
        }
        else
        {
            if(*(*(square + row + 1) + col) == 0)
                Siamese(row + 1, col, square, num + 1, n);
            else{}
        }

    }
    // case 3: row is not 0, col is not last
    if( row != 0 && col != n - 1)
    {
        if (*(*(square + row - 1) + col + 1) == 0)
        {
            Siamese(row - 1, col + 1, square, num + 1, n);
        }
        else
        {
            // sub case: if row is last
            if( row == n - 1)
            {
                if(*(*(square + 0) + col) == 0)
                Siamese(0, col, square, num + 1, n);
                else{}
            }
            else
            {
                if(*(*(square + row + 1) + col) == 0)
                Siamese(row + 1, col, square, num + 1, n);
                else{}
            }
        }

    }
    // case 4: row is not 0, col is last 
    if( row != 0 && col == n -1)
    {
        if (*(*(square + row - 1) + 0) == 0)
        {
            Siamese(row - 1, 0, square, num + 1, n);
        }
        else
        {
            // sub case: if row is last
            if( row == n - 1)
            {
                if(*(*(square + 0) + col) == 0)
                Siamese(0, col, square, num + 1, n);
                else{}
            }
            else
            {
                if(*(*(square + row + 1) + col) == 0)
                Siamese(row + 1, col, square, num + 1, n);
                else{}
            }
        }

    }
}