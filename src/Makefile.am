lib_LTLIBRARIES = libckit.la

libckit_la_SOURCES = \
	rbtree.c \
	heap.c \
	log.c \
	pbuf.c \
	timer.c \
	signal.c \
	hashtable.c

libckit_la_includedir=$(includedir)/ckit
libckit_la_include_HEADERS = \
	$(top_srcdir)/include/ckit/atomic.h \
	$(top_srcdir)/include/ckit/ckit.h \
	$(top_srcdir)/include/ckit/debug.h \
	$(top_srcdir)/include/ckit/event.h \
        $(top_srcdir)/include/ckit/hash.h \
        $(top_srcdir)/include/ckit/hashtable.h \
        $(top_srcdir)/include/ckit/heap.h \
        $(top_srcdir)/include/ckit/list.h \
        $(top_srcdir)/include/ckit/log.h \
	$(top_srcdir)/include/ckit/rbtree.h \
	$(top_srcdir)/include/ckit/pbuf.h \
        $(top_srcdir)/include/ckit/signal.h \
        $(top_srcdir)/include/ckit/time.h \
	$(top_srcdir)/include/ckit/timer.h

libckit_la_CPPFLAGS = \
	-I$(top_srcdir)/include
libckit_la_CFLAGS =
libckit_la_LDFLAGS = 

if OS_LINUX
libckit_la_SOURCES += \
	event_epoll.c

libckit_la_LDFLAGS += \
	-lrt
endif

if OS_BSD
libckit_la_SOURCES += \
	event_kqueue.c
endif

#bin_PROGRAMS = list_unittest

#list_unittest_SOURCES = \
#	list_unittest.c

#list_unittest_CPPFLAGS = \
#	-I$(top_srcdir)/include


clean-local:
	rm -f *~
