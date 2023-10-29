////////////////////////////////////////////////////////////////////////////////
// Main File:        -
// This File:        sendsig.c
// Other Files:      divison.c mySigHandler.c
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

/* Main fuction
 * 
 * Case 1: when there is -i, send SIGINT
 * Case 2: when there is -u, send SIGUSR1
 * Case 3: when neither -i or -u, exit.
 * argc, count the number of argument for stdin
 * argv, the array of argument for stdin
 * 
 */

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: sendsig <signal type> <pid>\n");
        exit(1);
    }

    pid_t pid = atoi(argv[2]);
    if (strlen(argv[1]) != 2)
    {
        printf("Invalid type of signal, use either -i or -u\n");
        exit(1);
    }

    switch (argv[1][1])
    {
    case 'i':
        if(kill(pid, SIGINT) == -1)
        {
            printf("Didn't kill success\n");
            exit(1);
        }
        break;
    case 'u':
        if(kill(pid, SIGUSR1) == -1)
        {
            printf("Didn't kill success\n");
            exit(1);
        }
        break;
    default:
        printf("Invalid type of signal, use either -i or -u\n");
        exit(1);
        break;
    }
}