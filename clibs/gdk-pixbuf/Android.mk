# Copyright 2014 The GTK+Android Developers. See the COPYRIGHT
# file at the top-level directory of this distribution and at
# https://github.com/eugals/GTKAndroid/wiki/COPYRIGHT.
#
# Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
# http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
# <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
# option. This file may not be copied, modified, or distributed
# except according to those terms.
#
# Author(s): Evgeny Sologubov
#

MAKEFILE_PATH := $(call my-dir)
GDK_PIXBUG_SOURCES_PATH := $(MAKEFILE_PATH)/distsrc
LOCAL_PATH:= $(GDK_PIXBUG_SOURCES_PATH)/gdk-pixbuf

include $(CLEAR_VARS)

include $(LOCAL_PATH)/Sources.mk
include $(GDK_PIXBUG_SOURCES_PATH)/Config.mk

PIXOPS_SOURCES = pixops/pixops.c

LOCAL_MODULE:= gdk-pixbuf
LOCAL_SRC_FILES:= $(filter %.c, $(libgdk_pixbuf_2_0_la_SOURCES)) $(PIXOPS_SOURCES) io-pixdata.c
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS += -DGDK_PIXBUF_COMPILATION=1 -DGDK_PIXBUF_ENABLE_BACKEND=1 -DNVALGRIND=1 \
                -DGDK_PIXBUF_BINARY_VERSION=\"$(GDK_PIXBUF_BINARY_VERSION)\" \
                -DLIBDIR=\"unknown-libdir-in-Android.mk\" \
                -DGDK_PIXBUF_LIBDIR=\"unknown-libdir-in-Android.mk\" \
                -DGDK_PIXBUF_LOCALEDIR=\".\"
LOCAL_C_INCLUDES := $(MAKEFILE_PATH) $(MAKEFILE_PATH)/include $(GDK_PIXBUG_SOURCES_PATH) \
                    $(GLIB_INCLUDES) \
                    $(NDK_ROOT)/sources/android/support/include \
                    $(NDK_ROOT)/sources/android/support/src/musl-locale
LOCAL_STATIC_LIBRARIES := gmodule

export GDK_PIXBUG_INCLUDES := $(GDK_PIXBUG_SOURCES_PATH)

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/support)
$(call import-module,gmodule)
