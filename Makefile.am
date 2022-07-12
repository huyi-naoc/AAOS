SUBDIRS = cores drivers src
ACLOCAL_AMFLAGS = -I m4

if WITH_PANDOC
SUBDIRS += docs
endif

#EXTRA_DIST = cores drivers src

dist-hook:
	rm -rf `find $(distdir) -name .git`
