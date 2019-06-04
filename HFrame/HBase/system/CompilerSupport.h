

#pragma once

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

 #define HCXX14_IS_AVAILABLE ((__cplusplus >= 201402L) || ((__GNUC__ * 100 + __GNUC_MINOR__) >= 409 && (__cplusplus >= 201300L)))
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
    unique_ptr<T> make_unique (Args&&... args)
    {
        return unique_ptr<T> (new T (forward<Args> (args)...));
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
