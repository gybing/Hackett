/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#if HWINDOWS
 #undef _WIN32_WINNT
 #define _WIN32_WINNT 0x500
 #undef STRICT
 #define STRICT 1
 #include <windows.h>
 #include <float.h>
 #if ! HMINGW
  #pragma warning (disable : 4312 4355)
 #endif
 #ifdef __INTEL_COMPILER
  #pragma warning (disable : 1899)
 #endif

#elif HLINUX
 #include <float.h>
 #include <sys/time.h>
 #include <X11/Xlib.h>
 #include <X11/Xutil.h>
 #include <X11/Xatom.h>
 #undef Font
 #undef KeyPress
 #undef Drawable
 #undef Time
#elif HANDROID
#else
 #if ! (defined (HSUPPORT_CARBON) || defined (__LP64__))
  #define HSUPPORT_CARBON 1
 #endif

 #ifdef __OBJC__
  #if HMAC
   #include <Cocoa/Cocoa.h>
  #elif HIOS
   #include <UIKit/UIKit.h>
  #else
   #error
  #endif
 #endif

 #if HSUPPORT_CARBON && (! HIOS)
  #include <Carbon/Carbon.h>
 #endif

 #include <objc/runtime.h>
 #include <objc/objc.h>
 #include <objc/message.h>
#endif
