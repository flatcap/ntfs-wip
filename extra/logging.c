#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include "logging.h"

/**
 * bob
 */
__attribute__ ((format (printf, 1, 0)))
static int bob (const char *format, va_list args)
{
	vfprintf (stdout, format, args);
	return 0;
}

/**
 * jim
 */
__attribute__ ((format (printf, 1, 2)))
static int jim (const char *format, ...)
{
	va_list args;

	va_start (args, format);
	bob (format, args);
	va_end (args);

	return 0;
}


int main (int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	jim ("hello %s\n", "Rich");
	return 0;
}

