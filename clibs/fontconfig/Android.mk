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
LOCAL_PATH    := $(MAKEFILE_PATH)/distsrc

include $(CLEAR_VARS)

export FONTCONFIG_INCLUDES := $(LOCAL_PATH)

include $(LOCAL_PATH)/src/Makefile.sources

libfontconfig_la_SOURCES := $(filter-out fcxml.c fcinit.c, $(libfontconfig_la_SOURCES))

LOCAL_MODULE     := fontconfig
LOCAL_SRC_FILES  := $(addprefix src/, $(filter %.c, $(libfontconfig_la_SOURCES))) \
                    ../fcinit_android.c ../fcxml_android.c
LOCAL_CFLAGS     += -DHAVE_CONFIG_H=1
LOCAL_C_INCLUDES := $(MAKEFILE_PATH) $(FREETYPE_INCLUDES)

LOCAL_STATIC_LIBRARIES := android_support freetype

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/support)
$(call import-module,freetype)