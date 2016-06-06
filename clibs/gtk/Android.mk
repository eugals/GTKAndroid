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
GTK_SOURCES_PATH := $(MAKEFILE_PATH)/distsrc
LOCAL_PATH:= $(GTK_SOURCES_PATH)/gtk

include $(CLEAR_VARS)

include $(LOCAL_PATH)/Sources.mk
include $(LOCAL_PATH)/a11y/Sources.mk
include $(GTK_SOURCES_PATH)/Config.mk

GTK_ANDROID_SOURCES := gtkainit.c gtkadummyobject.c gtkcssprovider_hack.c gtkandroidassets.c
GTK_ANDROID_SOURCES := $(addprefix ../../, $(GTK_ANDROID_SOURCES))

GTK_SOURCES = $(GTK_ANDROID_SOURCES) \
              gtkwin32theme.c gtkcssimagewin32.c  \
              $(addprefix a11y/, $(gtka11y_c_sources)) \
              $(filter-out gtkmountoperation% gtkprintoperation% \
                           gtkcssprovider.c \
                           gtkplug.c gtksocket.c gtkxembed.c \
                           %quartz.c %win32.c %-x11.c %-wayland.c gtkwin32% \
                           gtkapplication-dbus.c gtkapplication-quartz-menu.c \
                         , $(am__libgtk_3_la_SOURCES_DIST))


export GTK_INCLUDES = $(MAKEFILE_PATH)/include $(GTK_SOURCES_PATH) \
                      $(GLIB_INCLUDES) $(PANGO_INCLUDES) $(CAIRO_INCLUDES) \
                      $(GDK_PIXBUG_INCLUDES) $(ATK_INCLUDES) $(GDK_INCLUDES)

LOCAL_MODULE := gtk
LOCAL_CFLAGS += -DGTK_COMPILATION=1 -DNVALGRIND=1 -DGTK_VERSION=\"$(GTK_VERSION)\" \
                -DGTK_PRINT_BACKEND_ENABLE_UNSUPPORTED=1 -DATK_DISABLE_DEPRECATION_WARNINGS=1 \
                -DGTK_HOST=\"android\" -DGTK_BINARY_VERSION=\"$(GTK_BINARY_VERSION)\" \
                $(CAIRO_CFLAGS)
LOCAL_C_INCLUDES := $(MAKEFILE_PATH) $(MAKEFILE_PATH)/include $(GTK_SOURCES_PATH) \
                    $(GLIB_INCLUDES) $(PANGO_INCLUDES) $(CAIRO_INCLUDES) \
                    $(GDK_PIXBUG_INCLUDES) $(ATK_INCLUDES) \
                    $(GDK_INCLUDES) \
                    $(NDK_ROOT)/sources/android/support/include \
                    $(NDK_ROOT)/sources/android/support/src/musl-locale
LOCAL_SRC_FILES:= $(GTK_SOURCES)
LOCAL_STATIC_LIBRARIES := android_support android_native_app_glue glib gobject gmodule \
                          pango cairo gdk-pixbuf atk gdk

LOCAL_EXPORT_C_INCLUDES = $(GTK_INCLUDES)

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/support)
$(call import-module,android/native_app_glue)
$(call import-module,glib)
$(call import-module,gobject)
$(call import-module,pango)
$(call import-module,cairo)
$(call import-module,gdk-pixbuf)
$(call import-module,atk)
$(call import-module,gdk)
