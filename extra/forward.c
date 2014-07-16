#include <stdio.h>

//struct jim;
typedef struct jim JJ;

int fn (JJ *pj);

struct jim
{
	int x;
	int y;
	char z;
	long p;
};

//typedef struct jim JJ;

//int fn (struct jim *pj);

int main (int argc, char *argv[])
{
	JJ j;

	if (argc > 1)
		return 1;
	if (!argv[0])
		return 1;

	printf ("sizeof JJ = %d\n", sizeof (JJ));
	j.x = 4;
	return fn(&j);
}

int fn (JJ *pj)
{
	if (!pj)
		return 0;
	return pj->x;
}
