#ifndef __LDMINFO_H_
#define __LDMINFO_H_

#include "ldm.h"
#include <byteswap.h>
#include <endian.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define be16_to_cpu(s) bswap_16(s)
#define be32_to_cpu(s) bswap_32(s)
#define be64_to_cpu(s) bswap_64(s)
#define cpu_to_le16
#define cpu_to_le32
#define cpu_to_le64
#else
#define be16_to_cpu
#define be32_to_cpu
#define be64_to_cpu
#define cpu_to_le16(s) bswap_16(s)
#define cpu_to_le32(s) bswap_32(s)
#define cpu_to_le64(s) bswap_64(s)
#endif

extern int debug;

void dump_database (char *name, struct ldmdb *ldb);
void copy_database (char *file, int fd, long long size);
void ldm_free_vblks (struct list_head *vb);

#endif // __LDMINFO_H_

