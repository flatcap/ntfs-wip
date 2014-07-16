#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

typedef unsigned char	bool;
#define false		0
#define true		1

//__attribute__ ((format (printf, 4, 5)))

/**
 * generic_printf
 */
void generic_printf (struct _IO_FILE *stream, int *control, bool trigger, const char *format, ...)
{
	int olderr = errno;
	va_list args;

	if (!stream)
		return;
	if (control && ((*control && !trigger) || (!*control && trigger)))
		return;

	va_start (args, format);
	vfprintf (stream, format, args);
	va_end (args);
	errno = olderr;
}

static int verbose;
static int quiet;

#define Eprintf(f, a...)	generic_printf (stderr, NULL,     false, f, ##a)
#define Iprintf(f, a...)	generic_printf (stdout, &quiet,   false, f, ##a)
#define Vprintf(f, a...)	generic_printf (stdout, &verbose, true,  f, ##a)

#ifdef DEBUG
#define Dprintf(f, a...)	Iprintf		/* what about __FUNCTION__? */
else
#define Dprintf(f, a...)	do {} while (0)
#endif

/* keep the spinlock and buffer, then DEBUG -> all messages are prefixed with __FUNCTION__ */

int main (int argc, char **argv)
{
	verbose = 1;
	Vprintf ("verbose message 1\n");
	verbose = 0;
	Vprintf ("verbose message 2\n");

	quiet = 1;
	Iprintf ("quiet message 1\n");
	quiet = 0;
	Iprintf ("quiet message 2\n");

	Eprintf ("error message 1\n");

	return 0;
}
