/******************************************************************************
 * FILENAME: mem.c
 * AUTHOR:   cherin@cs.wisc.edu <Cherin Joseph>
 * MODIFIED BY: Kenny Gao, section 2
 * DATE:     20 Nov 2013
 * PROVIDES: Contains a set of library functions for memory allocation
 * *****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

/* this structure serves as the header for each block */
typedef struct block_hd{
  /* The blocks are maintained as a linked list */
  /* The blocks are ordered in the increasing order of addresses */
  struct block_hd* next;

  /* size of the block is always a multiple of 4 */
  /* ie, last two bits are always zero - can be used to store other information*/
  /* LSB = 0 => free block */
  /* LSB = 1 => allocated/busy block */

  /* So for a free block, the value stored in size_status will be the same as the block size*/
  /* And for an allocated block, the value stored in size_status will be one more than the block size*/

  /* The value stored here does not include the space required to store the header */

  /* Example: */
  /* For a block with a payload of 24 bytes (ie, 24 bytes data + an additional 8 bytes for header) */
  /* If the block is allocated, size_status should be set to 25, not 24!, not 23! not 32! not 33!, not 31! */
  /* If the block is free, size_status should be set to 24, not 25!, not 23! not 32! not 33!, not 31! */
  int size_status;

}block_header;

/* Global variable - This will always point to the first block */
/* ie, the block with the lowest address */
block_header* list_head = NULL;


/* Function used to Initialize the memory allocator */
/* Not intended to be called more than once by a program */
/* Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated */
/* Returns 0 on success and -1 on failure */
int Mem_Init(int sizeOfRegion)
{
  int pagesize;
  int padsize;
  int fd;
  int alloc_size;
  void* space_ptr;
  static int allocated_once = 0;
  
  //if Mem_Init has already allocated space, tell user
  if(0 != allocated_once)
  {
    fprintf(stderr,"Error:mem.c: Mem_Init has allocated space during a previous call\n");
    return -1;
  }

  //if size of region is invalid, tell user
  if(sizeOfRegion <= 0)
  {
    fprintf(stderr,"Error:mem.c: Requested block size is not positive\n");
    return -1;
  }

  /* Get the pagesize */
  pagesize = getpagesize();

  /* Calculate padsize as the padding required to round up sizeOfRegion to a multiple of pagesize */
  padsize = sizeOfRegion % pagesize;
  padsize = (pagesize - padsize) % pagesize;

  alloc_size = sizeOfRegion + padsize;

  /* Using mmap to allocate memory */
  fd = open("/dev/zero", O_RDWR);
  if(-1 == fd)
  {
    fprintf(stderr,"Error:mem.c: Cannot open /dev/zero\n");
    return -1;
  }
 space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (MAP_FAILED == space_ptr)
  {
    fprintf(stderr,"Error:mem.c: mmap cannot allocate space\n");
    allocated_once = 0;
    return -1;
  }
  
  allocated_once = 1;
  
  /* To begin with, there is only one big, free block */
  list_head = (block_header*)space_ptr;
  list_head->next = NULL;
  /* Remember that the 'size' stored in block size excludes the space for the header */
  list_head->size_status = alloc_size - (int)sizeof(block_header);

  return 0;
}

int indexBytes()
{
    block_header* curr = list_head;

    int indexBytes = 0;

    while(curr)
    {
        indexBytes += sizeof(block_header) + curr -> size_status;
        curr = curr -> next;
    }

    return indexBytes;
}


/**
* the purpose of this method is to find the address of the next split
* and return it
*
* no parameters
* @return block header pointer to split
*/

block_header* findNextBlock()
{
    int indexBytes = indexBytes();

    char* byte = (char*)list_head;
    return (block_header*)(byte + indexBytes);
}

block_header* bestFit(int size)
{
    block_header* curr = list_head;
    block_header best = NULL;

    while(curr)
    {
        //if size can fit and the block is available
        if(curr -> size_status >= size && !(curr -> size_status & 1))
        {
            if(!best)
            {
                best = curr;
            }

            //if smaller, update best
            else if(best -> status_size > curr -> size_status)
            {
                best = curr;
            }
        }
       
        curr = curr -> next;
    }

    return best;
}

int findIndex(char* ptr)
{
    return char*(list_head) - ptr;
}

int allocSize(int size)
{
     /* Calculate padsize as the padding for a multiple of 4*/
     int padSize = size % 4;
     padSize = (4 - padSize) % 4;
     int allocSize = padSize + size;
     return allocSize;
}

/* Function for allocating 'size' bytes. */
/* Returns address of allocated block on success */
/* Returns NULL on failure */
/* Here is what this function should accomplish */
/* - Check for sanity of size - Return NULL when appropriate */
/* - Round up size to a multiple of 4 */
/* - Traverse the list of blocks and allocate the best free block which can accommodate the requested size */
/* -- Also, when allocating a block - split it into two blocks when possible */
/* Tips: Be careful with pointer arithmetic */
void* Mem_Alloc(int size)
{
    /* Your code should go in here */
    //see if init was actually called
    if(!list_head)
    {
        fprintf(stderr, "ERROR: list was not initialized");
        return NULL;
    }

    //check to see if the list is of size 1
    if(!(list_head -> next))
    {
       //see if the linked list was already allocated
       if(list_head -> size_status & 1)
       {
           return NULL;
       }

       int allocSize = allocSize();

       if(allocSize > list_head -> size_status)
       {
           return NULL;
       }

       else if(allocSize <= list_head -> size_status - sizeof(block_header) - 4)
       {
           //split list into two if there's room to create a block header with space for at least 4 bytes of data
           int newBlockSize = list_head -> size_status - sizeof(block_header) - allocSize;
           list_head -> size_status = allocSize;
           
           block_header* next = findNextBlock();

           int index = indexBytes();
           next -> size_status = newBlockSize;

           list_head -> next = next;

           list_head[index] = *next;       
           return (list_head + 1);
       }

       else
       {
           (list_head -> size_status)++;
           return (list_head + 1);
       }
    }

    //not one node in list
    else
    {
        block_header* best = bestFit(size);

        int padSize = size % 4;
    }
}

/* Function for freeing up a previously allocated block */
/* Argument - ptr: Address of the block to be freed up */
/* Returns 0 on success */
/* Returns -1 on failure */
/* Here is what this function should accomplish */
/* - Return -1 if ptr is NULL */
/* - Return -1 if ptr is not pointing to the first byte of a busy block */
/* - Mark the block as free */
/* - Coalesce if one or both of the immediate neighbours are free */
int Mem_Free(void *ptr)
{
    /* Your code should go in here */
    //if ptr is null, return -1
    if(!ptr)
        return -1;

    //if ptr points to a block that isn't allocated, return -1
    block_header* point = (block_header*)ptr;

    if(!(point -> size_status & 1))
        return -1;  

    return 0;
}

/* Function to be used for debug */
/* Prints out a list of all the blocks along with the following information for each block */
/* No.      : Serial number of the block */
/* Status   : free/busy */
/* Begin    : Address of the first useful byte in the block */
/* End      : Address of the last byte in the block */
/* Size     : Size of the block (excluding the header) */
/* t_Size   : Size of the block (including the header) */
/* t_Begin  : Address of the first byte in the block (this is where the header starts) */
void Mem_Dump()
{
  int counter;
  block_header* current = NULL;
  char* t_Begin = NULL;
  char* Begin = NULL;
  int Size;
  int t_Size;
  char* End = NULL;
  int free_size;
  int busy_size;
  int total_size;
  char status[5];

  free_size = 0;
  busy_size = 0;
  total_size = 0;
  current = list_head;
  counter = 1;
  fprintf(stdout,"************************************Block list***********************************\n");
  fprintf(stdout,"No.\tStatus\tBegin\t\tEnd\t\tSize\tt_Size\tt_Begin\n");
  fprintf(stdout,"---------------------------------------------------------------------------------\n");
  while(NULL != current)
  {
    t_Begin = (char*)current;
    Begin = t_Begin + (int)sizeof(block_header);
    Size = current->size_status;
    strcpy(status,"Free");
    if(Size & 1) /*LSB = 1 => busy block*/
    {
      strcpy(status,"Busy");
      Size = Size - 1; /*Minus one for ignoring status in busy block*/
      t_Size = Size + (int)sizeof(block_header);
      busy_size = busy_size + t_Size;
    }
    else
    {
      t_Size = Size + (int)sizeof(block_header);
      free_size = free_size + t_Size;
    }
    End = Begin + Size;
    fprintf(stdout,"%d\t%s\t0x%08lx\t0x%08lx\t%d\t%d\t0x%08lx\n",counter,status,(unsigned long int)Begin,(unsigned long int)End,Size,t_Size,(unsigned long int)t_Begin);
    total_size = total_size + t_Size;
    current = current->next;
    counter = counter + 1;
  }
  fprintf(stdout,"---------------------------------------------------------------------------------\n");
  fprintf(stdout,"*********************************************************************************\n");

  fprintf(stdout,"Total busy size = %d\n",busy_size);
  fprintf(stdout,"Total free size = %d\n",free_size);
  fprintf(stdout,"Total size = %d\n",busy_size+free_size);
  fprintf(stdout,"*********************************************************************************\n");
  fflush(stdout);
  return;
}
