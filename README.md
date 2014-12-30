GTKAndroid
==========

This is a set of hacks, makefiles and configs allowing you to run GTK-based forms on Android platform.

Please see [**build instructions**](https://github.com/eugals/GTKAndroid/wiki/Building)


Known issues/limitations of this 0.1alpha version:

- It may take a few seconds for something to appear on the screen, when you run the app **for the first time**.
(`fontconfig` processes all the system fonts then)

- All libraries are statically linked atm 
  (several of them are LGPL-licensed, which may be inappropriate for some applications)

- Only "C" locale is supported by `libc` on Android

- No soft keyboard support yet
