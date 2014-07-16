#ifdef RAR
	runlist_element *rl = NULL;
	int i;
	int need1;
	int need2;
	//u8 buffer[50];
	ATTR_RECORD *rec;
	runlist *output = NULL;
	runlist *original = NULL;

	// NTFS_MOVE_LOC_BEST : need to make sure we don't fragment this run
	// Anything else we pass directly to find_unused.

	//printf ("move inode %lld\n", ino->mft_no);

	//printf ("file size = %lld\n", attr->data_size);

	//output = calloc (2, sizeof (runlist_element));
	output = calloc (1, 4096);
	output[0].vcn    = 0;
	output[0].lcn    = LCN_RL_NOT_MAPPED;
	output[0].length = attr->allocated_size >> vol->cluster_size_bits;
	output[1].vcn    = output[0].length;
	output[1].lcn    = LCN_ENOENT;
	output[1].vcn    = 0;

	ntfs_attr_map_runlist (attr, 0);

	ntfs_debug_runlist_dump2 (attr->rl, 5);

	//ntfs_debug_runlist_dump2 (output, 5);

	//printf ("This record is %d bytes long\n", ino->mrec->bytes_in_use);
	//printf ("There are %d bytes free for expansion\n", ino->mrec->bytes_allocated - ino->mrec->bytes_in_use);

	rec = find_first_attribute (AT_DATA, ino->mrec);

	//printf ("This attribute is %d bytes long\n", rec->length);

	for (i = 0; attr->rl[i].length > 0; i++) {
		//memset (buffer, 0, sizeof (buffer));

		//memcpy (buffer, attr->rl+i, sizeof (runlist_element));
		//rl = (runlist_element*) (buffer + sizeof (runlist_element));
		//rl->lcn = LCN_ENOENT;
		//need = ntfs_get_size_for_mapping_pairs (vol, (runlist_element*)buffer);
		//printf ("orig data run = %d bytes\n", need);
		//ntfs_mapping_pairs_build (vol, buffer, sizeof (buffer), (runlist*)buffer);
		//dump_runs (buffer, need);

		rl = find_unused (vol, attr->rl[i].length, loc, flags);

		rl[0].vcn = attr->rl[i].vcn;
		rl[1].vcn = rl[0].length;
		//printf ("vcn = %lld, lcn = %lld, length = %lld\n", rl[0].vcn, rl[0].lcn, rl[0].length);
		//printf ("vcn = %lld, lcn = %lld, length = %lld\n", rl[1].vcn, rl[1].lcn, rl[1].length);
		//ntfs_debug_runlist_dump2 (rl, 8);

		//need = ntfs_get_size_for_mapping_pairs (vol, rl);
		//printf ("new  data run = %d bytes\n", need);

		//ntfs_mapping_pairs_build (vol, buffer, sizeof (buffer), rl);
		//dump_runs (buffer, need);

		bitmap_alloc (vol, rl);

		data_copy (vol, attr->rl+i, rl);

		output = ntfs_runlists_merge (output, rl);

		/*
		if (ntfs_debug_runlist_dump2 (output, 5))
			break;
		*/
	}

	ntfs_debug_runlist_dump2 (output, 8);

	need1 = ntfs_get_size_for_mapping_pairs (vol, attr->rl);
	printf ("orig data run = %d bytes\n", need1);

	need2 = ntfs_get_size_for_mapping_pairs (vol, output);
	printf ("new  data run = %d bytes\n", need2);

	// copy original runlist
	original = ntfs_mapping_pairs_decompress (vol, rec, NULL);
	if (!original) {
		printf ("!original\n");
		return -1;
	}

	need1 = calc_attr_length (rec, need1);
	need2 = calc_attr_length (rec, need2);

	if (need2 <= 0) {
		printf ("!need2\n");
		return -1;
	}

	utils_dump_mem ((u8*) ino->mrec, 24, 8, FALSE);
	printf ("\n");
	utils_dump_mem ((u8*) ino->mrec, p2n(rec)-p2n(ino->mrec), need1, FALSE);
	printf ("\n");

	// only if need1 != need2
	if (need1 != need2) {
		if (resize_nonres_attr (ino->mrec, rec, need2) < 0) {
			printf ("!resize\n");
			return -1;
		}
	}

	//utils_dump_mem ((u8*) ino->mrec, 352, need2, FALSE);
	//printf ("\n");

	// wipe orig runs
	memset (((u8*)rec) +rec->mapping_pairs_offset, 0, need2 - rec->mapping_pairs_offset);

	// update data runs
	ntfs_mapping_pairs_build (vol, ((u8*)rec) + rec->mapping_pairs_offset, need2, output);

	rec = find_first_attribute (AT_DATA, ino->mrec);

	utils_dump_mem ((u8*) ino->mrec, 24, 8, FALSE);
	printf ("\n");
	utils_dump_mem ((u8*) ino->mrec, p2n(rec)-p2n(ino->mrec), need2, FALSE);

	//utils_dump_mem ((u8*) ino->mrec, 0, 1024, FALSE);

	// commit
	ntfs_inode_mark_dirty (ino);

	if (ntfs_inode_sync (ino) < 0) {
		printf ("!sync\n");
		return -1;
	}

	// release original space
	bitmap_free (vol, original);

	printf ("\n");
	free (output);
#endif
