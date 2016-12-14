#include <stdio.h>
#include <stdlib.h>

#define SEED 42

/* A program to generate random numbers. */
int main(int argc, char *argv[])
{
  int i, rand_num, max_num, total_num;

  if (argc != 3)
  {
    printf("USAGE: %s <total_nums> <max_num>\n", argv[0]);
    exit(1);
  }

  total_num = atoi(argv[1]);
  max_num = atoi(argv[2]);
  
  srand(SEED);
  for (i = 0; i < total_num; ++i)
  {
    rand_num = rand() % max_num;
    printf("%d\n",rand_num);   
  }
  return 0;
}
