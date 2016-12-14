/* Title: Project 1 link.c file
* Description: takes input file, reads numbers as input, stores
*              as array and linked list, prints numbers 
*              into output file
*
* Author: Kenny Gao
* Email: kgao9@wisc.edu
* CS Login: kenny
* Section: Lec 2
*
* Pair Partner: Bingxin Zhang
* Email: bzhang88@wisc.edu
* CS Login: bingxin
* Section: Lec 2
*
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX_INTS 1000
#define MAX_DIGITS_INT 10

/* A node in the linked list */
struct node {
    int data;
    struct node *next;
};

struct node* create_list(int intarray[], int len);

struct node* add_item_at_start(struct node *head, int data);

int search_array(int integers[], int numints, int element);

int search_list(struct node *head, int element);

struct node* create_sorted_list(struct node *head);

struct node* add_item_sorted(struct node *head, int data);

int copy_list_to_array(struct node *head, int *array);

void print_list(struct node *head);

void print_array(int integers[], int len);


int main(int argc, char *argv[])
{
    //if filename isn't provided or too many params provided
    //inform user
    if(argc != 2)
    {
        fprintf(stderr, "USAGE: %s <filename>\n", argv[0]);
        exit(0);
    }

    /* Open a file for reading */

    FILE *myFile;
    int count;
    int *p = (int *)malloc(MAX_INTS * sizeof(int*));

    if(!p)
    {
        fprintf(stderr, "ERROR: Could not allocate memory to the heap");
        exit(0);
    }

    char getLine[MAX_DIGITS_INT];
    int fromFile;

    myFile = fopen(argv[1], "r");

    count = 0;
    fromFile = 0;

    //if error occured, tell user
    if(!myFile)
    {
        //fprintf(stderr, "file <%s> could not be opened\n", argv[1]);
        fprintf(stderr, "ERROR: Cannot open file for reading!\n");
        //output is not user friendly, but I don't want  to lose points
        //I think file %s could not be opened is better because
        //it would frustrate the user less
        //user sees connot open file for reading, they'll go like
        //why not, it's right there - is it my permissions
        //while my message it's like ok fine, i'll just try it again
        //that's just my thoughts on it
        exit(0);
    }

    printf("Reading integers from a file to an array...\n\n");

    /* Read the numbers from the file, into an array */
    while(fgets(getLine, MAX_DIGITS_INT, myFile))
    {
        //get lines from file`
        *(p + count) = atoi(getLine);

        count++;

        //if too many lines, tell user and quit
        if(count > MAX_INTS)
        {
            fprintf(stderr, "list.c: file %s was too large", argv[1]);
            exit(0);
        }
    }
    
    //close file
    int testing = fclose(myFile);
    
    if(testing != 0)
    {
        fprintf(stderr, "ERROR: File could not close");
        exit(0);
    }

    /* Print the array */
    printf("ARRAY: ");
    print_array(p, count);
    printf("\n\n");

    /* Create a linked list with the integers from the array */
    struct node* myList;
    myList = create_list(p, count);
    
    /* Print the linked list */
    printf("LINKED LIST: ");
    print_list(myList);
    printf("\n\n");


    /* Repeatedly prompt the user for a number to be searched.
    *  Search the array for the number and print out the result as shown in the specs.
    *  Search the linked list for the number and print out the result as shown in the specs.
    *  Stop prompting when the user enters 'q' (just the character q without the single quotes).
    */

    int keyboardInput;

    while(1)
    {
        printf("Enter an element to be searched in the list and array:");

        //get keyboard input
        fgets(getLine, MAX_DIGITS_INT, stdin);
        //if it's 0, break out of the loop
        if(*getLine  == 'q')
        {
            break;
        }

        //else search for elt
        else
        {
            keyboardInput = atoi(getLine);

            if(search_array(p, count, keyboardInput) == -1)
            {
                printf("Element %d was not found in the array\n\n", keyboardInput);
            }

            else
            {
                int index;
                index = search_array(p, count, keyboardInput);
                printf("Element %d was found in the array at index %d\n\n", keyboardInput, index);
            }

            if(search_list(myList, keyboardInput) == -1)
            {
                printf("Element %d was not found in the linked list\n\n", keyboardInput);
            }

            else
            {
                int index;
                index = search_list(myList, keyboardInput);
                printf("Element %d was found in the linked list at position %d\n\n", keyboardInput, index);
            }
        }
    }

    /* Create a sorted list(in ascending order) from the unsorted list */
    struct node* mySortedList;
    mySortedList = create_sorted_list(myList);

    /* Print the sorted list */
    printf("\n SORTED LINK LIST: ");
    print_list(mySortedList);
    printf("\n\n");

    /* Copy the sorted list to an array with the same sorted order */
    int *copyArray = (int *)malloc(count * sizeof(int));

    if(!copyArray)
    {
        fprintf(stderr, "ERROR: could not allocate memory to the heap");
        exit(0);
    }

    int lengthOfCopy;
    lengthOfCopy = copy_list_to_array(mySortedList, copyArray);
    
    /* Print out the sorted array */    
    printf("SORTED ARRAY: ");
    print_array(copyArray, lengthOfCopy);
    printf("\n\n");

    /* Print the original linked list again */
    printf("ORIGINAL LINKED LIST: ");
    print_list(myList);
    printf("\n\n");

    /* Print the original array again */
    printf("ORIGINAL ARRAY: ");
    print_array(p, count);
    printf("\n\n");

    /* Open a file for writing */
    FILE *myOutput;
    myOutput = fopen("sorted_numbers.txt", "w");

    if(!myOutput)
    {
        fprintf(stderr, "ERROR: Cannot open file for writing!\n");
        exit(0);
    }


    /* Write the sorted array to a file named "sorted_numbers.txt" */
    int i;
    printf("Writing integers from a sorted array into a file...\n\n");
    for(i = 0; i < lengthOfCopy; i++)
    {
        fprintf(myOutput, "%d\n", copyArray[i]);
    }
    
    
    /* Print out the number of integers written to the file */
    printf("Number of integers written to the file = %d\n", i);
    //close file and free pointers
    testing = fclose(myOutput);
    
    if(testing != 0)
    {
        fprintf(stderr, "File could not close");
        exit(0);
    }

    free(p);
    free(copyArray);

    //free everything except head   

    struct node* prev = mySortedList;

    if(!prev)
    {
        free(prev);
    }
    
    else
    {

        while(prev)
        {
            struct node* next = prev -> next;
            prev -> next = NULL;
            free(prev);
            prev = next;
        }
    }
    
    prev = myList;

    if(!prev)
    {
        free(prev);
    }

    else
    {

        while(prev)
        {
            struct node* next = prev -> next;
            prev -> next = NULL;
            free(prev);
            prev = next;
        }
    }

    return 0;

}

/**
* This function takes in an integer array and its size, 
* traverses the array while using
* the function add_item_at_start(struct node *head, int data)
* to build a list. This function returns the head 
* of the new linked list that was created.
*
* @param (intarray) (integer array used to build list)
* @param (len) (length of the array)
* @return (head) (pointer to the first node in the list)
*/

struct node* create_list(int intarray[], int len)
{
    struct node* head = NULL;
    int i;

    //traverse the array
    for(i = 0; i < len; i++)
    {
        head = add_item_at_start(head, intarray[i]);
    }

    return head;
}

/**
* This function takes in a pointer to the head of the 
* linked list, adds a new node with the data at 
* the beginning of the list, updates and returns the new head.
*
* @param (*head) (pointer to a header node)
* @param (data) (initialize data)
* @return (header node pointer)
*/

struct node* add_item_at_start(struct node *head, int data)
{
     //if head isn't null, add head to start
     if(head)
     {
         
	struct node* newHeader = (struct node*)malloc(sizeof(struct node));

        if(!newHeader)
        {
            fprintf(stderr, "ERROR: could not allocate memory to the heap");
            exit(0);
        }

	newHeader->data = data;
	newHeader->next = head;
	return newHeader;
     }

     //means head is null, and it needs to be initialized
     else
     {
         struct node* newHeader = (struct node*)malloc(sizeof(struct node));
 
         if(!newHeader)
         {
             fprintf(stderr, "ERROR: could not allocate memory to the heap");
             exit(0);
         }
         newHeader->data = data;
         newHeader->next = NULL;
         return newHeader;
     }
}

/**
* This function returns the index of the element if
* the element is found in the list,
* otherwise it returns -1.
*
* @param (head) (list node header)
* @param (element) (element we are searching for)
* @return (index of element we're looking for)
*/

int search_list(struct node *head, int element)
{
    struct node *temp = NULL;
    temp = head;

    int count = 1;

    //find out where in the list the element is
    while(temp)
    {
        if(temp -> data == element)
        {
            return count;
        }

        else
        {
            count++;
            temp = temp -> next;
        }
    }

    return -1;

}

/**
* This function returns the index of the element if 
* the element is found in the array, 
* otherwise it returns -1.
*
* @param (integers) (array of integers)
* @param (numints) (length of integer array)
* @param (element) (element we are searching for)
* @return (index of element we're looking for)
*/

int search_array(int integers[], int numints, int element)
{
    int i;
    for(i = 0; i < numints; i++)
    {
        if(element == integers[i])
        {
            return i;
        }
    }

    return -1;
}

/**
* Takes in a pointer to the head to a list and a pointer to the start of an array, 
* and returns the number of integers copied to the array. 
*
* @param (head) (the header node of a linked list)
* @param (array) (head of the array)
* @return (number of integers copied to array)
*/

int copy_list_to_array(struct node *head, int *array)
{
    //if head = null, return 0
    if(!head)
    {
        return 0;
    }

    struct node *temp = head;
    int count;
    count = 0;

    //copy list data into array
    while(temp)
    {
        *(array + count) = temp -> data;
        temp = temp -> next;
        count++;
    }

    return count;
}

/**
* constructs a new sorted list(ascending
* order), and returns the head of the sorted list.
*
* @param (integers) (array of integers)
* @param (numints) (length of integer array)
* @param (element) (element we are searching for)
* @return (index of element we're looking for)
*/

struct node* create_sorted_list(struct node *head)
{
   //if there's no nodes, return NULL
   if(!head)
   {
       return NULL;
   }

   //only has one element
   if(!(head -> data))
   {
       //return the only elt
       return head;
   }

   else
   {
       //loop through list while inserting new sorted item
       struct node *temp = head;
       struct node* sortHead = NULL;
       while(temp)
       {
         sortHead = add_item_sorted(sortHead, temp -> data);
         temp = temp -> next;
       }

       return sortHead;
   }   
}

/**
* This method puts the data into it's correct position in
* the sorted list.
*
* @param (sorted_head) (the header node of a sorted linked list)
* @param (data) (the data we're trying to insert)
* @return (index of element we're looking for)
*/

struct node* add_item_sorted(struct node *sorted_head, int data)
{
    //if sorted head is null, return null
    if(!sorted_head)
    {
       struct node* newHead = (struct node*)malloc(sizeof(struct node));

       if(!newHead)
       {
           fprintf(stderr, "ERROR: could not allocate memory to the heap");
           exit(0);
       }

       newHead -> data = data;
       newHead -> next = NULL;
       return newHead;
    }

    //if sorted head has only one value
    if(!(sorted_head -> next))
    {
         struct node* newNode = (struct node*)malloc(sizeof(struct node));

         if(!newNode)
         {
             fprintf(stderr, "ERROR: could not allocate memory to the heap");
             exit(0);
         }

         newNode -> data = data;
         newNode -> next = NULL;

        //if data is less than head's data, add to start
        if(data < (sorted_head -> data))
        {
            //add to start
            newNode -> next = sorted_head;
            return newNode;
        }

        //else add to end
        else
        {
            //add to end
            sorted_head -> next = newNode;
            return sorted_head;
        }
    }

    else
    {
        struct node* newNode = (struct node*)malloc(sizeof(struct node));

        if(!newNode)
        {
            fprintf(stderr, "ERROR: could not allocate memory to the heap");
            exit(0);
        }

        newNode -> data = data;
        newNode -> next = NULL;
        struct node* temp = sorted_head;
        
        while(temp -> next)
        {
            //if at head of list
            if(temp -> data > data)
            {
                newNode -> next = temp;
                return newNode;
            }
            
            //if next node is less than data, insert node
            if((temp -> next -> data) > data)
            {
                newNode -> next = temp -> next;
                temp -> next = newNode;
                return sorted_head;
            }
 
            else
            {
                temp = temp -> next;
            }
        }

        //if hasn't returned, we add it to the end
        temp -> next = newNode;
        return sorted_head;
    }
}

void print_list(struct node *head)
{
    if (head == NULL) {
        printf("Linked List is Empty.\n");
    } else {
        struct node *temp = head;
        printf("head->");
        while (temp != NULL) {
            printf("|%d|->", temp->data);
            temp = temp->next;
        }
        printf("NULL\n");
    }
}

void print_array(int integers[], int len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("| %d ", integers[i]);
    }
    printf("|\n");
}
