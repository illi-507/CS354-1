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

int findUpIndex()
{
    printf("in findUpIndex\n");
    //total size
    int total_size = 0;
    block_header* curr = list_head;

    while(curr)
    {
        total_size += curr -> size_status + sizeof(block_header);
        curr = curr -> next;
    }

    int list_head_length = total_size/sizeof(block_header);

    //Mem_Dump();

    int i;

    for(i = 0; i < list_head_length; i++)
    {
        //if(!(list_head + i))
          //  return i;
         
        if(list_head[i].size_status <= 0)
        {
            return i;
        }
    }

    return -1;
}

int findIndex(block_header* ptr)
{
    //printf("in findIndex\n");
    //total size
    int total_size = 0;
    block_header* curr = list_head;

    while(curr)
    {
        total_size += curr -> size_status + sizeof(block_header);
        curr = curr -> next;
    }

    int list_head_length = total_size/sizeof(block_header);

    //Mem_Dump();

    int i;
    curr = list_head;

    for(i = 0; i < list_head_length; i++)
    {
        if(curr == ptr)
            return i;

        else
        {
            curr++;
        }
    }

    return -1;
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

       /* Calculate padsize as the padding required to round up sizeOfRegion to a multiple of pagesize */
       int padSize = size % 4;
       padSize = (4 - padSize) % 4;
       int allocSize = padSize + size;

       if(allocSize > list_head -> size_status)
       {
           return NULL;
       }

       else if(allocSize <= list_head -> size_status - sizeof(block_header) - 4)
       {
           //split list into two if there's room to create a block header with space for at least 4 bytes of data
           block_header* next = (list_head + 1);

           next -> next = NULL;
           next -> size_status = list_head -> size_status - allocSize - sizeof(block_header);
           list_head -> next = next;
           list_head -> size_status = allocSize + 1;

           //return allocated node
           return list_head;
       }

       else
       {
           list_head -> size_status = allocSize + 1;
           return list_head;
       }
    }

    else
    {
        //printf("in else");
        //more than one node in linked list
        //flag for if there's no space to allocate
        int full = 1;

        /* Calculate padsize as the padding required to round up sizeOfRegion to a multiple of pagesize */
        int padSize = size % 4;
        padSize = (4 - padSize) % 4;
        int allocSize = padSize + size;

        //for each node in the linked list, if there's room for size
        //to be allocated, break
        //else we return null
        block_header* curr = list_head;
        while(curr)
        {
            //if there's unallocated space and the space available is >= allocsize
            if(!(curr -> size_status & 1) && curr -> size_status >= allocSize)
            {
                full = 0;
                break;
            }
           
            curr = curr -> next;
        }

        //if full, return null
        if(full)
        {
            return NULL;
        }

        else
        {
             //printf("not full\n");

             //size of linked list
             int linkedListSize = 0;
             //node with best fit
             block_header* best_fit = list_head;
             //current node
             block_header* curr = list_head;
             //first found flag
             int found = 0;
             
             while(curr)
             {
                 //if no non-full node has been found
                 if(!found)
                 {
                     //see if there's space available
                     if(!(curr -> size_status & 1) && curr -> size_status >= allocSize)
                     {
                         found = 1;
                         best_fit = curr;
                     }
                 }

                 else
                 {
                     //see if there's a better one
                     if(!(curr -> size_status & 1) && curr -> size_status >= allocSize)
                     {
                         if(curr -> size_status < best_fit -> size_status)
                         {
                             best_fit = curr;
                         }
                     }
                 }
                 

                 curr = curr -> next;
                 linkedListSize++;
             }

             //see if there's room to split
             //if there is, split
             if(best_fit -> size_status > allocSize + sizeof(block_header) + 4)
             {
                  
                  //printf("room to split");
                  int newAlloc = findUpIndex();
                  //printf("out of up index");
                  block_header* next = (list_head + newAlloc);
                  
                  //int update = findIndex(best_fit);

                  //block_header* next = (list_head + linkedListSize);

                  next -> size_status = best_fit -> size_status - allocSize - sizeof(block_header);

                  //update best fit
                  best_fit -> size_status = allocSize;
                  (best_fit -> size_status)++;

                  //if best fit is at the end, we add next to the end
                  //otherwise, we add it behind best fit
                  
                  if(best_fit -> next)
                  {
                      //printf("in here\n");
                      next -> next = best_fit -> next;
                      best_fit -> next = next;
                      return best_fit;
                  }

                  else
                  {
                      //printf("no man");
                      best_fit -> next = next;
                      next -> next = NULL;
                      //printf("got here\nsize_Status of free: %d", best_fit -> next -> size_status);
                      return best_fit;
                  }
             }

             //if there's isn't room, just allocate and return
             else
             {
                 (best_fit -> size_status)++;
                 return best_fit;
             }
        }
        
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

    int found = 0;
    int previous_index = 0;
    block_header* prev = list_head;
    block_header* curr = list_head;
    
    while(curr -> next)
    {
        if(curr -> next == point)
        {
            prev = curr;
            found = 1;
            break;
        }
        curr = curr -> next;
        previous_index++;
    }

    if(!found && point != list_head)
    {
        //printf("previous wasn't found and parameter wasn't head.");
        return -1;
    }

    //first find length of list
    int length = 0;

    curr = list_head;
    while(curr)
    {
        curr = curr -> next;
        length++;
    }

    //if parameter is head
    if(!found && point == list_head)
    {
        (point -> size_status)--;
        
        //see if next node's free
        if(!((point -> next -> size_status) & 1))
        {
            //if it is free, update size and set point's next node size_status to -1
            list_head -> size_status += list_head -> next -> size_status + sizeof(block_header);
            list_head -> next -> size_status = -1;

            block_header* secondNode = list_head -> next;
            list_head -> next = secondNode -> next;
            secondNode -> next = NULL;
            secondNode -> size_status = -1;

            //int indexDown = findDownIndex(point -> next);
            //list_head[indexDown].size_status = -1;
            return 0;
        }

        else
        {
            return 0;
             
        }
    }

    //if parameter is tail
    else if(!(point -> next))
    {
        (point -> size_status)--;
        if(!((prev -> size_status) & 1))
        {
            //if prev is also free
            //merge
            prev -> size_status = point -> size_status + sizeof(block_header);
            prev -> next = NULL;
            point -> size_status = -1;
            point -> next = NULL;
            return 0;
        }

        else
        {
            return 0;
        }
    }

    else
    {
        //could be anywhere in the list
        //decrement point's status size
        (point -> size_status)--;

        //see if we need to update
        if(!((point -> next -> size_status) & 1) && !((prev -> size_status) & 1))
        {
            //need to update twice
            prev -> size_status += point -> size_status + sizeof(block_header);
            prev -> size_status += point -> next -> size_status + sizeof(block_header);
            prev -> next = point -> next -> next;
            point -> size_status = -1;
            point -> next -> size_status = -1;
            block_header* next = point -> next;
            point -> next = NULL;
            next -> next = NULL;
            return 0;
        }

        else if(!((point -> next -> size_status) & 1))
        {
            //printf("update up\n");
            point -> next -> size_status += point -> size_status + sizeof(block_header);
            prev -> next = point -> next;
            point -> next = NULL;
            point -> size_status = -1;
            
            return 0;
        }

        else if(!((prev -> size_status) & 1))
        {
            //printf("update down");

            //move index
            prev -> size_status += point -> size_status + sizeof(block_header);
            prev -> next = point -> next;
            point -> next = NULL;
            point -> size_status = -1;
            return 0;
        }
        
        else
        {
            return 0;
        }
 
    }
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
