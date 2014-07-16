CC      = gcc
RM      = rm -fr
AR	= ar
LN	= ln -f
MKDIR	= mkdir -p

#CFLAGS += -O2
#CFLAGS += -DDEBUG
#CFLAGS += -finstrument-functions
CFLAGS += -g
CFLAGS += -DHAVE_CONFIG_H
CFLAGS += -D_FILE_OFFSET_BITS=64
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

SRC	= jim.c
HDR     = $(SRC:.c=.h)
OBJ     = $(SRC:.c=.o)

OUT	= jim

all: $(OUT) tags

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

%: %.c
	$(CC) $(CFLAGS) -o $@ $^ -ldl

tags: dummy
	ctags *.[ch]

strip: $(OUT)
	strip $^

install: strip
	cp -a $(OUT) ~/bin

clean:
	$(RM) $(OUT) *.o *~ *.bak
	$(RM) tags

distclean: clean

dummy:

