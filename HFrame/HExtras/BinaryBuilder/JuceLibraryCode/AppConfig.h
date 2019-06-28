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

// (You can add your own code in this section, and the Projector will not overwrite it)

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
#define HMODULE_AVAILABLE_core      1

#define HGLOBAL_MODULE_SETTINGS_INCLUDED 1

//==============================================================================
// core flags:

#ifndef    HFORCE_DEBUG
 //#define HFORCE_DEBUG 0
#endif

#ifndef    HLOG_ASSERTIONS
 //#define HLOG_ASSERTIONS 0
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
 //#define HUSE_CURL 0
#endif

#ifndef    HLOAD_CURL_SYMBOLS_LAZILY
 //#define HLOAD_CURL_SYMBOLS_LAZILY 0
#endif

#ifndef    HCATCH_UNHANDLED_EXCEPTIONS
 //#define HCATCH_UNHANDLED_EXCEPTIONS 0
#endif

#ifndef    HALLOW_STATIC_NULL_VARIABLES
 //#define HALLOW_STATIC_NULL_VARIABLES 0
#endif

#ifndef    HSTRICT_REFCOUNTEDPOINTER
 //#define HSTRICT_REFCOUNTEDPOINTER 0
#endif

//==============================================================================
#ifndef    HSTANDALONE_APPLICATION
 #if defined(JucePlugin_Name) && defined(JucePlugin_Build_Standalone)
  #define  HSTANDALONE_APPLICATION JucePlugin_Build_Standalone
 #else
  #define  HSTANDALONE_APPLICATION 1
 #endif
#endif
