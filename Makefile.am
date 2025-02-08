SUBDIRS = cores drivers mics src
ACLOCAL_AMFLAGS = -I m4

if WITH_PANDOC
SUBDIRS += docs
endif

#EXTRA_DIST = cores drivers src

install-exec-hook:
	mkdir -p $(prefix)/data
	mkdir -p $(prefix)/run
	chmod 777 $(prefix)/run

#mkdir -p $(prefix)/log

dist-hook:
	rm -rf `find $(distdir) -name .git`
