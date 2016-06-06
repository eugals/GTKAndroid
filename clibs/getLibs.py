# Copyright 2014 The GTK+Android Developers. See the COPYRIGHT
# file at the top-level directory of this distribution and at
# https://github.com/eugals/GTKAndroid/wiki/COPYRIGHT.
#
# Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
# http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
# <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
# option. This file may not be copied, modified, or distributed
# except according to those terms.
#
# Author(s): Evgeny Sologubov
#

import os, os.path, re
import tarfile, subprocess
import time
try:
    # Python 2+
    from urllib2 import urlopen
    from cStringIO import StringIO
except:
    # Python 3+
    from urllib.request import urlopen
    from io import BytesIO as StringIO

EXTLIBS_PATH = os.path.split(os.path.realpath(__file__))[0]



class LibDownloader(object):
    """The core script worker class"""


    def __init__(self):
        """*LibDownloader* instance constructor"""
        self.xtractTarXZ = None
        for method in (self.xtractWithLZMA, self.xtractWith7Zip, self.xtractWithTarX):
            if method():
                self.xtractTarXZ = method
                break


    def download(self, name, url, libFullName = None):
        """Downloads, extracts and prepares for building the library specified"""

        ext = os.path.splitext(url)[-1]
        assert ext in (".xz", ".gz", ".bz2")
        if libFullName:
            archFileName = libFullName + ".tar" + ext
        else:
            archFileName = os.path.split(url)[-1]
        fullArchName = os.path.join(EXTLIBS_PATH, archFileName)
        extractedDirName = archFileName.split(".tar")[0]
        destPath = os.path.join(EXTLIBS_PATH, name)

        if os.path.exists(fullArchName):
            print ("%s already exists" % fullArchName)
        else:
            print ("Downloading %s" % url)
            remote = urlopen(url)
            with open(fullArchName, "wb") as f:
                f.write(remote.read())
            remote.close()

        if os.path.exists(destPath):
            print ("%s already exists" % destPath)
        else:
            if not os.path.exists(os.path.split(destPath)[0]):
                os.makedirs(os.path.split(destPath)[0])
            print ("Extractting library snapshot to %s" % destPath)
            self.xtractTar(archFileName)
            time.sleep(0.5) # wait a bit, to let Windows release all the necessary directory locks
            os.rename(os.path.join(EXTLIBS_PATH, extractedDirName), destPath)


    def xtractTar(self, inputName):
        """Extracts *.tar.xz or *.tar.gz archive
        """
        if inputName.endswith(".xz"):
            return self.xtractTarXZ(inputName)
        tarName = os.path.join(EXTLIBS_PATH, inputName)
        with tarfile.open(tarName, mode = "r:*") as tarf:
            tarf.extractall(EXTLIBS_PATH)

    
    def xtractWithLZMA(self, inputName = None):
        """Either attempts to extract *inputName* with Python3.3's lzma library
           or just checks if such an extraction is possible within the environment
        """
        if inputName is None:
            try:
                import lzma
            except ImportError:
                return False
            return True
        tarName = os.path.join(EXTLIBS_PATH, inputName)
        with tarfile.open(tarName, mode = "r:xz") as tarf:
            tarf.extractall(EXTLIBS_PATH)


    def xtractWith7Zip(self, inputName = None):
        """Either attempts to extract *inputName* with 7z.exe
           or just checks if such an extraction is possible within the environment
        """
        if inputName is None:
            try:
                subprocess.call(["7z"], stdout = subprocess.PIPE)
            except OSError:
                return False
            return True
        args = ["7z", "x", "-y", inputName]
        proc = subprocess.Popen(args, stdout = subprocess.PIPE, stderr = subprocess.PIPE, cwd = EXTLIBS_PATH)
        _, stderrData = proc.communicate()
        if stderrData:
            raise ValueError(stderrData)
        tarName = os.path.join(EXTLIBS_PATH, inputName.rstrip(".xz"))
        with tarfile.open(tarName) as tarf:
            tarf.extractall(EXTLIBS_PATH)
        os.unlink(tarName)


    def xtractWithTarX(self, inputName = None):
        """Either attempts to extract *inputName* with *tar* utility
           or just checks if such an extraction is possible within the environment
        """
        if inputName is None:
            try:
                subprocess.call(["tar", "--help"], stdout = subprocess.PIPE)
            except OSError:
                return False
            return True
        args = ["tar", "xf", inputName]
        proc = subprocess.Popen(args, stdout = subprocess.PIPE, stderr = subprocess.PIPE, cwd = EXTLIBS_PATH)
        _, stderrData = proc.communicate()
        if stderrData:
            raise ValueError(stderrData)


varStartRe = re.compile("^([a-zA-Z0-9_@]+)\s?\:?=(.*)$")


def makeVariablesFile(makefileName, outputName, variableNames):
    """Parses makefileName and extracts all the specified variables into a separate file"""
    print ("Extracting some variables from %s into %s" % (makefileName, outputName))
    allVars = {}
    incompleteVar = None
    # parse and extract all the variables
    with open(os.path.join(EXTLIBS_PATH, makefileName)) as f:
        for line in f.readlines():
            if incompleteVar:
                line = line.strip()
                if line.endswith("\\"):
                    allVars[incompleteVar] += " " + line[:-1]
                else:
                    allVars[incompleteVar] += " " + line
                    incompleteVar = None
            else:
                mo = varStartRe.match(line)
                if mo:
                    varName, var = mo.groups()
                    var = var.strip()
                    if var.endswith("\\"):
                        var = var[:-1]
                        incompleteVar = varName
                    allVars[varName] = var
    # form output file
    with open(os.path.join(EXTLIBS_PATH, outputName), "w") as f:
        for varName in variableNames:
            vals = allVars[varName].split()
            if "@" in varName:
                varName = varName.split("@")[-1]  # hide automake artifacts
            if len(vals) == 1:
                f.write("%s = %s\n" % (varName, vals[0]))
            else:
                vals = [val for val in vals if not val.startswith("@")]
                f.write("%s = %-40s\\\n" % (varName, vals[0]))
                for val in vals[1:-1]:
                    f.write(" " * len(varName) + "   %-40s\\\n" % val)
                f.write(" " * len(varName) + "   %-40s\n" % vals[-1])
            f.write("\n")


def run():
    """Main script entry point"""
    dl = LibDownloader()
    if not dl.xtractTarXZ:
        print ("This script wouldn't be able to uncompress *.tar.xz files stored on http://ftp.gnome.org/pub/gnome/sources/")
        print ("Please do ONE of the following to fix this:")
        print (" - Install 7Zip tools (http://www.7-zip.org/) and make them available to this script via PATH environment variable")
        print (" - Use Python 3.3+")
        return

    # ffi
    dl.download("ffi/distsrc", "https://github.com/android/platform_external_libffi/archive/android-4.4.4_r2.0.1.tar.gz",
                "platform_external_libffi-android-4.4.4_r2.0.1")

    # glib, gobject, gio
    dl.download("glib/distsrc", "http://ftp.gnome.org/pub/gnome/sources/glib/2.40/glib-2.40.2.tar.xz")
    makeVariablesFile("glib/distsrc/glib/Makefile.in", "glib/distsrc/glib/Sources.mk",
                      ["@OS_UNIX_TRUE@am__append_16",
                       "@THREADS_POSIX_TRUE@@THREADS_WIN32_FALSE@am__append_18",
                       "deprecated_sources",
                       "libglib_2_0_la_SOURCES",
                       "deprecatedinclude_HEADERS",
                       "glibsubinclude_HEADERS"])
    makeVariablesFile("glib/distsrc/gobject/Makefile.in", "glib/distsrc/gobject/Sources.mk",
                      ["gobject_c_sources"])
    makeVariablesFile("glib/distsrc/gio/Makefile.in", "glib/distsrc/gio/Sources.mk",
                      ["settings_sources", "application_sources", "gdbus_sources", 
                       "@OS_UNIX_TRUE@unix_sources", "local_sources", "libgio_2_0_la_SOURCES"])
    makeVariablesFile("glib/distsrc/gio/xdgmime/Makefile.in", "glib/distsrc/gio/xdgmime/Sources.mk", ["libxdgmime_la_SOURCES"])                       
    makeVariablesFile("glib/distsrc/gmodule/Makefile.in", "glib/distsrc/gmodule/Sources.mk",
                      ["libgmodule_2_0_la_SOURCES"])
    makeVariablesFile("glib/distsrc/glib/pcre/Makefile.in", "glib/distsrc/glib/pcre/Sources.mk",
                      ["libpcre_la_SOURCES", "libpcre_la_CPPFLAGS"])

    #dl.download("gobject-introspection/distsrc", "http://ftp.gnome.org/pub/gnome/sources/gobject-introspection/1.41/gobject-introspection-1.41.91.tar.xz")
    #makeVariablesFile("gobject-introspection/distsrc/Makefile.in", "gobject-introspection/distsrc/Sources.mk",
    #                  ["libcmph_la_SOURCES",
    #                   "libgirepository_internals_la_SOURCES",
    #                   "libgirepository_1_0_la_SOURCES"])

    # pango
    dl.download("pango/distsrc", "http://ftp.gnome.org/pub/gnome/sources/pango/1.36/pango-1.36.8.tar.xz")
    makeVariablesFile("pango/distsrc/pango/Makefile.in", "pango/distsrc/pango/Sources.mk",
                      ["libpango_1_0_la_SOURCES", "pango_headers", "pangoft2_public_sources", "pangocairo_core_sources"])

    # atk
    dl.download("atk/distsrc", "http://ftp.gnome.org/pub/gnome/sources/atk/2.13/atk-2.13.90.tar.xz")
    makeVariablesFile("atk/distsrc/atk/Makefile.in", "atk/distsrc/atk/Sources.mk",
                      ["atk_sources",
                       "libatk_1_0_la_SOURCES",
                       "atk_headers"])

    # gdk-pixbuf
    dl.download("gdk-pixbuf/distsrc", "http://ftp.gnome.org/pub/gnome/sources/gdk-pixbuf/2.31/gdk-pixbuf-2.31.1.tar.xz")
    makeVariablesFile("gdk-pixbuf/distsrc/gdk-pixbuf/Makefile.in", "gdk-pixbuf/distsrc/gdk-pixbuf/Sources.mk",
                      ["libgdk_pixbuf_2_0_la_SOURCES"])
    makeVariablesFile("gdk-pixbuf/distsrc/configure", "gdk-pixbuf/distsrc/Config.mk",
                      ["GDK_PIXBUF_MAJOR", "GDK_PIXBUF_MINOR", "GDK_PIXBUF_MICRO",
                       "GDK_PIXBUF_VERSION", "GDK_PIXBUF_API_VERSION", "GDK_PIXBUF_BINARY_VERSION"])

    # gtk & gdk
    dl.download("gtk/distsrc", "http://ftp.gnome.org/pub/gnome/sources/gtk+/3.12/gtk+-3.12.2.tar.xz")
    makeVariablesFile("gtk/distsrc/gtk/Makefile.in", "gtk/distsrc/gtk/Sources.mk",
                      ["am__libgtk_3_la_SOURCES_DIST"])
    makeVariablesFile("gtk/distsrc/configure", "gtk/distsrc/Config.mk",
                      ["GTK_MAJOR_VERSION", "GTK_MINOR_VERSION", "GTK_MICRO_VERSION",
                       "GTK_BINARY_AGE", "GTK_VERSION", "GTK_API_VERSION", "GTK_BINARY_VERSION"])
    makeVariablesFile("gtk/distsrc/gdk/Makefile.in", "gtk/distsrc/gdk/Sources.mk",
                      ["am__libgdk_3_la_SOURCES_DIST"])
    makeVariablesFile("gtk/distsrc/gtk/a11y/Makefile.in", "gtk/distsrc/gtk/a11y/Sources.mk",
                      ["gtka11y_c_sources"])

    # freetype, fontconfig, harfbuzz
    dl.download("freetype/distsrc", "http://download.savannah.gnu.org/releases/freetype/freetype-2.5.4.tar.bz2") 
    dl.download("fontconfig/distsrc", "http://www.freedesktop.org/software/fontconfig/release/fontconfig-2.11.1.tar.bz2") 
    makeVariablesFile("fontconfig/distsrc/src/Makefile.in", "fontconfig/distsrc/src/Makefile.sources",
                      ["libfontconfig_la_SOURCES"])
    dl.download("harfbuzz/distsrc", "http://www.freedesktop.org/software/harfbuzz/release/harfbuzz-0.9.36.tar.bz2") 
    makeVariablesFile("harfbuzz/distsrc/src/Makefile.in", "harfbuzz/distsrc/src/Makefile.sources",
                      ["HBSOURCES"])

    # cairo & pixman
    print ("Please be aware that downloading files from cairographics.org can take REALLY LONG TIME (up to 10-15 minutes)")
    dl.download("cairo/distsrc/cairo", "http://cairographics.org/releases/cairo-1.14.0.tar.xz")
    dl.download("cairo/distsrc/pixman", "http://cairographics.org/releases/pixman-0.32.6.tar.gz")

if __name__ == "__main__":
    run()
