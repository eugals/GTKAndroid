/*
 * fontconfig/src/fcinit.c
 *
 * Copyright © 2001 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the author(s) not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The authors make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE AUTHOR(S) DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "src/fcint.h"
#include <stdlib.h>

#if defined(FC_ATOMIC_INT_NIL)
#pragma message("Could not find any system to define atomic_int macros, library may NOT be thread-safe.")
#endif
#if defined(FC_MUTEX_IMPL_NIL)
#pragma message("Could not find any system to define mutex macros, library may NOT be thread-safe.")
#endif
#if defined(FC_ATOMIC_INT_NIL) || defined(FC_MUTEX_IMPL_NIL)
#pragma message("To suppress these warnings, define FC_NO_MT.")
#endif

static const char *androidAppCacheDir = NULL;

FcPublic void FcConfigSetupAndroidCacheDir(const char *dir)
{
    androidAppCacheDir = dir;
}

static FcConfig *
FcInitFallbackConfig (void)
{
    FcConfig    *config;

    config = FcConfigCreate ();
    if (!config)
        goto bail0;
    if (!FcConfigAddDir (config, (FcChar8 *) FC_DEFAULT_FONTS))
        goto bail1;
    if (androidAppCacheDir && !FcConfigAddCacheDir (config, (FcChar8 *) androidAppCacheDir))
        goto bail1;
    return config;

bail1:
    FcConfigDestroy (config);
bail0:
    return 0;
}

int
FcGetVersion (void)
{
    return FC_VERSION;
}

/*
 * Load the configuration files
 */
FcConfig *
FcInitLoadOwnConfig (FcConfig *config)
{
    return FcInitFallbackConfig ();
}

FcConfig *
FcInitLoadConfig (void)
{
    return FcInitLoadOwnConfig (NULL);
}

/*
 * Load the configuration files and scan for available fonts
 */
FcConfig *
FcInitLoadOwnConfigAndFonts (FcConfig *config)
{
    config = FcInitLoadOwnConfig (config);
    if (!config)
        return 0;
    if (!FcConfigBuildFonts (config))
    {
        FcConfigDestroy (config);
        return 0;
    }
    return config;
}

FcConfig *
FcInitLoadConfigAndFonts (void)
{
    return FcInitLoadOwnConfigAndFonts (NULL);
}

/*
 * Initialize the default library configuration
 */
FcBool
FcInit (void)
{
    return FcConfigInit ();
}

/*
 * Free all library-allocated data structures.
 */
void
FcFini (void)
{
    FcConfigFini ();
    FcCacheFini ();
    FcDefaultFini ();
}

/*
 * Reread the configuration and available font lists
 */
FcBool
FcInitReinitialize (void)
{
    FcConfig    *config;

    config = FcInitLoadConfigAndFonts ();
    if (!config)
        return FcFalse;
    return FcConfigSetCurrent (config);
}

FcBool
FcInitBringUptoDate (void)
{
    FcConfig    *config = FcConfigGetCurrent ();
    time_t  now;

    /*
     * rescanInterval == 0 disables automatic up to date
     */
    if (config->rescanInterval == 0)
        return FcTrue;
    /*
     * Check no more often than rescanInterval seconds
     */
    now = time (0);
    if (config->rescanTime + config->rescanInterval - now > 0)
        return FcTrue;
    /*
     * If up to date, don't reload configuration
     */
    if (FcConfigUptoDate (0))
        return FcTrue;
    return FcInitReinitialize ();
}

#define __fcinit__
#include "fcaliastail.h"
#undef __fcinit__
