#include "calc.h"
extern int num_err;
int main() {
	int res = power(4, -2);
	if (res == POWER_ERR) {
		return num_err;
	}
	return 0;
}
