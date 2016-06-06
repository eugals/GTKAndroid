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
// Various dummy classes/functions to shut the linker up

#include "config.h"

#include <gtk/gtk.h>

#include "gtkmountoperationprivate.h"
#include "gtkmountoperation.h"
#include "gtkprintoperation.h"
#include "gtkprintoperationpreview.h"
#include "gtkprivate.h"


///////////////////////////////////////////////////////////////////////////////////////
// GtkMountOperation

G_DEFINE_TYPE(GtkMountOperation, gtk_mount_operation, G_TYPE_MOUNT_OPERATION)

static void gtk_mount_operation_class_init(GtkMountOperationClass *klass)
{
}

static void gtk_mount_operation_init(GtkMountOperation *obj)
{
}

GMountOperation *gtk_mount_operation_new(GtkWindow *parent)
{
    return g_object_new (GTK_TYPE_MOUNT_OPERATION, "parent", parent, NULL);
}

///////////////////////////////////////////////////////////////////////////////////////
// GtkPrintOperation & GtkPrintOperationPreview

GQuark gtk_print_error_quark(void)
{
    return g_quark_from_static_string ("gtk-print-error-quark");
}

G_DEFINE_TYPE(GtkPrintOperation, gtk_print_operation, GTK_TYPE_PRINT_OPERATION)

static void gtk_print_operation_class_init(GtkPrintOperationClass *klass)
{
}

static void gtk_print_operation_init(GtkPrintOperation *obj)
{
}

GType gtk_print_operation_preview_get_type(void)
{
    static volatile gsize typeId = 0;
    if (g_once_init_enter(&typeId))
    {
        GType tId = g_type_register_static_simple(G_TYPE_INTERFACE,
                                       g_intern_static_string("GtkPrintOperationPreview"),
                                       sizeof(GtkPrintOperationPreviewIface),
                                       (GClassInitFunc) NULL,
                                       0,
                                       (GInstanceInitFunc) NULL,
                                       (GTypeFlags) 0);
        g_once_init_leave (&typeId, tId);
    }
    return typeId;
}
