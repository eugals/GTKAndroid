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
// Core android initialization functions

#ifndef __GTK_ANDROID_H__
#define __GTK_ANDROID_H__

#include <gtk/gtk.h>

// Prepares basic infrastructure needed by gtk+ applications.
// Call this function at the beginning of your android_main
// instead of *gtk_init* which would be used on other platforms
void gtk_android_init(struct android_app *state);

// Finished the activity and makes sure it is properly unloaded
void gtk_android_exit(struct android_app *state);

#endif // __GTK_ANDROID_H__