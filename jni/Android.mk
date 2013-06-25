LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

LOCAL_HDR_FILES := \
	../include/ckit/debug.h \
	../include/ckit/heap.h \
	../include/ckit/timer.h \
	../include/ckit/signal.h \
	../include/ckit/list.h \
	../include/ckit/hash.h \
	../include/ckit/hashtable.h \
	../include/ckit/rbtree.h 

LOCAL_SRC_FILES := \
	../src/heap.c \
	../src/signal.c \
	../src/timer.c \
	../src/rbtree.c \
	../src/hashtable.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include

#LOCAL_SHARED_LIBRARIES := liblog

EXTRA_DEFINES=
DEFINES=$(EXTRA_DEFINES)
LOCAL_CFLAGS :=-Wall -O2 -g $(DEFINES)
LOCAL_CPPFLAGS +=$(DEFINES)
LOCAL_PRELINK_MODULE := false
LOCAL_LDLIBS := -llog

LOCAL_MODULE := libckit

include $(BUILD_SHARED_LIBRARY)
