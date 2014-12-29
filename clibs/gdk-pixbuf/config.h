// Will use "config.h.win32" from GLIB distribution directory
// to obtain some invariants (like GLIB_*_VERSION)

#undef _GLIB_EXTERN
#include "config.h.win32"

// now we make some overrides

#undef _WIN32_WINNT
#undef BROKEN_POLL
#undef NO_FD_SET
#undef USE_MMX

#undef ENABLE_NLS

// the following two are defined in android/support module
extern char *gettext(const char *msgid);
extern char *dgettext(const char *domainname, const char *msgid);

// "config.h.win32" defines _GLIB_EXTERN incompatible with gmacros.h
#undef _GLIB_EXTERN
// redefine it!
#define _GLIB_EXTERN extern

// to prevent $(NDK_ROOT)/platforms/android-XX/<target>/usr/include/alloca.h
// from loading as galloca.h has this definition already
#define _ALLOCA_H
