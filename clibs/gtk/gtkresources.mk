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

GTK_CSS_SRC := $(call my-dir)/distsrc/gtk/resources/theme/gtk-default.css
GTK_CSS_DST := $(NDK_APP_LIBS_OUT)/assets/share/themes/Android/gtk-3.0/gtk.css

installed_modules: $(GTK_CSS_DST)

$(call generate-file-dir,$(GTK_CSS_DST))

$(GTK_CSS_DST): clean-installed-binaries
	$(hide) $(call host-install,$(GTK_CSS_SRC),$(GTK_CSS_DST))

