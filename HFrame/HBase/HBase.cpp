

#ifdef HCORE_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#define HCORE_INCLUDE_OBJC_HELPERS 1
#define HCORE_INCLUDE_COM_SMART_PTR 1
#define HCORE_INCLUDE_NATIVE_HEADERS 1
#define HCORE_INCLUDE_JNI_HELPERS 1

#include "HBase.h"

#include <locale>
#include <cctype>
#include <cstdarg>

#if ! HANDROID
 #include <sys/timeb.h>
 #include <cwctype>
#endif

#if HWINDOWS
 #include <ctime>

 #if ! HMINGW
  #pragma warning (push)
  #pragma warning (disable: 4091)
  #include <Dbghelp.h>
  #pragma warning (pop)

  #if ! HDONT_AUTOLINK_TO_WIN32_LIBRARIES
   #pragma comment (lib, "DbgHelp.lib")
  #endif
 #endif

 #if HMINGW
  #include <ws2spi.h>
  #include <cstdio>
  #include <locale.h>
 #endif

#else
 #if HLINUX || HANDROID
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/errno.h>
  #include <unistd.h>
  #include <netinet/in.h>
 #endif

 #if HLINUX
  #include <stdio.h>
  #include <langinfo.h>
  #include <ifaddrs.h>
  #include <sys/resource.h>

  #if HUSE_CURL
   #include <curl/curl.h>
  #endif
 #endif

 #include <pwd.h>
 #include <fcntl.h>
 #include <netdb.h>
 #include <arpa/inet.h>
 #include <netinet/tcp.h>
 #include <sys/time.h>
 #include <net/if.h>
 #include <sys/ioctl.h>

 #if ! HANDROID
  #include <execinfo.h>
 #endif
#endif

#if HMAC || HIOS
 #include <xlocale.h>
 #include <mach/mach.h>
 #include <signal.h>
#endif

#if HANDROID
 #include <ifaddrs.h>
 #include <android/log.h>
#endif

#undef check

//==============================================================================
#ifndef    HSTANDALONE_APPLICATION
 HCOMPILER_WARNING ("Please re-save your project with the latest Projector version to avoid this warning")
 #define   HSTANDALONE_APPLICATION 0
#endif

//==============================================================================
#include "containers/AbstractFifo.cpp"
#include "containers/ArrayBase.cpp"
#include "containers/NamedValueSet.cpp"
#include "containers/OwnedArray.cpp"
#include "containers/PropertySet.cpp"
#include "containers/ReferenceCountedArray.cpp"
#include "containers/SparseSet.cpp"
#include "files/DirectoryIterator.cpp"
#include "files/File.cpp"
#include "files/FileInputStream.cpp"
#include "files/FileOutputStream.cpp"
#include "files/FileSearchPath.cpp"
#include "files/TemporaryFile.cpp"
#include "logging/FileLogger.cpp"
#include "logging/Logger.cpp"
#include "maths/BigInteger.cpp"
#include "maths/Expression.cpp"
#include "maths/Random.cpp"
#include "memory/MemoryBlock.cpp"
#include "misc/RuntimePermissions.cpp"
#include "misc/Result.cpp"
#include "misc/Uuid.cpp"
#include "misc/StdFunctionCompat.cpp"
#include "misc/ConsoleApplication.cpp"
#include "network/MACAddress.cpp"
#include "network/NamedPipe.cpp"
#include "network/Socket.cpp"
#include "network/IPAddress.cpp"
#include "streams/BufferedInputStream.cpp"
#include "streams/FileInputSource.cpp"
#include "streams/InputStream.cpp"
#include "streams/MemoryInputStream.cpp"
#include "streams/MemoryOutputStream.cpp"
#include "streams/SubregionStream.cpp"
#include "system/SystemStats.cpp"
#include "text/CharacterFunctions.cpp"
#include "text/Identifier.cpp"
#include "text/LocalisedStrings.cpp"
#include "streams/OutputStream.cpp"
#include "text/StringArray.cpp"
#include "text/StringPairArray.cpp"
#include "text/StringPool.cpp"
#include "text/TextDiff.cpp"
#include "text/Base64.cpp"
#include "threads/ReadWriteLock.cpp"
#include "threads/Thread.cpp"
#include "threads/ThreadPool.cpp"
#include "threads/TimeSliceThread.cpp"
#include "time/PerformanceCounter.cpp"
#include "time/RelativeTime.cpp"
#include "time/Time.cpp"
#include "test/UnitTest.cpp"
#include "containers/Variant.cpp"
#include "javascript/JSON.cpp"
#include "javascript/Javascript.cpp"
#include "containers/DynamicObject.cpp"
#include "xml/XmlDocument.cpp"
#include "xml/XmlElement.cpp"
#include "zip/GZIPDecompressorInputStream.cpp"
#include "zip/GZIPCompressorOutputStream.cpp"
#include "zip/ZipFile.cpp"
#include "files/FileFilter.cpp"
#include "files/WildcardFileFilter.cpp"

//==============================================================================
#if ! HWINDOWS
#include "native/posix_SharedCode.h"
#include "native/posix_NamedPipe.cpp"
#if ! HANDROID || __ANDROID_API__ >= 24
 #include "native/posix_IPAddress.h"
#endif
#endif

//==============================================================================
#if HMAC || HIOS
#include "native/mac_Files.mm"
#include "native/mac_Network.mm"
#include "native/mac_Strings.mm"
#include "native/mac_SystemStats.mm"
#include "native/mac_Threads.mm"

//==============================================================================
#elif HWINDOWS
#include "native/win32_Files.cpp"
#include "native/win32_Network.cpp"
#include "native/win32_Registry.cpp"
#include "native/win32_SystemStats.cpp"
#include "native/win32_Threads.cpp"

//==============================================================================
#elif HLINUX
#include "native/linux_CommonFile.cpp"
#include "native/linux_Files.cpp"
#include "native/linux_Network.cpp"
#if HUSE_CURL
 #include "native/curl_Network.cpp"
#endif
#include "native/linux_SystemStats.cpp"
#include "native/linux_Threads.cpp"

//==============================================================================
#elif HANDROID

#include "native/linux_CommonFile.cpp"
#include "native/android_JNIHelpers.cpp"
#include "native/android_Files.cpp"
#include "native/android_Misc.cpp"
#include "native/android_Network.cpp"
#include "native/android_SystemStats.cpp"
#include "native/android_Threads.cpp"
#include "native/android_RuntimePermissions.cpp"

#endif

#include "threads/ChildProcess.cpp"
#include "threads/HighResolutionTimer.cpp"
#include "network/URL.cpp"
#include "network/WebInputStream.cpp"
#include "streams/URLInputSource.cpp"

//==============================================================================
#if HUNIT_TESTS
#include "containers/HashMap_test.cpp"
#endif