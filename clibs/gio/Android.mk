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
GLIB_SOURCES_PATH := $(MAKEFILE_PATH)/../glib/distsrc
LOCAL_PATH:= $(GLIB_SOURCES_PATH)/gio

include $(CLEAR_VARS)

include $(LOCAL_PATH)/Sources.mk
include $(LOCAL_PATH)/xdgmime/Sources.mk

unix_sources := gfiledescriptorbased.c                  \
                gunixconnection.c                       \
                gunixcredentialsmessage.c               \
                gunixfdlist.c                           \
                gunixfdmessage.c                        \
                gunixsocketaddress.c                    \
                gunixinputstream.c                      \
                gunixoutputstream.c                     \
                gcontenttype.c                          \
                gcontenttypeprivate.h                   \
                gfdonotificationbackend.c               \
                ggtknotificationbackend.c               \
                gdesktopappinfo.c


GIO_SOURCES := $(filter-out gthread-posix.c glocaldirectorymonitor.c, $(libgio_2_0_la_SOURCES)) \
               $(addprefix xdgmime/, $(libxdgmime_la_SOURCES))  \
               $(MAKEFILE_PATH)/giodummyobjects.c
GIO_SOURCES := $(filter %.c, $(GIO_SOURCES))

LOCAL_MODULE:= gio
LOCAL_SRC_FILES:= $(GIO_SOURCES)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS += -DNVALGRIND=1 -DGIO_COMPILATION=1 -DGIO_MODULE_DIR=\".\" -DXDG_PREFIX=_gio_xdg
LOCAL_C_INCLUDES := $(MAKEFILE_PATH) $(MAKEFILE_PATH)/../glib $(MAKEFILE_PATH)/include \
                     $(MAKEFILE_PATH)/include/gio $(GLIB_INCLUDES) $(FFI_INCLUDES) \
                    $(NDK_ROOT)/sources/android/support/include \
                    $(NDK_ROOT)/sources/android/support/src/musl-locale
LOCAL_STATIC_LIBRARIES := glib android_support

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/support)
$(call import-module,glib)
