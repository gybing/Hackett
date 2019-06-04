

#pragma once

//==============================================================================
/** Current H version number.

    See also SystemStats::getHVersion() for a string version.
*/
#define HMAJOR_VERSION      5
#define HMINOR_VERSION      4
#define HBUILDNUMBER        3

/** Current H version number.

    Bits 16 to 32 = major version.
    Bits 8 to 16 = minor version.
    Bits 0 to 8 = point release.

    See also SystemStats::getHVersion() for a string version.
*/
#define HVERSION   ((HMAJOR_VERSION << 16) + (HMINOR_VERSION << 8) + HBUILDNUMBER)


//==============================================================================
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
#include <map>

//==============================================================================
#include "CompilerSupport.h"
#include "PlatformDefs.h"

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
 #if HIOS
  #include <signal.h>
 #endif
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

// Include a replacement for std::function
#if HPROHR_LIVE_BUILD
 #include "../misc/StdFunctionCompat.h"
#endif

//==============================================================================
// DLL building settings on Windows
#if HMSVC
 #ifdef HDLL_BUILD
  #define API __declspec (dllexport)
  #pragma warning (disable: 4251)
 #elif defined (HDLL)
  #define API __declspec (dllimport)
  #pragma warning (disable: 4251)
 #endif
 #ifdef __INTEL_COMPILER
  #pragma warning (disable: 1125) // (virtual override warning)
 #endif
#elif defined (HDLL) || defined (HDLL_BUILD)
 #define API __attribute__ ((visibility("default")))
#endif

//==============================================================================
#ifndef API
 #define API   /**< This macro is added to all H public class declarations. */
#endif

#if HMSVC && HDLL_BUILD
 #define HPUBLIC_IN_DLL_BUILD(declaration)  public: declaration; private:
#else
 #define HPUBLIC_IN_DLL_BUILD(declaration)  declaration;
#endif

/** This macro is added to all H public function declarations. */
#define HPUBLIC_FUNCTION        API CALLTYPE

#if (! defined (HCATCH_DEPRECATED_CODE_MISUSE)) && HDEBUG && ! DOXYGEN
 /** This turns on some non-essential bits of code that should prevent old code from compiling
     in cases where method signatures have changed, etc.
 */
 #define HCATCH_DEPRECATED_CODE_MISUSE 1
#endif

#ifndef DOXYGEN
 #define HNAMESPACE H  // This old macro is deprecated: you should just use the H namespace directly.
#endif
