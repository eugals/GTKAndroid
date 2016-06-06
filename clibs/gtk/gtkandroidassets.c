// Copyright 2014 The GTK+Android Developers. See the COPYRIGHT
// file at the top-level directory of this distribution and at
// https://github.com/eugals/GTKAndroid/wiki/COPYRIGHT.
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

#include <android_native_app_glue.h>

#include "gtkandroidprivate.h"
#include "gtkandroidassets.h"


///////////////////////////////////////////////////////////////////////
// class implementations

struct GAAssetFile
{
    GObject parent_instance;

    AAsset* asset;
};

struct GAAssetFileClass
{
    GObjectClass parent_class;
};

static void g_aasset_file_file_iface_init(GFileIface *iface);

G_DEFINE_TYPE_WITH_CODE(GAAssetFile, g_aasset_file, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(G_TYPE_FILE, g_aasset_file_file_iface_init))

static void g_aasset_file_finalize(GObject *object)
{
    GAAssetFile *aaf = G_AASSET_FILE(object);

    AAsset_close(aaf->asset);

    G_OBJECT_CLASS(g_aasset_file_parent_class)->finalize(object);
}

static void g_aasset_file_class_init(GAAssetFileClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = g_aasset_file_finalize;
}

static void g_aasset_file_init(GAAssetFile *resource)
{
}

static GFileInputStream *g_aasset_file_read(GFile         *file,
                                            GCancellable  *cancellable,
                                            GError       **error)
{
    GAAssetFile *aaf = G_AASSET_FILE(file);
    GInputStream *stream;

    stream = g_memory_input_stream_new_from_data(AAsset_getBuffer(aaf->asset),
                                                 AAsset_getLength(aaf->asset),
                                                 NULL);

    // TODO: get rid of this hack in future
    return (GFileInputStream *) stream;
}

static void g_aasset_file_file_iface_init(GFileIface *iface)
{
    iface->read_fn = g_aasset_file_read;
}

///////////////////////////////////////////////////////////////////////
// public interface

gboolean g_aasset_test(const gchar *filename, GFileTest test)
{
    if (test & G_FILE_TEST_EXISTS && g_str_has_prefix(filename, "assets/"))
    {
        AAssetManager* assetManager = _gtk_android_glue_app->activity->assetManager;
        AAsset* asset =  AAssetManager_open(assetManager, filename + strlen("assets/"), AASSET_MODE_BUFFER);
        if (!asset)
            return FALSE;
        AAsset_close(asset);
        return TRUE;
    }
    return g_file_test(filename, test);
}

GFile *g_aasset_as_GFile_for_path(const char *path)
{
    GAAssetFile *aaf;
    AAsset* asset;
    AAssetManager* assetManager = _gtk_android_glue_app->activity->assetManager;

    g_return_val_if_fail(path != NULL, NULL);
    g_return_val_if_fail(g_str_has_prefix(path, "assets/"), NULL);

    asset =  AAssetManager_open(assetManager, path + strlen("assets/"), AASSET_MODE_BUFFER);
    g_return_val_if_fail(asset != NULL, NULL);

    aaf = g_object_new(G_TYPE_AASSET_FILE, NULL);
    aaf->asset = asset;

    return G_FILE(aaf);
}
