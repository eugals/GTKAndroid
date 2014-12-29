# Copyright 2014 The GTK+Android Developers. See the COPYRIGHT
# file at the top-level directory of this distribution and at
# http://p2lang.org/COPYRIGHT.
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
GTK_SOURCES_PATH := $(MAKEFILE_PATH)/../gtk/distsrc
LOCAL_PATH:= $(GTK_SOURCES_PATH)/gdk

include $(CLEAR_VARS)

include $(LOCAL_PATH)/Sources.mk

GDK_ANDROID_SOURCES := gdkwindow-android.c  gdkevents-android.c \
                       gdkscreen-android.c gdkdisplay-android.c \
                       gdkinput-android.c gdkjni.c
GDK_ANDROID_SOURCES := $(addprefix ../../../gdk/, $(GDK_ANDROID_SOURCES))

LOCAL_MODULE:= gdk
LOCAL_SRC_FILES:= $(GDK_ANDROID_SOURCES) $(gdk_c_sources) gdkenumtypes.c gdkmarshalers.c
LOCAL_EXPORT_LDLIBS := -llog
LOCAL_CFLAGS += -DNVALGRIND=1 -DGDK_COMPILATION=1 $(CAIRO_CFLAGS)
LOCAL_C_INCLUDES := $(MAKEFILE_PATH) $(MAKEFILE_PATH)/include $(MAKEFILE_PATH)/include/gdk $(MAKEFILE_PATH)/../gtk \
                    $(GTK_SOURCES_PATH) $(GDK_PIXBUG_INCLUDES) $(GLIB_INCLUDES) \
                    $(PANGO_INCLUDES) $(CAIRO_INCLUDES) $(FONTCONFIG_INCLUDES) \
                    $(NDK_ROOT)/sources/android/support/include \
                    $(NDK_ROOT)/sources/android/support/src/musl-locale
LOCAL_STATIC_LIBRARIES := android_support android_native_app_glue \
                          glib gio gobject gmodule fontconfig pango cairo gdk-pixbuf

export GDK_INCLUDES := $(LOCAL_PATH) $(MAKEFILE_PATH)/include

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/support)
$(call import-module,glib)
$(call import-module,gio)
$(call import-module,gobject)
$(call import-module,fontconfig)
$(call import-module,pango)
$(call import-module,cairo)
$(call import-module,gdk-pixbuf)
