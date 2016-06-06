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

GTKA_LIB_PATH := $(abspath $(call my-dir))/..

$(call import-add-path, $(GTKA_LIB_PATH))

LOCAL_LDLIBS     += -llog -landroid -lEGL -lGLESv2
LOCAL_C_INCLUDES += $(GTK_INCLUDES)
LOCAL_STATIC_LIBRARIES += gtk gdk atk gdk-pixbuf pango cairo gmodule gio gobject glib ffi pcre \
                          android_native_app_glue android_support

include $(call my-dir)/gtkresources.mk

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/support)
$(call import-module,android/native_app_glue)
$(call import-module,ffi)
$(call import-module,glib)
$(call import-module,gio)
$(call import-module,gobject)
$(call import-module,gmodule)
$(call import-module,freetype)
$(call import-module,fontconfig)
$(call import-module,harfbuzz)
$(call import-module,pango)
$(call import-module,cairo)
$(call import-module,gdk-pixbuf)
$(call import-module,gdk)
$(call import-module,atk)
$(call import-module,gtk)

