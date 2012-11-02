
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/rps

LOCAL_SRC_FILES := rpsx_jni.c \
                   rps/stb_image.c \
                   rps/system_android.c \
                   rps/timer.c \
                   rps/render.c \
                   rps/game.c

LOCAL_MODULE := rps
LOCAL_CFLAGS := -Werror -DANDROID
LOCAL_LDLIBS := -llog -landroid -lGLESv2

include $(BUILD_SHARED_LIBRARY)
