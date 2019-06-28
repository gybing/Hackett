/*

    IMPORTANT! This file is auto-generated each time you save your
    project - if you alter its contents, your changes may be overwritten!

    There's a section below where you can add your own custom code safely, and the
    Projector will preserve the contents of that block, but the best way to change
    any of these definitions is by using the Projector's project settings.

    Any commented-out settings will assume their default values.

*/

#pragma once

//==============================================================================
// [BEGIN_USER_CODE_SECTION]

/*
  ==============================================================================

   In accordance with the terms of the H 5 End-Use License Agreement, the
   H Code in SECTION A cannot be removed, changed or otherwise rendered
   ineffective unless you have a H Indie or Pro license, or are using H
   under the GPL v3 license.

   End User License Agreement: www.H.com/H-5-licence

  ==============================================================================
*/

// BEGIN SECTION A

#ifndef HR_ENABLE_GPL_MODE
 #define HR_ENABLE_GPL_MODE 0
#endif

// END SECTION A

// [END_USER_CODE_SECTION]

/*
  ==============================================================================

   In accordance with the terms of the H 5 End-Use License Agreement, the
   H Code in SECTION A cannot be removed, changed or otherwise rendered
   ineffective unless you have a H Indie or Pro license, or are using H
   under the GPL v3 license.

   End User License Agreement: www.H.com/H-5-licence

  ==============================================================================
*/

// BEGIN SECTION A

#ifndef HDISPLAY_SPLASH_SCREEN
 #define HDISPLAY_SPLASH_SCREEN 0
#endif

#ifndef HREPORT_APP_USAGE
 #define HREPORT_APP_USAGE 0
#endif

// END SECTION A

#define HUSE_DARK_SPLASH_SCREEN 1

//==============================================================================
#define HMODULE_AVAILABLE_analytics            1
#define HMODULE_AVAILABLE_core                 1
#define HMODULE_AVAILABLE_cryptography         1
#define HMODULE_AVAILABLE_data_structures      1
#define HMODULE_AVAILABLE_events               1
#define HMODULE_AVAILABLE_graphics             1
#define HMODULE_AVAILABLE_gui_basics           1
#define HMODULE_AVAILABLE_gui_extra            1

#define HGLOBAL_MODULE_SETTINGS_INCLUDED 1

//==============================================================================
// core flags:

#ifndef    HFORCE_DEBUG
 //#define HFORCE_DEBUG 0
#endif

#ifndef    HLOG_ASSERTIONS
 #define   HLOG_ASSERTIONS 1
#endif

#ifndef    HCHECK_MEMORY_LEAKS
 //#define HCHECK_MEMORY_LEAKS 1
#endif

#ifndef    HDONT_AUTOLINK_TO_WIN32_LIBRARIES
 //#define HDONT_AUTOLINK_TO_WIN32_LIBRARIES 0
#endif

#ifndef    HINCLUDE_ZLIB_CODE
 //#define HINCLUDE_ZLIB_CODE 1
#endif

#ifndef    HUSE_CURL
 #define   HUSE_CURL 1
#endif

#ifndef    HLOAD_CURL_SYMBOLS_LAZILY
 #define   HLOAD_CURL_SYMBOLS_LAZILY 1
#endif

#ifndef    HCATCH_UNHANDLED_EXCEPTIONS
 //#define HCATCH_UNHANDLED_EXCEPTIONS 0
#endif

#ifndef    HALLOW_STATIC_NULL_VARIABLES
 #define   HALLOW_STATIC_NULL_VARIABLES 0
#endif

#ifndef    HSTRICT_REFCOUNTEDPOINTER
 #define   HSTRICT_REFCOUNTEDPOINTER 1
#endif

//==============================================================================
// events flags:

#ifndef    HEXECUTE_APP_SUSPEND_ON_IOS_BACKGROUND_TASK
 //#define HEXECUTE_APP_SUSPEND_ON_IOS_BACKGROUND_TASK 0
#endif

//==============================================================================
// graphics flags:

#ifndef    HUSE_COREIMAGE_LOADER
 //#define HUSE_COREIMAGE_LOADER 1
#endif

#ifndef    HUSE_DIRECTWRITE
 //#define HUSE_DIRECTWRITE 1
#endif

#ifndef    HDISABLE_COREGRAPHICS_FONT_SMOOTHING
 //#define HDISABLE_COREGRAPHICS_FONT_SMOOTHING 0
#endif

//==============================================================================
// gui_basics flags:

#ifndef    HENABLE_REPAINT_DEBUGGING
 //#define HENABLE_REPAINT_DEBUGGING 0
#endif

#ifndef    HUSE_XRANDR
 //#define HUSE_XRANDR 1
#endif

#ifndef    HUSE_XINERAMA
 //#define HUSE_XINERAMA 1
#endif

#ifndef    HUSE_XSHM
 //#define HUSE_XSHM 1
#endif

#ifndef    HUSE_XRENDER
 //#define HUSE_XRENDER 0
#endif

#ifndef    HUSE_XCURSOR
 //#define HUSE_XCURSOR 1
#endif

#ifndef    HWIN_PER_MONITOR_DPI_AWARE
 //#define HWIN_PER_MONITOR_DPI_AWARE 1
#endif

//==============================================================================
// gui_extra flags:

#ifndef    HWEB_BROWSER
 //#define HWEB_BROWSER 1
#endif

#ifndef    HENABLE_LIVE_CONSTANT_EDITOR
 //#define HENABLE_LIVE_CONSTANT_EDITOR 0
#endif

//==============================================================================
#ifndef    HSTANDALONE_APPLICATION
 #if defined(JucePlugin_Name) && defined(JucePlugin_Build_Standalone)
  #define  HSTANDALONE_APPLICATION JucePlugin_Build_Standalone
 #else
  #define  HSTANDALONE_APPLICATION 1
 #endif
#endif
