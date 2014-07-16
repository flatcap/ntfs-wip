/**
 * mftdump - Part of the Linux-NTFS project.
 *
 * Copyright (c) 2004 Richard Russon
 *
 * This utility will dump an MFT record
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
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include <stdio.h>
#include <unistd.h>

#include "types.h"
#include "layout.h"
#include "mst.h"
#include "utils.h"

GEN_PRINTF (Eprintf, stderr, NULL, FALSE)
GEN_PRINTF (Vprintf, stdout, NULL, TRUE)
GEN_PRINTF (Qprintf, stdout, NULL, FALSE)

/**
 * main - Start here
 */
int main (int argc, char **argv)
{
	u8 buffer[1024];
	u8 *ptr;
	MFT_RECORD *rec;
	ATTR_RECORD *attr;
	int len;

	while (sizeof (buffer) == read (0, buffer, sizeof (buffer))) {
		//if (ntfs_mst_post_read_fixup ((NTFS_RECORD*)buffer, sizeof (buffer)))
			//break;
		rec = (MFT_RECORD*) buffer;
		len = rec->attrs_offset;

		utils_dump_mem (buffer, 0, len, DM_DEFAULTS);
		printf ("\n");

		ptr = buffer + len;
		while ((ptr - buffer) < sizeof (buffer)) {
			attr = (ATTR_RECORD*) ptr;
			if (attr->type == AT_END)
				len = 8;
			else
				len = attr->length;
			utils_dump_mem (buffer, (ptr-buffer), len, DM_DEFAULTS);
			printf ("\n");

			if (attr->type == AT_END)
				break;
			ptr += len;
		}
	}

	return 0;
}

