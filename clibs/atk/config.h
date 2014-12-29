// Will use "config.h.win32" from GLIB distribution directory
// to obtain some invariants (like GLIB_*_VERSION)

#undef _ATK_EXTERN
#include "config.h.win32"

// now we make some overrides

#undef ENABLE_NLS
#undef HAVE_GETTEXT
#undef HAVE_DCGETTEXT

// "config.h.win32" defines _GLIB_EXTERN incompatible with gmacros.h
#undef _ATK_EXTERN
// redefine it!
#define _ATK_EXTERN extern

// the following two are defined in android/support module
extern char *gettext(const char *msgid);
extern char *dgettext(const char *domainname, const char *msgid);

