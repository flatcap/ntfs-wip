#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"

static BOOL watcher(const char *function, int line, const char *expression, BOOL value)
{
	if (0)
		printf ("watching %s(%d) : %s\n", function, line, expression);

	if ((random() % 10) > 7)
		value = !value;

	return value;
}

#define if(x)	if (watcher(__FUNCTION__, __LINE__, #x, (x)))

int main(int argc, char *argv[])
{
	int a, b;

	if (argc == 3)
		srandom(time(NULL));
	else if (argc == 4)
		srandom(atoi(argv[3]));
	else
		return 1;

	a = atoi(argv[1]);
	b = atoi(argv[2]);

	if (a < b) {
		printf ("a is smaller\n");
	}

	if ((a + b) > 10) {
		printf ("sum is greater than 10\n");
	}

	if ((a * b) > 50) {
		printf ("product is greater than 50\n");
	}

	return 0;
}

