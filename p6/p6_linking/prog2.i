# 1 "prog2.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "prog2.c"
# 1 "calc.h" 1




int power(int base, int exp);
# 2 "prog2.c" 2
extern int num_err;
int main() {
 int res = power(4, -2);
 if (res == 2) {
  return num_err;
 }
 return 0;
}
