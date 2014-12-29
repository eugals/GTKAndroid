// Copyright 2014 The GTK+Android Developers. See the COPYRIGHT
// file at the top-level directory of this distribution and at
// http://p2lang.org/COPYRIGHT.
//
// Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
// http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
// <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.
//
// Author(s): Evgeny Sologubov
//
// gfileutils-like Android "assets" accessors

#ifndef __GTK_ANDROID_ASSETS_H__
#define __GTK_ANDROID_ASSETS_H__

#include "config.h"

#include <gio/gio.h>

#define G_TYPE_AASSET_FILE         (g_aasset_file_get_type ())
#define G_AASSET_FILE(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), G_TYPE_AASSET_FILE, GAAssetFile))
#define G_AASSET_FILE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), G_TYPE_AASSET_FILE, GAAssetFileClass))
#define G_IS_AASSET_FILE(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), G_TYPE_AASSET_FILE))
#define G_IS_AASSET_FILE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), G_TYPE_AASSET_FILE))
#define G_AASSET_FILE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), G_TYPE_AASSET_FILE, GAAssetFileClass))

typedef struct GAAssetFile        GAAssetFile;
typedef struct GAAssetFileClass   GAAssetFileClass;

GType g_aasset_file_get_type(void);

gboolean g_aasset_test(const gchar *filename, GFileTest test);

GFile *g_aasset_as_GFile_for_path(const char *path);


#endif // __GTK_ANDROID_ASSETS_H__