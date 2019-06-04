

/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projector to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 H Module Format.txt file.


 BEGIN_HMODULE_DECLARATION

  ID:               events
  vendor:           H
  version:          5.4.3
  name:             H message and event handling classes
  description:      Classes for running an application's main event loop and sending/receiving messages, timers, etc.
  website:          http://www.H.com/H
  license:          ISC

  dependencies:     core

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HEVENTS_H_INCLUDED

#include <core/core.h>

//==============================================================================
/** Config: HEXECUTE_APP_SUSPEND_ON_IOS_BACKGROUND_TASK
    Will execute your application's suspend method on an iOS background task, giving
    you extra time to save your applications state.
*/
#ifndef HEXECUTE_APP_SUSPEND_ON_BACKGROUND_TASK
 #define HEXECUTE_APP_SUSPEND_ON_BACKGROUND_TASK 0
#endif

#if HEVENTS_INCLUDE_WINRT_WRAPPER && HWINDOWS
 // If this header file is missing then you are probably attempting to use WinRT
 // functionality without the WinRT libraries installed on your system. Try installing
 // the latest Windows Standalone SDK and maybe also adding the path to the WinRT
 // headers to your build system. This path should have the form
 // "C:\Program Files (x86)\Windows Kits\10\Include\10.0.14393.0\winrt".
 #include <inspectable.h>
 #include <hstring.h>
#endif

#include "messages/MessageManager.h"
#include "messages/Message.h"
#include "messages/MessageListener.h"
#include "messages/CallbackMessage.h"
#include "messages/DeletedAtShutdown.h"
#include "messages/NotificationType.h"
#include "messages/ApplicationBase.h"
#include "messages/Initialisation.h"
#include "messages/MountedVolumeListChangeDetector.h"
#include "broadcasters/ActionBroadcaster.h"
#include "broadcasters/ActionListener.h"
#include "broadcasters/AsyncUpdater.h"
#include "broadcasters/ChangeListener.h"
#include "broadcasters/ChangeBroadcaster.h"
#include "timers/Timer.h"
#include "timers/MultiTimer.h"
#include "interprocess/InterprocessConnection.h"
#include "interprocess/InterprocessConnectionServer.h"
#include "interprocess/ConnectedChildProcess.h"
#include "interprocess/NetworkServiceDiscovery.h"

#if HLINUX
 #include "native/linux_EventLoop.h"
#endif

#if HWINDOWS
 #if HEVENTS_INCLUDE_WIN32_MESSAGE_WINDOW
  #include "native/win32_HiddenMessageWindow.h"
 #endif
 #if HEVENTS_INCLUDE_WINRT_WRAPPER
  #include "native/win32_WinRTWrapper.h"
 #endif
#endif
