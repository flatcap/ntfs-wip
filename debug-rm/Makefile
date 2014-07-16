WORKDIR	= ../rm

# include/ntfs
S1	= attrib.h attrlist.h bitmap.h bootsect.h collate.h compat.h \
	  compress.h debug.h device.h device_io.h dir.h endians.h index.h \
	  inode.h layout.h lcnalloc.h list.h logfile.h logging.h mft.h mst.h \
	  ntfstime.h rich.h runlist.h security.h support.h tree.h types.h \
	  unistr.h version.h volume.h

# libntfs
S2	= attrib.c attrlist.c bitmap.c bootsect.c collate.c compat.c \
	  compress.c debug.c device.c dir.c index.c inode.c lcnalloc.c \
	  logfile.c logging.c mft.c mst.c rich.c runlist.c security.c tree.c \
	  unistr.c unix_io.c version.c volume.c

# ntfsprogs
S3	= attrdef.c attrdef.h boot.c boot.h cluster.c cluster.h mkntfs.c \
	  ntfscat.c ntfscat.h ntfsclone.c ntfscluster.c ntfscluster.h \
	  ntfscmp.c ntfscp.c ntfsdump_logfile.c ntfsfix.c ntfsinfo.c \
	  ntfslabel.c ntfsls.c ntfsmftalloc.c ntfsmount.c ntfsmove.c \
	  ntfsmove.h ntfsresize.c ntfsrm.c ntfsrm.h ntfstruncate.c \
	  ntfsundelete.c ntfsundelete.h ntfswipe.c ntfswipe.h sd.c sd.h \
	  upcase.c upcase.h utils.c utils.h

#S3	= ntfsrm.c ntfsrm.h utils.c utils.h

CC      = gcc
RM      = rm -fr
AR	= ar
LN	= ln -f
MKDIR	= mkdir -p

#CFLAGS += -O2
#CFLAGS += -DDEBUG
#CFLAGS += -DNTFS_DISABLE_DEBUG_LOGGING
#CFLAGS += -DNTFS_TEST
#CFLAGS += -finstrument-functions
CFLAGS += -DNTFS_RICH
CFLAGS += -g
CFLAGS += -DHAVE_CONFIG_H
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -DFUSE_USE_VERSION=22
CFLAGS += -pipe
CFLAGS += -W
CFLAGS += -Wall
CFLAGS += -Waggregate-return
CFLAGS += -Wbad-function-cast
CFLAGS += -Wcast-align
CFLAGS += -Wcast-qual
CFLAGS += -Wdisabled-optimization
CFLAGS += -Wdiv-by-zero
CFLAGS += -Wfloat-equal
CFLAGS += -Winline
CFLAGS += -Wmissing-declarations
CFLAGS += -Wmissing-format-attribute
CFLAGS += -Wmissing-noreturn
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wmultichar
CFLAGS += -Wnested-externs
CFLAGS += -Wpointer-arith
CFLAGS += -Wredundant-decls
CFLAGS += -Wshadow
CFLAGS += -Wsign-compare
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wwrite-strings
#CFLAGS += -Werror

SRC	= attrdef.c attrib.c attrlist.c bitmap.c boot.c bootsect.c cluster.c \
	  collate.c compat.c compress.c debug.c device.c dir.c index.c inode.c \
	  lcnalloc.c logfile.c logging.c mft.c mst.c rich.c runlist.c sd.c \
	  security.c tree.c unistr.c unix_io.c upcase.c utils.c version.c \
	  volume.c

#SRC	= attrib.c attrlist.c bitmap.c bootsect.c \
	  collate.c compat.c compress.c debug.c device.c dir.c index.c inode.c \
	  lcnalloc.c logfile.c logging.c mft.c mst.c rich.c runlist.c \
	  security.c tree.c unistr.c unix_io.c utils.c version.c \
	  volume.c

#SRC	+= trace.c

HDR	= attrdef.h attrib.h attrlist.h bitmap.h boot.h bootsect.h cluster.h \
	  collate.h compat.h compress.h config.h debug.h device.h device_io.h \
	  dir.h endians.h index.h inode.h layout.h lcnalloc.h list.h logfile.h \
	  logging.h mft.h mst.h rich.h runlist.h security.h support.h tree.h \
	  types.h unistr.h upcase.h utils.h version.h volume.h

OBJ     = $(SRC:.c=.o)

OUT	= mkntfs ntfscat ntfsclone ntfscluster ntfscmp ntfscp \
	  ntfsdump_logfile ntfsfix ntfsinfo ntfslabel ntfsls ntfsmftalloc \
	  ntfsmount ntfsmove ntfsresize ntfstruncate ntfsundelete ntfswipe

TOOLSRC	= mkntfs.c ntfscat.c ntfsclone.c ntfscluster.c ntfscmp.c ntfscp.c \
	  ntfsdump_logfile.c ntfsfix.c ntfsinfo.c ntfslabel.c ntfsls.c \
	  ntfsmftalloc.c ntfsmount.c ntfsmove.c ntfsresize.c ntfsrm.c \
	  ntfstruncate.c ntfsundelete.c ntfswipe.c

OTHERSRC = boot.h cluster.h upcase.h ntfscat.h ntfsmove.h ntfswipe.h ntfsrm.h \
	   boot.c ntfscluster.h attrdef.h utils.c sd.h attrdef.c cluster.c \
	   upcase.c ntfsundelete.h sd.c utils.h

OUT	+= ntfsrm

LIB	= ntfsprogs.a

all: $(OUT) tags

$(LIB): $(OBJ)
	$(AR) crus $@ $^

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

ntfsmount: ntfsmount.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lfuse

%: %.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

setup: $(S1) $(S2) $(S3)

$(S1):
	$(LN) $(WORKDIR)/include/ntfs/$@

$(S2):
	$(LN) $(WORKDIR)/libntfs/$@

$(S3):
	$(LN) $(WORKDIR)/ntfsprogs/$@

tags: dummy
	ctags *.[ch]

strip: $(OUT)
	strip $^

install: strip
	cp -a $(OUT) ~/bin

clean:
	$(RM) $(OUT) $(LIB) *.o *~ *.bak
	$(RM) tags
	$(RM) jim

distclean: clean
	$(RM) $(S1)
	$(RM) $(S2)
	$(RM) $(S3)
	$(RM) tools
	$(RM) lib
	$(RM) include
	$(RM) other

dummy:

jim:	jim.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

tools: dummy
	$(MKDIR) -p $@
	for i in $(TOOLSRC); do $(LN) $$i $@/$$i; done

lib:	dummy
	$(MKDIR) -p $@
	for i in $(S2); do $(LN) $$i $@/$$i; done

include: dummy
	$(MKDIR) -p $@
	for i in $(S1); do $(LN) $$i $@/$$i; done

other: dummy
	$(MKDIR) -p $@
	for i in $(OTHERSRC); do $(LN) $$i $@/$$i; done

