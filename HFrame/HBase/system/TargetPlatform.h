

#pragma once
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
#define   HUSE_MP3AUDIOFORMAT 1
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
#define   HALLOW_STATIC_NULL_VARIABLES 0
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
#define   HUSE_CAMERA 1
#endif

#ifndef    HSYNC_VIDEO_VOLUME_WITH_OS_MEDIA_VOLUME
 //#define HSYNC_VIDEO_VOLUME_WITH_OS_MEDIA_VOLUME 1
#endif

//==============================================================================
// video flags:
#ifndef    HUNIT_TESTS
#define   HUNIT_TESTS 1
#endif


//==============================================================================
#ifndef    HSTANDALONE_APPLICATION
#if defined(JucePlugin_Name) && defined(JucePlugin_Build_Standalone)
#define  HSTANDALONE_APPLICATION JucePlugin_Build_Standalone
#else
#define  HSTANDALONE_APPLICATION 1
#endif
#endif


//==============================================================================
#if defined (_WIN32) || defined (_WIN64)
  #define       HWIN32 1
  #define       HWINDOWS 1
#elif defined (HANDROID)
  #undef        HANDROID
  #define       HANDROID 1
#elif defined (__FreeBSD__) || (__OpenBSD__)
  #define       HBSD 1
#elif defined (LINUX) || defined (__linux__)
  #define     HLINUX 1
#elif defined (__APPLE_CPP__) || defined (__APPLE_CC__)
  #define CF_EXCLUDE_CSTD_HEADERS 1
  #include <CoreFoundation/CoreFoundation.h> // (needed to find out what platform we're using)
  #include "../native/mac_ClangBugWorkaround.h"

  #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    #define     HIPHONE 1
    #define     HIOS 1
  #else
    #define     HMAC 1
  #endif
#else
  #error "Unknown platform!"
#endif

//==============================================================================
#if HWINDOWS
  #ifdef _MSC_VER
    #ifdef _WIN64
      #define H64BIT 1
    #else
      #define H32BIT 1
    #endif
  #endif

  #ifdef _DEBUG
    #define HDEBUG 1
  #endif

  #ifdef __MINGW32__
    #define HMINGW 1
    #ifdef __MINGW64__
      #define H64BIT 1
    #else
      #define H32BIT 1
    #endif
  #endif

  /** If defined, this indicates that the processor is little-endian. */
  #define HLITTLE_ENDIAN 1

  #define HINTEL 1
#endif

//==============================================================================
#if HMAC || HIOS

  #if defined (DEBUG) || defined (_DEBUG) || ! (defined (NDEBUG) || defined (_NDEBUG))
    #define HDEBUG 1
  #endif

  #if ! (defined (DEBUG) || defined (_DEBUG) || defined (NDEBUG) || defined (_NDEBUG))
    #warning "Neither NDEBUG or DEBUG has been defined - you should set one of these to make it clear whether this is a release build,"
  #endif

  #ifdef __LITTLE_ENDIAN__
    #define HLITTLE_ENDIAN 1
  #else
    #define HBIG_ENDIAN 1
  #endif

  #ifdef __LP64__
    #define H64BIT 1
  #else
    #define H32BIT 1
  #endif

  #if defined (__ppc__) || defined (__ppc64__)
    #error "PowerPC is no longer supported by H!"
  #elif defined (__arm__) || defined (__arm64__)
    #define HARM 1
  #else
    #define HINTEL 1
  #endif

  #if HMAC
    #if ! defined (MAC_OS_X_VERSION_10_11)
      #error "The 10.11 SDK (Xcode 7.3.1+) is required to build H apps. You can create apps that run on macOS 10.7+ by changing the deployment target."
    #elif MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_7
      #error "Building for OSX 10.6 is no longer supported!"
    #endif
  #endif
#endif

//==============================================================================
#if HLINUX || HANDROID

  #ifdef _DEBUG
    #define HDEBUG 1
  #endif

  // Allow override for big-endian Linux platforms
  #if defined (__LITTLE_ENDIAN__) || ! defined (HBIG_ENDIAN)
    #define HLITTLE_ENDIAN 1
    #undef HBIG_ENDIAN
  #else
    #undef HLITTLE_ENDIAN
    #define HBIG_ENDIAN 1
  #endif

  #if defined (__LP64__) || defined (_LP64) || defined (__arm64__)
    #define H64BIT 1
  #else
    #define H32BIT 1
  #endif

  #if defined (__arm__) || defined (__arm64__) || defined (__aarch64__)
    #define HARM 1
  #elif __MMX__ || __SSE__ || __amd64__
    #define HINTEL 1
  #endif
#endif

//==============================================================================
// Compiler type macros.

#ifdef __clang__
  #define HCLANG 1

#elif defined (__GNUC__)
  #define HGCC 1

#elif defined (_MSC_VER)
  #define HMSVC 1

#else
  #error unknown compiler
#endif
