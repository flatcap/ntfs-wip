#include <stdio.h>

int main (int argc, char **argv)
{
	char *spin = "|/-\\";
	int i;
	int s = 0;

	for (i = 0; i < 100000; i++) {
		if ((i % 50) == 0) {
			s++;
			if (s > 3)
				s = 0;
		}
		printf ("");
		printf ("%c %d", spin[s], i);
	}
	printf ("\n");

	return 0;
}
