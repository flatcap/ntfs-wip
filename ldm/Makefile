# Copyright (C) 2001-2005 Richard Russon

INFO	= ldminfo
UTIL	= ldmutil

all:
	make -C $(INFO)
	make -C $(UTIL)

clean:
	make -C $(INFO) clean
	make -C $(UTIL) clean

distclean:
	make -C $(INFO) distclean
	make -C $(UTIL) distclean
	find . -name tags -exec $(RM) {} \;
	find . -name \*.data -o -name \*.part -exec $(RM) {} \;

tags:	force
	ctags -R .
	make -C $(INFO) tags
	(cd $(UTIL); ctags -R .)

force:

