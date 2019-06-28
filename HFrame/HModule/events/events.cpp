

#ifdef HEVENTS_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#define HCORE_INCLUDE_OBJC_HELPERS 1
#define HCORE_INCLUDE_JNI_HELPERS 1
#define HCORE_INCLUDE_NATIVE_HEADERS 1
#define HCORE_INCLUDE_COM_SMART_PTR 1
#define HEVENTS_INCLUDE_WIN32_MESSAGE_WINDOW 1

#if HUSE_WINRT_MIDI
 #define HEVENTS_INCLUDE_WINRT_WRAPPER 1
#endif

#include "events.h"

//==============================================================================
#if HMAC
 #import <IOKit/IOKitLib.h>
 #import <IOKit/IOCFPlugIn.h>
 #import <IOKit/hid/IOHIDLib.h>
 #import <IOKit/hid/IOHIDKeys.h>
 #import <IOKit/pwr_mgt/IOPMLib.h>

#elif HLINUX
 #include <unistd.h>
#endif

//==============================================================================
#include "messages/ApplicationBase.cpp"
#include "messages/DeletedAtShutdown.cpp"
#include "messages/MessageListener.cpp"
#include "messages/MessageManager.cpp"
#include "broadcasters/ActionBroadcaster.cpp"
#include "broadcasters/AsyncUpdater.cpp"
#include "broadcasters/ChangeBroadcaster.cpp"
#include "timers/MultiTimer.cpp"
#include "timers/Timer.cpp"
#include "interprocess/InterprocessConnection.cpp"
#include "interprocess/InterprocessConnectionServer.cpp"
#include "interprocess/ConnectedChildProcess.cpp"
#include "interprocess/NetworkServiceDiscovery.cpp"

//==============================================================================
#if HMAC || HIOS

 #include "native/osx_MessageQueue.h"

 #if HCLANG
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wundeclared-selector"
 #endif

 #if HMAC
  #include "native/mac_MessageManager.mm"
 #else
  #include "native/ios_MessageManager.mm"
 #endif

 #if HCLANG
  #pragma clang diagnostic pop
 #endif

#elif HWINDOWS
 #include "native/win32_Messaging.cpp"
 #if HEVENTS_INCLUDE_WINRT_WRAPPER
  #include "native/win32_WinRTWrapper.cpp"
 #endif

#elif HLINUX
 #include "native/linux_Messaging.cpp"

#elif HANDROID
 #include "native/android_Messaging.cpp"

#endif
