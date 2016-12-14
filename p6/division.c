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

//headers
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

//max keyboard input size
int const DIGITS = 13;

//count number of divisions completed
int numberDivisions = 0;

/**
* handles Divide by zero signal by printing out num divisions and exiting
*
* no params
* no returns
*/

void divide_by_zero_handler()
{
    fprintf(stderr, "A divide by zero error has occured.\n");
    fprintf(stderr, "Number of successful divisions: %d\n", numberDivisions);
    exit(0);
}

/**
* Handles SIGINT by printing out num divisions and exiting
*
* no params
* no returns
*/

void keyboard_interrupt_handler()
{
    fprintf(stderr, "\nNumber of successful divisions: %d\n", numberDivisions);
    exit(0);
}

int main()
{
    //create sigaction struct
    struct sigaction action;
 
    //initialize struct
    struct sigaction *temptest = memset(&action, 0, sizeof(action));

    //check for errors
    if(temptest == NULL)
    {
        fprintf(stderr, "ERROR: could not allocate space for sigaction");
    }

    //create handler
    action.sa_handler = divide_by_zero_handler;
    
    //set handler for divide by zero
    int error = sigaction(8, &action, NULL);

    //check for errors
    if(error == -1)
    {
        char* err = "ERROR: Could not replace current sigaction with a new sigaction";
        fprintf(stderr, err);
        exit(0);
    }

    //set new handler    
    action.sa_handler = keyboard_interrupt_handler;

    //set handler for SIGINT
    error = sigaction(2, &action, NULL);

    //check for errors
    if(error == -1)
    {
        char* err = "ERROR: Could not replace current sigaction with a new sigaction";
        fprintf(stderr, err);
        exit(0);
    }

    //initialize ints and result
    int firstInteger = 0;
    int secondInteger = 0;
    int quotient = 0;
    int remainder = 0;

    while(1)
    {
        //create strings to hold user input
        char first[DIGITS];
        char second[DIGITS];

        //prompt user
        printf("Enter first integer: ");
 
        //get input
        fgets(first, DIGITS, stdin);

        //check for errors
        if(first == NULL)
        {
            fprintf(stderr, "ERROR: could not get keyboard input");
        }

        //parse input
        firstInteger = atoi(first);

        //prompt user
        printf("Enter second integer: ");
        
        //get user input
	fgets(second, DIGITS, stdin);

        //check for errors
        if(second == NULL)
        {
            fprintf(stderr, "ERROR: could not get keyboard input");
        }
        
        //parse input
        secondInteger = atoi(second);

        //calculate quotient and remainder
        quotient = firstInteger / secondInteger;
        remainder = firstInteger % secondInteger;

        //inform user of answer
        printf("%d / %d is %d with a remainder of %d\n", firstInteger, secondInteger, quotient, remainder);

        //increament num divisions
        numberDivisions++;
    }
}
