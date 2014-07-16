/**
 * trace.c - Function profiling.  Part of the Linux-NTFS project.
 *
 * Copyright (c) 2005 Richard Russon
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the Linux-NTFS
 * distribution in the file COPYING); if not, write to the Free Software
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

void __cyg_profile_func_enter (void *this_fn, void *call_site);
void __cyg_profile_func_exit  (void *this_fn, void *call_site);

static FILE *fptr;

const char *filename = "functions.txt";

/**
 * constructor - called before main()
 */
__attribute__((no_instrument_function, constructor))
static void constructor(void)
{
	fptr = fopen(filename, "w");
	if (fptr == NULL)
		exit(1);
}

/**
 * destructor - called after main()
 */
__attribute__((no_instrument_function, destructor))
static void destructor(void)
{
	fclose(fptr);
}

/**
 * __cyg_profile_func_enter - called before every function call
 */
__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void *this_fn, void *call_site __attribute__((unused)))
{
	fprintf(fptr, "%p enter\n", this_fn);
}

/**
 * __cyg_profile_func_exit - called after every function call
 */
__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *this_fn, void *call_site __attribute__((unused)))
{
	fprintf(fptr, "%p exit\n", this_fn);
}

