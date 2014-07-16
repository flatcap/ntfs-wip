#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char *f_list[50];
static int f_index = 0;
static const char *tabs_line = "																					";

int main(int argc, char *argv[])
{
	FILE *fptr = NULL;
	char direction[20];
	char function[64];
	int ret = 1;

	if (argc != 2)
		return 1;

	fptr = fopen(argv[1], "r");
	if (!fptr)
		return 1;

	while (1) {
		ret = fscanf(fptr, "%s %s\n", direction, function);
		if (ret < 1)
			break;
		if (strcmp (direction, "enter") == 0) {
			printf ("%.*s%s\n", f_index, tabs_line, function);
			f_list[f_index] = strdup(function);
			f_index++;
		} else {
			f_index--;
			free(f_list[f_index]);
		}
	}

	fclose(fptr);
	return 0;
}

