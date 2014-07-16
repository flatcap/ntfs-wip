#include <stdio.h>

int hweight8 (int w)
{
	int res = (w & 0x55) + ((w >> 1) & 0x55);
	res = (res & 0x33) + ((res >> 2) & 0x33);
	return (res & 0x0F) + ((res >> 4) & 0x0F);
}

int main (int argc, char *argv[])
{
	FILE *f;
	char buffer[1024];
	int read;
	int i;
	int count = 0;

	f = fopen (argv[1], "r");
	if (!f)
		return 1;

	do {
		read = fread (buffer, 1, sizeof (buffer), f);
		for (i = 0; i < read; i++) {
			count += hweight8 (buffer[i]);
		}
	} while (read > 0);

	fclose (f);
	printf ("count = %d\n", count);

	return 0;
}
