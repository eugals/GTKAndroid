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

#include "config.h"

#include <jni.h>

#include <android/native_activity.h>
#include <android_native_app_glue.h>

#include "gdk.h"
#include "gdkvisualprivate.h"
#include "gdkprivate-android.h"
#include "gdkinput-android.h"
#include "gdkdeviceprivate.h"

GList            *_gdk_input_devices;

//////////////////////////////////////////////////////////////////
// device class

typedef GdkDevice GdkAndroidDevice;

typedef struct GdkAndroidDeviceClass
{
    GdkDeviceClass parent_class;
} GdkAndroidDeviceClass;

G_DEFINE_TYPE(GdkAndroidDevice, gdk_android_device, GDK_TYPE_DEVICE)

#define GDK_TYPE_ANDROID_DEVICE         (gdk_android_device_get_type ())
#define GDK_ANDROID_DEVICE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GDK_TYPE_ANDROID_DEVICE, GdkAndroidDevice))
#define GDK_ANDROID_DEVICE_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), GDK_TYPE_ANDROID_DEVICE, GdkAndroidDeviceClass))
#define GDK_IS_ANDROID_DEVICE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GDK_TYPE_ANDROID_DEVICE))
#define GDK_IS_ANDROID_DEVICE_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), GDK_TYPE_ANDROID_DEVICE))
#define GDK_ANDROID_DEVICE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GDK_TYPE_ANDROID_DEVICE, GdkAndroidDeviceClass))

static gboolean gdk_android_device_get_history(GdkDevice      *device,
                                               GdkWindow      *window,
                                               guint32         start,
                                               guint32         stop,
                                               GdkTimeCoord ***events,
                                               gint           *n_events)
{
    return FALSE;
}

static void gdk_android_device_get_state(GdkDevice       *device,
                                         GdkWindow       *window,
                                         gdouble         *axes,
                                         GdkModifierType *mask)
{
    gdouble x, y;

    gdk_window_get_device_position_double(window, device, &x, &y, mask);
    if (axes)
    {
        axes[0] = x;
        axes[1] = y;
    }
}

static void gdk_android_device_set_window_cursor(GdkDevice *device,
                                                 GdkWindow *window,
                                                 GdkCursor *cursor)
{
}

static void gdk_android_device_warp(GdkDevice *device,
                                    GdkScreen *screen,
                                    gdouble    x,
                                    gdouble    y)
{
}

static void
gdk_android_device_query_state (GdkDevice        *device,
                                GdkWindow        *window,
                                GdkWindow       **root_window,
                                GdkWindow       **child_window,
                                gdouble          *root_x,
                                gdouble          *root_y,
                                gdouble          *win_x,
                                gdouble          *win_y,
                                GdkModifierType  *mask)
{
    if (root_window)
        *root_window = _gdk_screen->root_window;
    if (child_window)
        *child_window = NULL;
    if (mask)
        *mask = 0;
    if (win_x)
        *win_x = 0;
    if (win_y)
        *win_y = 0;
    if (root_x)
        *root_x = 0;
    if (root_y)
        *root_y = 0;
}

static GdkGrabStatus gdk_android_device_grab(GdkDevice    *device,
                                             GdkWindow    *window,
                                             gboolean      owner_events,
                                             GdkEventMask  event_mask,
                                             GdkWindow    *confine_to,
                                             GdkCursor    *cursor,
                                             guint32       time_)
{
    if(gdk_device_get_source(device) == GDK_SOURCE_KEYBOARD)
    {
        g_debug("gdk_android_device_grab: GDK_SOURCE_KEYBOARD");
        /* Device is a keyboard */
        return GDK_GRAB_SUCCESS;
    }
    g_debug("gdk_android_device_grab: other");
    return GDK_GRAB_ALREADY_GRABBED;
}

static void gdk_android_device_ungrab(GdkDevice *device,
                                      guint32    time_)
{
    GdkDisplay *display;
    GdkDeviceGrabInfo *grab;

    if (gdk_device_get_source (device) == GDK_SOURCE_KEYBOARD)
    {
        g_debug("gdk_android_device_ungrab: GDK_SOURCE_KEYBOARD");
    }
    else
    {
        g_debug("gdk_android_device_ungrab: other");
    }
}

static GdkWindow *gdk_android_device_window_at_position(GdkDevice       *device,
                                                        gdouble         *win_x,
                                                        gdouble         *win_y,
                                                        GdkModifierType *mask,
                                                        gboolean         get_toplevel)
{
    g_debug("gdk_android_device_window_at_position");

    if (win_x)
        *win_x = 0;
    if (win_y)
        *win_y = 0;
    if (mask)
        *mask = 0;

    return _gdk_screen->root_window;
}

static void gdk_android_device_select_window_events(GdkDevice    *device,
                                                    GdkWindow    *window,
                                                    GdkEventMask  event_mask)
{
    g_debug("gdk_android_device_select_window_events");
}

static void gdk_android_device_class_init(GdkAndroidDeviceClass *klass)
{
    GdkDeviceClass *device_class = GDK_DEVICE_CLASS(klass);

    device_class->get_history = gdk_android_device_get_history;
    device_class->get_state = gdk_android_device_get_state;
    device_class->set_window_cursor = gdk_android_device_set_window_cursor;
    device_class->warp = gdk_android_device_warp;
    device_class->query_state = gdk_android_device_query_state;
    device_class->grab = gdk_android_device_grab;
    device_class->ungrab = gdk_android_device_ungrab;
    device_class->window_at_position = gdk_android_device_window_at_position;
    device_class->select_window_events = gdk_android_device_select_window_events;
}

static void gdk_android_device_init(GdkAndroidDevice *device_core)
{
    GdkDevice *device;

    device = GDK_DEVICE(device_core);

    _gdk_device_add_axis(device, GDK_NONE, GDK_AXIS_X, 0, 0, 1);
    _gdk_device_add_axis(device, GDK_NONE, GDK_AXIS_Y, 0, 0, 1);
}

//////////////////////////////////////////////////////////////////
// device manager class

struct GdkDeviceManagerAndroidClass
{
    GdkDeviceManagerClass parent_class;
};

G_DEFINE_TYPE (GdkDeviceManagerAndroid, gdk_device_manager_android, GDK_TYPE_DEVICE_MANAGER)

static GdkDevice *create_pointer(GdkDeviceManager *device_manager,
                                 const char *name, GdkDeviceType type)
{
  return g_object_new(GDK_TYPE_ANDROID_DEVICE,
                      "name", name,
                      "type", type,
                      "input-source", GDK_SOURCE_TOUCHSCREEN,
                      "input-mode", GDK_MODE_SCREEN,
                      "has-cursor", FALSE,
                      "display", _gdk_display,
                      "device-manager", device_manager,
                      NULL);
}

static GdkDevice *create_keyboard(GdkDeviceManager *device_manager,
                                  const char *name, GdkDeviceType type)
{
  return g_object_new(GDK_TYPE_ANDROID_DEVICE,
                      "name", name,
                      "type", type,
                      "input-source", GDK_SOURCE_KEYBOARD,
                      "input-mode", GDK_MODE_SCREEN,
                      "has-cursor", FALSE,
                      "display", _gdk_display,
                      "device-manager", device_manager,
                      NULL);
}

static void gdk_device_manager_android_init(GdkDeviceManagerAndroid *device_manager_android)
{
}

static void gdk_device_manager_android_finalize(GObject *object)
{
  GdkDeviceManagerAndroid *device_manager;

  device_manager = GDK_DEVICE_MANAGER_ANDROID(object);

  g_object_unref(device_manager->pointer);
  g_object_unref(device_manager->keyboard);

  G_OBJECT_CLASS(gdk_device_manager_android_parent_class)->finalize(object);
}

static void gdk_device_manager_android_constructed(GObject *object)
{
    GdkDeviceManagerAndroid *device_manager = GDK_DEVICE_MANAGER_ANDROID(object);

    device_manager->pointer = create_pointer(GDK_DEVICE_MANAGER(device_manager),
                                             "Virtual Core Pointer",
                                             GDK_DEVICE_TYPE_MASTER);

    device_manager->keyboard = create_keyboard(GDK_DEVICE_MANAGER(device_manager),
                                               "Virtual Core Keyboard",
                                               GDK_DEVICE_TYPE_MASTER);
    _gdk_device_set_associated_device(device_manager->pointer, device_manager->keyboard);
    _gdk_device_set_associated_device(device_manager->keyboard, device_manager->pointer);
}

static GList *gdk_device_manager_android_list_devices(GdkDeviceManager *device_manager,
                                                      GdkDeviceType     type)
{
    GdkDeviceManagerAndroid *adm;
    GList *devices = NULL, *l;

    adm = (GdkDeviceManagerAndroid *) device_manager;

    if (type == GDK_DEVICE_TYPE_MASTER)
    {
        devices = g_list_prepend(devices, adm->keyboard);
        devices = g_list_prepend(devices, adm->pointer);
    }
    else
    {
        // none
    }

    return g_list_reverse(devices);
}

static GdkDevice *gdk_device_manager_android_get_client_pointer(GdkDeviceManager *device_manager)
{
    GdkDeviceManagerAndroid *adm = (GdkDeviceManagerAndroid *) device_manager;
    return adm->pointer;
}

static void gdk_device_manager_android_class_init(GdkDeviceManagerAndroidClass *klass)
{
    GdkDeviceManagerClass *device_manager_class = GDK_DEVICE_MANAGER_CLASS(klass);
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = gdk_device_manager_android_finalize;
    object_class->constructed = gdk_device_manager_android_constructed;
    device_manager_class->list_devices = gdk_device_manager_android_list_devices;
    device_manager_class->get_client_pointer = gdk_device_manager_android_get_client_pointer;
}

//////////////////////////////////////////////////////////////////
// other public functions

GList *gdk_android_display_list_devices(GdkDisplay *dpy)
{
    g_return_val_if_fail(dpy == (GdkDisplay *) _gdk_display, NULL);

    return _gdk_input_devices;
}

void gdk_android_input_init(GdkDisplay *display)
{
    GdkDeviceManagerAndroid *device_manager;

    device_manager = g_object_new(GDK_TYPE_DEVICE_MANAGER_ANDROID,
                                  "display", display,
                                  NULL);
    display->device_manager = GDK_DEVICE_MANAGER(device_manager);

    display->core_pointer = device_manager->pointer;

    _gdk_input_devices = g_list_append(NULL, display->core_pointer);
}

static void gdk_android_queue_event(GdkEvent *event)
{
    _gdk_event_queue_append(&_gdk_display->display, event);
}

static void generate_motion_event(GdkEventType type, AInputEvent *aevent, GdkEvent *crossingEvent)
{
    GdkEvent *ev = gdk_event_new(type);
    GdkDeviceManagerAndroid *devManager;

    devManager = GDK_DEVICE_MANAGER_ANDROID(gdk_display_get_device_manager(&_gdk_display->display));

    if (crossingEvent)
    {
        ev->button.x = crossingEvent->crossing.x;
        ev->button.y = crossingEvent->crossing.y;
        ev->button.x_root = crossingEvent->crossing.x_root;
        ev->button.y_root = crossingEvent->crossing.y_root;
        ev->button.window = crossingEvent->crossing.window;
        ev->button.time = crossingEvent->crossing.time;
    }
    else
    {
        ev->button.x = (gint16) AMotionEvent_getX(aevent, 0);
        ev->button.y = (gint16) AMotionEvent_getY(aevent, 0);
        ev->button.x_root = ev->button.x;
        ev->button.y_root = ev->button.y;
        ev->button.window = _gdk_android_find_window_for_screen_pos(&ev->button.x, &ev->button.y);
        ev->button.time = AMotionEvent_getEventTime(aevent) / 1000000;
    }
    g_object_ref(ev->button.window);
    ev->button.axes = NULL;
    ev->button.state = GDK_BUTTON1_MASK;
    ev->button.button = GDK_BUTTON_PRIMARY;
    gdk_event_set_device(ev, devManager->pointer);
    //gdk_event_set_source_device(event, device_manager->system_pointer);

    gdk_android_queue_event(ev);
}


GdkEvent *create_crossing_event(GdkEventType type, AInputEvent *aevent)
{
    GdkEvent *ev = gdk_event_new(type);
    GdkDeviceManagerAndroid *devManager;

    devManager = GDK_DEVICE_MANAGER_ANDROID(gdk_display_get_device_manager(&_gdk_display->display));

    ev->crossing.x = (gint16) AMotionEvent_getX(aevent, 0);
    ev->crossing.y = (gint16) AMotionEvent_getY(aevent, 0);
    ev->crossing.x_root = ev->crossing.x;
    ev->crossing.y_root = ev->crossing.y;
    ev->crossing.window = _gdk_android_find_window_for_screen_pos(&ev->crossing.x, &ev->crossing.y);
    g_object_ref(ev->crossing.window);
    ev->crossing.time = AMotionEvent_getEventTime(aevent) / 1000000; //android time is in nanoseconds
    gdk_event_set_device(ev, devManager->pointer);
    //gdk_event_set_source_device(event, device_manager->system_pointer);

    return ev;
}

int32_t android_handle_input(struct android_app *app, AInputEvent *event)
{
    GdkEvent *crossingEvent = NULL;
    if (AInputEvent_getType(event) == AMOTION_EVENT_AXIS_PRESSURE)
    {
        switch(AMotionEvent_getAction(event))
        {
        case AMOTION_EVENT_ACTION_DOWN:
            crossingEvent = create_crossing_event(GDK_ENTER_NOTIFY, event);
            gdk_android_queue_event(crossingEvent);
            generate_motion_event(GDK_BUTTON_PRESS, event, crossingEvent);
            break;
        case AMOTION_EVENT_ACTION_UP:
            crossingEvent = create_crossing_event(GDK_LEAVE_NOTIFY, event);
            generate_motion_event(GDK_BUTTON_RELEASE, event, crossingEvent);
            gdk_android_queue_event(crossingEvent);
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            generate_motion_event(GDK_MOTION_NOTIFY, event, crossingEvent);
            break;
        default:
            break;
        }
    }
    return 0;
}
