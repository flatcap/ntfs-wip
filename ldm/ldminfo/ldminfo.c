/**
 * ldminfo - Part of the Linux-NTFS project.
 *
 * Copyright (C) 2001-2005 Richard Russon <ldm@flatcap.org>
 *
 * Documentation is available at http://linux-ntfs.sourceforge.net/ldm
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the Linux-NTFS source
 * in the file COPYING); if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "ldminfo.h"

int debug = 0;

/**
 * dump_info - Display a list of partitions, a la fdisk
 */
static void dump_info (char *name, struct parsed_partitions *pp)
{
	int i;
	int numeric;

	name = basename (name);
	numeric = isdigit (name[strlen (name) - 1]);

	printf ("Device       | Offset Bytes    Sectors    MiB | Size   Bytes    Sectors    MiB\n"
		"-------------+--------------------------------+-------------------------------\n");
	for (i = 0; i < 256; i++) {
		char buf[16];
		long long start;
		long long size;

		if (pp->parts[i].size == 0)
			break;

		if (i)
			if (numeric)
				sprintf (buf, "%.10sp%d", name, i);
			else
				sprintf (buf, "%.10s%d", name, i);
		else
			sprintf (buf, "%.10s", name);

		start = pp->parts[i].from; start <<= 9;
		size  = pp->parts[i].size; size  <<= 9;
		printf ("%-12.12s | %12lld %10lld %6lld | %12lld %10lld %6lld\n", buf, start, start>>9, start>>20, size, size>>9, size>>20);
	}
	printf ("\n");
}

/**
 * main - ldminfo entry point
 */
int main (int argc, char *argv[])
{
	int a;
	int info  = 0;
	int dump  = 0;
	int copy  = 0;
	int help  = 0;
	int ver   = 0;
	int dev   = -1;
	struct stat64 st;
	struct ldmdb *ldb = NULL;

	for (a = 1; a < argc; a++) {
		if	(strcmp (argv[a], "--info")    == 0) info++;
		else if	(strcmp (argv[a], "--dump")    == 0) dump++;
		else if (strcmp (argv[a], "--copy")    == 0) copy++;
		else if (strcmp (argv[a], "--debug")   == 0) debug++;
		else if (strcmp (argv[a], "--help")    == 0) help++;
		else if (strcmp (argv[a], "--version") == 0) ver++;
		else continue;
		argv[a][0] = 0;
	}

	if (help || (argc - info - dump - copy - debug) < 2) {
		printf ("\nUsage:\n    %s [options] device ...\n", basename (argv[0]));
		printf ("\nOptions:\n"
			"    --info     A concise list of partitions (default)\n"
			"    --dump     The contents of the database in detail\n"
			"    --copy     Write the database to a file\n"
			"    --debug    Display lots of debugging information\n"
			"    --version  display the version number\n"
			"    --help     Show this short help\n\n");
		return 1;
	}

	if (ver) {
		printf ("\nldminfo v0.0.8\n\n");
		return 1;
	}

	ldb = malloc (sizeof (*ldb));
	if (!ldb) {
		printf ("Out of memory\n");
		return 1;
	}

	for (a = 1; a < argc; a++) {
		long long size;
		struct parsed_partitions pp;	//FIXME move to heap

		memset (ldb, 0, sizeof (*ldb));

		if (!argv[a][0])
			continue;

		if (stat64 (argv[a], &st)) {
			printf ("Couldn't open device (stat): %s\n", argv[a]);
			break;
		}

		if ((!S_ISBLK (st.st_mode)) && (!S_ISREG (st.st_mode))) {
			printf ("Couldn't open device (dev/file): %s\n", argv[a]);
			break;
		}

		dev = open64 (argv[a], O_RDONLY);
		if (dev < 0) {
			printf ("Couldn't open device (open): %s\n", argv[a]);
			break;
		}

		size = lseek64 (dev, 0, SEEK_END);
		if (size < 0) {
			printf ("Seek failed for device: %s\n", argv[a]);
			break;
		}

		if (copy) {
			copy_database (argv[a], dev, size);
			goto close;
		}

		/* Initialize vblk list in ldmdb struct */
		INIT_LIST_HEAD(&ldb->v_dgrp);
		INIT_LIST_HEAD(&ldb->v_disk);
		INIT_LIST_HEAD(&ldb->v_volu);
		INIT_LIST_HEAD(&ldb->v_comp);
		INIT_LIST_HEAD(&ldb->v_part);

		memset (&pp, 0, sizeof (pp));
		pp.parts[0].from = 0;
		pp.parts[0].size = size >> 9;
		pp.limit = 255;

		if (ldm_partition (dev, &pp, ldb) != 1) {
			printf ("Something went wrong, skipping device '%s'\n", argv[a]);
			goto free;
		}

		if (dump)
			dump_database (argv[a], ldb);
		else
			dump_info     (argv[a], &pp);

free:
		ldm_free_vblks(&ldb->v_dgrp);
		ldm_free_vblks(&ldb->v_disk);
		ldm_free_vblks(&ldb->v_volu);
		ldm_free_vblks(&ldb->v_comp);
		ldm_free_vblks(&ldb->v_part);
close:
		close (dev);
		dev = 0;
	}

	free (ldb);

	if (dev)
		close (dev);

	return 0;
}

