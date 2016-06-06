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
// ANativeWindow-based GDK visual and screen definitions

#include "config.h"

#include <jni.h>

#include <android/native_activity.h>
#include <android_native_app_glue.h>

#include "gdk.h"
#include "gdkvisualprivate.h"
#include "gdkprivate-android.h"
#include "gdkandroidscreen.h"
#include "gdkandroiddisplay.h"


#define JNI_ASSERT if (jni->ExceptionCheck(pjni)) \
    { \
        g_set_error(error, GDK_ANDROID_ERROR, GDK_ANDROID_ERROR_INIT, \
                    "JNI exception at %s:%u", __FILE__, __LINE__); \
        return FALSE; \
    }

#define JNI_GET_METHOD(clazz, name, sig) \
    methId = jni->GetMethodID(pjni, clazz, name, sig); \
    JNI_ASSERT


gboolean gdk_android_adjust_with_jni(GdkAndroidScreen *scr, GdkAndroidDisplay *disp,
                                     ANativeActivity *act, GError **error)
{
    JNIEnv *pjni, jni;
    jmethodID methId;
    jobject wm, display, displayMetrics, cacheDir;
    jclass activityClass, windowManagerClass, displayClass, displayMetricsClass, fileClass;
    jfieldID fldId;
    jstring jpath;
    const char* cachePath;

    (*act->vm)->AttachCurrentThread(act->vm, &pjni, NULL);
    jni = (*pjni);

    activityClass = jni->GetObjectClass(pjni, act->clazz); JNI_ASSERT

    // DPI
    JNI_GET_METHOD(activityClass, "getWindowManager", "()Landroid/view/WindowManager;");
    wm = jni->CallObjectMethod(pjni, act->clazz, methId); JNI_ASSERT

    windowManagerClass = jni->FindClass(pjni, "android/view/WindowManager"); JNI_ASSERT

    JNI_GET_METHOD(windowManagerClass, "getDefaultDisplay", "()Landroid/view/Display;");
    display = jni->CallObjectMethod(pjni, wm, methId); JNI_ASSERT

    displayClass = jni->FindClass(pjni, "android/view/Display"); JNI_ASSERT
    displayMetricsClass = jni->FindClass(pjni, "android/util/DisplayMetrics"); JNI_ASSERT

    JNI_GET_METHOD(displayMetricsClass, "<init>", "()V");
    displayMetrics = jni->NewObject(pjni, displayMetricsClass, methId); JNI_ASSERT

    JNI_GET_METHOD(displayClass, "getMetrics", "(Landroid/util/DisplayMetrics;)V");
    jni->CallVoidMethod(pjni, display, methId, displayMetrics); JNI_ASSERT

    fldId = jni->GetFieldID(pjni, displayMetricsClass, "xdpi", "F"); JNI_ASSERT
    scr->xdpi = jni->GetFloatField(pjni, displayMetrics, fldId); JNI_ASSERT
    scr->screen.resolution = scr->xdpi;

    fldId = jni->GetFieldID(pjni, displayMetricsClass, "ydpi", "F"); JNI_ASSERT
    scr->ydpi = jni->GetFloatField(pjni, displayMetrics, fldId); JNI_ASSERT

    g_debug("xdpi = %f, ydpi = %f", scr->xdpi, scr->ydpi);

    // cache DIR
    JNI_GET_METHOD(activityClass, "getCacheDir", "()Ljava/io/File;");
    cacheDir = jni->CallObjectMethod(pjni, act->clazz, methId); JNI_ASSERT
    fileClass = jni->FindClass(pjni, "java/io/File"); JNI_ASSERT
    JNI_GET_METHOD(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    jpath = (jstring) jni->CallObjectMethod(pjni, cacheDir, methId); JNI_ASSERT
    cachePath = jni->GetStringUTFChars(pjni, jpath, NULL);
    disp->cachePath = g_strdup(cachePath);

    g_debug("cachePath = %s", disp->cachePath);

    // Delete objects
    jni->ReleaseStringUTFChars(pjni, jpath, cachePath);
    jni->DeleteLocalRef(pjni, displayMetrics);
    jni->DeleteLocalRef(pjni, display);
    jni->DeleteLocalRef(pjni, wm);
    jni->DeleteLocalRef(pjni, cacheDir);
    (*act->vm)->DetachCurrentThread(act->vm);
    return TRUE;
}
