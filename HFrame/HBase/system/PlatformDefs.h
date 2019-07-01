

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

/** This macro defines the C calling convention used as the standard for H calls. */
#if HMSVC
 #define CALLTYPE   __stdcall
 #define HCDECL      __cdecl
#else
 #define CALLTYPE
 #define HCDECL
#endif

//==============================================================================
// Debugging and assertion macros

#ifndef HLOG_CURRENT_ASSERTION
 #if HLOG_ASSERTIONS || HDEBUG
  #define HLOG_CURRENT_ASSERTION    logAssertion (__FILE__, __LINE__);
 #else
  #define HLOG_CURRENT_ASSERTION
 #endif
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
  #define DBG(textToWrite)          HBLOCK_WITH_FORCED_SEMICOLON (Logger::outputDebugString (textToWrite);)

  //==============================================================================
  /** This will always cause an assertion failure.
      It is only compiled in a debug build, (unless HLOG_ASSERTIONS is enabled for your build).
      @see HAssert
  */
  #define HAssertfalse              HBLOCK_WITH_FORCED_SEMICOLON (HLOG_CURRENT_ASSERTION; if (isRunningUnderDebugger()) HBREAK_IN_DEBUGGER; HANALYZER_NORETURN)

  //==============================================================================
  /** Platform-independent assertion macro.

      This macro gets turned into a no-op when you're building with debugging turned off, so be
      careful that the expression you pass to it doesn't perform any actions that are vital for the
      correct behaviour of your program!
      @see HAssertfalse
  */
  #define HAssert(expression)       HBLOCK_WITH_FORCED_SEMICOLON (if (! (expression)) HAssertfalse;)

#else
  //==============================================================================
  // If debugging is disabled, these dummy debug and assertion macros are used..

  #define DBG(textToWrite)
  #define HAssertfalse              HBLOCK_WITH_FORCED_SEMICOLON (HLOG_CURRENT_ASSERTION)

  #if HLOG_ASSERTIONS
   #define HAssert(expression)      HBLOCK_WITH_FORCED_SEMICOLON (if (! (expression)) HAssertfalse;)
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


