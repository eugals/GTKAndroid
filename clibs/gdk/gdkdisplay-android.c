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
// ANativeWindow-based GDK display definition

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
#include "gdkinput-android.h"


struct GdkAndroidDisplay *_gdk_display = NULL;
struct GdkAndroidScreen  *_gdk_screen = NULL;

extern void FcConfigSetupAndroidCacheDir(const char *dir); // defined in ../fonconfig/fcinit_android.c

///////////////////////////////////////////////////////////////////////////////////////
// Display

struct GdkAndroidDisplayClass
{
    GdkDisplayClass display_class;
};

G_DEFINE_TYPE(GdkAndroidDisplay, gdk_android_display, GDK_TYPE_DISPLAY)

static gulong gdk_android_display_get_next_serial(GdkDisplay *display)
{
	return 0;
}

static const gchar *gdk_android_display_get_name(GdkDisplay *display)
{
    return "ANativeWindow";
}

static GdkScreen *gdk_android_display_get_default_screen(GdkDisplay *display)
{
    g_return_val_if_fail (GDK_IS_ANDROID_DISPLAY(display), NULL);
    return &_gdk_screen->screen;
}


static GdkWindow *gdk_android_display_get_default_group(GdkDisplay *display)
{
    g_return_val_if_fail (GDK_IS_ANDROID_DISPLAY(display), NULL);
    return NULL;
}

static gboolean gdk_android_display_supports_selection_notification(GdkDisplay *display)
{
    return FALSE;
}

static gboolean gdk_android_display_request_selection_notification(GdkDisplay *display,
						                                           GdkAtom     selection)

{
    return FALSE;
}

static gboolean gdk_android_display_supports_clipboard_persistence(GdkDisplay *display)
{
    return FALSE;
}

static void gdk_android_display_store_clipboard(GdkDisplay    *display,
			                                    GdkWindow     *clipboard_window,
			                                    guint32        time_,
			                                    const GdkAtom *targets,
			                                    gint           n_targets)
{
}

static gboolean gdk_android_display_supports_shapes(GdkDisplay *display)
{
    return FALSE;
}

static gboolean gdk_android_display_supports_input_shapes(GdkDisplay *display)
{
    return FALSE;
}

static gboolean gdk_android_display_supports_composite(GdkDisplay *display)
{
  return FALSE;
}

static void gdk_android_display_beep(GdkDisplay *display)
{
    // TODO
}

static void gdk_android_display_flush(GdkDisplay * display)
{
    g_return_if_fail (GDK_IS_ANDROID_DISPLAY(display));

    //g_info("flush called");

    //cairo_gl_surface_swapbuffers(GDK_ANDROID_DISPLAY(display)->cairoSurface);
}


static void gdk_android_display_sync(GdkDisplay * display)
{
    g_return_if_fail(GDK_IS_ANDROID_DISPLAY(display));
}

static void gdk_android_display_dispose(GObject *object)
{
    G_OBJECT_CLASS(gdk_android_display_parent_class)->dispose(object);
}

static void gdk_android_display_finalize(GObject *object)
{
    GdkAndroidDisplay *d = GDK_ANDROID_DISPLAY(object);

    if (d->defaultFont)
        g_free(d->defaultFont);

    if (d->cachePath)
        g_free(d->cachePath);

    G_OBJECT_CLASS(gdk_android_display_parent_class)->finalize(object);
}

static void gdk_android_display_init(GdkAndroidDisplay *d)
{
    d->app = NULL;
    d->eglDisplay = EGL_NO_DISPLAY;
    d->eglContext = EGL_NO_CONTEXT;
    d->eglSurface = EGL_NO_SURFACE;
    d->cairoDevice = NULL;
    d->cairoSurface = NULL;
    d->defaultFont = NULL;
    d->cachePath = NULL;
    d->notificationBarHeight = 25;
}


static void gdk_android_display_before_process_all_updates(GdkDisplay  *display)
{
}

static void gdk_android_display_after_process_all_updates(GdkDisplay  *display)
{
}
static void gdk_android_display_notify_startup_complete(GdkDisplay  *display,
                                                         const gchar *startup_id)
{
}

static void gdk_android_display_event_data_copy(GdkDisplay    *display,
                                                const GdkEvent *src,
                                                GdkEvent       *dst)
{
}

static void gdk_android_display_event_data_free(GdkDisplay *display,
                                                GdkEvent *event)
{
}

static void gdk_android_display_push_error_trap(GdkDisplay *display)
{
}

static gint gdk_android_display_pop_error_trap(GdkDisplay *display,
				                               gboolean    ignored)
{
    return 0;
}

static GList *_gdk_android_display_list_devices(GdkDisplay *dpy)
{
    g_debug("_gdk_android_display_list_devices is called");
    return NULL;
}

static GdkCursor *_gdk_android_display_get_cursor_for_type(GdkDisplay *display,
                                                           GdkCursorType cursor_type)
{
    g_debug("_gdk_android_display_get_cursor_for_type is called");
    return NULL;
}

static GdkCursor *_gdk_android_display_get_cursor_for_name(GdkDisplay *display,
                                                           const gchar *name)
{
    return NULL;
}

static GdkCursor *_gdk_android_display_get_cursor_for_surface(GdkDisplay *display,
                                                              cairo_surface_t  *surface,
                                                              gdouble x,
                                                              gdouble y)
{
    return NULL;
}

static void _gdk_android_display_get_default_cursor_size(GdkDisplay *display,
                                                         guint *width,
                                                         guint *height)
{
    if (width)
        *width = 1;
    if (height)
        *height = 0;
}

#define _gdk_android_display_get_maximal_cursor_size _gdk_android_display_get_default_cursor_size

static gboolean _gdk_android_display_supports_cursor_alpha(GdkDisplay *display)
{
    return FALSE;
}

#define _gdk_android_display_supports_cursor_color _gdk_android_display_supports_cursor_alpha

static GdkWindow *_gdk_android_display_get_selection_owner(GdkDisplay *display,
                                                           GdkAtom selection)
{
    g_debug("_gdk_android_display_get_selection_owner is called");
    return NULL;
}

static gboolean _gdk_android_display_set_selection_owner(GdkDisplay *display,
                                                         GdkWindow  *owner,
                                                         GdkAtom     selection,
                                                         guint32     time,
                                                         gboolean    send_event)
{
    g_debug("_gdk_android_display_set_selection_owner is called");
    return FALSE;
}

static void _gdk_android_display_send_selection_notify(GdkDisplay   *display,
                                                       GdkWindow    *requestor,
                                                       GdkAtom       selection,
                                                       GdkAtom       target,
                                                       GdkAtom       property,
                                                       guint32       time)
{
    g_debug("_gdk_android_display_send_selection_notify is called");
}

static gint _gdk_android_display_get_selection_property(GdkDisplay *display,
                                                        GdkWindow  *requestor,
                                                        guchar    **data,
                                                        GdkAtom    *ret_type,
                                                        gint       *ret_format)
{
    g_debug("_gdk_android_display_send_selection_notify is called");
    return 0;
}

static void _gdk_android_display_convert_selection(GdkDisplay *display,
                                                   GdkWindow *requestor,
                                                   GdkAtom    selection,
                                                   GdkAtom    target,
                                                   guint32    time)
{
    g_debug("_gdk_android_display_convert_selection is called");
}

static gint _gdk_android_display_text_property_to_utf8_list(GdkDisplay    *display,
                                                            GdkAtom        encoding,
                                                            gint           format,
                                                            const guchar  *text,
                                                            gint           length,
                                                            gchar       ***list)
{
    g_debug("_gdk_android_display_text_property_to_utf8_list is called");
    return 0;
}

static gchar *_gdk_android_display_utf8_to_string_target(GdkDisplay  *display,
                                                         const gchar *str)
{
    g_debug("_gdk_android_display_utf8_to_string_target is called");
    return NULL;
}

static void gdk_android_display_class_init (GdkAndroidDisplayClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GdkDisplayClass *display_class = GDK_DISPLAY_CLASS(klass);

    object_class->dispose = gdk_android_display_dispose;
    object_class->finalize = gdk_android_display_finalize;

    display_class->window_type = GDK_TYPE_ANDROID_WINDOW;

    display_class->get_name = gdk_android_display_get_name;
    display_class->get_default_screen = gdk_android_display_get_default_screen;
    display_class->beep = gdk_android_display_beep;
    display_class->sync = gdk_android_display_sync;
    display_class->flush = gdk_android_display_flush;
    display_class->has_pending = _gdk_android_display_has_pending;
    display_class->queue_events = _gdk_android_display_queue_events;
    display_class->get_default_group = gdk_android_display_get_default_group;

    display_class->supports_selection_notification = gdk_android_display_supports_selection_notification;
    display_class->request_selection_notification = gdk_android_display_request_selection_notification;
    display_class->supports_clipboard_persistence = gdk_android_display_supports_clipboard_persistence;
    display_class->store_clipboard = gdk_android_display_store_clipboard;
    display_class->supports_shapes = gdk_android_display_supports_shapes;
    display_class->supports_input_shapes = gdk_android_display_supports_input_shapes;
    display_class->supports_composite = gdk_android_display_supports_composite;

    display_class->list_devices = _gdk_android_display_list_devices;
    display_class->get_cursor_for_type = _gdk_android_display_get_cursor_for_type;
    display_class->get_cursor_for_name = _gdk_android_display_get_cursor_for_name;
    display_class->get_cursor_for_surface = _gdk_android_display_get_cursor_for_surface;
    display_class->get_default_cursor_size = _gdk_android_display_get_default_cursor_size;
    display_class->get_maximal_cursor_size = _gdk_android_display_get_maximal_cursor_size;
    display_class->supports_cursor_alpha = _gdk_android_display_supports_cursor_alpha;
    display_class->supports_cursor_color = _gdk_android_display_supports_cursor_color;

    display_class->before_process_all_updates = gdk_android_display_before_process_all_updates;
    display_class->after_process_all_updates = gdk_android_display_after_process_all_updates;
    display_class->get_next_serial = gdk_android_display_get_next_serial;
    display_class->notify_startup_complete = gdk_android_display_notify_startup_complete;
    display_class->event_data_copy = gdk_android_display_event_data_copy;
    display_class->event_data_free = gdk_android_display_event_data_free;
    display_class->create_window_impl = _gdk_android_display_create_window_impl;

    display_class->get_keymap = _gdk_android_display_get_keymap;
    display_class->push_error_trap = gdk_android_display_push_error_trap;
    display_class->pop_error_trap = gdk_android_display_pop_error_trap;
    display_class->get_selection_owner = _gdk_android_display_get_selection_owner;
    display_class->set_selection_owner = _gdk_android_display_set_selection_owner;
    display_class->send_selection_notify = _gdk_android_display_send_selection_notify;
    display_class->get_selection_property = _gdk_android_display_get_selection_property;
    display_class->convert_selection = _gdk_android_display_convert_selection;
    display_class->text_property_to_utf8_list = _gdk_android_display_text_property_to_utf8_list;
    display_class->utf8_to_string_target = _gdk_android_display_utf8_to_string_target;
}

gboolean gdk_android_init_display(struct android_app *app, GError **error)
{
    const EGLint attribs[] = {
                                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                EGL_BLUE_SIZE, 8,
                                EGL_GREEN_SIZE, 8,
                                EGL_RED_SIZE, 8,
                                EGL_NONE
                             };
    const EGLint ctxAttribs[] = {
                                    // request a context using Open GL ES 2.0
                                    EGL_CONTEXT_CLIENT_VERSION, 2,
                                    EGL_NONE
                                };
    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config;
    GdkAndroidDisplay *d;
    GdkAndroidScreen *s;

    if (_gdk_display)
        return TRUE;

    d = _gdk_display = GDK_ANDROID_DISPLAY(g_object_new(GDK_TYPE_ANDROID_DISPLAY, NULL));
    s = _gdk_screen =  GDK_ANDROID_SCREEN(g_object_new(GDK_TYPE_ANDROID_SCREEN, NULL));
    d->app = app;

    // Initialize EGL
    d->eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(d->eglDisplay, 0, 0);
    eglChooseConfig(d->eglDisplay, attribs, &config, 1, &numConfigs);
    eglGetConfigAttrib(d->eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

    d->eglSurface = eglCreateWindowSurface(d->eglDisplay, config, app->window, NULL);
    d->eglContext = eglCreateContext(d->eglDisplay, config, NULL, ctxAttribs);
    eglSurfaceAttrib(d->eglDisplay, d->eglSurface, EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED);

    if (eglMakeCurrent(d->eglDisplay, d->eglSurface, d->eglSurface, d->eglContext) == EGL_FALSE)
    {
        g_set_error(error, GDK_ANDROID_ERROR, GDK_ANDROID_ERROR_INIT,
                    "Unable to eglMakeCurrent at %s:%u",
                    __FILE__, __LINE__);
        return FALSE;
    }
    eglQuerySurface(d->eglDisplay, d->eglSurface, EGL_WIDTH, &w);
    eglQuerySurface(d->eglDisplay, d->eglSurface, EGL_HEIGHT, &h);

    g_info("%s - %dx%d (Window - %dx%d)", glGetString(GL_VERSION), w, h,
           ANativeWindow_getWidth(app->window), ANativeWindow_getHeight(app->window));

    // Initialize GL state.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    // Initialize Cairo
    d->cairoDevice = cairo_egl_device_create(d->eglDisplay, d->eglContext);
    if (cairo_device_status(d->cairoDevice))
    {
        g_set_error(error, GDK_ANDROID_ERROR, GDK_ANDROID_ERROR_INIT,
                    "Could not create cairo_device: %s (%s:%u)",
                    cairo_status_to_string(cairo_device_status(d->cairoDevice)),
                    __FILE__, __LINE__);
        return FALSE;
    }
    d->cairoSurface = cairo_gl_surface_create_for_egl(d->cairoDevice, d->eglSurface, w, h);
    if (cairo_surface_status(d->cairoSurface))
    {
        g_set_error(error, GDK_ANDROID_ERROR, GDK_ANDROID_ERROR_INIT,
                    "Could not create cairo_surface %s:%u: %s",
                    __FILE__, __LINE__, cairo_status_to_string(cairo_surface_status(d->cairoSurface)));
        return FALSE;
    }

    // screen
    if (!gdk_android_init_screen(s, app->activity, error))
        return FALSE;
    if (!gdk_android_adjust_with_jni(s, d, app->activity, error))
        return FALSE;


    // input
    gdk_android_input_init(&d->display);
    gdk_android_events_init();

    // font
    // the following dpi -> sp conversion formula came from
    // http://developer.android.com/design/style/typography.html
    // TODO: respect user-defined global text scale here
    d->defaultFont = g_strdup_printf("Roboto %d", (int) (14.0 * s->screen.resolution / 160));
    FcConfigSetupAndroidCacheDir(d->cachePath); // d->cachePath is set by gdk_android_adjust_with_jni

    // finalize
    (void) gdk_display_get_name(&d->display); // Precalculate display name
    d->notificationBarHeight = (int) (s->ydpi * 25 / 160);
    g_signal_emit_by_name(d, "opened");

    return TRUE;
}

gboolean gdk_android_term_display()
{
    GdkAndroidDisplay *d = _gdk_display;

    if (d->eglDisplay != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(d->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (d->eglContext != EGL_NO_CONTEXT)
            eglDestroyContext(d->eglDisplay, d->eglContext);
        if (d->eglSurface != EGL_NO_SURFACE)
            eglDestroySurface(d->eglDisplay, d->eglSurface);
        eglTerminate(d->eglDisplay);
    }
    d->eglDisplay = EGL_NO_DISPLAY;
    d->eglContext = EGL_NO_CONTEXT;
    d->eglSurface = EGL_NO_SURFACE;

    return TRUE;
}

const gchar *gdk_android_get_default_font()
{
    return _gdk_display->defaultFont;
}
