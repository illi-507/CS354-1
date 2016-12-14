/* Title: P6 intdate.c
* Description: Handles SIGALRM and SIGINT signals
*              SIGALRM prints out date every 3 seconds
*              SIGINT - 5 SIGINTS kills the program
*
* Author: Kenny Gao
* Email: kgao9@wisc.edu
* CS Login: kenny
* Section: 2
*
* Pair Partner: Bingxin Zhang
* Email: bzhang88@wisc.edu
* CS Login: bingxin
* Section: 2
*/

//header files
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

//var for setting up handler
struct sigaction action;

//for number of interrupts
int keyboardInturrupts = 0;

//constant to say when alram will rearm
int const SECONDS_TO_RING = 3;

/**
* The purpose of this method is to handle a SIGALRM and print out date and set new SIGALRM
*
* no params
* no returns
*/

void alarm_handler()
{
    //gets the current time
    time_t curr_time;
    time(&curr_time);

    //check for errors
    if(curr_time == (time_t) -1)
    {
        fprintf(stderr, "ERROR: Could not get current time");
        exit(0);
    }

    //converts time struct to string
    char* dateString = ctime(&curr_time);

    //check for errors
    if(!dateString)
    {
        fprintf(stderr, "ERROR: Could not get current date string");
        exit(0);
    }

    //print out current time
    printf("current time is %s", dateString);

    //set new SIGALRM signal to go off in 3 seconds
    alarm(SECONDS_TO_RING);
}

/**
* The purpose of this method is to handle a SIGINT - after 5 of those, program exits
*
* no params
* no returns
*/

void keyboard_inturrupts_handler()
{
    //increment keyboard interrupts handled
    keyboardInturrupts++;

    //if 5 have been handled, exit
    if(keyboardInturrupts == 5)
    {
        printf("\nFinal Control+c caught. Exiting\n");
        exit(0);
    }

    //inform user how many interrupts they have left
    printf("\nControl+c caught. %d more before program is ended\n", 5 - keyboardInturrupts);
}

/**
* THe purpose of this method is to initialize the sigaction struct
*
* no parameters
* no returns
*/

void init()
{
    //create struct in heap
    struct sigaction *temptest = memset(&action,0,sizeof(action));

    //test for errors
    if(temptest == NULL)
    {
        fprintf(stderr, "ERROR could not allocate space for sigaction");
    }

    //create handler
    action.sa_handler = alarm_handler;
    
    //block signals of type being handled
    sigemptyset(&action.sa_mask);

    //restart system calls if possible
    action.sa_flags = SA_RESTART;
}

int main()
{
    //initialize struct
    init();
    
    //set alarm
    alarm(SECONDS_TO_RING);

    //set handler for SIGALRM
    int error = sigaction(14, &action, NULL);

    //check if error occured
    if(error == -1)
    {
        char* err = "ERROR: Could not replace current sigaction with a new sigaction";
        fprintf(stderr, err);
        exit(0);
    }

    //set new handler
    action.sa_handler = keyboard_inturrupts_handler;
    
    //set handler for SIGINT
    error = sigaction(2, &action, NULL);

    //check for error
    if(error == -1)
    {
        char* err = "ERROR: Could not replace current sigaction with a new sigaction";
        fprintf(stderr, err);
        exit(0);
    }

    //inform user
    printf("Date will be printed every 3 seconds.\n");
    printf("Enter ^C to end the program: \n");

    //empty while
    while(1)
    {
    }
}
