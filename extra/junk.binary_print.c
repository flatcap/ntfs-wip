/**
 * ntfs_binary_print
 */
static void ntfs_binary_print (u8 num, BOOL backwards, BOOL colour)
{
	int i;

	if (backwards)
		for (i = 1; i < 129; i<<=1) {
			if (colour)
				printf ("%s", (num&i) ? "[31m1[0m" : "0");
			else
				printf ("%s", (num&i) ? "1" : "0");
		}
	else
		for (i = 128; i > 0; i>>=1) {
			if (colour)
				printf ("%s", (num&i) ? "[31m1[0m" : "0");
			else
				printf ("%s", (num&i) ? "1" : "0");
		}
}


