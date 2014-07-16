# Copyright (C) 2001-2005 Richard Russon

CC	= gcc
SRC	= copy.c dump.c ldm.c ldminfo.c sparse.c
OBJ	= $(SRC:.c=.o)

#CFLAGS += -O2
#CFLAGS += -DDEBUG
CFLAGS += -g
CFLAGS += -D_GNU_SOURCE
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -pipe
CFLAGS += -Wall
CFLAGS += -W
CFLAGS += -Waggregate-return
CFLAGS += -Wbad-function-cast
CFLAGS += -Wcast-align
CFLAGS += -Wcast-qual
CFLAGS += -Wmissing-declarations
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wpointer-arith
CFLAGS += -Wredundant-decls
CFLAGS += -Wshadow
CFLAGS += -Wsign-compare
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wwrite-strings
#CFLAGS += -Werror

INFODEP	= copy.o dump.o ldm.o ldminfo.o

OUT	= ldminfo sparse

all:	$(OUT)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

ldminfo: $(INFODEP)
	$(CC) -o ldminfo $^

sparse: sparse.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(OUT) $(OBJ)

distclean: clean
	$(RM) tags

tags:	force
	ctags *.[ch]

force:

