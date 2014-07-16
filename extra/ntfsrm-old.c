/**
 * ntfsrm
 */
static int ntfsrm (ntfs_volume *vol, char *name)
{
	struct ntfs_dir *dir = NULL;
	struct ntfs_dir *finddir = NULL;
	MFT_REF mft_num;
	ntfschar *uname = NULL;
	int len;

	dir = ntfs_dir_alloc (vol, FILE_root);
	if (!dir)
		return 1;

	//mft_num = ntfs_dir_find (dir, name);
	//printf ("%s = %lld\n", name, mft_num);

	mft_num = utils_pathname_to_mftref (vol, dir, name, &finddir);
	//printf ("mft_num = %lld\n", mft_num);
	//ntfs_dir_print (finddir, 0);

	if (!finddir) {
		printf ("Couldn't find the index entry for %s\n", name);
		return 1;
	}

	if (rindex (name, PATH_SEP))
		name = rindex (name, PATH_SEP) + 1;

	len = ntfs_mbstoucs (name, &uname, 0);
	if (len < 0)
		return 1;

	ntfs_dt_del_child (finddir->index, uname, len);

	ntfs_dir_free (dir);
	free (uname);
	return 0;
}

/**
 * ntfs_dt_del_child
 */
static int ntfs_dt_del_child (struct ntfs_dt *dt, ntfschar *uname, int len)
{
	struct ntfs_dt *del;
	INDEX_ENTRY *ie;
	ntfs_inode *ichild = NULL;
	ntfs_inode *iparent = NULL;
	ntfs_attr *attr = NULL;
	ntfs_attr_search_ctx *ctx = NULL;
	int index_num = 0;
	int res = 1;
	ATTR_RECORD *arec = NULL;
	MFT_REF mft_num = -1;
	FILE_NAME_ATTR *file;
	int filenames = 0;

	// compressed & encrypted files?

	del = ntfs_dt_find2 (dt, uname, len, &index_num);
	if (!del) {
		printf ("can't find item to delete\n");
		goto close;
	}

	if ((index_num < 0) || (index_num >= del->child_count)) {
		printf ("error in dt_find\n");
		goto close;
	}

	if (del->header->flags & INDEX_NODE) {
		printf ("can only delete leaf nodes\n");
		goto close;
	}

	/*
	if (!del->parent) {
		printf ("has 0xA0, but isn't in use\n");
		goto close;
	}
	*/

	ie = del->children[index_num];
	if (ie->key.file_name.file_attributes & FILE_ATTR_DIRECTORY) {
		printf ("can't delete directories\n");
		goto close;
	}

	if (ie->key.file_name.file_attributes & FILE_ATTR_SYSTEM) {
		printf ("can't delete system files\n");
		goto close;
	}

	ichild = ntfs_inode_open2 (dt->dir->vol, MREF (ie->indexed_file));
	if (!ichild) {
		printf ("can't open inode\n");
		goto close;
	}

	ctx = ntfs_attr_get_search_ctx (NULL, ichild->mrec);
	if (!ctx) {
		printf ("can't create a search context\n");
		goto close;
	}

	while (ntfs_attr_lookup(AT_UNUSED, NULL, 0, 0, 0, NULL, 0, ctx) == 0) {
		arec = ctx->attr;
		if (arec->type == AT_ATTRIBUTE_LIST) {
			printf ("can't delete files with an attribute list\n");
			goto close;
		}
		if (arec->type == AT_INDEX_ROOT) {
			printf ("can't delete directories\n");
			goto close;
		}
		if (arec->type == AT_FILE_NAME) {
			filenames++;
			file = (FILE_NAME_ATTR*) ((u8*) arec + arec->value_offset);
			mft_num = MREF (file->parent_directory);
		}
	}

	if (filenames != 1) {
		printf ("file has more than one name\n");
		goto close;
	}

	iparent = ntfs_inode_open2 (dt->dir->vol, mft_num);
	if (!iparent) {
		printf ("can't open parent directory\n");
		goto close;
	}

	/*
	attr = ntfs_attr_open (iparent, AT_INDEX_ALLOCATION, I30, 4);
	if (!attr) {
		printf ("parent doesn't have 0xA0\n");
		goto close;
	}
	*/

	//printf ("deleting file\n");
	//ntfs_dt_print (del->dir->index, 0);

	if (1) res = utils_free_non_residents (ichild);
	if (1) res = utils_mftrec_mark_free (dt->dir->vol, del->children[index_num]->indexed_file);
	if (1) res = utils_mftrec_mark_free2 (dt->dir->vol, del->children[index_num]->indexed_file);
	if (1) res = ntfs_dt_remove (del, index_num);

close:
	ntfs_attr_put_search_ctx (ctx);
	ntfs_attr_close (attr);
	ntfs_inode_close2 (iparent);
	ntfs_inode_close2 (ichild);

	return res;
}

/**
 * utils_free_non_residents
 */
static int utils_free_non_residents (ntfs_inode *inode)
{
	// XXX need to do this in memory

	ntfs_attr_search_ctx *ctx;
	ntfs_attr *na;
	ATTR_RECORD *arec;

	if (!inode)
		return -1;

	ctx = ntfs_attr_get_search_ctx (NULL, inode->mrec);
	if (!ctx) {
		printf ("can't create a search context\n");
		return -1;
	}

	while (ntfs_attr_lookup(AT_UNUSED, NULL, 0, 0, 0, NULL, 0, ctx) == 0) {
		arec = ctx->attr;
		if (arec->non_resident) {
			na = ntfs_attr_open (inode, arec->type, NULL, 0);
			if (na) {
				runlist_element *rl;
				LCN size;
				LCN count;
				ntfs_attr_map_whole_runlist (na);
				rl = na->rl;
				size = na->allocated_size >> inode->vol->cluster_size_bits;
				for (count = 0; count < size; count += rl->length, rl++) {
					//printf ("rl(%llu,%llu,%lld)\n", rl->vcn, rl->lcn, rl->length);
					//printf ("freed %d\n", ntfs_cluster_free (inode->vol, na, rl->vcn, rl->length));
					ntfs_cluster_free (inode->vol, na, rl->vcn, rl->length);
				}
				ntfs_attr_close (na);
			}
		}
	}

	ntfs_attr_put_search_ctx (ctx);
	return 0;
}

/**
 * utils_mftrec_mark_free
 */
static int utils_mftrec_mark_free (ntfs_volume *vol, MFT_REF mref)
{
	static u8 buffer[512];
	static s64 bmpmref = -sizeof (buffer) - 1; /* Which bit of $BITMAP is in the buffer */

	int byte, bit;

	if (!vol) {
		errno = EINVAL;
		return -1;
	}

	mref = MREF (mref);
	//printf ("mref = %lld\n", mref);
	/* Does mref lie in the section of $Bitmap we already have cached? */
	if (((s64)mref < bmpmref) || ((s64)mref >= (bmpmref +
			(sizeof (buffer) << 3)))) {
		Dprintf ("Bit lies outside cache.\n");

		/* Mark the buffer as not in use, in case the read is shorter. */
		memset (buffer, 0, sizeof (buffer));
		bmpmref = mref & (~((sizeof (buffer) << 3) - 1));

		if (ntfs_attr_pread (vol->mftbmp_na, (bmpmref>>3), sizeof (buffer), buffer) < 0) {
			Eprintf ("Couldn't read $MFT/$BITMAP: %s\n", strerror (errno));
			return -1;
		}

		Dprintf ("Reloaded bitmap buffer.\n");
	}

	bit  = 1 << (mref & 7);
	byte = (mref >> 3) & (sizeof (buffer) - 1);
	Dprintf ("cluster = %lld, bmpmref = %lld, byte = %d, bit = %d, in use %d\n",
		mref, bmpmref, byte, bit, buffer[byte] & bit);

	if ((buffer[byte] & bit) == 0) {
		Eprintf ("MFT record isn't in use (1).\n");
		return -1;
	}

	//utils_dump_mem (buffer, byte, 1, DM_NO_ASCII);
	buffer[byte] &= ~bit;
	//utils_dump_mem (buffer, byte, 1, DM_NO_ASCII);

	if (ntfs_attr_pwrite (vol->mftbmp_na, (bmpmref>>3), sizeof (buffer), buffer) < 0) {
		Eprintf ("Couldn't write $MFT/$BITMAP: %s\n", strerror (errno));
		return -1;
	}

	return (buffer[byte] & bit);
}

/**
 * utils_mftrec_mark_free2
 */
static int utils_mftrec_mark_free2 (ntfs_volume *vol, MFT_REF mref)
{
	u8 buffer[1024];
	s64 res;
	MFT_RECORD *rec;

	if (!vol)
		return -1;

	mref = MREF (mref);
	rec = (MFT_RECORD*) buffer;

	res = ntfs_mft_record_read (vol, mref, rec);
	printf ("res = %lld\n", res);

	if ((rec->flags & MFT_RECORD_IN_USE) == 0) {
		Eprintf ("MFT record isn't in use (2).\n");
		return -1;
	}

	rec->flags &= ~MFT_RECORD_IN_USE;

	//printf ("\n");
	//utils_dump_mem (buffer, 0, 1024, DM_DEFAULTS);

	res = ntfs_mft_record_write (vol, mref, rec);
	printf ("res = %lld\n", res);

	return 0;
}

/**
 * ntfs_dt_remove
 */
static int ntfs_dt_remove (struct ntfs_dt *dt, int index_num)
{
	if (!dt)
		return 1;
	if ((index_num < 0) || (index_num >= dt->child_count))
		return 1;

	if (ntfs_dt_root (dt))
		return ntfs_dt_remove_root (dt, index_num);
	else
		return ntfs_dt_remove_alloc (dt, index_num);
}

/**
 * ntfs_dt_remove_alloc
 */
static int ntfs_dt_remove_alloc (struct ntfs_dt *dt, int index_num)
{
	INDEX_ENTRY *ie = NULL;
	int i;
	u8 *dst;
	u8 *src;
	u8 *end;
	int off;
	int len;
	s64 res;

	//printf ("removing entry %d of %d\n", index_num+1, dt->child_count);
	//printf ("index size = %d\n", dt->data_len);
	//printf ("index use  = %d\n", dt->header->index_length);

	//utils_dump_mem (dt->data, 0, dt->data_len, DM_DEFAULTS);

	off = (u8*)dt->children[0] - dt->data;
	for (i = 0; i < dt->child_count; i++) {
		ie = dt->children[i];

		//printf ("%2d  %4d ", i, off);
		off += ie->length;

		if (ie->flags & INDEX_ENTRY_END) {
			//printf ("END (%d)\n", ie->length);
			break;
		}

		//ntfs_name_print (ie->key.file_name.file_name, ie->key.file_name.file_name_length);
		//printf (" (%d)\n", ie->length);
	}
	//printf ("total = %d\n", off);

	ie = dt->children[index_num];
	dst = (u8*)ie;

	src  = dst + ie->length;

	ie = dt->children[dt->child_count-1];
	end = (u8*)ie + ie->length;

	len  = end - src;

	//printf ("move %d bytes\n", len);
	//printf ("%d, %d, %d\n", dst - dt->data, src - dt->data, len);
	memmove (dst, src, len);

	//printf ("clear %d bytes\n", dt->data_len - (dst - dt->data) - len);
	//printf ("%d, %d, %d\n", dst - dt->data + len, 0, dt->data_len - (dst - dt->data) - len);

	//ntfs_dt_print (dt->dir->index, 0);

	memset (dst + len, 0, dt->data_len - (dst - dt->data) - len);

	for (i = 0; i < dt->child_count; i++) {
		if (dt->sub_nodes[i]) {
			printf ("this shouldn't happen %p\n", dt->sub_nodes[i]);
			ntfs_dt_free (dt->sub_nodes[i]);	// shouldn't be any, yet
		}
	}

	free (dt->sub_nodes);
	dt->sub_nodes = NULL;
	free (dt->children);
	dt->children = NULL;
	dt->child_count = 0;

	//printf ("before = %d\n", dt->header->index_length + 24);
	dt->header->index_length -= src - dst;
	//printf ("after  = %d\n", dt->header->index_length + 24);

	ntfs_dt_count_alloc (dt);

	//utils_dump_mem (dt->data, 0, dt->data_len, DM_DEFAULTS);

#if 0
	//printf ("\n");
	//printf ("index size = %d\n", dt->data_len);
	//printf ("index use  = %d\n", dt->header.index_length);

	off = (u8*)dt->children[0] - dt->data;
	for (i = 0; i < dt->child_count; i++) {
		ie = dt->children[i];

		printf ("%2d  %4d ", i, off);
		off += ie->length;

		if (ie->flags & INDEX_ENTRY_END) {
			printf ("END (%d)\n", ie->length);
			break;
		}

		ntfs_name_print (ie->key.file_name.file_name,
				 ie->key.file_name.file_name_length);
		printf (" (%d)\n", ie->length);
	}
#endif
	//utils_dump_mem (dt->data, 0, dt->data_len, DM_DEFAULTS);
	res = ntfs_attr_mst_pwrite (dt->dir->ialloc, dt->vcn*512, 1, dt->data_len, dt->data);
	printf ("res = %lld\n", res);

	return 0;
}

/**
 * ntfs_dt_remove_root
 */
static int ntfs_dt_remove_root (struct ntfs_dt *dt, int index_num)
{
	INDEX_ENTRY *ie = NULL;
	INDEX_ROOT *ir = NULL;
	int i;
	u8 *dst;
	u8 *src;
	u8 *end;
	int off;
	int len;
	s64 res;

	//printf ("removing entry %d of %d\n", index_num+1, dt->child_count);
	//printf ("index size = %d\n", dt->data_len);
	//printf ("index use  = %d\n", dt->header->index_length);

	//utils_dump_mem (dt->data, 0, dt->data_len, DM_DEFAULTS);

	off = (u8*)dt->children[0] - dt->data;
	for (i = 0; i < dt->child_count; i++) {
		ie = dt->children[i];

		//printf ("%2d  %4d ", i+1, off);
		off += ie->length;

		if (ie->flags & INDEX_ENTRY_END) {
			//printf ("END (%d)\n", ie->length);
			break;
		}

		//ntfs_name_print (ie->key.file_name.file_name, ie->key.file_name.file_name_length);
		//printf (" (%d)\n", ie->length);
	}
	//printf ("total = %d\n", off);

	ie = dt->children[index_num];
	dst = (u8*)ie;

	src  = dst + ie->length;

	ie = dt->children[dt->child_count-1];
	end = (u8*)ie + ie->length;

	len  = end - src;

	//printf ("move %d bytes\n", len);
	//printf ("%d, %d, %d\n", dst - dt->data, src - dt->data, len);
	memmove (dst, src, len);

	dt->data_len -= (src - dt->data - sizeof (INDEX_ROOT));
	dt->child_count--;

	ir = (INDEX_ROOT*) dt->data;
	ir->index.index_length   = dt->data_len - 16;
	ir->index.allocated_size = dt->data_len - 16;

	ntfs_mft_resize_resident (dt->dir->inode, AT_INDEX_ROOT, I30, 4, dt->data, dt->data_len);
	dt->data = realloc (dt->data, dt->data_len);

	//printf ("ih->index_length   = %d\n", ir->index.index_length);
	//printf ("ih->allocated_size = %d\n", ir->index.allocated_size);
	//printf ("dt->data_len       = %d\n", dt->data_len);

	//utils_dump_mem (dt->data, 0, dt->data_len, DM_DEFAULTS);
	//ntfs_dt_print (dt->dir->index, 0);
#if 1
	for (i = 0; i < dt->child_count; i++) {
		if (dt->sub_nodes[i]) {
			printf ("this shouldn't happen %p\n", dt->sub_nodes[i]);
			ntfs_dt_free (dt->sub_nodes[i]);	// shouldn't be any, yet
		}
	}

	free (dt->sub_nodes);
	dt->sub_nodes = NULL;
	free (dt->children);
	dt->children = NULL;
	dt->child_count = 0;

	//printf ("before = %d\n", dt->header->index_length + 24);
	dt->header->index_length -= src - dst;
	//printf ("after  = %d\n", dt->header->index_length + 24);

	ntfs_dt_count_root (dt);
#endif
	//utils_dump_mem (dt->data, 0, dt->data_len, DM_DEFAULTS);

#if 0
	//printf ("\n");
	//printf ("index size = %d\n", dt->data_len);
	//printf ("index use  = %d\n", dt->header.index_length);

	off = (u8*)dt->children[0] - dt->data;
	for (i = 0; i < dt->child_count; i++) {
		ie = dt->children[i];

		printf ("%2d  %4d ", i, off);
		off += ie->length;

		if (ie->flags & INDEX_ENTRY_END) {
			printf ("END (%d)\n", ie->length);
			break;
		}

		ntfs_name_print (ie->key.file_name.file_name,
				 ie->key.file_name.file_name_length);
		printf (" (%d)\n", ie->length);
	}
#endif
	//utils_dump_mem (dt->data, 0, dt->data_len, DM_DEFAULTS);

	res = ntfs_mft_record_write (dt->dir->inode->vol, dt->dir->inode->mft_no, dt->dir->inode->mrec);
	printf ("res = %lld\n", res);

	return 0;
}

/**
 * ntfs_test_bmp
 */
static int ntfs_test_bmp (ntfs_volume *vol, ntfs_inode *inode)
{
	ntfs_inode *volbmp;
	struct ntfs_bmp *bmp;
	struct ntfs_bmp *bmp2;
	//u8 *buffer;
	//int i;

	volbmp = ntfs_inode_open2 (vol, FILE_Bitmap);
	if (!volbmp)
		return 1;

	bmp = ntfs_bmp_alloc (volbmp, AT_DATA, NULL, 0);
	if (!bmp)
		return 1;

	bmp2 = ntfs_bmp_alloc (vol->mft_ni, AT_BITMAP, NULL, 0);
	if (!bmp2)
		return 1;

	if (0) ntfs_bmp_set_range (bmp, 0, 9, 1);
	if (0) utils_free_non_residents2 (inode, bmp);
	if (0) utils_mftrec_mark_free3 (bmp2, inode->mft_no);
	if (0) utils_mftrec_mark_free4 (inode);

	ntfs_bmp_free (bmp);
	return 0;
}

/**
 * utils_mftrec_mark_free3
 */
static int utils_mftrec_mark_free3 (struct ntfs_bmp *bmp, MFT_REF mref)
{
	return ntfs_bmp_set_range (bmp, (VCN) MREF (mref), 1, 0);
}

/**
 * utils_mftrec_mark_free4
 */
static int utils_mftrec_mark_free4 (ntfs_inode *inode)
{
	MFT_RECORD *rec;

	if (!inode)
		return -1;

	rec = (MFT_RECORD*) inode->mrec;

	if ((rec->flags & MFT_RECORD_IN_USE) == 0) {
		Eprintf ("MFT record isn't in use (3).\n");
		return -1;
	}

	rec->flags &= ~MFT_RECORD_IN_USE;

	//printf ("\n");
	//utils_dump_mem (buffer, 0, 1024, DM_DEFAULTS);

	printf (GREEN "Modified: inode %lld MFT_RECORD header\n" END, inode->mft_no);
	return 0;
}

/**
 * ntfs_file_add
 */
static int ntfs_file_add (ntfs_volume *vol, char *name)
{
	struct ntfs_dir *dir = NULL;
	struct ntfs_dir *finddir = NULL;
	struct ntfs_dt *del = NULL;
	INDEX_ENTRY *ie = NULL;
	MFT_REF mft_num;
	ntfschar *uname = NULL;
	int len;
	int index_num = 0;

	dir = ntfs_dir_alloc (vol, FILE_root);
	if (!dir)
		return 1;

	mft_num = utils_pathname_to_mftref (vol, dir, name, &finddir);
	//printf ("mft_num = %lld\n", mft_num);
	//ntfs_dir_print (finddir, 0);

	if (!finddir) {
		printf ("Couldn't find the index entry for %s\n", name);
		return 1;
	}

	if (rindex (name, PATH_SEP))
		name = rindex (name, PATH_SEP) + 1;

	len = ntfs_mbstoucs (name, &uname, 0);
	if (len < 0)
		return 1;

	del = ntfs_dt_find2 (finddir->index, uname, len, &index_num);
	if (!del) {
		printf ("can't find item to delete\n");
		goto done;
	}

	ie = ntfs_ie_copy (del->children[index_num]);
	if (!ie)
		goto done;

	free (uname);
	uname = NULL;

	len = ntfs_mbstoucs ("file26a", &uname, 0);
	if (len < 0)
		goto done;

	ie = ntfs_ie_set_name (ie, uname, len, FILE_NAME_WIN32);
	if (!ie)
		goto done;

	//utils_dump_mem ((u8*)ie, 0, ie->length, DM_DEFAULTS);
	//printf ("\n");
	//printf ("ie = %lld\n", MREF (ie->indexed_file));
	//ntfs_dt_del_child (finddir->index, uname, len);

	//ntfs_dt_print (finddir->index, 0);
	ntfs_dt_add (finddir->index, ie);

	// test
	if (0) ntfs_dt_alloc_add (del, ie);
	if (0) ntfs_dt_root_add (del, ie);
	// test

done:
	ntfs_dir_free (dir);
	free (uname);
	free (ie);
	return 0;
}

/**
 * ntfs_dt_add
 */
static int ntfs_dt_add (struct ntfs_dt *parent, INDEX_ENTRY *ie)
{
	FILE_NAME_ATTR *file;
	struct ntfs_dt *dt;
	int index_num = -1;

	if (!ie)
		return 0;

	file = &ie->key.file_name;

	dt = ntfs_dt_find3 (parent, file->file_name, file->file_name_length, &index_num);
	if (!dt)
		return 0;

	//printf ("dt = %p, index = %d\n", dt, index_num);
	//ntfs_ie_dump (dt->children[index_num]);
	//utils_dump_mem ((u8*)dt->children[index_num], 0, dt->children[index_num]->length, DM_DEFAULTS);
	//printf ("\n");

	if (0) ntfs_dt_add_alloc (dt, index_num, ie, NULL);
	if (0) ntfs_dt_add_root (dt->dir->index, 0, ie, NULL);

	return 0;
}

/**
 * utils_pathname_to_mftref
 */
static MFT_REF utils_pathname_to_mftref (ntfs_volume *vol, struct ntfs_dir *parent, const char *pathname, struct ntfs_dir **finddir)
{
	MFT_REF mft_num;
	MFT_REF result = -1;
	char *p, *q;
	char *ascii = NULL;
	struct ntfs_dir *dir = NULL;

	if (!vol || !parent || !pathname) {
		errno = EINVAL;
		return -1;
	}

	ascii = strdup (pathname);		// Work with a r/w copy
	if (!ascii) {
		Eprintf ("Out of memory.\n");
		goto close;
	}

	p = ascii;
	while (p && *p && *p == PATH_SEP)	// Remove leading /'s
		p++;
	while (p && *p) {
		q = strchr (p, PATH_SEP);	// Find the end of the first token
		if (q != NULL) {
			*q = '\0';
			q++;
		}

		//printf ("looking for %s in %p\n", p, parent);
		mft_num = ntfs_dir_find (parent, p);
		if (mft_num == (u64)-1) {
			Eprintf ("Couldn't find name '%s' in pathname '%s'.\n", p, pathname);
			goto close;
		}

		if (q) {
			dir = ntfs_dir_alloc (vol, mft_num);
			if (!dir) {
				Eprintf ("Couldn't allocate a new directory (%lld).\n", mft_num);
				goto close;
			}

			ntfs_dir_add (parent, dir);
			parent = dir;
		} else {
			//printf ("file %s\n", p);
			result = mft_num;
			if (finddir)
				*finddir = dir ? dir : parent;
			break;
		}

		p = q;
		while (p && *p && *p == PATH_SEP)
			p++;
	}

close:
	free (ascii);	// from strdup
	return result;
}

/**
 * ntfs_dir_find
 */
static MFT_REF ntfs_dir_find (struct ntfs_dir *dir, char *name)
{
	MFT_REF mft_num;
	ntfschar *uname = NULL;
	int len;

	if (!dir || !name)
		return -1;

	len = ntfs_mbstoucs (name, &uname, 0);
	if (len < 0)
		return -1;

	if (!dir->index)
		dir->index = ntfs_dt_alloc (dir, NULL, -1);

	//printf ("dir->index = %p\n", dir->index);
	//printf ("dir->child_count = %d\n", dir->child_count);
	//printf ("uname = %p\n", uname);
	mft_num = ntfs_dt_find (dir->index, uname, len);

	free (uname);
	return mft_num;
}

/**
 * ntfs_dt_alloc_children
 */
static INDEX_ENTRY ** ntfs_dt_alloc_children (INDEX_ENTRY **children, int count)
{
	// XXX calculate for 2K and 4K indexes max and min filenames (inc/exc VCN)
	int old = (count + 0x1e) & ~0x1f;
	int new = (count + 0x1f) & ~0x1f;

	if (old == new)
		return children;

	return realloc (children, new * sizeof (INDEX_ENTRY*));
}

/**
 * ntfs_dt_root_add
 */
static int ntfs_dt_root_add (struct ntfs_dt *add, INDEX_ENTRY *add_ie)
{
	FILE_NAME_ATTR *file;
	struct ntfs_dt *suc;
	int suc_num;
	int need;
	int space;
	u8 *src;
	u8 *dst;
	int len;
	u8 *new_data = NULL;
	int i;

	if (!add || !add_ie)
		return -1;

	need  = add_ie->length;
	space = ntfs_mft_free_space (add->dir);

	printf ("need %d, have %d\n", need, space);
	if (need > space) {
		printf ("no room\n");
		return -1;
	}

	new_data = realloc (add->data, add->data_len + need);
	if (!new_data) {
		return -1;
	}

	memset (new_data + add->data_len, 0, need);

	for (i = 0; i < add->child_count; i++)
		add->children[i] = (INDEX_ENTRY*) ((long)add->children[i] + (long)new_data - (long)add->data);	// rebase the children

	add->data = new_data;
	new_data = NULL;

	file = &add_ie->key.file_name;

	suc = ntfs_dt_find3 (add, file->file_name, file->file_name_length, &suc_num);
	if (!suc)
		return -1;

	// hmm, suc == add (i.e. entry already exists)

	src = (u8*) suc->children[suc_num];
	dst = src + need;
	len = ((add->data + add->data_len) - src);

	memmove (dst, src, len);

	dst = src;
	src = (u8*) add_ie;
	len = add_ie->length;

	memcpy (dst, src, len);

	add->data_len += need;
	add->header = (INDEX_HEADER*) (add->data + 0x10);
	add->header->index_length   = add->data_len - 16;
	add->header->allocated_size = add->data_len - 16;

	ntfs_mft_resize_resident (add->dir->inode, AT_INDEX_ROOT, I30, 4, add->data, add->data_len);

	//utils_dump_mem (add->data, 0, add->data_len, DM_DEFAULTS);
	//printf ("\n");

	add->changed = TRUE;

	printf (GREEN "Modified: inode %lld, $INDEX_ROOT\n" END, add->dir->inode->mft_no);
	return suc_num;
}

/**
 * ntfs_dt_alloc_add
 */
static int ntfs_dt_alloc_add (struct ntfs_dt *add, INDEX_ENTRY *add_ie)
{
	FILE_NAME_ATTR *file;
	struct ntfs_dt *suc_dt;
	int suc_num;
	int need;
	int space;
	u8 *src;
	u8 *dst;
	int len;

	if (!add || !add_ie)
		return 0;

	need  = add_ie->length;
	space = add->data_len - add->header->index_length - 24;

	file = &add_ie->key.file_name;

	suc_dt = ntfs_dt_find3 (add, file->file_name, file->file_name_length, &suc_num);
	if (!suc_dt)
		return 0;

	// hmm, suc_dt == add

	printf ("need %d, have %d\n", need, space);
	if (need > space) {
		printf ("no room");
		return 0;
	}

	//utils_dump_mem (add->data, 0, add->data_len, DM_DEFAULTS);
	//printf ("\n");

	src = (u8*) suc_dt->children[suc_num];
	dst = src + need;
	len = add->data + add->data_len - src - space;
	//printf ("src = %d\n", src - add->data);
	//printf ("dst = %d\n", dst - add->data);
	//printf ("len = %d\n", len);

	memmove (dst, src, len);

	dst = src;
	src = (u8*) add_ie;
	len = need;

	memcpy (dst, src, len);

	add->header->index_length += len;

	dst = add->data     + add->header->index_length + 24;
	len = add->data_len - add->header->index_length - 24;

	memset (dst, 0, len);

	//utils_dump_mem (add->data, 0, add->data_len, DM_DEFAULTS);
	//printf ("\n");

	add->changed = TRUE;

	printf (GREEN "Modified: inode %lld, $INDEX_ALLOCATION vcn %lld-%lld\n" END, add->dir->inode->mft_no, add->vcn, add->vcn + (add->dir->index_size>>9) - 1);
	return 0;
}

/**
 * ntfs_dir_map
 */
static int ntfs_dir_map (ntfs_volume *vol, struct ntfs_dir *dir)
{
	//struct ntfs_dt *dt;

	if (!vol)
		return 1;
	if (!dir)
		return 1;

#if 0
	printf ("dir = %p\n", dir);
	printf ("vol = %p\n", dir->vol);
	printf ("parent = %p\n", dir->parent);
	printf ("name = "); ntfs_name_print (dir->name, dir->name_len); printf ("\n");
	printf ("mftnum = %lld\n", MREF (dir->mft_num));
	printf ("dt = %p\n", dir->index);
	printf ("children = %p (%d)\n", dir->children, dir->child_count);
	printf ("bitmap = %p\n", dir->bitmap);
	printf ("inode = %p\n", dir->inode);
	printf ("iroot = %p\n", dir->iroot);
	printf ("ialloc = %p\n", dir->ialloc);
	printf ("isize = %d\n", dir->index_size);
#endif

	//dt->data_len = dir->iroot->allocated_size;
	//dt->data     = malloc (dt->data_len);
	//ntfs_attr_pread (dir->iroot, 0, dt->data_len, dt->data);

	//ntfs_dt_root_count (dt);

	return 0;
}

/**
 * ntfs_inode_dir_map
 */
static void ntfs_inode_dir_map (ntfs_inode *ino)
{
	ATTR_RECORD *rec;
	FILE_NAME_ATTR *fn;
	ntfs_inode *parent;

	if (!ino)
		return;

	printf ("open inode %lld\n", ino->mft_no);

	if (ino->mft_no == FILE_root) {
		printf ("done\n");
		return;
	}

	rec = find_first_attribute (AT_FILE_NAME, ino->mrec);
	if (!rec)
		return;

	fn = (FILE_NAME_ATTR *) ((char *) rec + le16_to_cpu (rec->value_offset));

	parent = ntfs_inode_open (ino->vol, fn->parent_directory);
	if (parent) {
		ntfs_inode_dir_map (parent);
		ntfs_inode_close (parent);
	}
}

/**
 * ntfs_binary_print
 */
static void ntfs_binary_print (u8 num, BOOL backwards, BOOL colour)
{
	int i;

	if (backwards)
		for (i = 1; i < 129; i<<=1) {
			if (colour)
				printf ("%s", (num&i) ? "[31m1[0m" : "0");
			else
				printf ("%s", (num&i) ? "1" : "0");
		}
	else
		for (i = 128; i > 0; i>>=1) {
			if (colour)
				printf ("%s", (num&i) ? "[31m1[0m" : "0");
			else
				printf ("%s", (num&i) ? "1" : "0");
		}
}

/**
 * ntfs_test_bmp2
 */
static int ntfs_test_bmp2 (ntfs_volume *vol)
{
	struct ntfs_bmp *bmp;
	int i, j;
	u8 value = 0xFF;

	bmp = calloc (1, sizeof (*bmp));
	if (!bmp)
		return 1;

	bmp->vol = vol;
	bmp->attr = calloc (1, sizeof (*bmp->attr));
	bmp->attr->type = 0xB0;
	bmp->attr->ni = calloc (1, sizeof (*bmp->attr->ni));
	bmp->count = 2;
	bmp->data = calloc (4, sizeof (u8*));
	bmp->data[0] = calloc (1, vol->cluster_size);
	bmp->data[1] = calloc (1, vol->cluster_size);
	bmp->data_vcn = calloc (4, sizeof (VCN));
	bmp->data_vcn[0] = 0;
	bmp->data_vcn[1] = 1;

	for (j = 4090; j < 4103; j++) {
		memset (bmp->data[0], ~value, vol->cluster_size);
		memset (bmp->data[1], ~value, vol->cluster_size);
		ntfs_bmp_set_range (bmp, j, 7, value);
		for (i = 0; i < 4; i++) { ntfs_binary_print (bmp->data[0][508+i], TRUE, TRUE); printf (" "); } printf ("| ");
		for (i = 0; i < 4; i++) { ntfs_binary_print (bmp->data[1][i], TRUE, TRUE); printf (" "); } printf ("\n");
	}

	printf ("\n");
	for (j = 0; j < 15; j++) {
		memset (bmp->data[0], ~value, vol->cluster_size);
		ntfs_bmp_set_range (bmp, j, 1, value);
		for (i = 0; i < 8; i++) { ntfs_binary_print (bmp->data[0][i], TRUE, TRUE); printf (" "); } printf ("\n");
	}

	printf ("\n");
	for (j = 0; j < 15; j++) {
		memset (bmp->data[0], ~value, vol->cluster_size);
		ntfs_bmp_set_range (bmp, j, 2, value);
		for (i = 0; i < 8; i++) { ntfs_binary_print (bmp->data[0][i], TRUE, TRUE); printf (" "); } printf ("\n");
	}

	printf ("\n");
	for (j = 0; j < 15; j++) {
		memset (bmp->data[0], ~value, vol->cluster_size);
		ntfs_bmp_set_range (bmp, j, 7, value);
		for (i = 0; i < 8; i++) { ntfs_binary_print (bmp->data[0][i], TRUE, TRUE); printf (" "); } printf ("\n");
	}

	printf ("\n");
	for (j = 0; j < 15; j++) {
		memset (bmp->data[0], ~value, vol->cluster_size);
		ntfs_bmp_set_range (bmp, j, 8, value);
		for (i = 0; i < 8; i++) { ntfs_binary_print (bmp->data[0][i], TRUE, TRUE); printf (" "); } printf ("\n");
	}

	free (bmp->attr->ni);
	ntfs_bmp_free (bmp);

	return 0;
}

/**
 * ntfs_index_dump_alloc
 */
static int ntfs_index_dump_alloc (ntfs_attr *attr, VCN vcn, int indent)
{
	u8 buffer[4096];
	INDEX_BLOCK *block;
	INDEX_ENTRY *entry;
	u8 *ptr;
	int size;
	VCN *newvcn = 0;

	ntfs_attr_mst_pread (attr, vcn*512, 1, sizeof (buffer), buffer);

	block = (INDEX_BLOCK*) buffer;
	size = block->index.allocated_size;

	for (ptr = buffer + 64; ptr < (buffer + size); ptr += entry->length) {
		entry = (INDEX_ENTRY*) ptr;

		if (entry->flags & INDEX_ENTRY_NODE) {
			newvcn = (VCN*) (ptr + ROUND_UP(entry->length, 8) - 8);
			ntfs_index_dump_alloc (attr, *newvcn, indent+4);
		}

		printf ("%.*s", indent, space_line);

		if (entry->flags & INDEX_ENTRY_END) {
			printf ("[END]");
		} else {
			ntfs_name_print (entry->key.file_name.file_name, entry->key.file_name.file_name_length);
		}

		if (entry->flags & INDEX_ENTRY_NODE) {
			printf (" (%lld)\n", *newvcn);
		} else {
			printf ("\n");
		}

		if (entry->flags & INDEX_ENTRY_END)
			break;
	}
	//printf ("%.*s", indent, space_line);
	//printf ("fill = %u/%u\n", (unsigned)block->index.index_length, (unsigned)block->index.allocated_size);
	return 0;
}

/**
 * ntfs_index_dump
 */
static int ntfs_index_dump (ntfs_inode *inode)
{
	u8 buffer[1024];
	ntfs_attr *iroot;
	ntfs_attr *ialloc;
	INDEX_ROOT *root;
	INDEX_ENTRY *entry;
	u8 *ptr;
	int size;
	VCN *vcn = 0;

	if (!inode)
		return 0;

	iroot  = ntfs_attr_open (inode, AT_INDEX_ROOT, I30, 4);
	if (!iroot) {
		printf ("not a directory\n");
		return 0;
	}

	ialloc = ntfs_attr_open (inode, AT_INDEX_ALLOCATION, I30, 4);

	size = (int) ntfs_attr_pread (iroot, 0, sizeof (buffer), buffer);

	root = (INDEX_ROOT*) buffer;

	ptr = buffer + root->index.entries_offset + 0x10;

	while (ptr < (buffer + size)) {
		entry = (INDEX_ENTRY*) ptr;
		if (entry->flags & INDEX_ENTRY_NODE) {
			vcn = (VCN*) (ptr + ROUND_UP(entry->length, 8) - 8);
			ntfs_index_dump_alloc (ialloc, *vcn, 4);
		}

		if (entry->flags & INDEX_ENTRY_END) {
			printf ("[END]");
		} else {
			ntfs_name_print (entry->key.file_name.file_name, entry->key.file_name.file_name_length);
		}

		if (entry->flags & INDEX_ENTRY_NODE) {
			printf (" (%lld)", *vcn);
		}
		printf ("\n");

		ptr += entry->length;
	}

	ntfs_attr_close (iroot);
	ntfs_attr_close (ialloc);

	//printf ("fill = %d\n", ptr - buffer);
	return 0;
}

/**
 * ntfs_ie_test
 */
static int ntfs_ie_test (void)
{
	INDEX_ENTRY *ie1 = NULL;
	INDEX_ENTRY *ie2 = NULL;
	int namelen = 0;
	ntfschar *name = NULL;

	if (1) {
		ie1 = ntfs_ie_create();
		//ntfs_ie_dump (ie1);
	}

	if (0) {
		ie2 = ntfs_ie_copy (ie1);
		ntfs_ie_dump (ie2);
	}

	if (1) {
		namelen = ntfs_mbstoucs("richard", &name, 0);
		ie1 = ntfs_ie_set_name (ie1, name, namelen, FILE_NAME_WIN32);
		free (name);
		name = NULL;
		ntfs_ie_dump (ie1);
	}

	if (1) {
		namelen = ntfs_mbstoucs("richard2", &name, 0);
		ie1 = ntfs_ie_set_name (ie1, name, namelen, FILE_NAME_WIN32);
		free (name);
		name = NULL;
		ntfs_ie_dump (ie1);
	}

	if (1) {
		ie1 = ntfs_ie_set_vcn (ie1, 1234);
		ntfs_ie_dump (ie1);
	}

	if (1) {
		ie1 = ntfs_ie_remove_vcn (ie1);
		ntfs_ie_dump (ie1);
	}

	if (0) {
		ie1 = ntfs_ie_remove_name (ie1);
		ntfs_ie_dump (ie1);
	} else {
		ie1->indexed_file = 1234;
		ie1->key.file_name.parent_directory = 5;
		ie1->key.file_name.creation_time = utc2ntfs (time(NULL));
		ie1->key.file_name.last_data_change_time = utc2ntfs (time(NULL));
		ie1->key.file_name.last_mft_change_time = utc2ntfs (time(NULL));
		ie1->key.file_name.last_access_time = utc2ntfs (time(NULL));
		ie1->key.file_name.allocated_size = 4096;
		ie1->key.file_name.data_size = 3973;
	}

	ntfs_ie_dump (ie1);
	free (name);
	ntfs_ie_free (ie1);
	ntfs_ie_free (ie2);
	return 0;
}

/**
 * ntfs_ie_dump
 */
static void ntfs_ie_dump (INDEX_ENTRY *ie)
{
	if (!ie)
		return;

	printf ("________________________________________________");
	printf ("\n");
	utils_dump_mem (ie, 0, ie->length, DM_DEFAULTS);

	printf ("MFT Ref: 0x%llx\n", ie->indexed_file);
	printf ("length: %d\n", ie->length);
	printf ("keylen: %d\n", ie->key_length);
	printf ("flags: ");
		if (ie->flags & INDEX_ENTRY_NODE) printf ("NODE ");
		if (ie->flags & INDEX_ENTRY_END)  printf ("END");
		if (!(ie->flags & (INDEX_ENTRY_NODE | INDEX_ENTRY_END))) printf ("none");
	printf ("\n");
	printf ("reserved 0x%04x\n", ie->reserved);
	if (ie->key_length > 0) {
		printf ("mft parent: 0x%llx\n", ie->key.file_name.parent_directory);

		printf ("ctime: %s", utils_time_to_str(ie->key.file_name.creation_time));
		printf ("dtime: %s", utils_time_to_str(ie->key.file_name.last_data_change_time));
		printf ("mtime: %s", utils_time_to_str(ie->key.file_name.last_mft_change_time));
		printf ("atime: %s", utils_time_to_str(ie->key.file_name.last_access_time));
		printf ("alloc size: %lld\n", ie->key.file_name.allocated_size);
		printf ("data size: %lld\n", ie->key.file_name.data_size);
		printf ("file flags: 0x%04x\n", ie->key.file_name.file_attributes);
		printf ("reserved: 0x%04x\n", ie->key.file_name.reserved); printf ("name len: %d\n", ie->key.file_name.file_name_length);
		if (ie->key.file_name.file_name_length > 0) {
			int i, r;
			printf ("name type: %d\n", ie->key.file_name.file_name_type);
			printf ("name: ");
			ntfs_name_print (ie->key.file_name.file_name, ie->key.file_name.file_name_length);
			printf ("\n");
			r = ATTR_SIZE (2 * (ie->key.file_name.file_name_length+1)) - (2 * (ie->key.file_name.file_name_length+1));
			if (r > 0) {
				u8 *ptr;
				printf ("padding: ");
				ptr = (u8*) (ie->key.file_name.file_name +  ie->key.file_name.file_name_length);
				for (i = 0; i < r; i++, ptr++)
					printf ("0x%02x ", *ptr);
				printf ("\n");
			}
		}
	}
	if (ie->flags == INDEX_ENTRY_NODE) {
		printf ("child vcn = %lld\n", ntfs_ie_get_vcn (ie));
	}
}

/**
 * utils_time_to_str
 */
static const char * utils_time_to_str(const s64 sle_ntfs_clock)
{
	time_t unix_clock = ntfs2utc(sle_ntfs_clock);
	if (sle_ntfs_clock == 0)
		return "none\n";
	else
		return ctime(&unix_clock);
}

/**
 * utils_test
 */
utils_test
{
	u8 **ptr;
	int i;

	ptr = calloc (6, sizeof (u8*));

	for (i = 0; i < 6; i++) {
	}

	ptr[0] = (u8*) 0x12345678;
	ptr[1] = (u8*) 0x23456789;
	ptr[2] = (u8*) 0x3456789A;
	ptr[3] = (u8*) 0x456789AB;
	ptr[4] = (u8*) 0x56789ABC;
	ptr[5] = (u8*) 0x6789ABCD;

	for (i = 0; i < 6; i++) {
		printf ("%p ", ptr[i]);
	}
	printf ("\n");

	utils_array_insert (ptr, 6, sizeof (u8*), 4, 3);

	for (i = 0; i < 9; i++) {
		printf ("%p ", ptr[i]);
	}
	printf ("\n");

	utils_array_remove (ptr, 9, sizeof (u8*), 1, 2);

	for (i = 0; i < 9; i++) {
		printf ("%p ", ptr[i]);
	}
	printf ("\n");
}
