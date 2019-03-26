#pragma once

//==============================================================================
/*  This file figures out which platform is being built, and defines some macros
    that the rest of the code can use for OS-specific compilation.

    Macros that will be set here are:

    - One of HWINDOWS, HMAC HLINUX, HIOS, HANDROID, etc.
    - Either H32BIT or H64BIT, depending on the architecture.
    - Either HLITTLE_ENDIAN or HBIG_ENDIAN.
    - Either HINTEL or HARM
    - Either HGCC or HCLANG or HMSVC
*/

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
  #include <CoreFoundation/CoreFoundation.h> // (needed to find out what platform we're using)
  
  // This hack is a workaround for a bug (?) in Apple's 10.11 SDK headers
  // which cause some configurations of Clang to throw out a spurious error..
  #if HPROHR_LIVE_BUILD && (defined (__APPLE_CPP__) || defined(__APPLE_CC__))
  #include <CoreFoundation/CFAvailability.h>
  #undef CF_OPTIONS
  #define CF_OPTIONS(_type, _name) _type _name; enum
  
  // This is a workaround for the Xcode 9 version of NSUUID.h causing some errors
  // in the live-build engine.
  #define _Nullable
  #define _Nonnull
  #endif

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

  #if HMAC && MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_5
    #error "Building for OSX 10.4 is no longer supported!"
  #endif

  #if HMAC && ! defined (MAC_OS_X_VERSION_10_6)
    #error "To build with 10.5 compatibility, use a later SDK and set the deployment target to 10.5"
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

//==============================================================================
// System Headers
#include <memory>
#include <cmath>
#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>
#include <limits>
#include <atomic>
#include <sstream>
#include <iomanip>

#include <string>
#include <set>
#include <map>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <locale>
#include <codecvt>
#include <exception>

//==============================================================================
/*
   This file provides flags for compiler features that aren't supported on all platforms.
*/

//==============================================================================
// GCC
#if HGCC

#if (__GNUC__ * 100 + __GNUC_MINOR__) < 407
#error "H requires GCC 4.7 or later"
#endif

#if ! (__cplusplus >= 201103L || defined (__GXX_EXPERIMENTAL_CXX0X__))
#error "H requires that GCC has C++11 compatibility enabled"
#endif

#define HCOMPILER_SUPPORTS_NOEXCEPT 1

#if (__GNUC__ * 100 + __GNUC_MINOR__) >= 500
#define HHAS_CONSTEXPR 1
#endif

#ifndef HEXCEPTIONS_DISABLED
#if ! __EXCEPTIONS
#define HEXCEPTIONS_DISABLED 1
#endif
#endif

#define HCXX14_IS_AVAILABLE (__cplusplus >= 201402L)
#define HCXX17_IS_AVAILABLE (__cplusplus >= 201703L)

#endif

//==============================================================================
// Clang
#if HCLANG

#if (__clang_major__ < 3) || (__clang_major__ == 3 && __clang_minor__ < 3)
#error "H requires Clang 3.3 or later"
#endif

#define HCOMPILER_SUPPORTS_NOEXCEPT 1
#define HHAS_CONSTEXPR 1

#ifndef HCOMPILER_SUPPORTS_ARC
#define HCOMPILER_SUPPORTS_ARC 1
#endif

#ifndef HEXCEPTIONS_DISABLED
#if ! __has_feature (cxx_exceptions)
#define HEXCEPTIONS_DISABLED 1
#endif
#endif

#define HCXX14_IS_AVAILABLE (__cplusplus >= 201402L)
#define HCXX17_IS_AVAILABLE (__cplusplus >= 201703L)

#endif

//==============================================================================
// MSVC
#if HMSVC

#if _MSC_VER < 1800 // VS2013
#error "H requires Visual Studio 2013 or later"
#endif

#if _MSC_VER >= 1900 // VS2015
#define HCOMPILER_SUPPORTS_NOEXCEPT 1
#define HHAS_CONSTEXPR 1
#else
#define _ALLOW_KEYWORD_MACROS 1 // prevent a warning
#undef  noexcept
#define noexcept  throw()
#endif

#ifndef HEXCEPTIONS_DISABLED
#if ! _CPPUNWIND
#define HEXCEPTIONS_DISABLED 1
#endif
#endif

#define HCXX14_IS_AVAILABLE (_MSVC_LANG >= 201402L)
#define HCXX17_IS_AVAILABLE (_MSVC_LANG >= 201703L)
#endif

//==============================================================================
// C++ library
#if (defined (__GLIBCXX__) && __GLIBCXX__ < 20130322) || (defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION < 3700))
#error "H requires a C++ library containing std::atomic"
#endif

//==============================================================================
#if HHAS_CONSTEXPR
#define HCONSTEXPR constexpr
#else
#define HCONSTEXPR
#endif

#if (! HMSVC) && (! HCXX14_IS_AVAILABLE)
namespace std
{
	template<typename T, typename... Args>
	unique_ptr<T> make_unique(Args&&... args)
	{
		return unique_ptr<T>(new T(forward<Args>(args)...));
	}
}
#endif

#if ! DOXYGEN
// These are old flags that are now supported on all compatible build targets
#define HCOMPILER_SUPPORTS_OVERRIDE_AND_FINAL 1
#define HCOMPILER_SUPPORTS_VARIADIC_TEMPLATES 1
#define HCOMPILER_SUPPORTS_INITIALIZER_LISTS 1
#define HDELETED_FUNCTION = delete
#endif

//==============================================================================
/*  This file defines miscellaneous macros for debugging, assertions, etc.
*/

//==============================================================================
#ifdef HFORCE_DEBUG
#undef HDEBUG

#if HFORCE_DEBUG
#define HDEBUG 1
#endif
#endif

/** This macro defines the C calling convention used as the standard for HBasecalls. */
#if HMSVC
#define HCALLTYPE   __stdcall
#define HCDECL      __cdecl
#else
#define HCALLTYPE
#define HCDECL
#endif

//==============================================================================
// Debugging and assertion macros

#if HLOG_ASSERTIONS
#define HLOG_CURRENT_ASSERTION    logAssertion (__FILE__, __LINE__);
#else
#define HLOG_CURRENT_ASSERTION
#endif

//==============================================================================
#if HIOS || HLINUX
  /** This will try to break into the debugger if the app is currently being debugged.
	  If called by an app that's not being debugged, the behaviour isn't defined - it may
	  crash or not, depending on the platform.
	  @see HAssert()
  */
#define HBREAK_IN_DEBUGGER        { ::kill (0, SIGTRAP); }
#elif HMSVC
#ifndef __INTEL_COMPILER
#pragma intrinsic (__debugbreak)
#endif
#define HBREAK_IN_DEBUGGER        { __debugbreak(); }
#elif HGCC || HMAC
#if HNO_INLINE_ASM
#define HBREAK_IN_DEBUGGER       { }
#else
#define HBREAK_IN_DEBUGGER       { asm ("int $3"); }
#endif
#elif HANDROID
#define HBREAK_IN_DEBUGGER        { __builtin_trap(); }
#else
#define HBREAK_IN_DEBUGGER        { __asm int 3 }
#endif

#if HCLANG && defined (__has_feature) && ! defined (HANALYZER_NORETURN)
#if __has_feature (attribute_analyzer_noreturn)
inline void __attribute__((analyzer_noreturn)) assert_noreturn() {}
#define HANALYZER_NORETURN assert_noreturn();
#endif
#endif

#ifndef HANALYZER_NORETURN
#define HANALYZER_NORETURN
#endif

//==============================================================================
#if HMSVC && ! DOXYGEN
#define HBLOCK_WITH_FORCED_SEMICOLON(x) \
   __pragma(warning(push)) \
   __pragma(warning(disable:4127)) \
   do { x } while (false) \
   __pragma(warning(pop))
#else
 /** This is the good old C++ trick for creating a macro that forces the user to put
	a semicolon after it when they use it.
 */
#define HBLOCK_WITH_FORCED_SEMICOLON(x) do { x } while (false)
#endif

 //==============================================================================
#if (HDEBUG && ! HDISABLE_ASSERTIONS) || DOXYGEN
  /** Writes a string to the standard error stream.
	  Note that as well as a single string, you can use this to write multiple items
	  as a stream, e.g.
	  @code
		DBG ("foo = " << foo << "bar = " << bar);
	  @endcode
	  The macro is only enabled in a debug build, so be careful not to use it with expressions
	  that have important side-effects!
	  @see Logger::outputDebugString
  */
#define DBG(textToWrite)          HBLOCK_WITH_FORCED_SEMICOLON (String tempDbgBuf; tempDbgBuf << textToWrite; Logger::outputDebugString (tempDbgBuf);)

  //==============================================================================
  /** This will always cause an assertion failure.
	  It is only compiled in a debug build, (unless HLOG_ASSERTIONS is enabled for your build).
	  @see HAssert
  */
#define HAssertFalse              HBLOCK_WITH_FORCED_SEMICOLON (HLOG_CURRENT_ASSERTION; if (isRunningUnderDebugger()) HBREAK_IN_DEBUGGER; HANALYZER_NORETURN)

  //==============================================================================
  /** Platform-independent assertion macro.

	  This macro gets turned into a no-op when you're building with debugging turned off, so be
	  careful that the expression you pass to it doesn't perform any actions that are vital for the
	  correct behaviour of your program!
	  @see HAssertFalse
  */
#define HAssert(expression)       HBLOCK_WITH_FORCED_SEMICOLON (if (! (expression)) assert(false);)

#else
  //==============================================================================
  // If debugging is disabled, these dummy debug and assertion macros are used..

#define DBG(textToWrite)
#define HAssertFalse              HBLOCK_WITH_FORCED_SEMICOLON (HLOG_CURRENT_ASSERTION)

#if HLOG_ASSERTIONS
#define HAssert(expression)      HBLOCK_WITH_FORCED_SEMICOLON (if (! (expression)) HAssertFalse;)
#else
#define HAssert(expression)      HBLOCK_WITH_FORCED_SEMICOLON ( ; )
#endif

#endif

//==============================================================================
#if ! DOXYGEN
#define HJOIN_MACRO_HELPER(a, b) a ## b
#define HSTRINGIFY_MACRO_HELPER(a) #a
#endif

/** A good old-fashioned C macro concatenation helper.
	This combines two items (which may themselves be macros) into a single string,
	avoiding the pitfalls of the ## macro operator.
*/
#define HJOIN_MACRO(item1, item2)  HJOIN_MACRO_HELPER (item1, item2)

/** A handy C macro for stringifying any symbol, rather than just a macro parameter. */
#define HSTRINGIFY(item)  HSTRINGIFY_MACRO_HELPER (item)

//==============================================================================
/** This is a shorthand macro for declaring stubs for a class's copy constructor and operator=.

	For example, instead of
	@code
	class MyClass
	{
		etc..

	private:
		MyClass (const MyClass&);
		MyClass& operator= (const MyClass&);
	};@endcode

	..you can just write:

	@code
	class MyClass
	{
		etc..

	private:
		HDECLARE_NON_COPYABLE (MyClass)
	};@endcode
*/
#define HDECLARE_NON_COPYABLE(className) \
    className (const className&) = delete;\
    className& operator= (const className&) = delete;

/** This is a shorthand way of writing both a HDECLARE_NON_COPYABLE and
	HLEAK_DETECTOR macro for a class.
*/
#define HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(className) \
    HDECLARE_NON_COPYABLE(className) \
    HLEAK_DETECTOR(className)

/** This macro can be added to class definitions to disable the use of new/delete to
	allocate the object on the heap, forcing it to only be used as a stack or member variable.
*/
#define HPREVENT_HEAP_ALLOCATION \
   private: \
    static void* operator new (size_t) = delete; \
    static void operator delete (void*) = delete;

//==============================================================================
#if HMSVC && ! defined (DOXYGEN)
#define HWARNING_HELPER(file, line, mess) message(file "(" HSTRINGIFY (line) ") : Warning: " #mess)
#define HCOMPILER_WARNING(message)  __pragma(HWARNING_HELPER (__FILE__, __LINE__, message))
#else
#ifndef DOXYGEN
#define HWARNING_HELPER(mess) message(#mess)
#endif

/** This macro allows you to emit a custom compiler warning message.
	Very handy for marking bits of code as "to-do" items, or for shaming
	code written by your co-workers in a way that's hard to ignore.

	GCC and Clang provide the \#warning directive, but MSVC doesn't, so this macro
	is a cross-compiler way to get the same functionality as \#warning.
*/
#define HCOMPILER_WARNING(message)  _Pragma(HSTRINGIFY (HWARNING_HELPER (message)))
#endif


//==============================================================================
#if HDEBUG || DOXYGEN
  /** A platform-independent way of forcing an inline function.
	  Use the syntax: @code
	  forcedinline void myfunction (int x)
	  @endcode
  */
#define forcedinline  inline
#else
#if HMSVC
#define forcedinline       __forceinline
#else
#define forcedinline       inline __attribute__((always_inline))
#endif
#endif

#if HMSVC || DOXYGEN
  /** This can be placed before a stack or member variable declaration to tell the compiler
	  to align it to the specified number of bytes. */
#define HALIGN(bytes)   __declspec (align (bytes))
#else
#define HALIGN(bytes)   __attribute__ ((aligned (bytes)))
#endif

//==============================================================================
// Cross-compiler deprecation macros..
#ifdef DOXYGEN
 /** This macro can be used to wrap a function which has been deprecated. */
#define HDEPRECATED(functionDef)
#define HDEPRECATED_WITH_BODY(functionDef, body)
#elif HMSVC && ! HNO_DEPRECATION_WARNINGS
#define HDEPRECATED_ATTRIBUTE                      __declspec(deprecated)
#define HDEPRECATED(functionDef)                   HDEPRECATED_ATTRIBUTE functionDef
#define HDEPRECATED_WITH_BODY(functionDef, body)   HDEPRECATED_ATTRIBUTE functionDef body
#elif (HGCC || HCLANG) && ! HNO_DEPRECATION_WARNINGS
#define HDEPRECATED_ATTRIBUTE                      __attribute__ ((deprecated))
#define HDEPRECATED(functionDef)                   functionDef HDEPRECATED_ATTRIBUTE
#define HDEPRECATED_WITH_BODY(functionDef, body)   functionDef HDEPRECATED_ATTRIBUTE body
#else
#define HDEPRECATED_ATTRIBUTE
#define HDEPRECATED(functionDef)                   functionDef
#define HDEPRECATED_WITH_BODY(functionDef, body)   functionDef body
#endif

#if HALLOW_STATIC_NULL_VARIABLES
#if ! (defined (DOXYGEN) || defined (HGCC) || (HMSVC && _MSC_VER <= 1900))
#define HDEPRECATED_STATIC(valueDef)       HDEPRECATED_ATTRIBUTE valueDef

#if HMSVC
#define HDECLARE_DEPRECATED_STATIC(valueDef) \
        __pragma(warning(push)) \
        __pragma(warning(disable:4996)) \
         valueDef \
        __pragma(warning(pop))
#else
#define HDECLARE_DEPRECATED_STATIC(valueDef)   valueDef
#endif
#else
#define HDEPRECATED_STATIC(valueDef)           valueDef
#define HDECLARE_DEPRECATED_STATIC(valueDef)   valueDef
#endif
#else
#define HDEPRECATED_STATIC(valueDef)
#define HDECLARE_DEPRECATED_STATIC(valueDef)
#endif

//==============================================================================
#if HANDROID && ! DOXYGEN
#define HMODAL_LOOPS_PERMITTED 0
#elif ! defined (HMODAL_LOOPS_PERMITTED)
 /** Some operating environments don't provide a modal loop mechanism, so this flag can be
	 used to disable any functions that try to run a modal loop. */
#define HMODAL_LOOPS_PERMITTED 1
#endif

	 //==============================================================================
#if HGCC || HCLANG
#define HPACKED __attribute__((packed))
#elif ! DOXYGEN
#define HPACKED
#endif

//==============================================================================
#if HGCC || DOXYGEN
 /** This can be appended to a function declaration to tell gcc to disable associative
	 math optimisations which break some floating point algorithms. */
#define HNO_ASSOCIATIVE_MATH_OPTIMISATIONS   __attribute__((__optimize__("no-associative-math")))
#else
#define HNO_ASSOCIATIVE_MATH_OPTIMISATIONS
#endif


//==============================================================================
// Now we'll include some common OS headers..
#if HMSVC
#pragma warning (push)
#pragma warning (disable: 4514 4245 4100)
#include <intrin.h>
#endif

#if HMAC || HIOS
#include <libkern/OSAtomic.h>
#include <xlocale.h>
#endif

#if HLINUX
#include <cstring>
#include <signal.h>

#if __INTEL_COMPILER
#if __ia64__
#include <ia64intrin.h>
#else
#include <ia32intrin.h>
#endif
#endif
#endif

#if HMSVC && HDEBUG
#include <crtdbg.h>
#endif

#if HMSVC
#pragma warning (pop)
#endif

#if HMINGW
#include <cstring>
#include <sys/types.h>
#endif

#if HANDROID
#include <cstring>
#include <atomic>
#include <byteswap.h>
#endif

// undef symbols that are sometimes set by misguided 3rd-party headers..
#undef TYPE_BOOL
#undef max
#undef min
#undef major
#undef minor
#undef KeyPress

//==============================================================================
// DLL building settings on Windows
#if HMSVC
#ifdef HDLL_BUILD
#define HAPI __declspec (dllexport)
#pragma warning (disable: 4251)
#elif defined (HDLL)
#define HAPI __declspec (dllimport)
#pragma warning (disable: 4251)
#endif
#ifdef __INTEL_COMPILER
#pragma warning (disable: 1125) // (virtual override warning)
#endif
#elif defined (HDLL) || defined (HDLL_BUILD)
#define HAPI __attribute__ ((visibility("default")))
#endif

//==============================================================================
#ifndef HAPI
#define HAPI   /**< This macro is added to all HBasepublic class declarations. */
#endif

#if HMSVC && HDLL_BUILD
#define HPUBLIC_IN_DLL_BUILD(declaration)  public: declaration; private:
#else
#define HPUBLIC_IN_DLL_BUILD(declaration)  declaration;
#endif

/** This macro is added to all HBasepublic function declarations. */
#define HPUBLIC_FUNCTION        HAPI HCALLTYPE

#if (! defined (HCATCH_DEPRECATED_CODE_MISUSE)) && HDEBUG && ! DOXYGEN
 /** This turns on some non-essential bits of code that should prevent old code from compiling
	 in cases where method signatures have changed, etc.
 */
#define HCATCH_DEPRECATED_CODE_MISUSE 1
#endif

//==============================================================================
// Native Headers
#undef T

//==============================================================================
#if HMAC || HIOS

#if HIOS
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <CoreData/CoreData.h>
#import <MobileCoreServices/MobileCoreServices.h>
#include <sys/fcntl.h>
#else
#import <Cocoa/Cocoa.h>
#if (! defined MAC_OS_X_VERSION_10_12) || MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_12
#define NSEventModifierFlagCommand       NSCommandKeyMask
#define NSEventModifierFlagControl       NSControlKeyMask
#define NSEventModifierFlagHelp          NSHelpKeyMask
#define NSEventModifierFlagNumericPad    NSNumericPadKeyMask
#define NSEventModifierFlagOption        NSAlternateKeyMask
#define NSEventModifierFlagShift         NSShiftKeyMask
#define NSCompositingOperationSourceOver NSCompositeSourceOver
#define NSEventMaskApplicationDefined    NSApplicationDefinedMask
#define NSEventTypeApplicationDefined    NSApplicationDefined
#define NSEventTypeCursorUpdate          NSCursorUpdate
#define NSEventTypeMouseMoved            NSMouseMoved
#define NSEventTypeLeftMouseDown         NSLeftMouseDown
#define NSEventTypeRightMouseDown        NSRightMouseDown
#define NSEventTypeOtherMouseDown        NSOtherMouseDown
#define NSEventTypeLeftMouseUp           NSLeftMouseUp
#define NSEventTypeRightMouseUp          NSRightMouseUp
#define NSEventTypeOtherMouseUp          NSOtherMouseUp
#define NSEventTypeLeftMouseDragged      NSLeftMouseDragged
#define NSEventTypeRightMouseDragged     NSRightMouseDragged
#define NSEventTypeOtherMouseDragged     NSOtherMouseDragged
#define NSEventTypeScrollWheel           NSScrollWheel
#define NSEventTypeKeyDown               NSKeyDown
#define NSEventTypeKeyUp                 NSKeyUp
#define NSEventTypeFlagsChanged          NSFlagsChanged
#define NSEventMaskAny                   NSAnyEventMask
#define NSWindowStyleMaskBorderless      NSBorderlessWindowMask
#define NSWindowStyleMaskClosable        NSClosableWindowMask
#define NSWindowStyleMaskFullScreen      NSFullScreenWindowMask
#define NSWindowStyleMaskMiniaturizable  NSMiniaturizableWindowMask
#define NSWindowStyleMaskResizable       NSResizableWindowMask
#define NSWindowStyleMaskTitled          NSTitledWindowMask
#define NSAlertStyleCritical             NSCriticalAlertStyle
#define NSControlSizeRegular             NSRegularControlSize
#define NSEventTypeMouseEntered          NSMouseEntered
#define NSEventTypeMouseExited           NSMouseExited
#define NSAlertStyleInformational        NSInformationalAlertStyle
#define NSEventTypeTabletPoint           NSTabletPoint
#define NSEventTypeTabletProximity       NSTabletProximity
#endif
#import <CoreAudio/HostTime.h>
#include <sys/dir.h>
#endif

#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/utsname.h>
#include <sys/mman.h>
#include <fnmatch.h>
#include <utime.h>
#include <dlfcn.h>
#include <ifaddrs.h>
#include <net/if_dl.h>
#include <mach/mach_time.h>
#include <mach-o/dyld.h>
#include <objc/runtime.h>
#include <objc/objc.h>
#include <objc/message.h>

//==============================================================================
#elif HWINDOWS
#if HMSVC
#ifndef _CPPRTTI
#error "You're compiling without RTTI enabled! This is needed for a lot of HBaseclasses, please update your compiler settings!"
#endif

#ifndef _CPPUNWIND
#error "You're compiling without exceptions enabled! This is needed for a lot of HBaseclasses, please update your compiler settings!"
#endif

#pragma warning (push, 0) // disable all warnings whilst including system headers
#endif

#define NOMINMAX

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#define STRICT 1
#define WIN32_LEAN_AND_MEAN 1
#if HMINGW
#define _WIN32_WINNT 0x0501
#else
#define _WIN32_WINNT 0x0602
#endif
#define _UNICODE 1
#define UNICODE 1
#ifndef _WIN32_IE
#define _WIN32_IE 0x0500
#endif

#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <stddef.h>
#include <ctime>
#include <wininet.h>
#include <nb30.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <mapi.h>
#include <float.h>
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <mmsystem.h>
#include <winioctl.h>
#include <ioapiset.h>
#include <MSWSock.h>
#if HMINGW
#include <basetyps.h>
#include <sys/time.h>
#ifndef alloca
#define alloca __builtin_alloca
#endif
#else
#include <crtdbg.h>
#include <comutil.h>
#endif

#ifndef S_FALSE
#define S_FALSE (1) // (apparently some obscure win32 dev environments don't define this)
#endif

#undef PACKED

#if HMSVC
#pragma warning (pop)
#pragma warning (4: 4511 4512 4100)
#endif

#if HMSVC && ! HDONT_AUTOLINK_TO_WIN32_LIBRARIES
#pragma comment (lib, "kernel32.lib")
#pragma comment (lib, "user32.lib")
#pragma comment (lib, "wininet.lib")
#pragma comment (lib, "advapi32.lib")
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "version.lib")
#pragma comment (lib, "shlwapi.lib")
#pragma comment (lib, "winmm.lib")

#ifdef _NATIVE_WCHAR_T_DEFINED
#ifdef _DEBUG
#pragma comment (lib, "comsuppwd.lib")
#else
#pragma comment (lib, "comsuppw.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment (lib, "comsuppd.lib")
#else
#pragma comment (lib, "comsupp.lib")
#endif
#endif
#endif

/* Used with DynamicLibrary to simplify importing functions from a win32 DLL.

   dll: the DynamicLibrary object
   functionName: function to import
   localFunctionName: name you want to use to actually call it (must be different)
   returnType: the return type
   params: list of params (bracketed)
*/
#define HLOAD_WINAPI_FUNCTION(dll, functionName, localFunctionName, returnType, params) \
    typedef returnType (WINAPI *type##localFunctionName) params; \
    type##localFunctionName localFunctionName = (type##localFunctionName) dll.getFunction (#functionName);

//==============================================================================
#elif HLINUX
#include <arpa/inet.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <pwd.h>
#include <sched.h>
#include <signal.h>
#include <stddef.h>
#include <sys/dir.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <sys/wait.h>
#include <utime.h>

//==============================================================================
#elif HBSD
#include <arpa/inet.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <pwd.h>
#include <sched.h>
#include <signal.h>
#include <stddef.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <utime.h>

//==============================================================================
#elif HANDROID
#include <jni.h>
#include <pthread.h>
#include <sched.h>
#include <sys/time.h>
#include <utime.h>
#include <errno.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/ptrace.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <pwd.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/wait.h>
#include <android/api-level.h>

// If you are getting include errors here, then you to re-build the Projector
// and re-save your .Hr file.
#include <cpu-features.h>
#endif

// Need to clear various moronic redefinitions made by system headers..
#undef max
#undef min
#undef direct
#undef check

#if HWINDOWS
#undef small
#endif

//==============================================================================
// Definitions for the int8, int16, int32, int64 and pointer_sized_int types.

/** A platform-independent 8-bit signed integer type. */
using int8 = signed char;
/** A platform-independent 8-bit unsigned integer type. */
using uint8 = unsigned char;
/** A platform-independent 16-bit signed integer type. */
using int16 = signed short;
/** A platform-independent 16-bit unsigned integer type. */
using uint16 = unsigned short;
/** A platform-independent 32-bit signed integer type. */
using int32 = signed int;
/** A platform-independent 32-bit unsigned integer type. */
using uint32 = unsigned int;

#if HMSVC
/** A platform-independent 64-bit integer type. */
using int64 = __int64;
/** A platform-independent 64-bit unsigned integer type. */
using uint64 = unsigned __int64;
#else
/** A platform-independent 64-bit integer type. */
using int64 = long long;
/** A platform-independent 64-bit unsigned integer type. */
using uint64 = unsigned long long;
#endif