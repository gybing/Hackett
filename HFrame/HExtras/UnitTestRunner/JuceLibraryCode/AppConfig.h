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
#define HMODULE_AVAILABLE_analytics              1
#define HMODULE_AVAILABLE_audio_basics           1
#define HMODULE_AVAILABLE_audio_devices          1
#define HMODULE_AVAILABLE_audio_formats          1
#define HMODULE_AVAILABLE_audio_processors       1
#define HMODULE_AVAILABLE_audio_utils            1
#define HMODULE_AVAILABLE_blocks_basics          1
#define HMODULE_AVAILABLE_core                   1
#define HMODULE_AVAILABLE_cryptography           1
#define HMODULE_AVAILABLE_data_structures        1
#define HMODULE_AVAILABLE_dsp                    1
#define HMODULE_AVAILABLE_events                 1
#define HMODULE_AVAILABLE_graphics               1
#define HMODULE_AVAILABLE_gui_basics             1
#define HMODULE_AVAILABLE_gui_extra              1
#define HMODULE_AVAILABLE_opengl                 1
#define HMODULE_AVAILABLE_osc                    1
#define HMODULE_AVAILABLE_product_unlocking      1
#define HMODULE_AVAILABLE_video                  1

#define HGLOBAL_MODULE_SETTINGS_INCLUDED 1

//==============================================================================
// audio_devices flags:

#ifndef    HUSE_WINRT_MIDI
 //#define HUSE_WINRT_MIDI 0
#endif

#ifndef    HASIO
 //#define HASIO 0
#endif

#ifndef    HWASAPI
 //#define HWASAPI 1
#endif

#ifndef    HWASAPI_EXCLUSIVE
 //#define HWASAPI_EXCLUSIVE 0
#endif

#ifndef    HDIRECTSOUND
 //#define HDIRECTSOUND 1
#endif

#ifndef    HALSA
 //#define HALSA 1
#endif

#ifndef    HJACK
 //#define HJACK 0
#endif

#ifndef    HBELA
 //#define HBELA 0
#endif

#ifndef    HUSE_ANDROID_OBOE
 //#define HUSE_ANDROID_OBOE 0
#endif

#ifndef    HUSE_ANDROID_OPENSLES
 //#define HUSE_ANDROID_OPENSLES 0
#endif

#ifndef    HDISABLE_AUDIO_MIXING_WITH_OTHER_APPS
 //#define HDISABLE_AUDIO_MIXING_WITH_OTHER_APPS 0
#endif

//==============================================================================
// audio_formats flags:

#ifndef    HUSE_FLAC
 //#define HUSE_FLAC 1
#endif

#ifndef    HUSE_OGGVORBIS
 //#define HUSE_OGGVORBIS 1
#endif

#ifndef    HUSE_MP3AUDIOFORMAT
 //#define HUSE_MP3AUDIOFORMAT 0
#endif

#ifndef    HUSE_LAME_AUDIO_FORMAT
 //#define HUSE_LAME_AUDIO_FORMAT 0
#endif

#ifndef    HUSE_WINDOWS_MEDIA_FORMAT
 //#define HUSE_WINDOWS_MEDIA_FORMAT 1
#endif

//==============================================================================
// audio_processors flags:

#ifndef    HPLUGINHOST_VST
 //#define HPLUGINHOST_VST 0
#endif

#ifndef    HPLUGINHOST_VST3
 //#define HPLUGINHOST_VST3 0
#endif

#ifndef    HPLUGINHOST_AU
 //#define HPLUGINHOST_AU 0
#endif

#ifndef    HPLUGINHOST_LADSPA
 //#define HPLUGINHOST_LADSPA 0
#endif

//==============================================================================
// audio_utils flags:

#ifndef    HUSE_CDREADER
 //#define HUSE_CDREADER 0
#endif

#ifndef    HUSE_CDBURNER
 //#define HUSE_CDBURNER 0
#endif

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
 #define   HSTRICT_REFCOUNTEDPOINTER 1
#endif

//==============================================================================
// dsp flags:

#ifndef    HASSERTION_FIRFILTER
 //#define HASSERTION_FIRFILTER 1
#endif

#ifndef    HDSP_USE_INTEL_MKL
 //#define HDSP_USE_INTEL_MKL 0
#endif

#ifndef    HDSP_USE_SHARED_FFTW
 //#define HDSP_USE_SHARED_FFTW 0
#endif

#ifndef    HDSP_USE_STATIC_FFTW
 //#define HDSP_USE_STATIC_FFTW 0
#endif

#ifndef    HDSP_ENABLE_SNAP_TO_ZERO
 //#define HDSP_ENABLE_SNAP_TO_ZERO 1
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
// video flags:

#ifndef    HUSE_CAMERA
 //#define HUSE_CAMERA 0
#endif

#ifndef    HSYNC_VIDEO_VOLUME_WITH_OS_MEDIA_VOLUME
 //#define HSYNC_VIDEO_VOLUME_WITH_OS_MEDIA_VOLUME 1
#endif

//==============================================================================
#ifndef    HSTANDALONE_APPLICATION
 #if defined(JucePlugin_Name) && defined(JucePlugin_Build_Standalone)
  #define  HSTANDALONE_APPLICATION JucePlugin_Build_Standalone
 #else
  #define  HSTANDALONE_APPLICATION 1
 #endif
#endif
