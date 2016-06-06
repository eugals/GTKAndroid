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
// ANativeWindow-based GDK display and screen definitions

#ifndef __GDK_PRIVATE_ANDROID_H__
#define __GDK_PRIVATE_ANDROID_H__

#include "config.h"

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <gdk/gdkprivate.h>
#include <gdk/gdkcursorprivate.h>

#include "gdkinternals.h"
#include "gdkdisplayprivate.h"
#include "gdkscreenprivate.h"

struct ANativeActivity;

struct GdkAndroidDisplay
{
    GdkDisplay display;
    struct android_app *app;
    EGLDisplay eglDisplay;
    EGLContext eglContext;
    EGLSurface eglSurface;
    cairo_device_t *cairoDevice;
    cairo_surface_t *cairoSurface;
    gchar *defaultFont;
    gchar *cachePath;
    gint notificationBarHeight;
};

struct GdkAndroidScreen
{
    GdkScreen screen;
    GdkVisual *visual;
    GdkWindow *root_window;
    gfloat xdpi;
    gfloat ydpi;
};

extern struct GdkAndroidDisplay *_gdk_display;
extern struct GdkAndroidScreen  *_gdk_screen;



gboolean _gdk_android_display_has_pending(GdkDisplay *display);
void _gdk_android_display_queue_events(GdkDisplay *display);


GdkWindow *_gdk_windowing_window_init       (GdkScreen *screen);
void _gdk_android_display_create_window_impl(GdkDisplay    *display,
                                             GdkWindow     *window,
                                             GdkWindow     *real_parent,
                                             GdkScreen     *screen,
                                             GdkEventMask   event_mask,
                                             GdkWindowAttr *attributes,
                                             gint           attributes_mask);

void       _gdk_push_modal_window   (GdkWindow *window);
void       _gdk_remove_modal_window (GdkWindow *window);
GdkWindow *_gdk_modal_current       (void);
gboolean   _gdk_modal_blocked       (GdkWindow *window);

GdkWindow *_gdk_android_root(void);
gboolean gdk_android_init_screen(struct GdkAndroidScreen *scr, struct ANativeActivity *act,
                                 GError **error);

gboolean gdk_android_adjust_with_jni(struct GdkAndroidScreen *scr, struct GdkAndroidDisplay *desk,
                                     struct ANativeActivity *act, GError **error);
void gdk_android_events_init();

GdkWindow *_gdk_android_find_window_for_screen_pos(gdouble *x, gdouble *y);

GdkKeymap *_gdk_android_display_get_keymap(GdkDisplay *display);
gboolean _gdk_android_window_simulate_key(GdkWindow      *window,
                                          gint            x,
                                          gint            y,
                                          guint           keyval,
                                          GdkModifierType modifiers,
                                          GdkEventType    key_pressrelease);
gboolean _gdk_android_window_simulate_button(GdkWindow      *window,
                                             gint            x,
                                             gint            y,
                                             guint           button, // 1..3
                                             GdkModifierType modifiers,
                                             GdkEventType    button_pressrelease);


#endif // __GDK_PRIVATE_ANDROID_H__
