/**
 * ntfs_log_handler_stack
 */
__attribute__((format(printf, 6, 0)))
static int ntfs_log_handler_stack(const char *function, const char *file,
	int line, u32 level, void *data __attribute__((unused)),
	const char *format, va_list args)
{
	int indent = 0;
	int ret;

	if (top_level == NULL)
		goto done;

	/* No, a for loop won't work. */
	if (__builtin_return_address(0)  == top_level) { indent =  0; goto done; }
	if (__builtin_return_address(1)  == top_level) { indent =  1; goto done; }
	if (__builtin_return_address(2)  == top_level) { indent =  2; goto done; }
	if (__builtin_return_address(3)  == top_level) { indent =  3; goto done; }
	if (__builtin_return_address(4)  == top_level) { indent =  4; goto done; }
	if (__builtin_return_address(5)  == top_level) { indent =  5; goto done; }
	if (__builtin_return_address(6)  == top_level) { indent =  6; goto done; }
	if (__builtin_return_address(7)  == top_level) { indent =  7; goto done; }
	if (__builtin_return_address(8)  == top_level) { indent =  8; goto done; }
	if (__builtin_return_address(9)  == top_level) { indent =  9; goto done; }
	if (__builtin_return_address(10) == top_level) { indent = 10; goto done; }
	if (__builtin_return_address(11) == top_level) { indent = 11; goto done; }
	if (__builtin_return_address(12) == top_level) { indent = 12; goto done; }
	if (__builtin_return_address(13) == top_level) { indent = 13; goto done; }
	if (__builtin_return_address(14) == top_level) { indent = 14; goto done; }
	if (__builtin_return_address(15) == top_level) { indent = 15; goto done; }
	if (__builtin_return_address(16) == top_level) { indent = 16; goto done; }
	if (__builtin_return_address(17) == top_level) { indent = 17; goto done; }
	if (__builtin_return_address(18) == top_level) { indent = 18; goto done; }
	if (__builtin_return_address(19) == top_level) { indent = 19; goto done; }
	if (__builtin_return_address(20) == top_level) { indent = 20; goto done; }
done:
	if (indent >= 3)
		indent -= 3;	/* Ignore this function, redirect and main */
	else
		indent = 0;

	if (indent > 0)
		fprintf(stdout, "%.*s", indent*4, space_line);

	if (0)
	{
		char *off;
		char *format2;
		off = strstr(format, "%p%d");
		if (off) {
			format2 = calloc(1, strlen(format + 3));
			memcpy(format2, format, 12);
			sprintf(format2+12, "%s\n", "wibble");
			format = format2;
		}
	}

	if (0)
	{
		char *off;
		void **argarr;
		void *ptr;
		char *format2;
		off = strstr(format, "%p%d");
		if (off) {
			format2 = strdup(format);
			format2[13] = 'd';
			format2[15] = 'p';
			format = format2;
			argarr = (void**)args;
			ptr = argarr[1];
			argarr[1] = argarr[2];
			argarr[2] = ptr;
		}
	}

	if (0)
	{
		char *off;
		void **argarr;
		char *format2;
		ntfschar *uname = NULL;
		int uname_len = 0;
		char *aname = NULL;

		off = strstr(format, "%p%d");
		if (off) {
			format2 = strdup(format);
			format2[12] = '\'';
			format2[13] = '%';
			format2[14] = 's';
			format2[15] = '\'';
			format = format2;

			argarr = (void**)args;

			uname     = (ntfschar*)argarr[1];
			uname_len = (int)argarr[2];

			ntfs_ucstombs(uname, uname_len, &aname, 0);

			argarr[1] = aname;
		}
	}

	//if (strstr(format, "%p%d"))
	if (0)
	{
		void **argarr;
		int i;
		int count = 0;
		const char *str;

		str = format;
		while (str) {
			str = strchr(str, '%');
			if (str) {
				count++;
				str++;
			}
		}

		argarr = (void**)args;

		printf ("%d arguments\n", count);
		for (i = 0; i < count; i++)
			printf("arg[%d] = %p\n", i, argarr[i]);
	}

	ret = ntfs_log_handler_fprintf(function, file, line, level, stdout, format, args);

#if 0
	free(format); // if we alloc'd it
#endif
	return ret;
}

