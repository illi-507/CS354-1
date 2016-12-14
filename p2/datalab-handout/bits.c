
/* 
 * CS:APP Data Lab 
 * 
 * Author: Kenny Gao
 * Email: kgao9@wisc.edu
 * CS Login: kenny
 * Section: lec 2
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. The maximum number of ops for each function is given in the
     header comment for each function. Consider this file the authoritative source for Max Ops

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 */


#endif
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Difficulty: 2
 */
int getByte(int x, int n) {
  //create constants
  int mask = 0X000000FF;
  int three = 0X00000003;

  //shift right 8 * n times
  //8 is the number of bits in a byte
  int shiftRightTimes = n << three;

  int result = x >> shiftRightTimes & mask;
  
  return result;
}

/* 
 * isNotEqual - return 0 if x == y, and 1 otherwise 
 *   Examples: isNotEqual(5,5) = 0, isNotEqual(4,5) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Difficulty: 2
 */
int isNotEqual(int x, int y) {
  //bitwise xor returns all 0s if they are equal
  int result = x ^ y;
 
  //test if it's zero
  result = !result;

  //flip the result
  result = !result;

  return result;
}

/* 
 * isEqual - return 1 if x == y, and 0 otherwise 
 *   Examples: isEqual(5,5) = 1, isEqual(4,5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Difficulty: 2
 */
int isEqual(int x, int y) {
  //bitwise xor returns all 0s if they are equal
  int result = x ^ y;
  
  //test if it's zero
  result = !result;

  return result;
}

/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Difficulty: 2
 */
int fitsBits(int x, int n) {
  int mask = 0X00000001;

  //rotate right n-1 times
  int rotateRight = n + (~(1) + 1);
  int result = x >> rotateRight;
 
  //see if the sign extension matches the result
  int testResult = result & mask;
  testResult = ~testResult + 1;
  result = !(result ^ testResult);
  
  return result;
}

/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Difficulty: 2
 */
int negate(int x) {
  //transform into 2's complement
  return (~x + 1);
}

/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Note: isPositive(0) should return 0 not 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Difficulty: 3
 */
int isPositive(int x) {
  int isZero = !(x);
  int sign = x >> 31;

  int result = !(sign | isZero);
  return result;
}

/* 
 * isNegative - return 1 if x < 0, return 0 otherwise 
 *   Example: isNegative(-1) = 1.
 *   Note: isNegative(0) should return 0 not 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Difficulty: 2
 */
int isNegative(int x) {
  //if negative, sign extension forms 0XFFFF, adding 1 gives 0
  int result = (x >> 31) + 1;

  //see if it is 0
  result = !result;
  return result;
}


/* 
 * sign - return 1 if positive, 0 if zero, and -1 if negative
 *  Examples: sign(130) = 1
 *            sign(-23) = -1
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 10
 *  Difficulty: 2
 */
int sign(int x) {
   int getSign = (x >> 31);
   int isNotZero = !(!(x));
   int result = getSign | isNotZero;
   return result;
}
