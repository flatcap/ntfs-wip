static int ntfs_bmp_set_range (struct ntfs_bmp *bmp, VCN vcn, s64 length, int value)
{
	// shouldn't all the vcns be lcns?
	s64 i;
	u8 *buffer;
	VCN begin;
	VCN end;
	int start;
	int finish;
	u8 sta_part = 0;
	u8 fin_part = 0;
	u8 sta_bit;
	u8 fin_bit;
	VCN clust_size;
	VCN clust_bits;
	VCN a;
	VCN b;

	int p, q, r, s, t, u, v;

	if (!bmp)
		return -1;

	clust_size = bmp->vol->cluster_size;
	clust_bits = bmp->vol->cluster_size_bits; // unused?

	a = ROUND_DOWN (vcn, clust_size<<3);
	b = ROUND_UP (vcn + length - 1, clust_size<<3);

	p = vcn & 511;
	q = vcn >> 3;
	r = vcn & 7;
	s = 0xFF << r;
	t = 0xFF >> (7 - r);
	u = 0xFF >> (8 - r);
	v = 0xFE << r;

	for (i = a; i < b; i += (clust_size<<3)) {
		buffer = ntfs_bmp_get_data (bmp, i);
		if (!buffer) {
			printf ("no buffer\n");
			return -1;
		}

#if 1
		if (value) {
			memset (buffer, 0x00, clust_size);
			//value = 1;
		} else {
			memset (buffer, 0xFF, clust_size);
			//value = 0;
		}
#endif

		//utils_dump_mem (buffer, 0, 16, DM_DEFAULTS); //printf ("\n");

		begin = i;
		end   = begin + (clust_size<<3) - 1;

		sta_bit = (vcn & 7);
		fin_bit = ((vcn + length - 1) & 7);

		if (value) {
			sta_part = (0xFF << sta_bit);
			fin_part = (0xFF >> (7 - fin_bit));
		} else {
			sta_part = (0xFF >> (8 - sta_bit));
			fin_part = (0xFE << fin_bit);
		}

		//printf ("sta_part = %02x, fin_part = %02x\n", sta_part, fin_part);
		if ((vcn >= begin) && (vcn <= end)) {
			start = (vcn >> 3) & (clust_size-1);
		} else {
			start = 0;
		}

		if (((vcn+length-1) >= begin) && ((vcn+length-1) <= end)) {
			finish = ((vcn+length-1) >> 3) & (clust_size-1);
		} else {
			finish = clust_size-1;
		}

		// refactor this section
		if (value) {
			if ((finish - start) > 0) {
				memset (buffer+start+1, 0xff, finish-start-1);
			} else {
				sta_part &= fin_part;
				fin_part = 0;
			}

			buffer[start]  |= sta_part;
			buffer[finish] |= fin_part;
		} else {
			if ((finish - start) > 0) {
				memset (buffer+start+1, 0x00, finish-start-1);
			} else {
				sta_part |= fin_part;
				fin_part = 0xff;
			}

#if 0
			printf ("sta_part = "); ntfs_binary_print (sta_part, TRUE, FALSE); printf (" ");
			printf ("fin_part = "); ntfs_binary_print (fin_part, TRUE, FALSE); printf (" ");
			printf ("start = %d, finish = %d\n", start, finish);
#endif

			//printf ("buffer[start]  = %02x\n", buffer[start]);
			//printf ("buffer[finish] = %02x\n", buffer[finish]);
			buffer[start]  &= sta_part;
			buffer[finish] &= fin_part;
			//printf ("buffer[start]  = %02x\n", buffer[start]);
			//printf ("buffer[finish] = %02x\n", buffer[finish]);
		}
#if 0
		printf ("%4lld %4d (%02x) ", vcn, start, sta_part);
		ntfs_binary_print (sta_part, TRUE); printf (" ");
		ntfs_binary_print (fin_part, TRUE); printf (" ");
		printf ("(%02x) %4d\n", fin_part, finish);
#else
		//utils_dump_mem (buffer, 0, 16, DM_DEFAULTS); //printf ("\n");
		ntfs_binary_print (buffer[0], TRUE, TRUE); printf (" ");
		ntfs_binary_print (buffer[1], TRUE, TRUE); printf (" ");
		ntfs_binary_print (buffer[2], TRUE, TRUE); printf (" ");
		ntfs_binary_print (buffer[3], TRUE, TRUE); printf (" ");
		ntfs_binary_print (buffer[4], TRUE, TRUE); printf (" ");
		ntfs_binary_print (buffer[5], TRUE, TRUE); printf (" ");
		ntfs_binary_print (buffer[6], TRUE, TRUE); printf (" ");
		ntfs_binary_print (buffer[7], TRUE, TRUE); printf ("\n");
#endif
	}

#if 0 //TEMP
	printf (GREEN "Modified: inode %lld, ", bmp->attr->ni->mft_no);
	switch (bmp->attr->type) {
		case AT_BITMAP: printf ("$BITMAP"); break;
		case AT_DATA:   printf ("$DATA");   break;
		default:			    break;
	}
	printf (" vcn %lld-%lld\n" END, vcn>>12, (vcn+length-1)>>12);
#endif

	return 1;
}

