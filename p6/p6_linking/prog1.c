#include "calc.h"
int main() {
	int res = power(3, 2);
	if (res == POWER_ERR) {
		return -1;
	}
	return 0;
}
