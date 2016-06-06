// Copyright 2014-2015 The GTK+Android Developers. See the COPYRIGHT
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
// Core android initialization functions

#include "config.h"

#include <stdlib.h>

#include <jni.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include "gtkandroid.h"
#include "gdkandroiddisplay.h"
#include "gdkandroidevents.h"
#include "gtkandroidprivate.h"

struct android_app *_gtk_android_glue_app = NULL;

////////////////////////////////////////////////////////////////////////////////////
// logging

static void log_handler(const gchar *log_domain, GLogLevelFlags log_level,
                        const gchar *message, gpointer user_data)
{
    android_LogPriority priority = ANDROID_LOG_VERBOSE;
    if (log_level & G_LOG_LEVEL_ERROR)
        priority = ANDROID_LOG_FATAL;
    else if (log_level & G_LOG_LEVEL_CRITICAL)
        priority = ANDROID_LOG_ERROR;
    else if (log_level & G_LOG_LEVEL_WARNING)
        priority = ANDROID_LOG_WARN;
    else if (log_level & G_LOG_LEVEL_INFO || log_level & G_LOG_LEVEL_MESSAGE)
        priority = ANDROID_LOG_INFO;
    else if (log_level & G_LOG_LEVEL_DEBUG)
        priority = ANDROID_LOG_DEBUG;
    __android_log_print(priority, log_domain, "%s", message);
}


///////////////////////////////////////////////////////////////////////////////////
// locale

// Android NDK doesn't really support any locale except "C"
char *setlocale(int category, const char *locale)
{
    static const char C_LOCALE_SETTING[] = "C";
    return (char*) C_LOCALE_SETTING;
}

///////////////////////////////////////////////////////////////////////////////////
// window event handlers & initialization

static void (*default_gtk_window_class_constructed)(GObject *object) = NULL;

void android_gtk_window_class_constructed(GObject *object)
{
    GtkWindow *w = GTK_WINDOW(object);

    if (default_gtk_window_class_constructed)
        (*default_gtk_window_class_constructed)(object);

    gtk_window_set_has_resize_grip(w, FALSE);
}

static void adjust_default_font()
{
    GtkSettings *settings = gtk_settings_get_default();
    const gchar *font = gdk_android_get_default_font();
    g_assert(settings != NULL);

    if (font)
    {
        gtk_settings_set_string_property(settings, "gtk-font-name", font, "gtk_android_init");
        g_info("Default GTK+ font is set to '%s'", font);
    }
}

static gtk_window_tweak_class()
{
    GObjectClass *objClass = G_OBJECT_CLASS(g_type_class_ref(GTK_TYPE_WINDOW));

    if (objClass->constructed != android_gtk_window_class_constructed)
    {
        default_gtk_window_class_constructed = objClass->constructed;
        objClass->constructed = android_gtk_window_class_constructed;
    }
}

static void (*defaultActivityDestroyHandler)(ANativeActivity* activity) = NULL;

static void onActivityDestroy(ANativeActivity* activity)
{
    // wait until the native_app is detached
    defaultActivityDestroyHandler(activity);
    // and kill this process
    exit(0);
}

///////////////////////////////////////////////////////////////////////////////////
// public API

// Prepares basic infrastructure needed by gtk+ applications.
// Call this function at the beginning of your android_main
// instead of *gtk_init* which would be used on other platforms
void gtk_android_init(struct android_app *state)
{
    app_dummy(); // Make sure glue isn't stripped.

    _gtk_android_glue_app = state;

    g_log_set_default_handler(log_handler, NULL);

    gdk_android_setup_app_callbacks(state, gtk_main_quit);
    gdk_android_process_events_until_display_ready(state);

    gtk_window_tweak_class();

    gtk_init(NULL, NULL);

    // g_object_get(settings, "gtk-font-name", &font_name, NULL);
    adjust_default_font();
}

void gtk_android_exit(struct android_app *state)
{
    int ident;
    int events;
    struct android_poll_source* source;

    // replace onDestroy handler to terminate the process AFTER everything is unloaded
    if (!defaultActivityDestroyHandler)
    {
        defaultActivityDestroyHandler = state->activity->callbacks->onDestroy;
        state->activity->callbacks->onDestroy = onActivityDestroy;
    }

    ANativeActivity_finish(state->activity);

    while (!state->destroyRequested)
    {
        // Read all pending events.
        while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0)
        {
            // Process this event.
            if (source != NULL)
                source->process(state, source);
        }
    }
}
