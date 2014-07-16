/**
 * ntfsck - Part of the Linux-NTFS project.
 *
 * Copyright (c) 2003 Richard Russon
 *
 * This utility will search an NTFS volume for errors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the Linux-NTFS
 * distribution in the file COPYING); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include <stdio.h>
#include <getopt.h>

#include "types.h"
#include "utils.h"
#include "ntfsck.h"

static const char *EXEC_NAME = "ntfsck";
static struct options opts;

GEN_PRINTF (Eprintf, stderr, NULL,          FALSE)
GEN_PRINTF (Vprintf, stdout, &opts.verbose, TRUE)
GEN_PRINTF (Qprintf, stdout, &opts.quiet,   FALSE)

#define RED	"[31m"
#define YELLOW	"[33m"
#define GREEN	"[01;32m"
#define NORM	"[0m"

/**
 * version - Print version information about the program
 *
 * Print a copyright statement and a brief description of the program.
 *
 * Return:  none
 */
void version (void)
{
	printf ("\n%s v%s - Search an NTFS Volume for errors.\n\n",
		EXEC_NAME, VERSION);
	printf ("Copyright (c) 2003 Richard Russon\n");
	printf ("\n%s\n%s%s\n", ntfs_gpl, ntfs_bugs, ntfs_home);
}

/**
 * usage - Print a list of the parameters to the program
 *
 * Print a list of the parameters and options for the program.
 *
 * Return:  none
 */
void usage (void)
{
	printf ("\nUsage: %s [options] device\n"
		"\n"
		"    -n      --no-action    Do not write to disk\n"
		"    -f      --force        Use less caution\n"
		"    -h      --help         Print this help\n"
		"    -q      --quiet        Less output\n"
		"    -V      --version      Version information\n"
		"    -v      --verbose      More output\n\n",
		EXEC_NAME);
	printf ("%s%s\n", ntfs_bugs, ntfs_home);
}

/**
 * parse_options - Read and validate the programs command line
 *
 * Read the command line, verify the syntax and parse the options.
 * This function is very long, but quite simple.
 *
 * Return:  1 Success
 *	    0 Error, one or more problems
 */
int parse_options (int argc, char **argv)
{
	static const char *sopt = "-fh?nqVv";
	static const struct option lopt[] = {
		{ "force",	no_argument,		NULL, 'f' },
		{ "help",	no_argument,		NULL, 'h' },
		{ "no-action",	no_argument,		NULL, 'n' },
		{ "quiet",	no_argument,		NULL, 'q' },
		{ "verbose",	no_argument,		NULL, 'v' },
		{ "version",	no_argument,		NULL, 'V' },
		{ NULL,		0,			NULL, 0   }
	};

	char c = -1;
	int err  = 0;
	int ver  = 0;
	int help = 0;

	opterr = 0; /* We'll handle the errors, thank you. */

	while ((c = getopt_long (argc, argv, sopt, lopt, NULL)) != -1) {
		switch (c) {
		case 1:	/* A non-option argument */
			if (!opts.device) {
				opts.device = argv[optind-1];
			} else {
				opts.device = NULL;
				err++;
			}
			break;
		case 'f':
			opts.force++;
			break;
		case 'h':
		case '?':
			help++;
			break;
		case 'n':
			opts.noaction++;
			break;
		case 'q':
			opts.quiet++;
			break;
		case 'V':
			ver++;
			break;
		case 'v':
			opts.verbose++;
			break;
		default:
			Eprintf ("Unknown option '%s'.\n", argv[optind-1]);
			err++;
			break;
		}
	}

	if (help || ver) {
		opts.quiet = 0;
	} else {
		if (opts.device == NULL) {
			if (argc > 1)
				Eprintf ("You must specify exactly one device.\n");
			err++;
		}

		if (opts.quiet && opts.verbose) {
			Eprintf("You may not use --quiet and --verbose at the "
					"same time.\n");
			err++;
		}
	}

	if (ver)
		version();
	if (help || err)
		usage();

	return (!err && !help && !ver);
}


/**
 * check_boot
 */
int check_boot (ntfs_volume *vol, int flags)
{
	// Validate the boot sector
	return 1;
}

/**
 * check_mftmirr
 */
int check_mftmirr (ntfs_volume *vol, int flags)
{
	// Check that the MFT and MFTMirr agree
	return 1;
}

/**
 * check_mft
 */
int check_mft (ntfs_volume *vol, int flags)
{
	// Check that the MFT and its BITMAP agree
	// Check that every MFT record is valid (size, etc)
	// Validate each attribute (size and against $AttrDef, timestamps)
	// Check non-resident attributes' data runs (against volume size)
	// Do we fix the MFT with BAAD?
	return 1;
}

/**
 * check_bitmap
 */
int check_bitmap (ntfs_volume *vol, int flags)
{
	// Check the $Bitmap against every non-resident attribute
	// Check for free space actually in use
	// Check for clusters that aren't being used
	// Check for multiply used clusters
	// Validate against $BadClus

	struct mft_search_ctx *m_ctx = NULL;
	ntfs_attr_search_ctx  *a_ctx = NULL;
	ATTR_RECORD *rec;
	runlist *runs;
	int i;
	int result = -1;

	if (!vol)
		return -1;

	m_ctx = mft_get_search_ctx (vol);
	m_ctx->flags_search = FEMR_IN_USE | FEMR_BASE_RECORD;

	while (mft_next_record (m_ctx) == 0) {
		printf (RED "Inode: %lld\n" NORM, m_ctx->inode->mft_no);

		if (!(m_ctx->flags_match & FEMR_BASE_RECORD))
			continue;

		a_ctx = ntfs_attr_get_search_ctx (NULL, m_ctx->inode->mrec);

		while ((rec = find_attribute (AT_UNUSED, a_ctx))) {

			if (!rec->non_resident) {
				printf ("0x%02x skipped - attr is resident\n", a_ctx->attr->type);
				continue;
			}

			runs = ntfs_mapping_pairs_decompress (vol, a_ctx->attr, NULL);
			if (!runs) {
				Eprintf ("Couldn't read the data runs.\n");
				goto done;
			}

			printf ("\t[0x%02X]\n", a_ctx->attr->type);

			printf ("\t\tVCN\tLCN\tLength\n");
			for (i = 0; runs[i].length > 0; i++) {
				LCN a_begin = runs[i].lcn;
				//LCN a_end   = a_begin + runs[i].length - 2;

				if (a_begin < 0)
					continue;	// sparse, discontiguous, etc

				printf ("\t\t%lld\t%lld-%lld (%lld)\n", runs[i].vcn, runs[i].lcn, runs[i].lcn + runs[i].length - 1, runs[i].length);
				//dprint list
			}
		}

		ntfs_attr_put_search_ctx (a_ctx);
		a_ctx = NULL;
	}

	result = 0;
done:
	ntfs_attr_put_search_ctx (a_ctx);
	mft_put_search_ctx (m_ctx);

	return result;
}

/**
 * check_metadata
 */
int check_metadata (ntfs_volume *vol, int flags)
{
	// Validate $AttrDef
	// Validate $Volume
	// Validate $UpCase
	// Validate $Secure
	// When do we validate $AttrDef?  And how?
	// What about other metadata?
	return 1;
}

/**
 * check_dirs
 */
int check_dirs (ntfs_volume *vol, int flags)
{
	// Check INDX records
	// Check sort order
	// Check BITMAP agrees
	// Check for circular references
	return 1;
}

/**
 * check_security
 */
int check_security (ntfs_volume *vol, int flags)
{
	// Validate that the structures make sense
	// What else?  How?
	return 1;
}

/**
 * check_bad_sectors
 */
int check_bad_sectors (ntfs_volume *vol, int flags)
{
	// Check every sector
	// Write to $BadClus if necessary
	return 1;
}

/**
 * ntfs_chkdsk
 */
int ntfs_chkdsk (ntfs_volume *vol, int flags)
{
	if (!check_bitmap (vol, flags))
		return 0;

	return 1;
}


/**
 * main - Begin here
 *
 * Start from here.
 *
 * Return:  0  Success, the program worked
 *	    1  Error, something went wrong
 */
int main (int argc, char *argv[])
{
	ntfs_volume *vol;
	int flags = 0;
	int result = 1;

	if (!parse_options (argc, argv))
		return 1;

	utils_set_locale();

	if (opts.noaction)
		flags |= MS_RDONLY;

	// work on the device only?
	// do a pseudo mount?
	// build up an ntfs_volume as we validate things?
	vol = utils_mount_volume (opts.device, flags, opts.force);
	if (!vol) {
		printf ("!vol\n");
		return 1;
	}

	result = ntfs_chkdsk (vol, 0);

	if (result)
		printf ("failed\n");
	else
		printf ("success\n");

	ntfs_umount (vol, FALSE);
	return result;
}

