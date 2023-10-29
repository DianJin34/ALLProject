////////////////////////////////////////////////////////////////////////////////
// Main File:        -
// This File:        mySigHandler.c
// Other Files:      divison.c sendsig.c
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

int usr1_counter = 0;

/* handle the alarm
 *
 * check the pid and real time, and handle the alarm every 4 seconds
 */

void alarm_handler()
{
    pid_t pid = getpid();

    time_t seconds;
    if(time(&seconds) == -1)
    {
        printf("Error, second < 0\n");
        exit(1);
    }
    char *formatted_time = ctime(&seconds);
    if(formatted_time == NULL)
    {
        printf("Error\n");
        exit(1);
    }
    printf("PID: %d CURRENT TIME: %s", pid, formatted_time);
    alarm(4);
}

/* handle the sigusr1
 *
 * when there is a sending sigusr1, function will handle it.
 */
void sigusr1_handler()
{
    printf("SIGUSR1 handled and counted!\n");
    usr1_counter++;
}

/* handle when interrupt
 *
 * when user try to interrupt, call this function and exit.
 * 
 */
void interrupt_handler()
{
    printf("\n");
    printf("SIGINT handled.\n");
    printf("SIGUSR1 was handled %d times. Exiting now.\n", usr1_counter);
    exit(0);
}

/* Main fuction
 * 
 * this main function is use to perform the everything above and none stop
 * argc, count the number of argument for stdin
 * argv, the array of argument for stdin
 * 
 */

int main(int argc, char **argv)
{
    alarm(4);

    struct sigaction a_action;
    memset(&a_action, 0, sizeof a_action);
    a_action.sa_handler = alarm_handler;
    if (sigaction(SIGALRM, &a_action, NULL) != 0)
    {
        printf("Error: binding SIGALRM Handle\n");
        exit(1);
    }

    struct sigaction u_action;
    memset(&u_action, 0, sizeof u_action);
    u_action.sa_handler = sigusr1_handler;
    if (sigaction(SIGUSR1, &u_action, NULL) != 0)
    {
        printf("Error: binding SIGUSR1 Handle\n");
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

    printf("PID and time print every 4 seconds.\n");
    printf("Type Ctrl-C to end the program.\n");

    while (1)
    {
    }
    return 0;
}