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
LOCAL_PATH    := $(MAKEFILE_PATH)/distsrc/src

include $(CLEAR_VARS)

export HARFBUZZ_INCLUDES := $(LOCAL_PATH)

include $(LOCAL_PATH)/Makefile.sources

HB_OT_SOURCES = \
    hb-ot-font.cc \
    hb-ot-layout.cc \
    hb-ot-layout-common-private.hh \
    hb-ot-layout-gdef-table.hh \
    hb-ot-layout-gpos-table.hh \
    hb-ot-layout-gsubgpos-private.hh \
    hb-ot-layout-gsub-table.hh \
    hb-ot-layout-jstf-table.hh \
    hb-ot-layout-private.hh \
    hb-ot-map.cc \
    hb-ot-map-private.hh \
    hb-ot-shape.cc \
    hb-ot-shape-complex-arabic.cc \
    hb-ot-shape-complex-arabic-fallback.hh \
    hb-ot-shape-complex-arabic-table.hh \
    hb-ot-shape-complex-arabic-win1256.hh \
    hb-ot-shape-complex-default.cc \
    hb-ot-shape-complex-hangul.cc \
    hb-ot-shape-complex-hebrew.cc \
    hb-ot-shape-complex-indic.cc \
    hb-ot-shape-complex-indic-machine.hh \
    hb-ot-shape-complex-indic-private.hh \
    hb-ot-shape-complex-indic-table.cc \
    hb-ot-shape-complex-myanmar.cc \
    hb-ot-shape-complex-myanmar-machine.hh \
    hb-ot-shape-complex-sea.cc \
    hb-ot-shape-complex-sea-machine.hh \
    hb-ot-shape-complex-thai.cc \
    hb-ot-shape-complex-tibetan.cc \
    hb-ot-shape-complex-private.hh \
    hb-ot-shape-normalize-private.hh \
    hb-ot-shape-normalize.cc \
    hb-ot-shape-fallback-private.hh \
    hb-ot-shape-fallback.cc \
    hb-ot-shape-private.hh

HBSOURCES += $(HB_OT_SOURCES) hb-ft.cc hb-fallback-shape.cc hb-glib.cc

LOCAL_MODULE     := harfbuzz
LOCAL_SRC_FILES  := $(filter %.cc, $(HBSOURCES))
LOCAL_CFLAGS     += -DHAVE_CONFIG_H=1
LOCAL_C_INCLUDES := $(MAKEFILE_PATH) $(FREETYPE_INCLUDES) $(GLIB_INCLUDES)

LOCAL_STATIC_LIBRARIES := android_support glib freetype

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/support)
$(call import-module,freetype)
$(call import-module,glib)