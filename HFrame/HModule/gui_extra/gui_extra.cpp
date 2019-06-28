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

#ifdef HGUI_EXTRA_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#define HCORE_INCLUDE_OBJC_HELPERS 1
#define HCORE_INCLUDE_COM_SMART_PTR 1
#define HCORE_INCLUDE_JNI_HELPERS 1
#define HCORE_INCLUDE_NATIVE_HEADERS 1
#define HEVENTS_INCLUDE_WIN32_MESSAGE_WINDOW 1
#define HGRAPHICS_INCLUDE_COREGRAPHICS_HELPERS 1

#ifndef HPUSH_NOTIFICATIONS
 #define HPUSH_NOTIFICATIONS 0
#endif

#include "gui_extra.h"

//==============================================================================
#if HMAC
 #import <WebKit/WebKit.h>
 #import <IOKit/IOKitLib.h>
 #import <IOKit/IOCFPlugIn.h>
 #import <IOKit/hid/IOHIDLib.h>
 #import <IOKit/hid/IOHIDKeys.h>
 #import <IOKit/pwr_mgt/IOPMLib.h>

 #if HPUSH_NOTIFICATIONS
  #import <Foundation/NSUserNotification.h>

  #include "native/mac_PushNotifications.cpp"
 #endif

//==============================================================================
#elif HIOS
 #if HPUSH_NOTIFICATIONS
  #if defined (__IPHONE_10_0) && __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
   #import <UserNotifications/UserNotifications.h>
  #endif

  #include "native/ios_PushNotifications.cpp"
 #endif

//==============================================================================
#elif HANDROID
 #if HPUSH_NOTIFICATIONS
  #include "native/android_PushNotifications.cpp"
 #endif

//==============================================================================
#elif HWINDOWS
 #include <windowsx.h>
 #include <vfw.h>
 #include <commdlg.h>

 #if HWEB_BROWSER
  #include <exdisp.h>
  #include <exdispid.h>
 #endif

//==============================================================================
#elif HLINUX
 #include <X11/Xlib.h>
 #include <X11/Xatom.h>
 #include <X11/Xutil.h>
 #undef SIZEOF
 #undef KeyPress

 #if HWEB_BROWSER
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/wait.h>

  #if HGCC && __GNUC__ > 7
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wparentheses"
  #endif

  #include <gtk/gtk.h>

  #if HGCC && __GNUC__ > 7
   #pragma GCC diagnostic pop
  #endif

  #include <gtk/gtkx.h>
  #include <glib-unix.h>
  #include <webkit2/webkit2.h>
 #endif
#endif

//==============================================================================
#include "documents/FileBasedDocument.cpp"
#include "code_editor/CodeDocument.cpp"
#include "code_editor/CodeEditorComponent.cpp"
#include "code_editor/CPlusPlusCodeTokeniser.cpp"
#include "code_editor/XMLCodeTokeniser.cpp"
#include "code_editor/LuaCodeTokeniser.cpp"
#include "misc/BubbleMessageComponent.cpp"
#include "misc/ColourSelector.cpp"
#include "misc/KeyMappingEditorComponent.cpp"
#include "misc/PreferencesPanel.cpp"
#include "misc/PushNotifications.cpp"
#include "misc/RecentlyOpenedFilesList.cpp"
#include "misc/SplashScreen.cpp"
#include "misc/SystemTrayIconComponent.cpp"
#include "misc/LiveConstantEditor.cpp"
#include "misc/AnimatedAppComponent.cpp"

//==============================================================================
#if HMAC || HIOS

 #if HCLANG
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wundeclared-selector"
 #endif

 #if HMAC
  #include "native/mac_NSViewComponent.mm"
  #include "native/mac_AppleRemote.mm"
  #include "native/mac_SystemTrayIcon.cpp"
 #endif

 #if HIOS
  #include "native/ios_UIViewComponent.mm"
 #endif

 #if HWEB_BROWSER
  #include "native/mac_WebBrowserComponent.mm"
 #endif

 #if HCLANG
  #pragma clang diagnostic pop
 #endif

//==============================================================================
#elif HWINDOWS
 #include "native/win32_ActiveXComponent.cpp"
 #if HWEB_BROWSER
  #include "native/win32_WebBrowserComponent.cpp"
 #endif
 #include "native/win32_SystemTrayIcon.cpp"

//==============================================================================
#elif HLINUX
  #include "native/linux_XEmbedComponent.cpp"
 #if HWEB_BROWSER
  #include "native/linux_X11_WebBrowserComponent.cpp"
 #endif
 #include "native/linux_X11_SystemTrayIcon.cpp"

//==============================================================================
#elif HANDROID
 #include "native/AndroidViewComponent.cpp"

 #if HWEB_BROWSER
  #include "native/android_WebBrowserComponent.cpp"
 #endif
#endif

#if HWEB_BROWSER
namespace H
{
    bool WebBrowserComponent::pageAboutToLoad (const String&)  { return true; }
    void WebBrowserComponent::pageFinishedLoading (const String&) {}
    bool WebBrowserComponent::pageLoadHadNetworkError (const String&) { return true; }
    void WebBrowserComponent::windowCloseRequest() {}
    void WebBrowserComponent::newWindowAttemptingToLoad (const String&) {}
}
#endif
