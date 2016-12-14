# 1 "calc.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "calc.c"
# 1 "calc.h" 1




int power(int base, int exp);
# 2 "calc.c" 2

int num_err = 0;
static int num_calc = 0;

int power(int base, int exp)
{
    if(exp < 0)
    {
        num_err++;
        return 2;
    }

    else
    {
        num_calc++;
        int res = 1;
        for (int i = 0; i < exp; i++)
        {
            res *= base;
 }
        return res;
    }
}
