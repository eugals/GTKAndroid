// Will use "config.h.win32" from GTK+ distribution directory
// to obtain some invariants

#undef _GDK_EXTERN
#include "config.h.win32"

// now we make some overrides
#undef ENABLE_NLS

#undef _GDK_EXTERN
#define _GDK_EXTERN extern

#define GTK_DATA_PREFIX "assets"
#define GTK_DATADIR "assets/share"
#define GTK_LIBDIR  "."
#define GTK_SYSCONFDIR  "/etc/xdg"
#define GTK_PRINT_BACKENDS ""
#define GTK_PRINT_PREVIEW_COMMAND ""

#undef gid_t
#undef uid_t

// to prevent $(NDK_ROOT)/platforms/android-XX/<target>/usr/include/alloca.h
// from loading as galloca.h has this definition already
#define _ALLOCA_H
