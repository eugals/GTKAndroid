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
// Android->GDK events conversion code

#include "config.h"

#include <jni.h>

#include <android/sensor.h>
#include <android_native_app_glue.h>

#include "gdk.h"
#include "gdkdisplayprivate.h"
#include "gdkprivate-android.h"
#include "gdkandroiddisplay.h"
#include "gdkinput-android.h"

static void (*gdk_android_stop)(void) = NULL;
static ASensorEventQueue *sensorEventQueue = NULL;
static const ASensor* accelerometerSensor = NULL;


static gboolean gdk_android_event_prepare(GSource *source, gint *timeout)
{
    gboolean retval;

    gdk_threads_enter();

    retval = _gdk_event_queue_find_first(GDK_DISPLAY(_gdk_display)) != NULL;
    if (timeout)
        *timeout = 100;

    gdk_threads_leave();

    return retval;
}

static gboolean gdk_android_event_check(GSource *source)
{
    gboolean retval;
    struct android_app *app = _gdk_display->app;
    int astate = app->activityState;

    gdk_threads_enter();

    retval = _gdk_event_queue_find_first(GDK_DISPLAY(_gdk_display)) != NULL;

    if (!retval && astate != APP_CMD_PAUSE && astate != APP_CMD_STOP)
    {
        int ident, events;
        struct android_poll_source *asource;
        // Read all pending events.
        while ((ident = ALooper_pollAll(0, NULL, &events, (void**) &asource)) >= 0)
        {
            // Process this event.
            if (asource != NULL)
                asource->process(app, asource);

            if (ident == LOOPER_ID_USER)
            {
                if (sensorEventQueue != NULL)
                {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0)
                    {
                        g_debug("sensor event received");
                    }
                }
            }

            if (app->destroyRequested != 0 && gdk_android_stop)
            {
                gdk_android_stop();
                break;
            }
        }
    }

    gdk_threads_leave();

    return retval;
}

static gboolean gdk_android_event_dispatch(GSource *source, GSourceFunc callback, gpointer user_data)
{
    GdkEvent *event;

    gdk_threads_enter();

    event = _gdk_event_unqueue(GDK_DISPLAY(_gdk_display));

    if (event)
    {
        _gdk_event_emit(event);
        gdk_event_free(event);
    }

    gdk_threads_leave();

    return TRUE;
}

void gdk_android_events_init()
{
    static GSourceFuncs event_funcs = {
        gdk_android_event_prepare,
        gdk_android_event_check,
        gdk_android_event_dispatch,
        NULL
    };

    GSource *source = g_source_new(&event_funcs, sizeof(GSource));
    g_source_set_name(source, "Android ALooper event source");
    g_source_set_priority(source, GDK_PRIORITY_EVENTS);
    g_source_set_can_recurse(source, TRUE);
    g_source_attach(source, NULL);
}

gboolean _gdk_android_display_has_pending(GdkDisplay *display)
{
    g_debug("_gdk_android_display_has_pending is called");
    return _gdk_event_queue_find_first(display) != NULL;
}

void _gdk_android_display_queue_events(GdkDisplay *display)
{
    int ident;
    int events;
    struct android_poll_source* source;
    struct android_app *state = GDK_ANDROID_DISPLAY(display)->app;
    // Read all pending events.
    while ((ident = ALooper_pollAll(0, NULL, &events, (void**) &source)) >= 0)
    {
        // Process this event.
        if (source != NULL)
            source->process(state, source);

        // Check if we are exiting.
        if (state->destroyRequested != 0)
        {
            gdk_android_term_display();
            return;
        }
    }
}

void gdk_android_handle_glue_cmd(struct android_app* app, int32_t cmd)
{
    GError *err = NULL;
    //GTKAndroidInitPhase *engine = (GTKAndroidInitPhase *) app->userData;
    switch (cmd)
    {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            app->savedState = NULL;
            app->savedStateSize = 0;
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (app->window != NULL && !gdk_android_init_display(app, &err))
            {
                g_error("%s", err->message);
                g_error_free(err);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            if (gdk_android_stop)
                gdk_android_stop();
            break;
        case APP_CMD_PAUSE:
        case APP_CMD_STOP:
            if (gdk_android_stop)
                gdk_android_stop();
            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            if (accelerometerSensor != NULL)
            {
                ASensorEventQueue_enableSensor(sensorEventQueue, accelerometerSensor);
                ASensorEventQueue_setEventRate(sensorEventQueue, accelerometerSensor, 0.5);
            }
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            if (accelerometerSensor != NULL)
                ASensorEventQueue_disableSensor(sensorEventQueue, accelerometerSensor);
            break;

    }
}

static void onLowMemory(ANativeActivity* activity) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    g_info("LowMemory: %p\n", activity);
    //android_app_write_cmd(android_app, APP_CMD_LOW_MEMORY);
}

static void onNativeWindowResized(ANativeActivity* activity, ANativeWindow* window) {
    g_info("NativeWindowResized: %p -- %p\n", activity, window);
    //android_app_write_cmd((struct android_app*)activity->instance, APP_CMD_WINDOW_RESIZED);
}

static void onNativeWindowRedrawNeeded(ANativeActivity* activity, ANativeWindow* window) {
    g_info("NativeWindowRedrawNeeded: %p -- %p\n", activity, window);
    //android_app_wait_redraw((struct android_app*)activity->instance);
}

static void onContentRectChanged(ANativeActivity* activity, const ARect* rect) {
    g_info("ContentRectChanged: %p -- (%d,%d)-(%d,%d)\n", activity, rect->left,
           rect->top, rect->right, rect->bottom);
    ((struct android_app*)activity->instance)->contentRect = *rect;
}

void gdk_android_setup_app_callbacks(struct android_app *state, void (*onStop)())
{
    if (0)
    {
        ASensorManager* sensorManager = NULL;

        sensorManager = ASensorManager_getInstance();
        accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager,
                                                              ASENSOR_TYPE_ACCELEROMETER);
        sensorEventQueue = ASensorManager_createEventQueue(sensorManager, state->looper,
                                                           LOOPER_ID_USER, NULL, NULL);
    }

    gdk_android_stop = onStop;

    state->onAppCmd = gdk_android_handle_glue_cmd;
    state->activity->callbacks->onNativeWindowResized = onNativeWindowResized;
    state->activity->callbacks->onNativeWindowRedrawNeeded = onNativeWindowRedrawNeeded;
    state->activity->callbacks->onContentRectChanged = onContentRectChanged;
    state->activity->callbacks->onLowMemory = onLowMemory;
    // TODO: consider overriding state->inputPollSource.process instead of the following
    //       or should we even get rid of this native_app for good?
    state->onInputEvent = android_handle_input;
}

gboolean gdk_android_process_events_until_display_ready(struct android_app *app)
{
    int ident;
    int events;
    struct android_poll_source* source;
    while (1)
    {
        // Read all pending events.
        while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0)
        {
            // Process this event.
            if (source != NULL)
                source->process(app, source);

            // Check if we are exiting.
            if (app->destroyRequested != 0)
            {
                gdk_android_term_display();
                return FALSE;
            }

            if (_gdk_display)
                return TRUE;
        }
    }
}

GdkKeymap *_gdk_android_display_get_keymap(GdkDisplay *display)
{
    g_debug("_gdk_android_display_get_keymap is called");
    return NULL;
}

gboolean _gdk_android_window_simulate_key(GdkWindow      *window,
                                          gint            x,
                                          gint            y,
                                          guint           keyval,
                                          GdkModifierType modifiers,
                                          GdkEventType    key_pressrelease)
{
    return FALSE;
}

gboolean _gdk_android_window_simulate_button(GdkWindow      *window,
                                             gint            x,
                                             gint            y,
                                             guint           button, // 1..3
                                             GdkModifierType modifiers,
                                             GdkEventType    button_pressrelease)
{
    return FALSE;
}
