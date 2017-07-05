## Makefile.am -- Process this file with automake to produce Makefile.in
AM_CPPFLAGS   = -I$(top_builddir) -I$(top_srcdir) \
                -I$(top_builddir)/src -I$(top_srcdir)/src

AUTOMAKE_OPTIONS = color-tests

TESTS = metafs_regressions

SUBDIRS = 

check_PROGRAMS = 
bin_PROGRAMS =

bin_PROGRAMS += metafs
metafs_SOURCES = src/main.c
metafs_SOURCES += src/wrap.cc src/wrap.hh
metafs_SOURCES += src/metafs.cpp src/metafs.h
metafs_SOURCES += src/log.cpp src/log.h
metafs_CXXFLAGS = -DBOOST_LOG_DYN_LINK $(PTHREAD_CFLAGS) $(FUSE_CFLAGS) $(SQLITE_CFLAGS) $(BOOST_CPPFLAGS) -g
metafs_CFLAGS = -DBOOST_LOG_DYN_LINK $(PTHREAD_CFLAGS) $(FUSE_CFLAGS) $(SQLITE_CFLAGS) -g
metafs_LDFLAGS = -DBOOST_LOG_DYN_LINK $(BOOST_LDFLAGS)
metafs_LDADD = $(PTHREAD_LIBS) $(FUSE_LIBS) $(SQLITE_LIBS) $(BOOST_SYSTEM_LIB) $(BOOST_THREAD_LIB) $(BOOST_LOG_LIB)

strip: $(bin_PROGRAMS)
	$(STRIP) $^

TESTS += $(check_PROGRAMS)

package: distdir
	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	cd $(distdir) && \
	dh_make --library -e '$(PACKAGE_BUGREPORT)' -f ../$(distdir).tar.gz && \
	mv debian/control debian/control.EX && \
	awk '{if(match($$0, /^Build-Depends:/)){print $$0 depends}else{print $$0}}' depends="`dpkg-depcheck -d ./configure 2>&1 | sed 's|^[[:space:]]\+||g' | awk '{if(match($$0, /^Packages.needed:$$/)){keep=1}else if(keep==1){str=str", "$$0}}END{print str}'`" debian/control.EX | sed 's|BROKEN|1|g' > debian/control && \
	[ -f debian/$(PACKAGE)-dev.install ] && ( mv debian/$(PACKAGE)-dev.install debian/$(PACKAGE)-dev.install.EX && \
	  cat debian/$(PACKAGE)-dev.install.EX | grep -v usr/lib/pkgconfig/ > debian/$(PACKAGE)-dev.install ) && \
	[ -f debian/$(PACKAGE)1.install ] && echo "usr/bin/*" >> debian/$(PACKAGE)1.install && \
	dpkg-buildpackage -sa -rfakeroot -us
