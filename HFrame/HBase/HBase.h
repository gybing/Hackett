#pragma once
#define HCORE_H_INCLUDED

//==============================================================================
#ifdef _MSC_VER
#pragma warning(push)
// Disable warnings for long class names, padding, and undefined preprocessor definitions.
#pragma warning(disable : 4251 4786 4668 4820)
#ifdef __INTEL_COMPILER
#pragma warning(disable : 1125)
#endif
#endif

#include "system/TargetPlatform.h"

//==============================================================================
/** Config: HFORCE_DEBUG

    Normally, HDEBUG is set to 1 or 0 based on compiler and project settings,
    but if you define this value, you can override this to force it to be true or false.
*/
#ifndef HFORCE_DEBUG
//#define HFORCE_DEBUG 0
#endif

//==============================================================================
/** Config: HLOG_ASSERTIONS

    If this flag is enabled, the HAssert and HAssertfalse macros will always use Logger::writeToLog()
    to write a message when an assertion happens.

    Enabling it will also leave this turned on in release builds. When it's disabled,
    however, the HAssert and HAssertfalse macros will not be compiled in a
    release build.

    @see HAssert, HAssertfalse, Logger
*/
#ifndef HLOG_ASSERTIONS
#if HANDROID
#define HLOG_ASSERTIONS 1
#else
#define HLOG_ASSERTIONS 0
#endif
#endif

//==============================================================================
/** Config: HCHECK_MEMORY_LEAKS

    Enables a memory-leak check for certain objects when the app terminates. See the LeakedObjectDetector
    class and the HLEAK_DETECTOR macro for more details about enabling leak checking for specific classes.
*/
#if HDEBUG && !defined(HCHECK_MEMORY_LEAKS)
#define HCHECK_MEMORY_LEAKS 1
#endif

//==============================================================================
/** Config: HDONT_AUTOLINK_TO_WIN32_LIBRARIES

    In a Visual C++  build, this can be used to stop the required system libs being
    automatically added to the link stage.
*/
#ifndef HDONT_AUTOLINK_TO_WIN32_LIBRARIES
#define HDONT_AUTOLINK_TO_WIN32_LIBRARIES 0
#endif

/** Config: HINCLUDE_ZLIB_CODE
    This can be used to disable Juce's embedded 3rd-party zlib code.
    You might need to tweak this if you're linking to an external zlib library in your app,
    but for normal apps, this option should be left alone.

    If you disable this, you might also want to set a value for HZLIB_INCLUDE_PATH, to
    specify the path where your zlib headers live.
*/
#ifndef HINCLUDE_ZLIB_CODE
#define HINCLUDE_ZLIB_CODE 1
#endif

#ifndef HZLIB_INCLUDE_PATH
#define HZLIB_INCLUDE_PATH <zlib.h>
#endif

/** Config: HUSE_CURL
    Enables http/https support via libcurl (Linux only). Enabling this will add an additional
    run-time dynamic dependency to libcurl.

    If you disable this then https/ssl support will not be available on linux.
*/
#ifndef HUSE_CURL
#if HLINUX
#define HUSE_CURL 1
#else
#define HUSE_CURL 0
#endif
#endif

/** Config: HLOAD_CURL_SYMBOLS_LAZILY
    If enabled, H will load libcurl lazily when required (for example, when WebInputStream
    is used). Enabling this flag may also help with library dependency erros as linking
    libcurl at compile-time may instruct the linker to hard depend on a specific version
    of libcurl. It's also useful if you want to limit the amount of H dependencies and
    you are not using WebInputStream or the URL classes.
*/
#ifndef HLOAD_CURL_SYMBOLS_LAZILY
#define HLOAD_CURL_SYMBOLS_LAZILY 0
#endif

/** Config: HCATCH_UNHANDLED_EXCEPTIONS
    If enabled, this will add some exception-catching code to forward unhandled exceptions
    to your HApplicationBase::unhandledException() callback.
*/
#ifndef HCATCH_UNHANDLED_EXCEPTIONS
#define HCATCH_UNHANDLED_EXCEPTIONS 0
#endif

/** Config: HALLOW_STATIC_NULL_VARIABLES
    If disabled, this will turn off dangerous static globals like String::empty, var::null, etc
    which can cause nasty order-of-initialisation problems if they are referenced during static
    constructor code.
*/
#ifndef HALLOW_STATIC_NULL_VARIABLES
#define HALLOW_STATIC_NULL_VARIABLES 0
#endif

/** Config: HSTRICT_REFCOUNTEDPOINTER
    If enabled, this will make the ReferenceCountedObjectPtr class stricter about allowing
    itself to be cast directly to a raw pointer. By default this is disabled, for compatibility
    with old code, but if possible, you should always enable it to improve code safety!
*/
#ifndef HSTRICT_REFCOUNTEDPOINTER
#define HSTRICT_REFCOUNTEDPOINTER 0
#endif

#ifndef HSTRING_UTF_TYPE
#define HSTRING_UTF_TYPE 8
#endif

//==============================================================================

#if HCORE_INCLUDE_NATIVE_HEADERS
#include "native/BasicNativeHeaders.h"
#endif

#if HWINDOWS
#undef small
#endif

#include "system/StandardHeader.h"

class StringRef;
class MemoryBlock;
class File;
class InputStream;
class OutputStream;
class DynamicObject;
class FileInputStream;
class FileOutputStream;
class XmlElement;

extern API bool CALLTYPE isRunningUnderDebugger() noexcept;
extern API void CALLTYPE logAssertion(const char *file, int line) noexcept;

#include "memory/Memory.h"
#include "maths/MathsFunctions.h"
#include "memory/ByteOrder.h"
#include "memory/Atomic.h"
#include "text/CharacterFunctions.h"

#if HMSVC
#pragma warning(push)
#pragma warning(disable : 4514 4996)
#endif

#include "text/CharPointer_UTF8.h"
#include "text/CharPointer_UTF16.h"
#include "text/CharPointer_UTF32.h"
#include "text/CharPointer_ASCII.h"

#if HMSVC
#pragma warning(pop)
#endif

#include "text/String.h"
#include "text/StringRef.h"
#include "logging/Logger.h"
#include "memory/LeakedObjectDetector.h"
#include "memory/ContainerDeletePolicy.h"
#include "memory/HeapBlock.h"
#include "memory/MemoryBlock.h"
#include "memory/ReferenceCountedObject.h"
#include "memory/ScopedPointer.h"
#include "memory/OptionalScopedPointer.h"
#include "memory/Singleton.h"
#include "memory/WeakReference.h"
#include "threads/ScopedLock.h"
#include "threads/CriticalSection.h"
#include "maths/Range.h"
#include "maths/NormalisableRange.h"
#include "maths/StatisticsAccumulator.h"
#include "containers/ElementComparator.h"
#include "containers/ArrayAllocationBase.h"
#include "containers/ArrayBase.h"
#include "containers/Array.h"
#include "containers/LinkedListPointer.h"
#include "containers/ListenerList.h"
#include "containers/OwnedArray.h"
#include "containers/ReferenceCountedArray.h"
#include "containers/ScopedValueSetter.h"
#include "containers/SortedSet.h"
#include "containers/SparseSet.h"
#include "containers/AbstractFifo.h"
#include "text/NewLine.h"
#include "text/StringPool.h"
#include "text/Identifier.h"
#include "text/StringArray.h"
#include "system/SystemStats.h"
#include "memory/HeavyweightLeakedObjectDetector.h"
#include "text/StringPairArray.h"
#include "text/TextDiff.h"
#include "text/LocalisedStrings.h"
#include "text/Base64.h"
#include "misc/Result.h"
#include "misc/Uuid.h"
#include "misc/ConsoleApplication.h"
#include "containers/Variant.h"
#include "containers/NamedValueSet.h"
#include "containers/DynamicObject.h"
#include "containers/HashMap.h"
#include "time/RelativeTime.h"
#include "time/Time.h"
#include "streams/InputStream.h"
#include "streams/OutputStream.h"
#include "streams/BufferedInputStream.h"
#include "streams/MemoryInputStream.h"
#include "streams/MemoryOutputStream.h"
#include "streams/SubregionStream.h"
#include "streams/InputSource.h"
#include "files/File.h"
#include "files/DirectoryIterator.h"
#include "files/FileInputStream.h"
#include "files/FileOutputStream.h"
#include "files/FileSearchPath.h"
#include "files/MemoryMappedFile.h"
#include "files/TemporaryFile.h"
#include "files/FileFilter.h"
#include "files/WildcardFileFilter.h"
#include "streams/FileInputSource.h"
#include "logging/FileLogger.h"
#include "javascript/JSON.h"
#include "javascript/Javascript.h"
#include "maths/BigInteger.h"
#include "maths/Expression.h"
#include "maths/Random.h"
#include "misc/RuntimePermissions.h"
#include "misc/WindowsRegistry.h"
#include "threads/ChildProcess.h"
#include "threads/DynamicLibrary.h"
#include "threads/HighResolutionTimer.h"
#include "threads/InterProcessLock.h"
#include "threads/Process.h"
#include "threads/SpinLock.h"
#include "threads/WaitableEvent.h"
#include "threads/Thread.h"
#include "threads/ThreadLocalValue.h"
#include "threads/ThreadPool.h"
#include "threads/TimeSliceThread.h"
#include "threads/ReadWriteLock.h"
#include "threads/ScopedReadLock.h"
#include "threads/ScopedWriteLock.h"
#include "network/IPAddress.h"
#include "network/MACAddress.h"
#include "network/NamedPipe.h"
#include "network/Socket.h"
#include "network/URL.h"
#include "network/WebInputStream.h"
#include "streams/URLInputSource.h"
#include "time/PerformanceCounter.h"
#include "test/UnitTest.h"
#include "xml/XmlDocument.h"
#include "xml/XmlElement.h"
#include "zip/GZIPCompressorOutputStream.h"
#include "zip/GZIPDecompressorInputStream.h"
#include "zip/ZipFile.h"
#include "containers/PropertySet.h"
#include "memory/SharedResourcePointer.h"

#if HCORE_INCLUDE_OBJC_HELPERS && (HMAC || HIOS)
#include "native/osx_ObjCHelpers.h"
#endif

#if HCORE_INCLUDE_COM_SMART_PTR && HWINDOWS
#include "native/win32_ComSmartPtr.h"
#endif

#if HCORE_INCLUDE_JNI_HELPERS && HANDROID
#include <jni.h>
#include "native/android_JNIHelpers.h"
#endif

#if HUNIT_TESTS
#include "test/UnitTestCategories.h"
#endif

/*
    As the very long class names here try to explain, the purpose of this code is to cause
    a linker error if not all of your compile units are consistent in the options that they
    enable before including H headers. The reason this is important is that if you have
    two cpp files, and one includes the H headers with debug enabled, and another does so
    without that, then each will be generating code with different class layouts, and you'll
    get subtle and hard-to-track-down memory corruption!
 */
#if HDEBUG
struct API this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_debug_mode
{
    this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_debug_mode() noexcept;
};
static this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_debug_mode compileUnitMismatchSentinel;
#else
struct API this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_release_mode
{
    this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_release_mode() noexcept;
};
static this_will_fail_to_link_if_some_of_your_compile_units_are_built_in_release_mode compileUnitMismatchSentinel;
#endif

#if HMSVC
#pragma warning(pop)

// In DLL builds, need to disable this warnings for other modules
#if defined(HDLL_BUILD) || defined(HDLL)
#pragma warning(disable : 4251)
#endif
#endif
