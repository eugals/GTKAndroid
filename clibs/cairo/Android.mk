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
LOCAL_PATH:= $(MAKEFILE_PATH)/distsrc
CAIRO_SOURCES_PATH := $(LOCAL_PATH)/cairo
PIXMAN_SOURCES_PATH := $(LOCAL_PATH)/pixman

include $(CLEAR_VARS)

LIBPIXMAN_CFLAGS = -D_USE_MATH_DEFINES -DPIXMAN_NO_TLS -DPACKAGE="android-cairo" -O2 \
                -I$(PIXMAN_SOURCES_PATH)/pixman -I$(MAKEFILE_PATH)/pixman-extra \
                -include "pixman-elf-fix.h" \
                -Wno-missing-field-initializers
                
export CAIRO_CFLAGS := -DCAIRO_HAS_GOBJECT_FUNCTIONS=1

LIBCAIRO_CFLAGS:= $(LIBPIXMAN_CFLAGS) $(CAIRO_CFLAGS)               \
    -DPACKAGE_VERSION="\"android-cairo\""                           \
    -DPACKAGE_BUGREPORT="\"http://github.com/anoek/android-cairo\"" \
    -DCAIRO_NO_MUTEX=1 -DHAVE_UINT64_T=1                            \
    -DHAVE_STDINT_H -Wno-attributes

include $(CAIRO_SOURCES_PATH)/src/Makefile.sources
include $(PIXMAN_SOURCES_PATH)/pixman/Makefile.sources

ifeq ($(TARGET_ARCH),arm)
    libpixman_sources += pixman-arm-neon.c pixman-arm-simd.c \
                         pixman-arm-simd-asm.S pixman-arm-simd-asm-scaled.S \
                         pixman-arm-neon-asm.S pixman-arm-neon-asm-bilinear.S
    LIBPIXMAN_CFLAGS += -DUSE_ARM_NEON -DUSE_ARM_SIMD
endif

include $(CLEAR_VARS)


LOCAL_MODULE    := cairo
LOCAL_CFLAGS    := -O2 $(LIBCAIRO_CFLAGS) -I$(PIXMAN_SOURCES_PATH)/pixman \
                   -I$(CAIRO_SOURCES_PATH)/src -I$(MAKEFILE_PATH)/cairo-extra \
                   -I$(MAKEFILE_PATH)/pixman-extra -Wno-missing-field-initializers

# remove the following line once NDK updates it's GCC to a version
# with this fix https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56982
LOCAL_CFLAGS    += -fno-tree-dominator-opts

LOCAL_SRC_FILES := cairo/util/cairo-gobject/cairo-gobject-enums.c \
                   cairo/util/cairo-gobject/cairo-gobject-structs.c \
                   $(addprefix pixman/pixman/, $(libpixman_sources)) \
                   $(addprefix cairo/src/, $(cairo_sources) $(cairo_gl_sources) $(cairo_egl_sources) \
                                           $(cairo_ft_sources))
LOCAL_C_INCLUDES := $(GLIB_INCLUDES) $(LOCAL_PATH)/glib/glib \
                    $(FONTCONFIG_INCLUDES) $(FREETYPE_INCLUDES) \
                    $(NDK_ROOT)/sources/android/support/include \
                    $(NDK_ROOT)/sources/android/support/src/musl-locale
LOCAL_STATIC_LIBRARIES := cpufeatures glib fontconfig freetype

export CAIRO_INCLUDES := $(MAKEFILE_PATH)/cairo-extra $(MAKEFILE_PATH)/pixman-extra \
                         $(CAIRO_SOURCES_PATH)/src $(PIXMAN_SOURCES_PATH)/pixman \
                         $(CAIRO_SOURCES_PATH)/util/cairo-gobject

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/cpufeatures)
$(call import-module,glib)
$(call import-module,freetype)
$(call import-module,fontconfig)