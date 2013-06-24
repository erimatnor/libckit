lib_LTLIBRARIES = libckit.la

libckit_la_SOURCES = \
	rbtree.c \
	heap.c \
	timer.c \
	signal.c

libckit_la_includedir=$(includedir)/ckit
libckit_la_include_HEADERS = \
	$(top_srcdir)/include/ckit/rbtree.h \
        $(top_srcdir)/include/ckit/signal.h \
        $(top_srcdir)/include/ckit/heap.h \
        $(top_srcdir)/include/ckit/timer.h

libckit_la_CPPFLAGS = \
	-I$(top_srcdir)/include
libckit_la_CFLAGS =
libckit_la_LDFLAGS =

clean-local:
	rm -f *~
