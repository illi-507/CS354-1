/* Name: Kenny Gao
 * Partner's name: BingXin Zhang
 * CS logins: kenny
 * Section(s): Lec 2 for both
 *
 *
 *
 * csim.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss. (I examined the trace,
 *  the largest request I saw was for 8 bytes).
 *  2. Instruction loads (I) are ignored, since we are interested in evaluating
 *  trans.c in terms of its data cache performance.
 *  3. data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus an possible eviction.
 *
 * The function printSummary() is given to print output.
 * Please use this function to print the number of hits, misses and evictions.
 * This is crucial for the driver to evaluate your work. 
 */
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include<stdbool.h>

#include "cachelab.h"

// #define DEBUG_ON 
#define ADDRESS_LENGTH 64

/****************************************************************************/
/***** DO NOT MODIFY THESE VARIABLE NAMES ***********************************/

/* Globals set by command line args */
int verbosity = 0; /* print trace if set */
int s = 0; /* set index bits */
int b = 0; /* block offset bits */
int E = 0; /* associativity */
char* trace_file = NULL;

/* Derived from command line args */
int S; /* number of sets S = 2^s In C, you can use "pow" function*/
int B; /* block size (bytes) B = 2^b In C, you can use "pow" function*/

/* Counters used to record cache statistics */
int miss_count = 0;
int hit_count = 0;
int eviction_count = 0;
/*****************************************************************************/


/* Type: Memory address 
 * Use this type whenever dealing with addresses or address masks
  */
typedef unsigned long long int mem_addr_t;

/* Type: Cache line
 * TODO 
 * 
 * NOTE: 
 * You might (not necessarily though) want to add an extra field to this struct
 * depending on your implementation
 * 
 * For example, to use a linked list based LRU,
 * you might want to have a field "struct cache_line * next" in the struct 
 */
typedef struct cache_line {
    char valid;
    mem_addr_t tag;
    struct cache_line* next;
} cache_line_t;

typedef cache_line_t* cache_set_t;   //pointer to lines - think like array of lines
typedef cache_set_t* cache_t; //pointer to sets - think like arrays of sets


/* The cache we are simulating */
cache_t cache;  

/* TODO - COMPLETE THIS FUNCTION
 * initCache - 
 * Allocate data structures to hold info regrading the sets and cache lines
 * use struct "cache_line_t" here
 * Initialize valid and tag field with 0s.
 * calculate S = 2^s
 * use S and E while allocating the data structures here
 */
void initCache()
{
    //calculate amount of sets in a cache
    S = pow(2,s);

    //malloc S sets
    cache = (cache_set_t*)malloc(sizeof(cache_set_t) * S);

    //check malloc
    if(!cache)
    {
        fprintf(stderr, "ERROR: could not allocate memory to the heap");
        exit(0);
    }

    //for each set, malloc E lines
    for(int set = 0; set < S; set++)
    {
        //for each line
        for(int line = 0; line < E; line++)
        {
            //allocate a single line
            cache_line_t* newLine = (cache_line_t*)malloc(sizeof(cache_line_t));
            
            //check malloc
            if(!cache)
            {
                fprintf(stderr, "ERROR: could not allocate memory to the heap");
                exit(0);
            }

            newLine -> valid = '0';
            newLine -> next = NULL;

            if(!cache[set])
            {
                cache[set] = newLine;
            }

            else
            {
                cache_line_t* temp = cache[set];

                while(temp -> next)
                {
                    temp = temp -> next;
                }

                temp -> next = newLine;
            }
        }
    }
}

 /*
 * TODO - COMPLETE THIS FUNCTION 
 * freeCache - free each piece of memory you allocated using malloc 
 * inside initCache() function
 */
void freeCache()
{
    //for each set in the cache
    for(int set = 0; set < S; set++)
    {
         //free set
         cache_line_t* prev = cache[set];

         //if only one line
         if(!(prev -> next))
         {
             //free line
             free(prev);
         }

         else
         {
             cache_line_t* curr = prev -> next;
             while(curr)
             {
                  //free all lines
                  free(prev);
                  prev = curr;
                  curr = prev -> next;
             }
         }    
    }
}


/* TODO - COMPLETE THIS FUNCTION 
 * accessData - Access data at memory address addr.
 *   If it is already in cache, increase hit_count
 *   If it is not in cache, bring it in cache, increase miss count.
 *   Also increase eviction_count if a line is evicted.
 *   
 *   You will manipulate data structures allocated in initCache() here
 *   Implement Least-Recently-Used (LRU) cache replacement policy
 */
void accessData(mem_addr_t addr)
{
    mem_addr_t sMask = pow(2,s) - 1;
    
    //get set index and tag bits
    mem_addr_t setIndex = sMask & (addr >> b);
    mem_addr_t tag = (addr >> (s + b));

    //get set
    cache_line_t* getSet = cache[setIndex];

    //see if there's only one line
    if(!(getSet -> next))
    {
        //if hit, update
        if(getSet -> tag == tag && getSet -> valid == '1')
        {
            hit_count++;
            if(verbosity)
                printf("hit");
            return;
        }

        //else see if empty line
        else
        {
            if(getSet -> valid == '0')
            {
                getSet -> valid = '1';
                getSet -> tag = tag;
                cache[setIndex] = getSet;
                miss_count++;
                if(verbosity)
                    printf("miss");
                return;
            }

            //need to evict
            else
            {
                getSet -> tag = tag;
                cache[setIndex] = getSet;
                miss_count++;
                if(verbosity)
                    printf("miss eviction");
                eviction_count++;
            }
        }
    }
    
    else
    {
        //more than one line
	//if the hit is the head, move head to end
	if(getSet -> tag == tag && getSet -> valid == '1')
        {
            cache[setIndex] = getSet -> next;
            cache_line_t* last = getSet;
            while(last -> next)
            {
                last = last -> next;
            }

            last -> next = getSet;
            getSet -> next = NULL; 
            hit_count++;
            if(verbosity)
                printf("hit");
            return;
        }

        else
        {
            cache_line_t* curr = getSet;
            while(curr -> next)
            {
                cache_line_t* nextNode = curr -> next;

                //if match found
                if(nextNode -> tag == tag && nextNode -> valid == '1')
                {
                    //if last node, update hit    
                    if(!(nextNode -> next))
                    {
                        hit_count++;
                        if(verbosity)
                            printf("hit");
                        return;
                    }
     
                    //update curr next
                    curr -> next = nextNode -> next;

                    //move next node to end
                    cache_line_t* last = curr;
                    while(last -> next)
                    {
                        last = last -> next;
                    }

                    last -> next = nextNode;
                    nextNode -> next = NULL;
                    hit_count++;
                    if(verbosity)
                        printf("hit");
                    return;
                }

                curr = curr -> next;
            }

            //see if head is empty
            //if it is, set isn't full
            if(getSet -> valid == '0')
            {
                //set getSet
                getSet -> valid = '1';
                getSet -> tag = tag;
                cache[setIndex] = getSet -> next;

                //move to back
                cache_line_t* last = cache[setIndex];
                while(last -> next)
                {
                    last = last -> next;
                }
                last -> next = getSet;
                getSet -> next = NULL;
                miss_count++;
                if(verbosity)
                    printf("miss");
                return;
            }

            //else evict
            else
            {
                getSet -> tag = tag;
                cache[setIndex] = getSet -> next;
               
                //move to back
                 cache_line_t* last = cache[setIndex];
                while(last -> next)
                {
                    last = last -> next;
                }
                last -> next = getSet;
                getSet -> next = NULL;
                
                miss_count++;
                if(verbosity)
                    printf("miss eviction");
                eviction_count++;
                return;
            }
        }
    }
}
/* TODO - FILL IN THE MISSING CODE
 * replayTrace - replays the given trace file against the cache 
 * reads the input trace file line by line
 * extracts the type of each memory access : L/S/M
 * YOU MUST TRANSLATE one "L" as a load i.e. 1 memory access
 * YOU MUST TRANSLATE one "S" as a store i.e. 1 memory access
 * YOU MUST TRANSLATE one "M" as a load followed by a store i.e. 2 memory accesses 
 * Ignore instruction fetch "I"
 */
void replayTrace(char* trace_fn)
{
    char buf[1000];
    mem_addr_t addr=0;
    unsigned int len=0;
    FILE* trace_fp = fopen(trace_fn, "r");

    if(!trace_fp){
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while( fgets(buf, 1000, trace_fp) != NULL) {
        if(buf[1]=='S' || buf[1]=='L' || buf[1]=='M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);
      
            if(verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

           // TODO - MISSING CODE
           // now you have: 
           // 1. address accessed in variable - addr 
           // 2. type of acccess(S/L/M)  in variable - buf[1] 
           // call accessData function here depending on type of access

            if(buf[1] == 'S' || buf[1] == 'L')
            {
                //one memory access
                accessData(addr);
            }

            else
            {
                //two accesses
                accessData(addr);
                if(verbosity)
                    printf(" ");
                accessData(addr);
            }


            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 * main - Main routine 
 */
int main(int argc, char* argv[])
{
    char c;
    
    // Parse the command line arguments: -h, -v, -s, -E, -b, -t 
    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
        switch(c){
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace_file = optarg;
            break;
        case 'v':
            verbosity = 1;
            break;
        case 'h':
            printUsage(argv);
            exit(0);
        default:
            printUsage(argv);
            exit(1);
        }
    }

    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }


    /* Initialize cache */
    initCache();

#ifdef DEBUG_ON
    printf("DEBUG: S:%u E:%u B:%u trace:%s\n", S, E, B, trace_file);
#endif
 
    replayTrace(trace_file);

    /* Free allocated memory */
    freeCache();

    /* Output the hit and miss statistics for the autograder */
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
