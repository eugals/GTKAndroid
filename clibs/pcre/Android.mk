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
GLIB_SOURCES_PATH := $(MAKEFILE_PATH)/../glib/distsrc
LOCAL_PATH:= $(GLIB_SOURCES_PATH)/glib/pcre

include $(CLEAR_VARS)

include $(LOCAL_PATH)/Sources.mk

LOCAL_MODULE:= pcre
LOCAL_SRC_FILES:= $(filter %.c, $(libpcre_la_SOURCES))
LOCAL_CFLAGS += $(libpcre_la_CPPFLAGS) -DNVALGRIND=1
LOCAL_C_INCLUDES := $(GLIB_SOURCES_PATH) $(GLIB_SOURCES_PATH)/glib  \
                    $(MAKEFILE_PATH)/../glib $(MAKEFILE_PATH)/../glib/include \
                    $(NDK_ROOT)/sources/android/support/include \
                    $(NDK_ROOT)/sources/android/support/src/musl-locale
LOCAL_STATIC_LIBRARIES := android_support

include $(BUILD_STATIC_LIBRARY)
$(call import-module,android/support)
