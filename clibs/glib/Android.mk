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
GLIB_SOURCES_PATH := $(MAKEFILE_PATH)/distsrc
LOCAL_PATH:= $(GLIB_SOURCES_PATH)/glib

include $(CLEAR_VARS)

include $(LOCAL_PATH)/Sources.mk

GNULIB_SOURCES = $(addprefix gnulib/, vasnprintf.c printf-parse.c printf-args.c printf.c asnprintf.c)
CHARSET_SOURCES = libcharset/localcharset.c
GLIB_CORE_SOURCES = $(filter %.c, $(libglib_2_0_la_SOURCES)) gspawn.c giounix.c

LOCAL_MODULE:= glib
LOCAL_SRC_FILES:= $(GLIB_CORE_SOURCES) $(GNULIB_SOURCES) $(CHARSET_SOURCES)
LOCAL_CFLAGS += -DGLIB_COMPILATION=1 -DNVALGRIND=1
LOCAL_EXPORT_LDLIBS := -lz
LOCAL_C_INCLUDES := $(MAKEFILE_PATH) $(MAKEFILE_PATH)/include $(GLIB_SOURCES_PATH) \
                    $(NDK_ROOT)/sources/android/support/include \
                    $(NDK_ROOT)/sources/android/support/src/musl-locale
LOCAL_STATIC_LIBRARIES := android_support pcre

export GLIB_INCLUDES := $(MAKEFILE_PATH)/include $(GLIB_SOURCES_PATH) \
                        $(GLIB_SOURCES_PATH)/gmodule $(LOCAL_PATH)

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/support)
$(call import-module,pcre)