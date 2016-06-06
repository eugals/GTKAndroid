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
// ANativeWindow-based GDK visual and screen definitions

#include "config.h"

#include <jni.h>

#include <android/native_window.h>
#include <android/native_activity.h>
#include <android_native_app_glue.h>

#include <cairo-gl.h>

#include "gdk.h"
#include "gdkvisualprivate.h"
#include "gdkprivate-android.h"
#include "gdkandroiddisplay.h"
#include "gdkandroidscreen.h"
#include "gdkwindow-android.h"


enum { COLOR_DEPTH = 32 };

///////////////////////////////////////////////////////////////////////////////////////
// Visual

static gint _gdk_android_screen_visual_get_best_depth(GdkScreen *screen)
{
    return COLOR_DEPTH;
}

static void _gdk_android_screen_query_depths(GdkScreen  *screen,
                                             gint      **depths,
                                             gint       *count)
{
    static gint static_depths[] = { COLOR_DEPTH };

    *count = G_N_ELEMENTS(static_depths);
    *depths = static_depths;
}

static GdkVisualType _gdk_android_screen_visual_get_best_type(GdkScreen *screen)
{
    return GDK_VISUAL_TRUE_COLOR;
}

static void _gdk_android_screen_query_visual_types(GdkScreen      *screen,
                                                   GdkVisualType **visual_types,
                                                   gint           *count)
{
    static GdkVisualType static_visual_types[] = { GDK_VISUAL_TRUE_COLOR };

    *count = G_N_ELEMENTS(static_visual_types);
    *visual_types = static_visual_types;
}

static GdkVisual *_gdk_android_screen_visual_get_best(GdkScreen *screen)
{
    return GDK_ANDROID_SCREEN(screen)->visual;
}

static GdkVisual *_gdk_android_screen_visual_get_best_with_depth(GdkScreen *screen,
                                                                 gint       depth)
{
    return GDK_ANDROID_SCREEN(screen)->visual;
}

static GdkVisual *_gdk_android_screen_visual_get_best_with_type(GdkScreen *screen,
                                                                GdkVisualType  visual_type)
{
    return GDK_ANDROID_SCREEN(screen)->visual;
}

static GdkVisual *_gdk_android_screen_visual_get_best_with_both(GdkScreen     *screen,
                                                                gint           depth,
                                                                GdkVisualType  visual_type)
{
    return GDK_ANDROID_SCREEN(screen)->visual;
}

#define _gdk_android_screen_get_system_visual _gdk_android_screen_visual_get_best

static GList *_gdk_android_screen_list_visuals(GdkScreen *screen)
{
    return g_list_append(NULL, GDK_ANDROID_SCREEN(screen)->visual);
}

static GdkVisual *gdk_android_visual_new(GdkAndroidScreen *screen)
{
    GdkVisual *visual = g_object_new(GDK_TYPE_VISUAL, NULL);
    visual->screen = &screen->screen;
    visual->type = _gdk_android_screen_visual_get_best_type(visual->screen);
    visual->depth = _gdk_android_screen_visual_get_best_depth(visual->screen);

    return visual;
}

///////////////////////////////////////////////////////////////////////////////////////
// Screen

struct GdkAndroidScreenClass
{
    GdkScreenClass parent_class;
};

G_DEFINE_TYPE(GdkAndroidScreen, gdk_android_screen, GDK_TYPE_SCREEN)

static void gdk_android_screen_init(GdkAndroidScreen *display)
{
}

static GdkDisplay *gdk_android_screen_get_display(GdkScreen *screen)
{
    return &_gdk_display->display;
}

static gint gdk_android_screen_get_width(GdkScreen *screen)
{
    gint rv;
    eglQuerySurface(_gdk_display->eglDisplay, _gdk_display->eglSurface, EGL_WIDTH, &rv);
    return rv;
}

static gint gdk_android_screen_get_height(GdkScreen *screen)
{
    gint rv;
    eglQuerySurface(_gdk_display->eglDisplay, _gdk_display->eglSurface, EGL_HEIGHT, &rv);
    return rv;
}

#define JNI_ASSERT(jni) if (jni->ExceptionCheck(pjni)) \
    { \
        g_set_error(error, GDK_ANDROID_ERROR, GDK_ANDROID_ERROR_INIT, \
                    "JNI exception at %s:%u", __FILE__, __LINE__); \
        return FALSE; \
    }

gboolean gdk_android_init_screen(GdkAndroidScreen *scr, ANativeActivity *act, GError **error)
{
    scr->visual = gdk_android_visual_new(scr);
    scr->root_window = _gdk_windowing_window_init(&scr->screen);
    return TRUE;
}

GdkWindow *_gdk_android_root(void)
{
    if (!_gdk_screen)
        return NULL;
    return _gdk_screen->root_window;
}

static gint gdk_android_screen_get_width_mm(GdkScreen *screen)
{
    g_debug("width: %gmm", (double) gdk_screen_get_width(screen) / _gdk_screen->xdpi * 25.4);
    return (double) gdk_screen_get_width(screen) / _gdk_screen->xdpi * 25.4;
}

static gint gdk_android_screen_get_height_mm (GdkScreen *screen)
{
    g_debug("height: %gmm", (double) gdk_screen_get_height (screen) / _gdk_screen->ydpi * 25.4);
    return (double) gdk_screen_get_height (screen) / _gdk_screen->ydpi * 25.4;
}

static GdkWindow *gdk_android_screen_get_root_window(GdkScreen *screen)
{
    return GDK_ANDROID_SCREEN(screen)->root_window;
}

static gint gdk_android_screen_get_n_monitors(GdkScreen *screen)
{
    return 1;
}

static gint gdk_android_screen_get_primary_monitor(GdkScreen *screen)
{
    return 0;
}

static gint gdk_android_screen_get_monitor_width_mm(GdkScreen *screen, gint num_monitor)
{
    return gdk_android_screen_get_width_mm(screen);
}

static gint gdk_android_screen_get_monitor_height_mm(GdkScreen *screen, gint num_monitor)
{
    return gdk_android_screen_get_height_mm(screen);
}

static gchar *gdk_android_screen_get_monitor_plug_name (GdkScreen *screen, gint num_monitor)
{
    return g_strdup("android display");
}

static void gdk_android_screen_get_monitor_geometry(GdkScreen    *screen,
                                                    gint          num_monitor,
                                                    GdkRectangle *dest)
{
    g_return_if_fail(screen == (GdkScreen *)_gdk_screen);

    dest->x = 0;
    dest->y = 0;
    dest->width = gdk_android_screen_get_width(screen);
    dest->height = gdk_android_screen_get_height(screen);
}

static GdkVisual *gdk_android_screen_get_rgba_visual(GdkScreen *screen)
{
    g_return_val_if_fail(screen == (GdkScreen *)_gdk_screen, NULL);

    return NULL;
}

static gint gdk_android_screen_get_number(GdkScreen *screen)
{
    return 0;
}

static gchar *gdk_android_screen_make_display_name(GdkScreen *screen)
{
    return g_strdup(gdk_display_get_name(&_gdk_display->display));
}

static GdkWindow *gdk_android_screen_get_active_window(GdkScreen *screen)
{
    g_return_val_if_fail (GDK_IS_SCREEN(screen), NULL);

    return NULL;
}

static GList *gdk_android_screen_get_window_stack(GdkScreen *screen)
{
    g_return_val_if_fail (GDK_IS_SCREEN(screen), NULL);

    return NULL;
}

static gboolean gdk_android_screen_is_composited(GdkScreen *screen)
{
    g_return_val_if_fail(GDK_IS_SCREEN(screen), FALSE);

    return FALSE;
}

static void gdk_android_screen_finalize(GObject *object)
{
    GdkAndroidScreen *s = GDK_ANDROID_SCREEN(object);

    if (s->root_window)
        g_object_unref(s->root_window);

    g_object_unref(s->visual);
}

static void gdk_android_screen_dispose(GObject *object)
{
    GdkAndroidScreen *s = GDK_ANDROID_SCREEN(object);

    if (s->root_window)
        _gdk_window_destroy(s->root_window, FALSE);

    G_OBJECT_CLASS(gdk_android_screen_parent_class)->dispose(object);
}

static gboolean _gdk_android_screen_get_setting(GdkScreen   *screen,
                                                const gchar *name,
                                                GValue      *value)
{
    g_return_val_if_fail(GDK_IS_ANDROID_SCREEN(screen), FALSE);

    if (strcmp("gtk-theme-name", name) == 0)
    {
        g_value_set_string(value, "Android");
        return TRUE;
    }

    return FALSE;
}

static void gdk_android_screen_class_init(GdkAndroidScreenClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GdkScreenClass *screen_class = GDK_SCREEN_CLASS(klass);

  object_class->dispose = gdk_android_screen_dispose;
  object_class->finalize = gdk_android_screen_finalize;

  screen_class->get_display = gdk_android_screen_get_display;
  screen_class->get_width = gdk_android_screen_get_width;
  screen_class->get_height = gdk_android_screen_get_height;
  screen_class->get_width_mm = gdk_android_screen_get_width_mm;
  screen_class->get_height_mm = gdk_android_screen_get_height_mm;
  screen_class->get_number = gdk_android_screen_get_number;
  screen_class->get_root_window = gdk_android_screen_get_root_window;
  screen_class->get_n_monitors = gdk_android_screen_get_n_monitors;
  screen_class->get_primary_monitor = gdk_android_screen_get_primary_monitor;
  screen_class->get_monitor_width_mm = gdk_android_screen_get_monitor_width_mm;
  screen_class->get_monitor_height_mm = gdk_android_screen_get_monitor_height_mm;
  screen_class->get_monitor_plug_name = gdk_android_screen_get_monitor_plug_name;
  screen_class->get_monitor_geometry = gdk_android_screen_get_monitor_geometry;
  screen_class->get_monitor_workarea = gdk_android_screen_get_monitor_geometry;
  screen_class->get_system_visual = _gdk_android_screen_get_system_visual;
  screen_class->get_rgba_visual = gdk_android_screen_get_rgba_visual;
  screen_class->is_composited = gdk_android_screen_is_composited;
  screen_class->make_display_name = gdk_android_screen_make_display_name;
  screen_class->get_active_window = gdk_android_screen_get_active_window;
  screen_class->get_window_stack = gdk_android_screen_get_window_stack;
  screen_class->get_setting = _gdk_android_screen_get_setting;
  screen_class->visual_get_best_depth = _gdk_android_screen_visual_get_best_depth;
  screen_class->visual_get_best_type = _gdk_android_screen_visual_get_best_type;
  screen_class->visual_get_best = _gdk_android_screen_visual_get_best;
  screen_class->visual_get_best_with_depth = _gdk_android_screen_visual_get_best_with_depth;
  screen_class->visual_get_best_with_type = _gdk_android_screen_visual_get_best_with_type;
  screen_class->visual_get_best_with_both = _gdk_android_screen_visual_get_best_with_both;
  screen_class->query_depths = _gdk_android_screen_query_depths;
  screen_class->query_visual_types = _gdk_android_screen_query_visual_types;
  screen_class->list_visuals = _gdk_android_screen_list_visuals;
}
