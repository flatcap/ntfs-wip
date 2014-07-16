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
	VCN cs;
	VCN cb;
	VCN a;
	VCN b;

	int p, q, r, s, t, u, v;

	//printf ("vcn = %lld\n", vcn);

	if (!bmp) {
		printf ("no bmp\n");
		return -1;
	}

	cs = bmp->vol->cluster_size;
	cb = bmp->vol->cluster_size_bits;

	//printf ("\n");
	//printf ("set range: %lld - %lld (%d)\n", vcn, vcn+length-1, cs<<3);

	a = ROUND_DOWN (vcn, cs<<3);
	b = ROUND_UP (vcn + length - 1, cs<<3);
	//printf ("vcn   = %lld\n", vcn);
	//printf ("a     = %lld\n", a >> (cb+3));
	//printf ("b     = %lld\n", b >> (cb+3));
	//printf ("count = %lld\n", (b-a)/(cs<<3));

	//  0  0  0  11111111  00000001  00000000  11111110
	//  1  0  1  11111110  00000011  00000001  11111100
	//  2  0  2  11111100  00000111  00000011  11111000
	//  3  0  3  11111000  00001111  00000111  11110000
	//  4  0  4  11110000  00011111  00001111  11100000
	//  5  0  5  11100000  00111111  00011111  11000000
	//  6  0  6  11000000  01111111  00111111  10000000
	//  7  0  7  10000000  11111111  01111111  00000000

	p = vcn & 511;
	q = vcn >> 3;
	r = vcn & 7;
	s = 0xff << r;
	t = 0xff >> (7 - r);
	u = 0xff >> (8 - r);
	v = 0xfe << r;

#if 0
	printf ("%d\t%d\t%d\t", p, q, r);
	ntfs_binary_print (s); printf ("\t");
	ntfs_binary_print (t); printf ("\t");
	ntfs_binary_print (u); printf ("\t");
	ntfs_binary_print (v); printf ("\n");
#endif

	for (i = a; i < b; i += (cs<<3)) {
		buffer = ntfs_bmp_get_data (bmp, i);
		if (!buffer) {
			printf ("no buffer\n");
			return -1;
		}

#if 0
		memset (buffer, 0xFF, cs);
		value = 0;
#endif
		memset (buffer, 0x00, cs);
		value = 1;

		//utils_dump_mem (buffer, 0, 16, DM_DEFAULTS); //printf ("\n");

		begin = i;
		end   = begin + (cs<<3) - 1;
		//printf ("begin = %lld, vcn = %lld,%lld end = %lld\n", begin, vcn, vcn+length-1, end);

		if ((vcn >= begin) && (vcn <= end)) {
			start = (vcn >> 3) & (cs-1);
			sta_part = 0xff << (vcn&7);
			//printf ("1 start %d, sta_part %d\n", start, sta_part);
		} else {
			start = 0;
			//printf ("2 start %d\n", start);
		}

		if (((vcn+length-1) >= begin) && ((vcn+length-1) <= end)) {
			finish = ((vcn+length-1) >> 3) & (cs-1);
			fin_part = 0xff >> (7-((vcn+length-1)&7));
			//printf ("3 finish %d, fin_part %d\n", finish, fin_part);
		} else {
			finish = cs-1;
			//printf ("4 finish %d\n", finish);
		}

#if 0
		printf ("%4lld %4d (%02x) ", vcn, start, sta_part);
		ntfs_binary_print (sta_part); printf (" ");
		ntfs_binary_print (fin_part); printf (" ");
		printf ("(%02x) %4d\n", fin_part, finish);
#endif

#if 0
		//printf ("\n");
		printf ("%lld) ", i>>12);
		if (start > 0) {
			printf ("(%02x) ", sta_part);
		} else {
			printf ("     ");
		}

		printf ("%d - %d", start, finish);

		if (finish < (cs-1)) {
			printf (" (%02x)\n", fin_part);
		} else {
			printf ("     \n");
		}
#endif
		if (value) {
			buffer[start] |= sta_part;
			if ((finish - start) > 0)
				memset (buffer+start+1, 0xff, finish-start-1);
			buffer[finish] |= fin_part;
		} else {
			if (start != 0)
				buffer[start-1] &= ~sta_part;
			if ((finish - start) > 0)
				memset (buffer+start, 0x00, finish-start);
			buffer[finish] &= ~fin_part;
		}
		utils_dump_mem (buffer, 0, 16, DM_DEFAULTS); //printf ("\n");
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

#if 0
	printf ("bmp->count = %d\n", bmp->count);
	for (i = 0; i < bmp->count; i++) {
		printf ("  vcn = %lld\n", bmp->data_vcn[i]);
	}
#endif

	return 1;
}

