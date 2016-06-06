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
// ANativeWindow-based GDK display declaration

#ifndef __GDK_ANDROID_DISPLAY_H__
#define __GDK_ANDROID_DISPLAY_H__

#include <gdk/gdk.h>

G_BEGIN_DECLS

typedef struct GdkAndroidDisplay GdkAndroidDisplay;
typedef struct GdkAndroidDisplayClass GdkAndroidDisplayClass;

#define GDK_TYPE_ANDROID_DISPLAY              (gdk_android_display_get_type())
#define GDK_ANDROID_DISPLAY(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), GDK_TYPE_ANDROID_DISPLAY, GdkAndroidDisplay))
#define GDK_ANDROID_DISPLAY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), GDK_TYPE_ANDROID_DISPLAY, GdkAndroidDisplayClass))
#define GDK_IS_ANDROID_DISPLAY(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), GDK_TYPE_ANDROID_DISPLAY))

GType gdk_android_display_get_type(void);

gboolean gdk_android_init_display(struct android_app *app,
                                  GError **error);
gboolean gdk_android_term_display();
struct android_app *_gdk_android_display_get_app(GdkDisplay *display);

#define GDK_ANDROID_ERROR  g_quark_from_static_string ("gdk-android")

enum GDKAndroidErrorCode
{
    GDK_ANDROID_ERROR_INIT = 1
};

const gchar *gdk_android_get_default_font();

G_END_DECLS

#endif /* __GDK_ANDROID_DISPLAY_H__ */
