// Will use "config.h.win32" from GTK+ distribution directory
// to obtain some invariants

#include "config.h.win32"

// now we make some overrides
#undef ENABLE_NLS
#undef HAVE_CAIRO_PDF
#undef HAVE_CAIRO_PNG
#undef HAVE_CAIRO_PS
#undef HAVE_CAIRO_WIN32
//#undef HAVE_CAIRO_FREETYPE

// to prevent $(NDK_ROOT)/platforms/android-XX/<target>/usr/include/alloca.h
// from loading as galloca.h has this definition already
#define _ALLOCA_H

#include <stdlib.h>