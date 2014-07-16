#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "logging.h"
#include "inode.h"
#include "attrib.h"

// ntfs_bmp
// ntfs_dir
// ntfs_dt
// ntfs_ie

// inode/attrlist
// mft_num/mft_ref

/**
 * dump_runlist
 */
static void dump_runlist(char *buffer, int buflen, runlist *rl)
{
	int i;

	if (!buffer || !buflen)
		return;

	if (!rl) {
		snprintf(buffer, buflen, "<none>");
		return;
	}

	for (i = 0; rl[i].length; i++)
		;

	snprintf(buffer, buflen, "%p, %d run%s", rl, i, (i!=1) ? "s" : "");
}

/**
 * dump_attr_state
 * Needs a max of 24 chars in the buffer
 */
static void dump_attr_state(char *buffer, int buflen, ntfs_attr *attr)
{
	if (!buffer || (buflen < 25) || !attr)
		return;

	if (attr->state == 0) {
		snprintf (buffer, buflen, "<none>");
		return;
	}

	if (NAttrInitialized(attr)) {
		snprintf(buffer, buflen, "Initialized ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (NAttrNonResident(attr)) {
		snprintf(buffer, buflen, "NonResident ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
}

/**
 * dump_file_attr_flags
 * Needs a max of 191 chars in the buffer
 */
static void dump_file_attr_flags(char *buffer, int buflen, u32 flags)
{
	if (!buffer || (buflen < 192))
		return;

	if (flags == 0) {
		snprintf(buffer, buflen, "<none>");
		return;
	}

	if (flags & FILE_ATTR_READONLY) {
		snprintf(buffer, buflen, "READONLY ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_HIDDEN) {
		snprintf(buffer, buflen, "HIDDEN ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_SYSTEM) {
		snprintf(buffer, buflen, "SYSTEM ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_DIRECTORY) {
		snprintf(buffer, buflen, "DIRECTORY ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_ARCHIVE) {
		snprintf(buffer, buflen, "ARCHIVE ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_DEVICE) {
		snprintf(buffer, buflen, "DEVICE ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_NORMAL) {
		snprintf(buffer, buflen, "NORMAL ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_TEMPORARY) {
		snprintf(buffer, buflen, "TEMPORARY ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_SPARSE_FILE) {
		snprintf(buffer, buflen, "SPARSE_FILE ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_REPARSE_POINT) {
		snprintf(buffer, buflen, "REPARSE_POINT ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_COMPRESSED) {
		snprintf(buffer, buflen, "COMPRESSED ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_OFFLINE) {
		snprintf(buffer, buflen, "OFFLINE ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_NOT_CONTENT_INDEXED) {
		snprintf(buffer, buflen, "NOT_CONTENT_INDEXED ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_ENCRYPTED) {
		snprintf(buffer, buflen, "ENCRYPTED ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_I30_INDEX_PRESENT) {
		snprintf(buffer, buflen, "I30_INDEX_PRESENT ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & FILE_ATTR_VIEW_INDEX_PRESENT) {
		snprintf(buffer, buflen, "VIEW_INDEX_PRESENT ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
}

/**
 * dump_attr_type
 * Needs a max of 24 chars in the buffer
 */
static void dump_attr_type(char *buffer, int buflen, u32 type)
{
	if (!buffer || (buflen < 25))
		return;

	switch (type) {
		case AT_UNUSED:
			snprintf(buffer, buflen, "UNUSED");
			break;
		case AT_STANDARD_INFORMATION:
			snprintf(buffer, buflen, "STANDARD_INFORMATION");
			break;
		case AT_ATTRIBUTE_LIST:
			snprintf(buffer, buflen, "ATTRIBUTE_LIST");
			break;
		case AT_FILE_NAME:
			snprintf(buffer, buflen, "FILE_NAME");
			break;
		case AT_OBJECT_ID:
			snprintf(buffer, buflen, "OBJECT_ID");
			break;
		case AT_SECURITY_DESCRIPTOR:
			snprintf(buffer, buflen, "SECURITY_DESCRIPTOR");
			break;
		case AT_VOLUME_NAME:
			snprintf(buffer, buflen, "VOLUME_NAME");
			break;
		case AT_VOLUME_INFORMATION:
			snprintf(buffer, buflen, "VOLUME_INFORMATION");
			break;
		case AT_DATA:
			snprintf(buffer, buflen, "DATA");
			break;
		case AT_INDEX_ROOT:
			snprintf(buffer, buflen, "INDEX_ROOT");
			break;
		case AT_INDEX_ALLOCATION:
			snprintf(buffer, buflen, "INDEX_ALLOCATION");
			break;
		case AT_BITMAP:
			snprintf(buffer, buflen, "BITMAP");
			break;
		case AT_REPARSE_POINT:
			snprintf(buffer, buflen, "REPARSE_POINT");
			break;
		case AT_EA_INFORMATION:
			snprintf(buffer, buflen, "EA_INFORMATION");
			break;
		case AT_EA:
			snprintf(buffer, buflen, "EA");
			break;
		case AT_PROPERTY_SET:
			snprintf(buffer, buflen, "PROPERTY_SET");
			break;
		case AT_LOGGED_UTILITY_STREAM:
			snprintf(buffer, buflen, "LOGGED_UTILITY_STREAM");
			break;
		default:
			snprintf(buffer, buflen, "<unknown 0x%02x>", type);
			break;
	}
}

/**
 * dump_mft_record_flags
 * Needs a max of 30 chars in the buffer
 */
static void dump_mft_record_flags(char *buffer, int buflen, u32 flags)
{
	if (!buffer || (buflen < 31))
		return;

	if (flags == 0) {
		snprintf(buffer, buflen, "<none>");
		return;
	}

	if (flags & MFT_RECORD_IN_USE) {
		snprintf(buffer, buflen, "IN_USE ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & MFT_RECORD_IS_DIRECTORY) {
		snprintf(buffer, buflen, "IS_DIRECTORY ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & MFT_RECORD_IS_4) {
		snprintf(buffer, buflen, "IS_4 ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (flags & MFT_RECORD_IS_VIEW_INDEX) {
		snprintf(buffer, buflen, "IS_VIEW_INDEX ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
}

/**
 * dump_inode_state
 * Needs a max of 71 chars in the buffer
 */
static void dump_inode_state(char *buffer, int buflen, ntfs_inode *inode)
{
	if (!buffer || (buflen < 72) || !inode)
		return;

	if (inode->state == 0) {
		snprintf (buffer, buflen, "<none>");
		return;
	}

	if (NInoDirty(inode)) {
		snprintf(buffer, buflen, "Dirty ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (NInoAttrList(inode)) {
		snprintf(buffer, buflen, "AttrList ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (NInoAttrListDirty(inode)) {
		snprintf(buffer, buflen, "AttrListDirty ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
	if (NInoFileNameDirty(inode)) {
		snprintf(buffer, buflen, "FileNameDirty ");
		buflen -= strlen(buffer);
		buffer += strlen(buffer);
	}
}

/**
 * dump_name
 */
static void dump_name(char *buffer, int buflen, ntfschar *name, int name_len)
{
	if (!buffer || !buflen)
		return;
	if (!name || !name_len) {
		snprintf(buffer, buflen, "<none>");
		return;
	}

	ntfs_ucstombs(name, name_len, &buffer, buflen);
}

/**
 * dump_date
 */
static void dump_date(char *buffer, int buflen, time_t date)
{
	struct tm *tm = NULL;

	if (!buffer || !buflen)
		return;
	if (!date) {
		snprintf(buffer, buflen, "<none>");
		return;
	}

	tm = localtime(&date);
	strftime(buffer, buflen, "%F %T %Z", tm);
}


/**
 * dump_mft_record
 */
static void dump_mft_record(ntfs_volume *vol, MFT_RECORD *rec)
{
	char buffer[128];

	if (!vol)
		return;
	if (!rec)
		return;

	printf("MFT_RECORD (%p)\n", rec);
	printf("    magic                           %.4s\n", (char*)&rec->magic);			// NTFS_RECORD_TYPES
	printf("    usa_ofs                         %d\n", rec->usa_ofs);				// u16
	printf("    usa_count                       %d\n", rec->usa_count);				// u16
	printf("    lsn                             %llu\n", rec->lsn);					// LSN
	printf("    sequence_number                 %d\n", rec->sequence_number);			// u16
	printf("    link_count                      %d\n", rec->link_count);				// u16
	printf("    attrs_offset                    %d\n", rec->attrs_offset);				// u16
	dump_mft_record_flags(buffer, sizeof(buffer), rec->flags);
	printf("    flags                           %s\n", buffer);					// MFT_RECORD_FLAGS
	printf("    bytes_in_use                    %d\n", rec->bytes_in_use);				// u32
	printf("    bytes_allocated                 %d\n", rec->bytes_allocated);			// u32
	printf("    base_mft_record                 %llu\n", rec->base_mft_record);			// MFT_REF
	printf("    next_attr_instance              %d\n", rec->next_attr_instance);			// u16

	if (NTFS_V3_1(vol->major_ver, vol->minor_ver))
	printf("    mft_record_number               %d\n", rec->mft_record_number);			// u32

	printf("\n");
}

/**
 * dump_ntfs_attr
 */
static void dump_ntfs_attr(ntfs_attr *attr)
{
	char buffer[256];

	if (!attr)
		return;

	printf("ntfs_attr (%p)\n", attr);
	dump_runlist(buffer, sizeof(buffer), attr->rl);
	printf("    rl                                  %s\n", buffer);					// runlist_element *
	printf("    ni                                  %p\n", attr->ni);				// ntfs_inode *
	dump_attr_type(buffer, sizeof(buffer), attr->type);
	printf("    type                                %s\n", buffer);					// ATTR_TYPES
	dump_name(buffer, sizeof(buffer), attr->name, attr->name_len);
	printf("    name                                %s\n", buffer);					// ntfschar *
	printf("    name_len                            %u\n", attr->name_len);				// u32
	dump_attr_state(buffer, sizeof(buffer), attr);
	printf("    state                               %s\n", buffer);					// unsigned long
	printf("    allocated_size                      %lld\n", attr->allocated_size);			// s64
	printf("    data_size                           %lld\n", attr->data_size);			// s64
	printf("    initialized_size                    %lld\n", attr->initialized_size);		// s64
	printf("    compressed_size                     %lld\n", attr->compressed_size);		// s64
	printf("    compression_block_size              %d\n", attr->compression_block_size);		// u32
	printf("    compression_block_size_bits         %d\n", attr->compression_block_size_bits);	// u8
	printf("    compression_block_clusters          %d\n", attr->compression_block_clusters);	// u8
	printf("\n");
}

/**
 * dump_ntfs_inode
 */
static void dump_ntfs_inode(ntfs_volume *vol, ntfs_inode *inode)
{
	char buffer[128];

	if (!inode)
		return;

	printf("ntfs_inode (%p)\n", inode);

	printf("    mft_no                      %lld\n", inode->mft_no);			// u64
	printf("    mrec                        %p\n", inode->mrec);				// MFT_RECORD *
	printf("    vol                         %p\n", inode->vol);				// ntfs_volume *
	dump_inode_state(buffer, sizeof(buffer), inode);
	printf("    state                       %s\n", buffer);					// unsigned long
	dump_file_attr_flags(buffer, sizeof(buffer), inode->flags);
	printf("    flags                       %s\n", buffer);					// FILE_ATTR_FLAGS
	printf("    attr_list_size              %d\n", inode->attr_list_size);			// u32
	printf("    attr_list                   %p\n", inode->attr_list);			// u8 *
	printf("    nr_extents                  %d\n", inode->nr_extents);			// s32

	printf("    extent_nis                  %p\n", inode->extent_nis);			// ntfs_inode **
	printf("    base_ni                     %p\n", inode->base_ni);				// ntfs_inode *

	printf("    private_data                %p\n", inode->private_data);			// void *
	printf("    ref_count                   %d\n", inode->ref_count);			// int
	printf("    data_size                   %lld\n", inode->data_size);			// s64
	printf("    allocated_size              %lld\n", inode->allocated_size);		// s64
	dump_date(buffer, sizeof(buffer), inode->creation_time);
	printf("    creation_time               %s\n", buffer);					// time_t
	dump_date(buffer, sizeof(buffer), inode->last_data_change_time);
	printf("    last_data_change_time       %s\n", buffer);					// time_t
	dump_date(buffer, sizeof(buffer), inode->last_mft_change_time);
	printf("    last_mft_change_time        %s\n", buffer);					// time_t
	dump_date(buffer, sizeof(buffer), inode->last_access_time);
	printf("    last_access_time            %s\n", buffer);					// time_t
	printf("\n");

	dump_mft_record(vol, inode->mrec);
}

/**
 * dump_ntfs_volume
 */
static void dump_ntfs_volume(ntfs_volume *vol)
{
	if (!vol)
		return;

	printf("ntfs_volume (%p)\n", vol);
	printf("    dev                    %p\n", vol->dev);			// struct ntfs_device *
	printf("    vol_name               %s\n", vol->vol_name);		// char *
	printf("    state                  %ld\n", vol->state);			// unsigned long
	printf("    vol_ni                 %p\n", vol->vol_ni);			// ntfs_inode *
	printf("    major_ver              %d\n", vol->major_ver);		// u8
	printf("    minor_ver              %d\n", vol->minor_ver);		// u8
	printf("    flags                  %d\n", vol->flags);			// u16
	printf("    sector_size            %d\n", vol->sector_size);		// u16
	printf("    sector_size_bits       %d\n", vol->sector_size_bits);	// u8
	printf("    cluster_size           %d\n", vol->cluster_size);		// u32
	printf("    mft_record_size        %d\n", vol->mft_record_size);	// u32
	printf("    indx_record_size       %d\n", vol->indx_record_size);	// u32
	printf("    cluster_size_bits      %d\n", vol->cluster_size_bits);	// u8
	printf("    mft_record_size_bits   %d\n", vol->mft_record_size_bits);	// u8
	printf("    indx_record_size_bits  %d\n", vol->indx_record_size_bits);	// u8
	printf("    mft_zone_multiplier    %d\n", vol->mft_zone_multiplier);	// u8
	printf("    mft_data_pos           %lld\n", vol->mft_data_pos);		// s64
	printf("    mft_zone_start         %lld\n", vol->mft_zone_start);	// LCN
	printf("    mft_zone_end           %lld\n", vol->mft_zone_end);		// LCN
	printf("    mft_zone_pos           %lld\n", vol->mft_zone_pos);		// LCN
	printf("    data1_zone_pos         %lld\n", vol->data1_zone_pos);	// LCN
	printf("    data2_zone_pos         %lld\n", vol->data2_zone_pos);	// LCN
	printf("    nr_clusters            %lld\n", vol->nr_clusters);		// s64
	printf("    lcnbmp_ni              %p\n", vol->lcnbmp_ni);		// ntfs_inode *
	printf("    lcnbmp_na              %p\n", vol->lcnbmp_na);		// ntfs_attr *
	printf("    mft_lcn                %lld\n", vol->mft_lcn);		// LCN
	printf("    mft_ni                 %p\n", vol->mft_ni);			// ntfs_inode *
	printf("    mft_na                 %p\n", vol->mft_na);			// ntfs_attr *
	printf("    mftbmp_na              %p\n", vol->mftbmp_na);		// ntfs_attr *
	printf("    mftmirr_size           %d\n", vol->mftmirr_size);		// int
	printf("    mftmirr_lcn            %lld\n", vol->mftmirr_lcn);		// LCN
	printf("    mftmirr_ni             %p\n", vol->mftmirr_ni);		// ntfs_inode *
	printf("    mftmirr_na             %p\n", vol->mftmirr_na);		// ntfs_attr *
	printf("    upcase                 %p\n", vol->upcase);			// ntfschar *
	printf("    upcase_len             %d\n", vol->upcase_len);		// u32
	printf("    attrdef                %p\n", vol->attrdef);		// ATTR_DEF *
	printf("    attrdef_len            %d\n", vol->attrdef_len);		// s32
	printf("    private_data           %p\n", vol->private_data);		// void *
	printf("    private_bmp1           %p\n", vol->private_bmp1);		// void *
	printf("    private_bmp2           %p\n", vol->private_bmp2);		// void *
	printf("\n");

	dump_ntfs_inode(vol, vol->vol_ni);
	dump_ntfs_inode(vol, vol->lcnbmp_ni);
	dump_ntfs_inode(vol, vol->mft_ni);
	dump_ntfs_inode(vol, vol->mftmirr_ni);

	dump_ntfs_attr(vol->lcnbmp_na);
	dump_ntfs_attr(vol->mft_na);
	dump_ntfs_attr(vol->mftbmp_na);
	dump_ntfs_attr(vol->mftmirr_na);
}


/**
 * main
 */
int main(int argc, char *argv[])
{
	ntfs_volume *vol;

	if (argc != 2)
		return 1;

	utils_set_locale();

	vol = utils_mount_volume(argv[1], MS_RDONLY, 0);
	if (!vol)
		return 1;

	dump_ntfs_volume(vol);

	ntfs_umount(vol, FALSE);
	return 0;
}

