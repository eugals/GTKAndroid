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
// The initial implementation was based on gdk's gdkwindow-*.h files code
// which is licensed under LGPLv2.1 terms
//
// ANativeWindow-based GDK window declarations

#ifndef __GDK_WINDOW_ANDROID_H__
#define __GDK_WINDOW_ANDROID_H__

#include "gdkprivate-android.h"
#include "gdk/gdkwindowimpl.h"
#include "gdk/gdkcursor.h"


G_BEGIN_DECLS

/* Window implementation for Android
 */

typedef struct GdkWindowImplAndroid GdkWindowImplAndroid;
typedef struct GdkWindowImplAndroidClass GdkWindowImplAndroidClass;

#define GDK_TYPE_WINDOW_IMPL_ANDROID              (gdk_window_impl_android_get_type())
#define GDK_WINDOW_IMPL_ANDROID(object)           (G_TYPE_CHECK_INSTANCE_CAST((object), GDK_TYPE_WINDOW_IMPL_ANDROID, GdkWindowImplAndroid))
#define GDK_WINDOW_IMPL_ANDROID_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GDK_TYPE_WINDOW_IMPL_ANDROID, GdkWindowImplAndroidClass))
#define GDK_IS_WINDOW_IMPL_ANDROID(object)        (G_TYPE_CHECK_INSTANCE_TYPE((object), GDK_TYPE_WINDOW_IMPL_ANDROID))
#define GDK_IS_WINDOW_IMPL_ANDROID_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass), GDK_TYPE_WINDOW_IMPL_ANDROID))
#define GDK_WINDOW_IMPL_ANDROID_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GDK_TYPE_WINDOW_IMPL_ANDROID, GdkWindowImplAndroidClass))

struct GdkWindowImplAndroid
{
    GdkWindowImpl parent;

    GdkWindow *wrapper;

    cairo_surface_t *cairo_surface;
    guint override_redirect;
    GdkEventMask native_event_mask;
    GdkWindowTypeHint type_hint;
    gboolean inside_end_paint;
};
 
struct GdkWindowImplAndroidClass 
{
    GdkWindowImplClass parent;
};

GType gdk_window_impl_android_get_type();

#define GDK_TYPE_ANDROID_WINDOW              (gdk_android_window_get_type ())
#define GDK_ANDROID_WINDOW(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), GDK_TYPE_ANDROID_WINDOW, GdkWin32Window))
#define GDK_ANDROID_WINDOW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), GDK_TYPE_ANDROID_WINDOW, GdkWin32WindowClass))
#define GDK_IS_ANDROID_WINDOW(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), GDK_TYPE_ANDROID_WINDOW))
#define GDK_IS_ANDROID_WINDOW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GDK_TYPE_ANDROID_WINDOW))
#define GDK_ANDROID_WINDOW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), GDK_TYPE_ANDROID_WINDOW, GdkWin32WindowClass))

#ifdef GDK_COMPILATION
typedef struct GdkAndroidWindow GdkAndroidWindow;
#else
typedef GdkWindow GdkAndroidWindow;
#endif
typedef struct GdkAndroidWindowClass GdkAndroidWindowClass;

GDK_AVAILABLE_IN_ALL
GType gdk_android_window_get_type(void);


G_END_DECLS

#endif /* __GDK_WINDOW_ANDROID_H__ */
