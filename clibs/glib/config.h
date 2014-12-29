// Will use "config.h.win32" from GLIB distribution directory
// to obtain some invariants (like GLIB_*_VERSION)

#undef _GLIB_EXTERN
#include "config.h.win32"

// now we make some overrides

#undef _WIN32_WINNT
#undef BROKEN_POLL
#undef NO_FD_SET
#undef ENABLE_NLS
#undef HAVE_INT64_AND_I64
#undef HAVE_WINT_T
#undef THREADS_WIN32

#define HAVE_SYS_STATFS_H 1
#define USE_STATFS        1
#define HAVE_SYS_MOUNT_H  1
#define HAVE_SYS_VFS_H    1
#define HAVE_SYS_PARAM_H  1
#define HAVE_POLL         1

#define HAVE_PTHREAD_COND_TIMEDWAIT_RELATIVE_NP 1

// the following tree are defined in android/support module
extern char *gettext(const char *msgid);
extern char *dgettext(const char *domainname, const char *msgid);
extern char *ngettext(const char *msgid1, const char *msgid2, unsigned long int n);


// "config.h.win32" defines _GLIB_EXTERN incompatible with gmacros.h
#undef _GLIB_EXTERN
// redefine it!
#define _GLIB_EXTERN extern

// to prevent $(NDK_ROOT)/platforms/android-XX/<target>/usr/include/alloca.h
// from loading as galloca.h has this definition already
#define _ALLOCA_H
