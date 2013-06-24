LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

LOCAL_HDR_FILES := \
	$(LOCAL_PATH)/../include/common/debug.h \
	$(LOCAL_PATH)/../include/common/heap.h \
	$(LOCAL_PATH)/../include/common/timer.h \
	$(LOCAL_PATH)/../include/common/signal.h

LOCAL_SRC_FILES := \
	heap.c \
	signal.c \
	timer.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../include

LOCAL_SHARED_LIBRARIES := liblog

EXTRA_DEFINES=
DEFINES=-DOS_ANDROID -DOS_LINUX $(EXTRA_DEFINES)
LOCAL_CFLAGS :=-O2 -g $(DEFINES)
LOCAL_CPPFLAGS +=$(DEFINES)
LOCAL_PRELINK_MODULE := false
LOCAL_EXPORT_LDLIBS := -llog
LOCAL_MODULE := libckit

include $(BUILD_STATIC_LIBRARY)
