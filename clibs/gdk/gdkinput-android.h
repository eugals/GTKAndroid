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
// Inputs handlers and device manager

#ifndef __GDK_INPUT_ANDROID_H__
#define __GDK_INPUT_ANDROID_H__

#include <gdk/gdkdevicemanagerprivate.h>

G_BEGIN_DECLS

#define GDK_TYPE_DEVICE_MANAGER_ANDROID         (gdk_device_manager_android_get_type ())
#define GDK_DEVICE_MANAGER_ANDROID(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GDK_TYPE_DEVICE_MANAGER_ANDROID, GdkDeviceManagerAndroid))
#define GDK_DEVICE_MANAGER_ANDROID_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), GDK_TYPE_DEVICE_MANAGER_ANDROID, GdkDeviceManagerAndroidClass))
#define GDK_IS_DEVICE_MANAGER_ANDROID(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GDK_TYPE_DEVICE_MANAGER_ANDROID))
#define GDK_IS_DEVICE_MANAGER_ANDROID_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), GDK_TYPE_DEVICE_MANAGER_ANDROID))
#define GDK_DEVICE_MANAGER_ANDROID_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GDK_TYPE_DEVICE_MANAGER_ANDROID, GdkDeviceManagerAndroidClass))

typedef struct GdkDeviceManagerAndroid
{
    GdkDeviceManager parent_object;
    // Master Devices
    GdkDevice *pointer;
    GdkDevice *keyboard;
} GdkDeviceManagerAndroid;
typedef struct GdkDeviceManagerAndroidClass GdkDeviceManagerAndroidClass;

GType gdk_device_manager_android_get_type (void) G_GNUC_CONST;

void gdk_android_input_init(GdkDisplay *display);
GList *gdk_android_display_list_devices(GdkDisplay *dpy);

struct AInputEvent;
int32_t android_handle_input(struct android_app *app, struct AInputEvent *event);


G_END_DECLS

#endif // __GDK_INPUT_ANDROID_H__
