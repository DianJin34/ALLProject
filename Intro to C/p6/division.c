////////////////////////////////////////////////////////////////////////////////
// Main File:        -
// This File:        divison.c
// Other Files:      mySigHandler.c sendsig.c
// Semester:         CS 354 Lecture 00? Fall 2022
// Instructor:       deppeler
// 
// Author:           Dian Jin
// Email:            djin34@wisc.edu
// CS Login:         dian
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
//////////////////////////// 80 columns wide ///////////////////////////////////
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define BUFFER_SIZE 100

int success_counter = 0;

/* handle when divide by zero
 *
 * when the second integer is set to 0, exit.
 * 
 */

void fpe_handler()
{
    printf("Error: a division by 0 operation was attempted.\n");
    printf("Total number of operations completed successfully: %d\n", success_counter);
    printf("The program will be terminated.");
    exit(0);
}

/* handle when interrupt by Ctrl-c
 *
 * when user try to interrupt, call this function and exit.
 * 
 */
void interrupt_handler()
{
    printf("\n");
    printf("Total number of operations completed successfully: %d\n", success_counter);
    printf("The program will be terminated.");
    exit(0);
}

/* Main fuction
 * 
 * this main function is use to perform the divide by zero algorithm in inifite loop
 * and call the handler that catches interrupt and divide by zero
 * argc, count the number of argument for stdin
 * argv, the array of argument for stdin
 * 
 */
int main(int argc, char **argv)
{
    struct sigaction f_action;
    memset(&f_action, 0, sizeof f_action);
    f_action.sa_handler = fpe_handler;
    if (sigaction(SIGFPE, &f_action, NULL) != 0)
    {
        printf("Error: binding SIGFPE Handle\n");
        exit(1);
    }

    struct sigaction i_action;
    memset(&i_action, 0, sizeof i_action);
    i_action.sa_handler = interrupt_handler;
    if (sigaction(SIGINT, &i_action, NULL) != 0)
    {
        printf("Error: binding SIGUSR1 Handle\n");
        exit(1);
    }

    int a;
    int b;
    char buffer[BUFFER_SIZE];
    while (1)
    {
        printf("Enter first integer: ");
        if(fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            printf("Didn't get anything\n");
            exit(1);
        }
        a = atoi(buffer);
        printf("Enter second integer: ");
        if(fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            printf("Didn't get anything\n");
            exit(1);
        }
        b = atoi(buffer);
        printf("%d / %d is %d with a remainder of %d\n", a, b, a / b, a % b);
        success_counter++;
    }
    return 0;
}