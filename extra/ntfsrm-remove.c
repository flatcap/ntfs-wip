static int ntfs_file_remove (ntfs_volume *vol, struct ntfs_dt *del, int del_num)
{
	struct ntfs_dir *find_dir = NULL;
	struct ntfs_dt *top = NULL;
	struct ntfs_dt *suc = NULL;
	struct ntfs_dt *old = NULL;
	struct ntfs_dt *par = NULL;
	struct ntfs_dt *ded = NULL;
	ntfschar *uname;
	int name_len;
	int suc_num = 0;
	int par_num = -1;
	INDEX_ENTRY *del_ie = NULL;
	INDEX_ENTRY *suc_ie = NULL;
	INDEX_ENTRY *par_ie = NULL;
	INDEX_ENTRY *add_ie = NULL;
	int res;
	VCN vcn;
	FILE_NAME_ATTR *file = NULL;

	if (!vol || !del) {
		return 1;
	}

	find_dir = del->dir;

	uname    = del->children[del_num]->key.file_name.file_name;
	name_len = del->children[del_num]->key.file_name.file_name_length;

	top = del->dir->index;
	ntfs_dt_find_all (top);
	ntfs_dt_print (top, 0);

	del_ie = del->children[del_num];

	/*
	 * If the key is not in a leaf node, then replace it with its successor.
	 * Continue the delete as if the successor had been deleted.
	 */

	if (del->header->flags & INDEX_NODE) {
		printf (BOLD YELLOW "Replace key with its successor:\n" END);

		vcn = ntfs_ie_get_vcn (del_ie);
		suc = ntfs_dt_find4 (find_dir->index, uname, name_len, &suc_num);
		suc_ie = ntfs_ie_copy (suc->children[suc_num]);
		suc_ie = ntfs_ie_set_vcn (suc_ie, vcn);

		file = &del_ie->key.file_name; printf ("\trep name: "); ntfs_name_print (file->file_name, file->file_name_length); printf ("\n");
		file = &suc_ie->key.file_name; printf ("\tsuc name: "); ntfs_name_print (file->file_name, file->file_name_length); printf ("\n");

		if (ntfs_dt_root (del))
			res = ntfs_dt_root_replace (del, del_num, del_ie, suc_ie);
		else
			res = ntfs_dt_alloc_replace (del, del_num, del_ie, suc_ie);

		free (suc_ie);

		if (res == FALSE)
			goto done;

		del     = suc;		// Continue delete with the successor
		del_num = suc_num;
		del_ie  = suc->children[suc_num];
	}

	/*
	 * Now we have the simpler case of deleting from a leaf node.
	 * If this step creates an empty node, we have more to do.
	 */

	printf ("\n");
	printf (BOLD YELLOW "Delete key:\n" END);

	file = &del->children[del_num]->key.file_name; printf ("\tdel name: "); ntfs_name_print (file->file_name, file->file_name_length); printf ("\n");

	// XXX if del->child_count == 2, we could skip this step
	// no, if we combine with another node, we'll have to remember
	if (ntfs_dt_root (del))
		ntfs_dt_root_remove (del, del_num);
	else
		ntfs_dt_alloc_remove (del, del_num);

	if (del->child_count > 1)	// XXX ntfs_dt_empty (dt),  ntfs_dt_full (dt, new)
		goto commit;

	/*
	 * Ascend the tree until we find a node that is not empty.  Take the
	 * ancestor key and unhook it.  This will free up some space in the
	 * index allocation.  Finally add the ancestor to the node of its
	 * successor.
	 */

	// find the key nearest the root which has no descendants
	printf ("\n");
	printf (BOLD YELLOW "Find childless parent:\n" END);
	for (par = del->parent, old = par; par; old = par, par = par->parent) {
		if (par->child_count > 1)
			break;
		par_num = ntfs_dt_find_parent (par);
	}

	printf ("par = %p, par->parent = %p, num = %d\n", par, par->parent, par_num);
	par_num = 0; // TEMP

	if (par) {
		file = &par->children[par_num]->key.file_name;
		printf ("\tpar name: ");
		ntfs_name_print (file->file_name, file->file_name_length);
		printf ("\n");
	}

	if (par == NULL) {
		// unhook everything
		goto freedts;
	}

	printf ("\n");

	// find if parent has left siblings
	if (par->children[par_num]->flags & INDEX_ENTRY_END) {
		printf (BOLD YELLOW "Swap the children of the parent and its left sibling\n" END);

		par_ie = par->children[par_num];
		vcn = ntfs_ie_get_vcn (par_ie);

		file = &par->children[par_num]  ->key.file_name; printf ("\tpar name: "); ntfs_name_print (file->file_name, file->file_name_length); printf ("\n");
		file = &par->children[par_num-1]->key.file_name; printf ("\tsib name: "); ntfs_name_print (file->file_name, file->file_name_length); printf ("\n");

		old                       = par->sub_nodes[par_num];
		par->sub_nodes[par_num]   = par->sub_nodes[par_num-1];
		par->sub_nodes[par_num-1] = old;

		par_ie = par->children[par_num-1];
		vcn = ntfs_ie_get_vcn (par_ie);

		par_ie = par->children[par_num];
		ntfs_ie_set_vcn (par_ie, vcn);

		par_num--;

		if (ntfs_dt_root (par))
			printf (GREEN "Modified: inode %lld, $INDEX_ROOT\n" END, par->dir->inode->mft_no);
		else
			printf (GREEN "Modified: inode %lld, $INDEX_ALLOCATION vcn %lld-%lld\n" END, par->dir->inode->mft_no, par->vcn, par->vcn + (par->dir->index_size>>9) - 1);
	}

	// unhook and hold onto the ded dt's
	printf ("\n");
	printf (BOLD YELLOW "Remove parent\n" END);

	file = &par->children[par_num]->key.file_name; printf ("\tpar name: "); ntfs_name_print (file->file_name, file->file_name_length); printf ("\n");

	add_ie = ntfs_ie_copy (par->children[par_num]);
	add_ie = ntfs_ie_remove_vcn (add_ie);
	if (!add_ie)
		goto done;

	ded = par->sub_nodes[par_num];
	par->sub_nodes[par_num] = NULL;

	if (ntfs_dt_root (par))
		ntfs_dt_root_remove (par, par_num);
	else
		ntfs_dt_alloc_remove (par, par_num);

	printf ("\n");
	printf (BOLD YELLOW "Add childless parent\n" END);

	file = &add_ie->key.file_name; printf ("\tadd name: "); ntfs_name_print (file->file_name, file->file_name_length); printf ("\n");
	suc     = NULL;
	suc_num = -1;
	suc = ntfs_dt_find4 (top, file->file_name, file->file_name_length, &suc_num);

	if (!suc)
		goto done;

	file = &suc->children[suc_num]->key.file_name; printf ("\tsuc name: "); ntfs_name_print (file->file_name, file->file_name_length); printf ("\n");

	// insert key into successor
	// if any new nodes are needed, reuse the preserved nodes
	if (!ntfs_dt_add2 (add_ie, suc, suc_num, ded))
		goto done;

	// remove any unused nodes

	// XXX mark dts, dirs and inodes dirty
	// XXX add freed dts to a list for immediate reuse (attach to dir?)
	// XXX any ded dts means we may need to adjust alloc
	// XXX commit will free list of spare dts
	// XXX reduce size of alloc
	// XXX if ded, don't write it back, just update bitmap

	printf ("empty\n");
	goto done;

freedts:
	printf ("\twhole dir is empty\n");
commit:
done:
	return 0;
}
