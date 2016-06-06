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

export FREETYPE_INCLUDES := $(LOCAL_PATH)/include

FREETYPE_SOURCES := $(addprefix base/, ftadvanc ftcalc ftdbgmem ftgloadr \
                                       ftobjs   ftoutln  ftrfork  ftsnames \
                                       ftstream fttrigon ftutil ftbdf \
                                       basepic  ftpic ftbitmap fttype1 \
                                       ftsystem ftinit) \
                    bdf/bdf cid/type1cid psnames/psnames sfnt/sfnt \
                    smooth/smooth winfonts/winfnt truetype/truetype \
                    pcf/pcf type42/type42 psaux/psaux pfr/pfr \
                    autofit/autofit cff/cff type1/type1 pshinter/pshinter \
                    raster/raster gzip/ftgzip lzw/ftlzw 


LOCAL_MODULE     := freetype
LOCAL_SRC_FILES  := $(addprefix src/, $(addsuffix .c, $(FREETYPE_SOURCES)))
LOCAL_CFLAGS     := -DFT2_BUILD_LIBRARY=1
LOCAL_C_INCLUDES := $(FREETYPE_INCLUDES) $(FONTCONFIG_INCLUDES)


include $(BUILD_STATIC_LIBRARY)
