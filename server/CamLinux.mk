#
# 1. Set the path and clear environment
# 	TARGET_PATH := $(call my-dir)
# 	include $(ENV_CLEAR)
#
# 2. Set the source files and headers files
#	TARGET_SRC := xxx_1.c xxx_2.c
#	TARGET_INc := xxx_1.h xxx_2.h
#
# 3. Set the output target
#	TARGET_MODULE := xxx
#
# 4. Include the main makefile
#	include $(BUILD_BIN)
#
# Before include the build makefile, you can set the compilaion
# flags, e.g. TARGET_ASFLAGS TARGET_CFLAGS TARGET_CPPFLAGS
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
#LOCAL_C_INCLUDE := example.h
LOCAL_LDLIBS += -lpthread
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/wsServer/include
LOCAL_SRC_FILES := devmem-server.c \
	wsServer/src/ws.c \
    wsServer/src/base64/base64.c \
    wsServer/src/sha1/sha1.c \
    wsServer/src/handshake/handshake.c
LOCAL_MODULE := devmem-server
include $(BUILD_EXECUTABLE)