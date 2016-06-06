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
// The initial implementation was based on gdk's gdkwindow-*.c files code
// which is licensed under LGPLv2.1 terms
//
// ANativeWindow-based GDK window definitions

#include "config.h"
#include <stdlib.h>

#include <android_native_app_glue.h>

#include "gdk.h"
#include "gdkwindowimpl.h"
#include "gdkprivate-android.h"
#include "gdkdeviceprivate.h"
#include "gdkenumtypes.h"
#include "gdkdisplayprivate.h"
#include "gdkvisualprivate.h"
#include "gdkinput-android.h"
#include "gdkwindow-android.h"

static GSList *modal_window_stack = NULL;
static GSList *toplevel_stack = NULL;

static const cairo_user_data_key_t gdk_android_cairo_key;

struct GdkAndroidWindow
{
    GdkWindow parent;
};

struct GdkAndroidWindowClass
{
    GdkWindowClass parent;
};

G_DEFINE_TYPE (GdkAndroidWindow, gdk_android_window, GDK_TYPE_WINDOW)

static void gdk_android_window_init(GdkAndroidWindow *window)
{
}

static void gdk_android_window_finalize(GObject *object)
{
    GdkWindow *window = GDK_WINDOW(object);
    G_OBJECT_CLASS(gdk_android_window_parent_class)->finalize(object);
}

static void gdk_android_window_class_init(GdkAndroidWindowClass *klass)
{
    GObjectClass *objectClass = G_OBJECT_CLASS(klass);
    objectClass->finalize = gdk_android_window_finalize;
}

G_DEFINE_TYPE(GdkWindowImplAndroid, gdk_window_impl_android, GDK_TYPE_WINDOW_IMPL)

static void gdk_window_impl_android_init(GdkWindowImplAndroid *impl)
{
    impl->inside_end_paint = FALSE;
}

static void gdk_window_impl_android_finalize(GObject *object)
{
    G_OBJECT_CLASS(gdk_window_impl_android_parent_class)->finalize(object);
}


GdkWindow *_gdk_windowing_window_init(GdkScreen *screen)
{
    GdkWindow *window;
    GdkWindowImplAndroid *impl;

    window = _gdk_display_create_window(&_gdk_display->display);

    window->impl = g_object_new(GDK_TYPE_WINDOW_IMPL_ANDROID, NULL);
    impl = GDK_WINDOW_IMPL_ANDROID(window->impl);
    impl->wrapper = window;

    window->impl_window = window;
    window->visual = gdk_screen_get_system_visual(screen);

    window->window_type = GDK_WINDOW_ROOT;
    window->depth = window->visual->depth;
    window->x = window->y = 0;
    window->abs_x = window->abs_y = 0;
    window->width  = gdk_screen_get_width(screen);
    window->height = gdk_screen_get_height(screen);
    window->viewable = TRUE;

    return window;
}

static const gchar *get_default_title(void)
{
    const char *title = g_get_application_name();
    if (!title)
        title = g_get_prgname();

    return title;
}

void _gdk_android_display_create_window_impl(GdkDisplay    *display,
                                             GdkWindow     *window,
                                             GdkWindow     *real_parent,
                                             GdkScreen     *screen,
                                             GdkEventMask   event_mask,
                                             GdkWindowAttr *attributes,
                                             gint           attributes_mask)
{
    GdkWindowImplAndroid *impl;
    const gchar *title;
    gint window_width, window_height;

    impl = g_object_new(GDK_TYPE_WINDOW_IMPL_ANDROID, NULL);
    impl->wrapper = GDK_WINDOW(window);
    window->impl = GDK_WINDOW_IMPL(impl);

    toplevel_stack = g_slist_prepend(toplevel_stack, window);

    impl->override_redirect = (attributes_mask & GDK_WA_NOREDIR) != 0
                            && !!attributes->override_redirect;

    if (attributes_mask & GDK_WA_TITLE)
        title = attributes->title;
    else
        title = get_default_title();

    if (!title || !*title)
        title = "";

    impl->native_event_mask = GDK_STRUCTURE_MASK | event_mask;

    if (attributes_mask & GDK_WA_TYPE_HINT)
        gdk_window_set_type_hint(window, attributes->type_hint);

    GDK_NOTE(MISC_OR_EVENTS, gdk_window_set_title(window, title));

    if(attributes_mask & GDK_WA_CURSOR)
        gdk_window_set_cursor (window, attributes->cursor);

    window->x = 0;
    window->y = _gdk_display->notificationBarHeight;


}

static void gdk_android_window_destroy(GdkWindow *window,
                                       gboolean   recursing,
                                       gboolean   foreign_destroy)
{
    GdkWindowImplAndroid *window_impl = GDK_WINDOW_IMPL_ANDROID(window->impl);
    GSList *tmp;

    g_return_if_fail(GDK_IS_WINDOW(window));
  
    GDK_NOTE(MISC, g_debug("gdk_android_window_destroy\n"));

    // Remove ourself from the toplevel stack
    if (toplevel_stack)
    {
        tmp = g_slist_find(toplevel_stack, window);
        if (tmp != NULL)
            toplevel_stack = g_slist_delete_link(toplevel_stack, tmp);
    }

    // Remove ourself from the modal stack
    _gdk_remove_modal_window(window);

    if (!recursing && !foreign_destroy)
    {
        window->destroyed = TRUE;
    }
}

static gboolean gdk_android_window_resize_cairo_surface(GdkWindow       *window,
                                                        cairo_surface_t *surface,
                                                        gint             width,
                                                        gint             height)
{
    return FALSE;
}

static void gdk_android_window_destroy_foreign(GdkWindow *window)
{
    g_warning("gdk_android_window_destroy_foreign is not supposed to be called on Android!");
    gdk_window_hide (window);
    gdk_window_reparent(window, NULL, 0, 0);
}

GdkWindow *_gdk_android_find_window_for_screen_pos(gdouble *x, gdouble *y)
{
    GSList *l;

    for (l = toplevel_stack; l != NULL; l = l->next)
    {
        GdkWindow *w = l->data;

        if (   *x >= w->x && *x <= (w->x + w->width)
            && *y >= w->y && *y <= (w->y + w->height))
        {
            gdouble cx = *x - w->x, cy = *y - w->y;
            GList *cl = gdk_window_peek_children(w);
            for (; cl != NULL; cl = cl->next)
            {
                GdkWindow *cw = cl->data;
                if (   cx >= cw->x && cx <= (cw->x + cw->width)
                    && cy >= cw->y && cy <= (cw->y + cw->height))
                {
                    w = cw;
                    cx -= cw->x;
                    cy -= cw->y;
                    return cw;
                }
            }
            *x -= cx;
            *y -= cy;
            GDK_NOTE(EVENTS, g_debug("found window for event %p @ (%d, %d)", w, cx, cy));
            return w;
        }
    }

    return NULL;
}


static void gdk_android_window_destroy_notify(GdkWindow *window)
{
    g_return_if_fail(GDK_IS_WINDOW(window));

    if (!GDK_WINDOW_DESTROYED(window))
    {
        _gdk_window_destroy(window, TRUE);
    }
  
    g_object_unref(window);
}

static void gdk_android_window_show(GdkWindow *window, gboolean already_mapped)
{
    GdkEvent *event = NULL;
    GdkWindowImplAndroid *impl;
    GdkDeviceManagerAndroid *devManager;
    GdkDisplay *display;
    GList *node;

    if (GDK_WINDOW_DESTROYED(window))
        return;

    impl = GDK_WINDOW_IMPL_ANDROID(window->impl);
    display = &_gdk_display->display;
    devManager = GDK_DEVICE_MANAGER_ANDROID(gdk_display_get_device_manager(display));

    // don't know if this does anything meaning. just copying these 3 lines from wayland
    //_gdk_make_event(window, GDK_MAP, NULL, FALSE);
    //event = _gdk_make_event(window, GDK_VISIBILITY_NOTIFY, NULL, FALSE);
    //event->visibility.state = GDK_VISIBILITY_UNOBSCURED;

    // GDK_ENTER_NOTIFY
    event = gdk_event_new(GDK_ENTER_NOTIFY);
    event->crossing.window = g_object_ref(window);
    gdk_event_set_device(event, devManager->pointer);
    event->crossing.subwindow = NULL;
    event->crossing.time = (guint32) (g_get_monotonic_time () / 1000);
    event->crossing.mode = GDK_CROSSING_NORMAL;
    event->crossing.detail = GDK_NOTIFY_ANCESTOR;
    event->crossing.focus = TRUE;
    event->crossing.state = 0;

    node = _gdk_event_queue_append(display, event);
    _gdk_windowing_got_event(display, node, event, _gdk_display_get_next_serial(display));
}

static void gdk_android_window_hide(GdkWindow *window)
{
    if (GDK_WINDOW_DESTROYED(window))
        return;

    g_debug("gdk_android_window_hide is called");
}

static void gdk_android_window_withdraw(GdkWindow *window)
{
    if (GDK_WINDOW_DESTROYED(window))
        return;

    gdk_window_hide (window);
}

static void gdk_android_window_move(GdkWindow *window, gint x, gint y)
{
    g_return_if_fail(GDK_IS_WINDOW(window));

    if (GDK_WINDOW_DESTROYED(window))
        return;

    window->x = x;
    window->y = y;
}

static void gdk_android_window_resize(GdkWindow *window, gint width, gint height)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    window->width = width;
    window->height = height;
}

static void gdk_android_window_move_resize(GdkWindow *window,
                                           gboolean   with_move,
                                           gint       x,
                                           gint       y,
                                           gint       width,
                                           gint       height)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    if (with_move)
    {
        window->x = x;
        window->y = y;
    }
    window->width = width;
    window->height = height;
}

static gboolean gdk_android_window_reparent(GdkWindow *window,
                                            GdkWindow *parent,
                                            gint       x,
                                            gint       y)
{
    GdkWindowImplAndroid *impl;
    GdkWindow *old_parent;

    g_return_val_if_fail(GDK_IS_WINDOW(window), FALSE);
    if (GDK_WINDOW_DESTROYED(window))
        return FALSE;

    old_parent = window->parent;
    if (!parent)
        parent = _gdk_android_root();
    window->parent = parent;

    impl = GDK_WINDOW_IMPL_ANDROID(window->impl);

    if (old_parent)
        old_parent->children = g_list_remove(old_parent->children, window);

    parent->children = g_list_prepend(parent->children, window);

    return FALSE;
}

static void gdk_android_window_raise(GdkWindow *window)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    // g_debug("gdk_android_window_raise is called");
}

static void gdk_android_window_lower(GdkWindow *window)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    // g_debug("gdk_android_window_lower is called");
}

static void gdk_android_window_set_urgency_hint(GdkWindow *window,
                                                gboolean   urgent)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    g_debug("gdk_android_window_set_urgency_hint is called");
}

static gboolean get_effective_window_decorations(GdkWindow       *window,
                                                 GdkWMDecoration *decoration)
{
    g_debug("get_effective_window_decorations is called");

    return FALSE;
}

static void gdk_android_window_set_geometry_hints(GdkWindow         *window,
                                                  const GdkGeometry *geometry,
                                                  GdkWindowHints     geom_mask)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    //g_debug("gdk_android_window_set_geometry_hints is called");
}

static void gdk_android_window_set_title(GdkWindow *window, const gchar *title)
{
    g_debug("gdk_android_window_set_title is called");
    if (title)
        GDK_NOTE(MISC_OR_EVENTS, g_free((char *) title));
}

static void gdk_android_window_set_role(GdkWindow *window, const gchar *role)
{
    g_debug("gdk_android_window_set_role is called");
}

static void gdk_android_window_set_transient_for(GdkWindow *window, GdkWindow *parent)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    g_debug("gdk_android_window_set_transient_for is called");
}

void _gdk_push_modal_window(GdkWindow *window)
{
    modal_window_stack = g_slist_prepend(modal_window_stack, window);
}

void _gdk_remove_modal_window(GdkWindow *window)
{
    GSList *tmp;

    g_return_if_fail (window != NULL);

    if (modal_window_stack == NULL)
        return;

    tmp = g_slist_find(modal_window_stack, window);
    if (tmp != NULL)
    {
        modal_window_stack = g_slist_delete_link (modal_window_stack, tmp);
    }
}

gboolean _gdk_modal_blocked(GdkWindow *window)
{
    GSList *l;
    gboolean found_any = FALSE;

    for (l = modal_window_stack; l != NULL; l = l->next)
    {
        GdkWindow *modal = l->data;

        if (modal == window)
            return FALSE;

        if (GDK_WINDOW_IS_MAPPED(modal))
            found_any = TRUE;
    }

    return found_any;
}

GdkWindow *_gdk_modal_current(void)
{
    GSList *l;

    for (l = modal_window_stack; l != NULL; l = l->next)
    {
        GdkWindow *modal = l->data;

        if (GDK_WINDOW_IS_MAPPED (modal))
            return modal;
    }

    return NULL;
}

static void gdk_android_window_set_background(GdkWindow *window, cairo_pattern_t *pattern)
{
}

static void gdk_android_window_set_device_cursor(GdkWindow *window,
                                                 GdkDevice *device,
                                                 GdkCursor *cursor)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    //g_debug("gdk_android_window_set_device_cursor is called");
}

static void gdk_android_window_get_geometry(GdkWindow *window,
                                            gint      *x,
                                            gint      *y,
                                            gint      *width,
                                            gint      *height)
{
    if (!window)
        window = _gdk_android_root();

    *x = window->x;
    *y = window->y;
    *width = window->width;
    *height = window->height;
}

static void gdk_android_window_get_root_coords(GdkWindow *window,
                                               gint       x,
                                               gint       y,
                                               gint      *root_x,
                                               gint      *root_y)
{
    *root_x = 0;
    *root_y = 0;
}

static void gdk_android_window_restack_under(GdkWindow *window, GList *native_siblings)
{
}

static void gdk_android_window_restack_toplevel(GdkWindow *window, GdkWindow *sibling,
                                                gboolean   above)
{
}

static void gdk_android_window_get_frame_extents(GdkWindow *window, GdkRectangle *rect)
{
    g_return_if_fail (GDK_IS_WINDOW(window));
    g_return_if_fail (rect != NULL);

    rect->x = 0;
    rect->y = 0;
    rect->width = 1;
    rect->height = 1;

    g_debug("gdk_android_window_get_frame_extents is called");
}

static gboolean gdk_window_android_get_device_state(GdkWindow       *window,
                                                    GdkDevice       *device,
                                                    gdouble         *x,
                                                    gdouble         *y,
                                                    GdkModifierType *mask)
{
    GdkWindow *child;

    g_return_val_if_fail(window == NULL || GDK_IS_WINDOW (window), FALSE);

    GDK_DEVICE_GET_CLASS(device)->query_state(device, window, NULL, &child,
                                              NULL, NULL, x, y, mask);
    return (child != NULL);
}

static GdkEventMask gdk_android_window_get_events(GdkWindow *window)
{
    GdkWindowImplAndroid *impl;

    if (GDK_WINDOW_DESTROYED(window))
        return 0;

    impl = GDK_WINDOW_IMPL_ANDROID(window->impl);

    return impl->native_event_mask;
}

static void gdk_android_window_set_events(GdkWindow *window, GdkEventMask event_mask)
{
    GdkWindowImplAndroid *impl;

    impl = GDK_WINDOW_IMPL_ANDROID(window->impl);

    impl->native_event_mask = GDK_STRUCTURE_MASK | event_mask;
}

static void gdk_android_window_set_override_redirect(GdkWindow *window, gboolean override_redirect)
{
    g_return_if_fail(GDK_IS_WINDOW(window));

    GDK_WINDOW_IMPL_ANDROID(window->impl)->override_redirect = !!override_redirect;
}

static void gdk_android_window_set_accept_focus(GdkWindow *window, gboolean accept_focus)
{
    g_return_if_fail (GDK_IS_WINDOW(window));

    accept_focus = accept_focus != FALSE;

    if (window->accept_focus != accept_focus)
        window->accept_focus = accept_focus;
}

static void gdk_android_window_set_focus_on_map (GdkWindow *window, gboolean focus_on_map)
{
    g_return_if_fail(GDK_IS_WINDOW(window));

    focus_on_map = focus_on_map != FALSE;

    if (window->focus_on_map != focus_on_map)
        window->focus_on_map = focus_on_map;
}

static void gdk_android_window_set_icon_list(GdkWindow *window, GList *pixbufs)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    //g_warning("gdk_android_window_set_icon_list is called");
}

static void gdk_android_window_set_icon_name(GdkWindow *window, const gchar *name)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    g_warning("gdk_android_window_set_icon_name is called");
}

static GdkWindow *gdk_android_window_get_group(GdkWindow *window)
{
    g_return_val_if_fail(GDK_IS_WINDOW (window), NULL);
    g_return_val_if_fail(GDK_WINDOW_TYPE(window) != GDK_WINDOW_CHILD, NULL);

    if (GDK_WINDOW_DESTROYED(window))
        return NULL;
  
    g_warning("gdk_window_get_group not is called");

    return NULL;
}

static void gdk_android_window_set_group(GdkWindow *window, GdkWindow *leader)
{
    g_return_if_fail(GDK_IS_WINDOW (window));
    g_return_if_fail(GDK_WINDOW_TYPE (window) != GDK_WINDOW_CHILD);
    g_return_if_fail(leader == NULL || GDK_IS_WINDOW (leader));

    if (GDK_WINDOW_DESTROYED (window) || GDK_WINDOW_DESTROYED (leader))
        return;
  
    g_warning("gdk_window_set_group is called");
}

static GQuark get_functions_quark()
{
    static GQuark quark = 0;

    if (!quark)
        quark = g_quark_from_static_string ("gdk-window-functions");

    return quark;
}

static void update_system_menu(GdkWindow *window)
{
    GdkWMFunction* functions_set;

    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    functions_set = g_object_get_qdata(G_OBJECT(window), get_functions_quark());

    g_warning("update_system_menu is called");
}

static GQuark get_decorations_quark()
{
    static GQuark quark = 0;

    if (!quark)
        quark = g_quark_from_static_string("gdk-window-decorations");

    return quark;
}

static void gdk_android_window_set_decorations(GdkWindow *window, GdkWMDecoration decorations)
{
    GdkWMDecoration* decorations_copy;

    g_return_if_fail(GDK_IS_WINDOW(window));

    decorations_copy = g_malloc (sizeof (GdkWMDecoration));
    *decorations_copy = decorations;
    g_object_set_qdata_full(G_OBJECT(window), get_decorations_quark(), decorations_copy, g_free);
}

static gboolean gdk_android_window_get_decorations(GdkWindow       *window,
                                                   GdkWMDecoration *decorations)
{
    GdkWMDecoration* decorations_set;
  
    g_return_val_if_fail(GDK_IS_WINDOW(window), FALSE);

    decorations_set = g_object_get_qdata(G_OBJECT(window), get_decorations_quark());
    if (decorations_set)
        *decorations = *decorations_set;

    return (decorations_set != NULL);
}

static void gdk_android_window_set_functions (GdkWindow *window, GdkWMFunction functions)
{
    GdkWMFunction* functions_copy;

    g_return_if_fail(GDK_IS_WINDOW(window));
  
    functions_copy = g_malloc (sizeof (GdkWMFunction));
    *functions_copy = functions;
    g_object_set_qdata_full(G_OBJECT (window), get_functions_quark (), functions_copy, g_free);

    update_system_menu(window);
}


static gboolean gdk_android_window_set_static_gravities(GdkWindow *window, gboolean use_static)
{
    g_return_val_if_fail(GDK_IS_WINDOW(window), FALSE);

    return !use_static;
}

static void gdk_android_window_begin_resize_drag(GdkWindow     *window,
                                                 GdkWindowEdge  edge,
                                                 GdkDevice     *device,
                                                 gint           button,
                                                 gint           root_x,
                                                 gint           root_y,
                                                 guint32        timestamp)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    g_debug("unsupported gdk_android_window_begin_resize_drag is called");
}

static void gdk_android_window_begin_move_drag(GdkWindow *window,
                                               GdkDevice *device,
                                               gint       button,
                                               gint       root_x,
                                               gint       root_y,
                                               guint32    timestamp)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    g_debug("unsupported gdk_android_window_begin_move_drag is called");
}


static void gdk_android_window_iconify(GdkWindow *window)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    g_warning("unsupported gdk_android_window_iconify is called");
}

static void gdk_android_window_deiconify(GdkWindow *window)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    //g_warning("unsupported gdk_android_window_deiconify is called");
}

static void gdk_android_window_stick(GdkWindow *window)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    // g_debug("unsupported gdk_android_window_stick is called");
}

static void gdk_android_window_unstick(GdkWindow *window)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    // g_debug("unsupported gdk_android_window_unstick is called");
}

static void gdk_android_window_maximize(GdkWindow *window)
{
    ARect *arect;

    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    // arect contains the Java application content rectangle which respects: 
    // notification area, system button bar, and a default window header.
    // Our window wouldn't have any header, so we ignore arect->top and 
    // only use are arect->bottom to find out where the system button bar begins,
    // because there is not way to get such information from GL context
    arect = &_gdk_display->app->contentRect;

    gdk_window_move_resize(window, 
                           arect->left, 
                           _gdk_display->notificationBarHeight,
                           arect->right - arect->left, 
                           arect->bottom - _gdk_display->notificationBarHeight);
}

static void gdk_android_window_unmaximize(GdkWindow *window)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    // g_debug("unsupported gdk_android_window_unmaximize is called");
}

static void gdk_android_window_fullscreen(GdkWindow *window)
{
    gdk_android_window_maximize(window);

}

static void gdk_android_window_unfullscreen(GdkWindow *window)
{
    gdk_android_window_unmaximize(window);

    gdk_synthesize_window_state(window, GDK_WINDOW_STATE_FULLSCREEN, 0);
}

static void gdk_android_window_set_keep_above(GdkWindow *window, gboolean setting)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    //g_debug("unsupported gdk_android_window_set_keep_above is called");
}

static void gdk_android_window_set_keep_below(GdkWindow *window, gboolean setting)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    //g_debug("unsupported gdk_android_window_set_keep_below is called");
}

static void gdk_android_window_focus(GdkWindow *window, guint32 timestamp)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    //g_debug("unsupported gdk_android_window_focus is called");
}

static void gdk_android_window_set_modal_hint(GdkWindow *window, gboolean modal)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
  
    if (GDK_WINDOW_DESTROYED(window))
        return;

    if (modal == window->modal_hint)
        return;

    window->modal_hint = modal;

    if (modal)
    {
        _gdk_push_modal_window(window);
        gdk_window_raise(window);
    }
    else
    {
        _gdk_remove_modal_window(window);
    }
}

static void gdk_android_window_set_skip_taskbar_hint(GdkWindow *window, gboolean skips_taskbar)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    g_debug("unsupported gdk_android_window_set_skip_taskbar_hint is called");
}

static void gdk_android_window_set_skip_pager_hint(GdkWindow *window, gboolean skips_pager)
{
    g_return_if_fail(GDK_IS_WINDOW(window));

    g_debug("unsupported gdk_android_window_set_skip_pager_hint is called");
}

static void gdk_android_window_set_type_hint(GdkWindow *window, GdkWindowTypeHint hint)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    GDK_WINDOW_IMPL_ANDROID(window->impl)->type_hint = hint;
}

static GdkWindowTypeHint gdk_android_window_get_type_hint(GdkWindow *window)
{
    g_return_val_if_fail(GDK_IS_WINDOW(window), GDK_WINDOW_TYPE_HINT_NORMAL);
  
    if (GDK_WINDOW_DESTROYED (window))
        return GDK_WINDOW_TYPE_HINT_NORMAL;

    return GDK_WINDOW_IMPL_ANDROID(window->impl)->type_hint;
}

static void gdk_android_window_shape_combine_region(GdkWindow       *window,
                                                    const cairo_region_t *shape_region,
                                                    gint             offset_x,
                                                    gint             offset_y)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    g_debug("unsupported gdk_android_window_shape_combine_region is called");
}

static void gdk_android_window_set_opacity(GdkWindow *window, gdouble opacity)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    if (opacity != 1.0)
        g_warning("gdk_android_window_set_opacity: unsupported opacity value %g", opacity);
}

static cairo_region_t *gdk_android_window_get_shape(GdkWindow *window)
{
    g_return_val_if_fail(GDK_IS_WINDOW(window), NULL);
    if (GDK_WINDOW_DESTROYED(window))
        return NULL;

    cairo_rectangle_int_t rect = {
                                    window->x,
                                    window->y,
                                    window->width,
                                    window->height
                                 };

    return cairo_region_create_rectangle(&rect);
}

static gboolean _gdk_android_window_queue_antiexpose(GdkWindow *window, cairo_region_t *area)
{
    return FALSE;
}

static void gdk_android_input_shape_combine_region(GdkWindow *window,
                                                   const cairo_region_t *shape_region,
                                                   gint offset_x,
                                                   gint offset_y)
{
    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    g_debug("unsupported gdk_android_input_shape_combine_region is called");
}

static void gdk_android_window_process_updates_recurse(GdkWindow *window, cairo_region_t *region)
{
    _gdk_window_process_updates_recurse(window, region);
}

static void gdk_android_cairo_surface_destroy(void *data)
{
    GdkWindowImplAndroid *impl = data;

    if (impl->inside_end_paint)
    {
        cairo_gl_surface_swapbuffers(_gdk_display->cairoSurface);
        impl->inside_end_paint = FALSE;
    }

    impl->cairo_surface = NULL;
}

static cairo_surface_t *gdk_android_ref_cairo_surface(GdkWindow *window)
{
    GdkWindowImplAndroid *impl;

    g_return_val_if_fail(GDK_IS_WINDOW(window), NULL);
    if (GDK_WINDOW_DESTROYED(window))
        return NULL;

    impl = GDK_WINDOW_IMPL_ANDROID(window->impl);

    if (impl->cairo_surface)
        cairo_surface_reference(impl->cairo_surface);
    else
    {
        impl->cairo_surface =
            cairo_surface_create_for_rectangle(_gdk_display->cairoSurface, window->x, window->y,
                                               window->width, window->height);

        cairo_surface_set_user_data(impl->cairo_surface, &gdk_android_cairo_key,
                                    impl, gdk_android_cairo_surface_destroy);
    }

//    {
//        cairo_t *cr;
//
//        cr = cairo_create(_gdk_display->cairoSurface); // impl->cairo_surface);
//        if (cairo_status(cr))
//        {
//            g_error("Could not create cairo object %s:%u: %s",
//                    __FILE__, __LINE__, cairo_status_to_string(cairo_status(cr)));
//            return FALSE;
//        }
//        //eglMakeCurrent(display, surface, surface, context);
//
//        cairo_set_line_width(cr, 9);
//        cairo_set_source_rgb(cr, 0.69, 0.69, 0);
//
//        cairo_translate(cr, 250, 250);
//        cairo_arc(cr, 0, 0, 250, 0, 2 * 3.1415927);
//        cairo_stroke_preserve(cr);
//
//        cairo_set_source_rgb(cr, 0.9, 0.4, 0.3);
//        cairo_fill(cr);
//        cairo_restore(cr);
//
//        cairo_destroy(cr);
//
//        g_debug("some test drawn!");
//    }

    return impl->cairo_surface;
}


static void gdk_window_impl_android_end_paint(GdkWindow *window)
{
    GdkWindowImplAndroid *impl;

    g_return_if_fail(GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED(window))
        return;

    impl = GDK_WINDOW_IMPL_ANDROID(window->impl);
    // this would be used later in gdk_android_cairo_surface_destroy
    // to avoid some unnecessary buffer swapping
    impl->inside_end_paint = TRUE;
}

static GdkDragProtocol _gdk_android_window_get_drag_protocol(GdkWindow *window, GdkWindow **target)
{
    GdkDragProtocol protocol = GDK_DRAG_PROTO_NONE;
    //protocol = GDK_DRAG_PROTO_LOCAL;

    if (target)
        *target = NULL;

    return protocol;
}

void _gdk_android_window_register_dnd(GdkWindow *window)
{
}

GdkDragContext *_gdk_android_window_drag_begin(GdkWindow *window,
                                               GdkDevice *device,
                                               GList     *targets)
{
    return NULL;
}

gint _gdk_android_window_get_property(GdkWindow   *window,
                                      GdkAtom      property,
                                      GdkAtom      type,
                                      gulong       offset,
                                      gulong       length,
                                      gint         pdelete,
                                      GdkAtom     *actual_property_type,
                                      gint        *actual_format_type,
                                      gint        *actual_length,
                                      guchar     **data)
{
    g_return_val_if_fail (GDK_IS_WINDOW(window), FALSE);

    if (GDK_WINDOW_DESTROYED (window))
        return FALSE;

    g_debug("unsupported _gdk_android_window_get_property is called");

    return FALSE;
}

void _gdk_android_window_change_property(GdkWindow    *window,
                                         GdkAtom       property,
                                         GdkAtom       type,
                                         gint          format,
                                         GdkPropMode   mode,
                                         const guchar *data,
                                         gint          nelements)
{
    g_return_if_fail (GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED (window))
        return;

    g_debug("unsupported _gdk_android_window_change_property is called");
}

void _gdk_android_window_delete_property(GdkWindow *window, GdkAtom property)
{
    g_return_if_fail (GDK_IS_WINDOW(window));
    if (GDK_WINDOW_DESTROYED (window))
        return;

    g_debug("unsupported _gdk_android_window_delete_property is called");
}

static void gdk_window_impl_android_class_init(GdkWindowImplAndroidClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GdkWindowImplClass *impl_class = GDK_WINDOW_IMPL_CLASS(klass);

  //parent_class = g_type_class_peek_parent(klass);

  object_class->finalize = gdk_window_impl_android_finalize;
  
  impl_class->ref_cairo_surface = gdk_android_ref_cairo_surface;

  impl_class->show = gdk_android_window_show;
  impl_class->hide = gdk_android_window_hide;
  impl_class->withdraw = gdk_android_window_withdraw;
  impl_class->set_events = gdk_android_window_set_events;
  impl_class->get_events = gdk_android_window_get_events;
  impl_class->raise = gdk_android_window_raise;
  impl_class->lower = gdk_android_window_lower;
  impl_class->restack_under = gdk_android_window_restack_under;
  impl_class->restack_toplevel = gdk_android_window_restack_toplevel;
  impl_class->move_resize = gdk_android_window_move_resize;
  impl_class->set_background = gdk_android_window_set_background;
  impl_class->reparent = gdk_android_window_reparent;
  impl_class->set_device_cursor = gdk_android_window_set_device_cursor;
  impl_class->get_geometry = gdk_android_window_get_geometry;
  impl_class->get_device_state = gdk_window_android_get_device_state;
  impl_class->get_root_coords = gdk_android_window_get_root_coords;

  impl_class->shape_combine_region = gdk_android_window_shape_combine_region;
  impl_class->input_shape_combine_region = gdk_android_input_shape_combine_region;
  impl_class->set_static_gravities = gdk_android_window_set_static_gravities;
  impl_class->queue_antiexpose = _gdk_android_window_queue_antiexpose;
  impl_class->destroy = gdk_android_window_destroy;
  impl_class->destroy_foreign = gdk_android_window_destroy_foreign;
  impl_class->resize_cairo_surface = gdk_android_window_resize_cairo_surface;
  impl_class->get_shape = gdk_android_window_get_shape;
  impl_class->end_paint = gdk_window_impl_android_end_paint;

  impl_class->focus = gdk_android_window_focus;
  impl_class->set_type_hint = gdk_android_window_set_type_hint;
  impl_class->get_type_hint = gdk_android_window_get_type_hint;
  impl_class->set_modal_hint = gdk_android_window_set_modal_hint;
  impl_class->set_skip_taskbar_hint = gdk_android_window_set_skip_taskbar_hint;
  impl_class->set_skip_pager_hint = gdk_android_window_set_skip_pager_hint;
  impl_class->set_urgency_hint = gdk_android_window_set_urgency_hint;
  impl_class->set_geometry_hints = gdk_android_window_set_geometry_hints;
  impl_class->set_title = gdk_android_window_set_title;
  impl_class->set_role = gdk_android_window_set_role;
  impl_class->set_transient_for = gdk_android_window_set_transient_for;
  impl_class->get_frame_extents = gdk_android_window_get_frame_extents;
  impl_class->set_override_redirect = gdk_android_window_set_override_redirect;
  impl_class->set_accept_focus = gdk_android_window_set_accept_focus;
  impl_class->set_focus_on_map = gdk_android_window_set_focus_on_map;
  impl_class->set_icon_list = gdk_android_window_set_icon_list;
  impl_class->set_icon_name = gdk_android_window_set_icon_name;
  impl_class->iconify = gdk_android_window_iconify;
  impl_class->deiconify = gdk_android_window_deiconify;
  impl_class->stick = gdk_android_window_stick;
  impl_class->unstick = gdk_android_window_unstick;
  impl_class->maximize = gdk_android_window_maximize;
  impl_class->unmaximize = gdk_android_window_unmaximize;
  impl_class->fullscreen = gdk_android_window_fullscreen;
  impl_class->unfullscreen = gdk_android_window_unfullscreen;
  impl_class->set_keep_above = gdk_android_window_set_keep_above;
  impl_class->set_keep_below = gdk_android_window_set_keep_below;
  impl_class->get_group = gdk_android_window_get_group;
  impl_class->set_group = gdk_android_window_set_group;
  impl_class->set_decorations = gdk_android_window_set_decorations;
  impl_class->get_decorations = gdk_android_window_get_decorations;
  impl_class->set_functions = gdk_android_window_set_functions;

  impl_class->begin_resize_drag = gdk_android_window_begin_resize_drag;
  impl_class->begin_move_drag = gdk_android_window_begin_move_drag;
  impl_class->set_opacity = gdk_android_window_set_opacity;
  impl_class->destroy_notify = gdk_android_window_destroy_notify;
  impl_class->get_drag_protocol = _gdk_android_window_get_drag_protocol;
  impl_class->register_dnd = _gdk_android_window_register_dnd;
  impl_class->drag_begin = _gdk_android_window_drag_begin;
  impl_class->process_updates_recurse = gdk_android_window_process_updates_recurse;
  impl_class->simulate_key = _gdk_android_window_simulate_key;
  impl_class->simulate_button = _gdk_android_window_simulate_button;
  impl_class->get_property = _gdk_android_window_get_property;
  impl_class->change_property = _gdk_android_window_change_property;
  impl_class->delete_property = _gdk_android_window_delete_property;
}
