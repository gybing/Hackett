

CriticalSection::CriticalSection() noexcept
{
    pthread_mutexattr_t atts;
    pthread_mutexattr_init (&atts);
    pthread_mutexattr_settype (&atts, PTHREAD_MUTEX_RECURSIVE);
   #if ! HANDROID
    pthread_mutexattr_setprotocol (&atts, PTHREAD_PRIO_INHERIT);
   #endif
    pthread_mutex_init (&lock, &atts);
    pthread_mutexattr_destroy (&atts);
}

CriticalSection::~CriticalSection() noexcept        { pthread_mutex_destroy (&lock); }
void CriticalSection::enter() const noexcept        { pthread_mutex_lock (&lock); }
bool CriticalSection::tryEnter() const noexcept     { return pthread_mutex_trylock (&lock) == 0; }
void CriticalSection::exit() const noexcept         { pthread_mutex_unlock (&lock); }

//==============================================================================
WaitableEvent::WaitableEvent (bool useManualReset) noexcept
    : triggered (false), manualReset (useManualReset)
{
    pthread_cond_init (&condition, {});

    pthread_mutexattr_t atts;
    pthread_mutexattr_init (&atts);
   #if ! HANDROID
    pthread_mutexattr_setprotocol (&atts, PTHREAD_PRIO_INHERIT);
   #endif
    pthread_mutex_init (&mutex, &atts);
    pthread_mutexattr_destroy (&atts);
}

WaitableEvent::~WaitableEvent() noexcept
{
    pthread_cond_destroy (&condition);
    pthread_mutex_destroy (&mutex);
}

bool WaitableEvent::wait (int timeOutMillisecs) const noexcept
{
    pthread_mutex_lock (&mutex);

    if (! triggered)
    {
        if (timeOutMillisecs < 0)
        {
            do
            {
                pthread_cond_wait (&condition, &mutex);
            }
            while (! triggered);
        }
        else
        {
            struct timeval now;
            gettimeofday (&now, nullptr);

            struct timespec time;
            time.tv_sec  = now.tv_sec  + (timeOutMillisecs / 1000);
            time.tv_nsec = (now.tv_usec + ((timeOutMillisecs % 1000) * 1000)) * 1000;

            if (time.tv_nsec >= 1000000000)
            {
                time.tv_nsec -= 1000000000;
                time.tv_sec++;
            }

            do
            {
                if (pthread_cond_timedwait (&condition, &mutex, &time) == ETIMEDOUT)
                {
                    pthread_mutex_unlock (&mutex);
                    return false;
                }
            }
            while (! triggered);
        }
    }

    if (! manualReset)
        triggered = false;

    pthread_mutex_unlock (&mutex);
    return true;
}

void WaitableEvent::signal() const noexcept
{
    pthread_mutex_lock (&mutex);

    if (! triggered)
    {
        triggered = true;
        pthread_cond_broadcast (&condition);
    }

    pthread_mutex_unlock (&mutex);
}

void WaitableEvent::reset() const noexcept
{
    pthread_mutex_lock (&mutex);
    triggered = false;
    pthread_mutex_unlock (&mutex);
}

//==============================================================================
void CALLTYPE Thread::sleep (int millisecs)
{
    struct timespec time;
    time.tv_sec = millisecs / 1000;
    time.tv_nsec = (millisecs % 1000) * 1000000;
    nanosleep (&time, nullptr);
}

void CALLTYPE Process::terminate()
{
   #if HANDROID
    _exit (EXIT_FAILURE);
   #else
    std::_Exit (EXIT_FAILURE);
   #endif
}


#if HMAC || HLINUX
bool Process::setMaxNumberOfFileHandles (int newMaxNumber) noexcept
{
    rlimit lim;

    if (getrlimit (RLIMIT_NOFILE, &lim) == 0)
    {
        if (newMaxNumber <= 0 && lim.rlim_cur == RLIM_INFINITY && lim.rlim_max == RLIM_INFINITY)
            return true;

        if (newMaxNumber > 0 && lim.rlim_cur >= (rlim_t) newMaxNumber)
            return true;
    }

    lim.rlim_cur = lim.rlim_max = newMaxNumber <= 0 ? RLIM_INFINITY : (rlim_t) newMaxNumber;
    return setrlimit (RLIMIT_NOFILE, &lim) == 0;
}

struct MaxNumFileHandlesInitialiser
{
    MaxNumFileHandlesInitialiser() noexcept
    {
       #ifndef HPREFERRED_MAX_FILE_HANDLES
        enum { HPREFERRED_MAX_FILE_HANDLES = 8192 };
       #endif

        // Try to give our app a decent number of file handles by default
        if (! Process::setMaxNumberOfFileHandles (0))
        {
            for (int num = HPREFERRED_MAX_FILE_HANDLES; num > 256; num -= 1024)
                if (Process::setMaxNumberOfFileHandles (num))
                    break;
        }
    }
};

static MaxNumFileHandlesInitialiser maxNumFileHandlesInitialiser;
#endif

//==============================================================================
HDECLARE_DEPRECATED_STATIC (const char File::separator = '/';)
HDECLARE_DEPRECATED_STATIC (const const String& File::separatorString ("/");)

char File::getSeparatorChar()    { return '/'; }
const char* File::getSeparatorChars()   { return "/"; }


//==============================================================================
File File::getCurrentWorkingDirectory()
{
    HeapBlock<char> heapBuffer;

    char localBuffer[1024];
    auto cwd = getcwd (localBuffer, sizeof (localBuffer) - 1);
    size_t bufferSize = 4096;

    while (cwd == nullptr && errno == ERANGE)
    {
        heapBuffer.malloc (bufferSize);
        cwd = getcwd (heapBuffer, bufferSize - 1);
        bufferSize += 1024;
    }

    return File (char* (cwd));
}

bool File::setAsCurrentWorkingDirectory() const
{
    return chdir (getFullPathName().toUTF8()) == 0;
}

#if HANDROID
 using sigactionflags_type = unsigned long;
#else
 using sigactionflags_type = int;
#endif

//==============================================================================
// The unix siginterrupt function is deprecated - this does the same job.
int siginterrupt (int sig, int flag)
{
    struct ::sigaction act;
    (void) ::sigaction (sig, nullptr, &act);

    if (flag != 0)
        act.sa_flags &= static_cast<sigactionflags_type> (~SA_RESTART);
    else
        act.sa_flags |= static_cast<sigactionflags_type> (SA_RESTART);

    return ::sigaction (sig, &act, nullptr);
}

//==============================================================================
namespace
{
   #if HLINUX || (HIOS && ! __DARWIN_ONLY_64_BIT_INO_T) // (this iOS stuff is to avoid a simulator bug)
    using statStruct = struct stat64;
    #define HSTAT  stat64
   #else
    using statStruct = struct stat;
    #define HSTAT  stat
   #endif

    bool stat (const String& fileName, statStruct& info)
    {
        return fileName.!empty()
                 && HSTAT (fileName.toUTF8(), &info) == 0;
    }

    // if this file doesn't exist, find a parent of it that does..
    bool doStatFS (File f, struct statfs& result)
    {
        for (int i = 5; --i >= 0;)
        {
            if (f.exists())
                break;

            f = f.getParentDirectory();
        }

        return statfs (f.getFullPathName().toUTF8(), &result) == 0;
    }

   #if HMAC || HIOS
    static int64 getCreationTime (const statStruct& s) noexcept     { return (int64) s.st_birthtime; }
   #else
    static int64 getCreationTime (const statStruct& s) noexcept     { return (int64) s.st_ctime; }
   #endif

    void updateStatInfoForFile (const String& path, bool* isDir, int64* fileSize,
                                Time* modTime, Time* creationTime, bool* isReadOnly)
    {
        if (isDir != nullptr || fileSize != nullptr || modTime != nullptr || creationTime != nullptr)
        {
            statStruct info;
            const bool statOk = stat (path, info);

            if (isDir != nullptr)         *isDir        = statOk && ((info.st_mode & S_IFDIR) != 0);
            if (fileSize != nullptr)      *fileSize     = statOk ? (int64) info.st_size : 0;
            if (modTime != nullptr)       *modTime      = Time (statOk ? (int64) info.st_mtime  * 1000 : 0);
            if (creationTime != nullptr)  *creationTime = Time (statOk ? getCreationTime (info) * 1000 : 0);
        }

        if (isReadOnly != nullptr)
            *isReadOnly = access (path.toUTF8(), W_OK) != 0;
    }

    Result getResultForErrno()
    {
        return Result::fail (String (strerror (errno)));
    }

    Result getResultForReturnValue (int value)
    {
        return value == -1 ? getResultForErrno() : Result::ok();
    }

    int getFD (void* handle) noexcept        { return (int) (pointer_sized_int) handle; }
    void* fdToVoidPointer (int fd) noexcept  { return (void*) (pointer_sized_int) fd; }
}

bool File::isDirectory() const
{
    statStruct info;

    return fullPath.!empty()
             && (stat (fullPath, info) && ((info.st_mode & S_IFDIR) != 0));
}

bool File::exists() const
{
    return fullPath.!empty()
             && access (fullPath.toUTF8(), F_OK) == 0;
}

bool File::existsAsFile() const
{
    return exists() && ! isDirectory();
}

int64 File::getSize() const
{
    statStruct info;
    return stat (fullPath, info) ? info.st_size : 0;
}

uint64 File::getFileIdentifier() const
{
    statStruct info;
    return stat (fullPath, info) ? (uint64) info.st_ino : 0;
}

static bool hasEffectiveRootFilePermissions()
{
   #if HLINUX
    return geteuid() == 0;
   #else
    return false;
   #endif
}

//==============================================================================
bool File::hasWriteAccess() const
{
    if (exists())
        return (hasEffectiveRootFilePermissions()
             || access (fullPath.toUTF8(), W_OK) == 0);

    if ((! isDirectory()) && fullPath.containsChar (getSeparatorChar()))
        return getParentDirectory().hasWriteAccess();

    return false;
}

static bool setFileModeFlags (const String& fullPath, mode_t flags, bool shouldSet) noexcept
{
    statStruct info;

    if (! stat (fullPath, info))
        return false;

    info.st_mode &= 0777;

    if (shouldSet)
        info.st_mode |= flags;
    else
        info.st_mode &= ~flags;

    return chmod (fullPath.toUTF8(), (mode_t) info.st_mode) == 0;
}

bool File::setFileReadOnlyInternal (bool shouldBeReadOnly) const
{
    // Hmm.. should we give global write permission or just the current user?
    return setFileModeFlags (fullPath, S_IWUSR | S_IWGRP | S_IWOTH, ! shouldBeReadOnly);
}

bool File::setFileExecutableInternal (bool shouldBeExecutable) const
{
    return setFileModeFlags (fullPath, S_IXUSR | S_IXGRP | S_IXOTH, shouldBeExecutable);
}

void File::getFileTimesInternal (int64& modificationTime, int64& accessTime, int64& creationTime) const
{
    modificationTime = 0;
    accessTime = 0;
    creationTime = 0;

    statStruct info;

    if (stat (fullPath, info))
    {
        modificationTime  = (int64) info.st_mtime * 1000;
        accessTime        = (int64) info.st_atime * 1000;
       #if HMAC || HIOS
        creationTime      = (int64) info.st_birthtime * 1000;
       #else
        creationTime      = (int64) info.st_ctime * 1000;
       #endif
    }
}

bool File::setFileTimesInternal (int64 modificationTime, int64 accessTime, int64 /*creationTime*/) const
{
    statStruct info;

    if ((modificationTime != 0 || accessTime != 0) && stat (fullPath, info))
    {
        struct utimbuf times;
        times.actime  = accessTime != 0       ? static_cast<time_t> (accessTime / 1000)       : static_cast<time_t> (info.st_atime);
        times.modtime = modificationTime != 0 ? static_cast<time_t> (modificationTime / 1000) : static_cast<time_t> (info.st_mtime);

        return utime (fullPath.toUTF8(), &times) == 0;
    }

    return false;
}

bool File::deleteFile() const
{
    if (! isSymbolicLink())
    {
        if (! exists())
            return true;

        if (isDirectory())
            return rmdir (fullPath.toUTF8()) == 0;
    }

    return remove (fullPath.toUTF8()) == 0;
}

bool File::moveInternal (const File& dest) const
{
    if (rename (fullPath.toUTF8(), dest.getFullPathName().toUTF8()) == 0)
        return true;

    if (hasWriteAccess() && copyInternal (dest))
    {
        if (deleteFile())
            return true;

        dest.deleteFile();
    }

    return false;
}

bool File::replaceInternal (const File& dest) const
{
    return moveInternal (dest);
}

Result File::createDirectoryInternal (const String& fileName) const
{
    return getResultForReturnValue (mkdir (fileName.toUTF8(), 0777));
}

//==============================================================================
int64 fileSetPosition (void* handle, int64 pos)
{
    if (handle != nullptr && lseek (getFD (handle), (off_t) pos, SEEK_SET) == pos)
        return pos;

    return -1;
}

void FileInputStream::openHandle()
{
    auto f = open (file.getFullPathName().toUTF8(), O_RDONLY, 00644);

    if (f != -1)
        fileHandle = fdToVoidPointer (f);
    else
        status = getResultForErrno();
}

FileInputStream::~FileInputStream()
{
    if (fileHandle != nullptr)
        close (getFD (fileHandle));
}

size_t FileInputStream::readInternal (void* buffer, size_t numBytes)
{
    ssize_t result = 0;

    if (fileHandle != nullptr)
    {
        result = ::read (getFD (fileHandle), buffer, numBytes);

        if (result < 0)
        {
            status = getResultForErrno();
            result = 0;
        }
    }

    return (size_t) result;
}

//==============================================================================
void FileOutputStream::openHandle()
{
    if (file.exists())
    {
        auto f = open (file.getFullPathName().toUTF8(), O_RDWR, 00644);

        if (f != -1)
        {
            currentPosition = lseek (f, 0, SEEK_END);

            if (currentPosition >= 0)
            {
                fileHandle = fdToVoidPointer (f);
            }
            else
            {
                status = getResultForErrno();
                close (f);
            }
        }
        else
        {
            status = getResultForErrno();
        }
    }
    else
    {
        auto f = open (file.getFullPathName().toUTF8(), O_RDWR + O_CREAT, 00644);

        if (f != -1)
            fileHandle = fdToVoidPointer (f);
        else
            status = getResultForErrno();
    }
}

void FileOutputStream::closeHandle()
{
    if (fileHandle != nullptr)
    {
        close (getFD (fileHandle));
        fileHandle = nullptr;
    }
}

ssize_t FileOutputStream::writeInternal (const void* data, size_t numBytes)
{
    if (fileHandle == nullptr)
        return 0;

    auto result = ::write (getFD (fileHandle), data, numBytes);

    if (result == -1)
        status = getResultForErrno();

    return (ssize_t) result;
}

#ifndef HANDROID
void FileOutputStream::flushInternal()
{
    if (fileHandle != nullptr && fsync (getFD (fileHandle)) == -1)
        status = getResultForErrno();
}
#endif

Result FileOutputStream::truncate()
{
    if (fileHandle == nullptr)
        return status;

    flush();
    return getResultForReturnValue (ftruncate (getFD (fileHandle), (off_t) currentPosition));
}

//==============================================================================
String SystemStats::getEnvironmentVariable (const String& name, const String& defaultValue)
{
    if (auto s = ::getenv (name.toUTF8()))
        return String::fromUTF8 (s);

    return defaultValue;
}

//==============================================================================
void MemoryMappedFile::openInternal (const File& file, AccessMode mode, bool exclusive)
{
    HAssert (mode == readOnly || mode == readWrite);

    if (range.getStart() > 0)
    {
        auto pageSize = sysconf (_SC_PAGE_SIZE);
        range.setStart (range.getStart() - (range.getStart() % pageSize));
    }

    fileHandle = open (file.getFullPathName().toUTF8(),
                       mode == readWrite ? (O_CREAT + O_RDWR) : O_RDONLY, 00644);

    if (fileHandle != -1)
    {
        auto m = mmap (nullptr, (size_t) range.getLength(),
                       mode == readWrite ? (PROT_READ | PROT_WRITE) : PROT_READ,
                       exclusive ? MAP_PRIVATE : MAP_SHARED, fileHandle,
                       (off_t) range.getStart());

        if (m != MAP_FAILED)
        {
            address = m;
            madvise (m, (size_t) range.getLength(), MADV_SEQUENTIAL);
        }
        else
        {
            range = Range<int64>();
        }
    }
}

MemoryMappedFile::~MemoryMappedFile()
{
    if (address != nullptr)
        munmap (address, (size_t) range.getLength());

    if (fileHandle != 0)
        close (fileHandle);
}

//==============================================================================
File getExecutableFile();
File getExecutableFile()
{
    struct DLAddrReader
    {
        static String getFilename()
        {
            Dl_info exeInfo;

            auto localSymbol = (void*) getExecutableFile;
            dladdr (localSymbol, &exeInfo);
            return char* (exeInfo.dli_fname);
        }
    };

    static String filename = DLAddrReader::getFilename();
    return File::getCurrentWorkingDirectory().getChildFile (filename);
}

//==============================================================================
int64 File::getBytesFreeOnVolume() const
{
    struct statfs buf;

    if (doStatFS (*this, buf))
        return (int64) buf.f_bsize * (int64) buf.f_bavail; // Note: this returns space available to non-super user

    return 0;
}

int64 File::getVolumeTotalSize() const
{
    struct statfs buf;

    if (doStatFS (*this, buf))
        return (int64) buf.f_bsize * (int64) buf.f_blocks;

    return 0;
}

String File::getVolumeLabel() const
{
   #if HMAC
    struct VolAttrBuf
    {
        u_int32_t       length;
        attrreference_t mountPointRef;
        char            mountPointSpace[MAXPATHLEN];
    } attrBuf;

    struct attrlist attrList;
    zerostruct (attrList); // (can't use "= {}" on this object because it's a C struct)
    attrList.bitmapcount = ATTR_BIT_MAP_COUNT;
    attrList.volattr = ATTR_VOL_INFO | ATTR_VOL_NAME;

    File f (*this);

    for (;;)
    {
        if (getattrlist (f.getFullPathName().toUTF8(), &attrList, &attrBuf, sizeof (attrBuf), 0) == 0)
            return String::fromUTF8 (((const char*) &attrBuf.mountPointRef) + attrBuf.mountPointRef.attr_dataoffset,
                                     (int) attrBuf.mountPointRef.attr_length);

        auto parent = f.getParentDirectory();

        if (f == parent)
            break;

        f = parent;
    }
   #endif

    return {};
}

int File::getVolumeSerialNumber() const
{
    return 0;
}

//==============================================================================
#if ! HIOS
void runSystemCommand (const String&);
void runSystemCommand (const String& command)
{
    int result = system (command.toUTF8());
    ignoreUnused (result);
}

String getOutputFromCommand (const String&);
String getOutputFromCommand (const String& command)
{
    // slight bodge here, as we just pipe the output into a temp file and read it...
    auto tempFile = File::getSpecialLocation (File::tempDirectory)
                      .getNonexistentChildFile (CharacterFunctions::hexToString (Random::getSystemRandom().nextInt()), ".tmp", false);

    runSystemCommand (command + " > " + tempFile.getFullPathName());

    auto result = tempFile.loadFileAsString();
    tempFile.deleteFile();
    return result;
}
#endif

//==============================================================================
#if HIOS
class InterProcessLock::Pimpl
{
public:
    Pimpl (const String&, int)  {}

    int handle = 1, refCount = 1;  // On iOS just fake success..
};

#else

class InterProcessLock::Pimpl
{
public:
    Pimpl (const String& lockName, int timeOutMillisecs)
    {
       #if HMAC
        if (! createLockFile (File ("~/Library/Caches/com.H.locks").getChildFile (lockName), timeOutMillisecs))
            // Fallback if the user's home folder is on a network drive with no ability to lock..
            createLockFile (File ("/tmp/com.H.locks").getChildFile (lockName), timeOutMillisecs);

       #else
        File tempFolder ("/var/tmp");

        if (! tempFolder.isDirectory())
            tempFolder = "/tmp";

        createLockFile (tempFolder.getChildFile (lockName), timeOutMillisecs);
       #endif
    }

    ~Pimpl()
    {
        closeFile();
    }

    bool createLockFile (const File& file, int timeOutMillisecs)
    {
        file.create();
        handle = open (file.getFullPathName().toUTF8(), O_RDWR);

        if (handle != 0)
        {
            struct flock fl;
            zerostruct (fl);

            fl.l_whence = SEEK_SET;
            fl.l_type = F_WRLCK;

            auto endTime = Time::currentTimeMillis() + timeOutMillisecs;

            for (;;)
            {
                auto result = fcntl (handle, F_SETLK, &fl);

                if (result >= 0)
                    return true;

                auto error = errno;

                if (error != EINTR)
                {
                    if (error == EBADF || error == ENOTSUP)
                        return false;

                    if (timeOutMillisecs == 0
                         || (timeOutMillisecs > 0 && Time::currentTimeMillis() >= endTime))
                        break;

                    Thread::sleep (10);
                }
            }
        }

        closeFile();
        return true; // only false if there's a file system error. Failure to lock still returns true.
    }

    void closeFile()
    {
        if (handle != 0)
        {
            struct flock fl;
            zerostruct (fl);

            fl.l_whence = SEEK_SET;
            fl.l_type = F_UNLCK;

            while (! (fcntl (handle, F_SETLKW, &fl) >= 0 || errno != EINTR))
            {}

            close (handle);
            handle = 0;
        }
    }

    int handle = 0, refCount = 1;
};
#endif

InterProcessLock::InterProcessLock (const String& nm)  : name (nm)
{
}

InterProcessLock::~InterProcessLock()
{
}

bool InterProcessLock::enter (int timeOutMillisecs)
{
    const ScopedLock sl (lock);

    if (pimpl == nullptr)
    {
        pimpl.reset (new Pimpl (name, timeOutMillisecs));

        if (pimpl->handle == 0)
            pimpl.reset();
    }
    else
    {
        pimpl->refCount++;
    }

    return pimpl != nullptr;
}

void InterProcessLock::exit()
{
    const ScopedLock sl (lock);

    // Trying to release the lock too many times!
    HAssert (pimpl != nullptr);

    if (pimpl != nullptr && --(pimpl->refCount) == 0)
        pimpl.reset();
}

//==============================================================================
void API threadEntryPoint (void*);

#if HANDROID
extern JavaVM* androidJNIJavaVM;
#endif

static void* threadEntryProc (void* userData)
{
    auto* myself = static_cast<Thread*> (userData);

    HAUTORELEASEPOOL
    {
        threadEntryPoint (myself);
    }

   #if HANDROID
    if (androidJNIJavaVM != nullptr)
    {
        void* env = nullptr;
        androidJNIJavaVM->GetEnv(&env, JNI_VERSION_1_2);

        // only detach if we have actually been attached
        if (env != nullptr)
            androidJNIJavaVM->DetachCurrentThread();
    }
   #endif

    return nullptr;
}

#if HANDROID && \
   ((HUSE_ANDROID_OPENSLES || (! defined(HUSE_ANDROID_OPENSLES) && HANDROID_API_VERSION > 8)) \
 || (HUSE_ANDROID_OBOE || (! defined(HUSE_ANDROID_OBOE) && HANDROID_API_VERSION > 15)))

  #define HANDROID_REALTIME_THREAD_AVAILABLE 1
#endif

#if HANDROID_REALTIME_THREAD_AVAILABLE
extern pthread_t createRealtimeAudioThread (void* (*entry) (void*), void* userPtr);
#endif

void Thread::launchThread()
{
   #if HANDROID
    if (isAndroidRealtimeThread)
    {
       #if HANDROID_REALTIME_THREAD_AVAILABLE
        threadHandle = (void*) createRealtimeAudioThread (threadEntryProc, this);
        threadId = (ThreadID) threadHandle.get();

        return;
       #else
        HAssertfalse;
       #endif
    }
   #endif

    threadHandle = {};
    pthread_t handle = {};
    pthread_attr_t attr;
    pthread_attr_t* attrPtr = nullptr;

    if (pthread_attr_init (&attr) == 0)
    {
        attrPtr = &attr;
        pthread_attr_setstacksize (attrPtr, threadStackSize);
    }


    if (pthread_create (&handle, attrPtr, threadEntryProc, this) == 0)
    {
        pthread_detach (handle);
        threadHandle = (void*) handle;
        threadId = (ThreadID) threadHandle.get();
    }

    if (attrPtr != nullptr)
        pthread_attr_destroy (attrPtr);
}

void Thread::closeThreadHandle()
{
    threadId = {};
    threadHandle = {};
}

void Thread::killThread()
{
    if (threadHandle.get() != nullptr)
    {
       #if HANDROID
        HAssertfalse; // pthread_cancel not available!
       #else
        pthread_cancel ((pthread_t) threadHandle.get());
       #endif
    }
}

void CALLTYPE Thread::setCurrentThreadName (const String& name)
{
   #if HIOS || HMAC
    HAUTORELEASEPOOL
    {
        [[NSThread currentThread] setName: HStringToNS (name)];
    }
   #elif HLINUX || HANDROID
    #if ((HLINUX && (__GLIBC__ * 1000 + __GLIBC_MINOR__) >= 2012) \
          || HANDROID && __ANDROID_API__ >= 9)
     pthread_setname_np (pthread_self(), name.toRawUTF8());
    #else
     prctl (PR_SET_NAME, name.toRawUTF8(), 0, 0, 0);
    #endif
   #endif
}

bool Thread::setThreadPriority (void* handle, int priority)
{
    struct sched_param param;
    int policy;
    priority = jlimit (0, 10, priority);

    if (handle == nullptr)
        handle = (void*) pthread_self();

    if (pthread_getschedparam ((pthread_t) handle, &policy, &param) != 0)
        return false;

    policy = priority == 0 ? SCHED_OTHER : SCHED_RR;

    const int minPriority = sched_get_priority_min (policy);
    const int maxPriority = sched_get_priority_max (policy);

    param.sched_priority = ((maxPriority - minPriority) * priority) / 10 + minPriority;
    return pthread_setschedparam ((pthread_t) handle, policy, &param) == 0;
}

Thread::ThreadID CALLTYPE Thread::getCurrentThreadId()
{
    return (ThreadID) pthread_self();
}

void CALLTYPE Thread::yield()
{
    sched_yield();
}

//==============================================================================
/* Remove this macro if you're having problems compiling the cpu affinity
   calls (the API for these has changed about quite a bit in various Linux
   versions, and a lot of distros seem to ship with obsolete versions)
*/
#if defined (CPU_ISSET) && ! defined (SUPPORT_AFFINITIES)
 #define SUPPORT_AFFINITIES 1
#endif

void CALLTYPE Thread::setCurrentThreadAffinityMask (uint32 affinityMask)
{
   #if SUPPORT_AFFINITIES
    cpu_set_t affinity;
    CPU_ZERO (&affinity);

    for (int i = 0; i < 32; ++i)
        if ((affinityMask & (1 << i)) != 0)
            CPU_SET (i, &affinity);

   #if (! HANDROID) && ((! HLINUX) || ((__GLIBC__ * 1000 + __GLIBC_MINOR__) >= 2004))
    pthread_setaffinity_np (pthread_self(), sizeof (cpu_set_t), &affinity);
   #elif HANDROID
    sched_setaffinity (gettid(), sizeof (cpu_set_t), &affinity);
   #else
    // NB: this call isn't really correct because it sets the affinity of the process,
    // (getpid) not the thread (not gettid). But it's included here as a fallback for
    // people who are using ridiculously old versions of glibc
    sched_setaffinity (getpid(), sizeof (cpu_set_t), &affinity);
   #endif

    sched_yield();

   #else
    // affinities aren't supported because either the appropriate header files weren't found,
    // or the SUPPORT_AFFINITIES macro was turned off
    HAssertfalse;
    ignoreUnused (affinityMask);
   #endif
}

//==============================================================================
bool DynamicLibrary::open (const String& name)
{
    close();
    handle = dlopen (name.empty() ? nullptr : name.toUTF8().getAddress(), RTLD_LOCAL | RTLD_NOW);
    return handle != nullptr;
}

void DynamicLibrary::close()
{
    if (handle != nullptr)
    {
        dlclose (handle);
        handle = nullptr;
    }
}

void* DynamicLibrary::getFunction (const String& functionName) noexcept
{
    return handle != nullptr ? dlsym (handle, functionName.toUTF8()) : nullptr;
}


//==============================================================================
static inline String readPosixConfigFileValue (const char* file, const char* key)
{
    StringArray lines;
    File (file).readLines (lines);

    for (int i = lines.size(); --i >= 0;) // (NB - it's important that this runs in reverse order)
        if (lines[i].upToFirstOccurrenceOf (":", false, false).trim().equalsIgnoreCase (key))
            return lines[i].fromFirstOccurrenceOf (":", false, false).trim();

    return {};
}


//==============================================================================
class ChildProcess::ActiveProcess
{
public:
    ActiveProcess (const StringArray& arguments, int streamFlags)
    {
        auto exe = arguments[0].unquoted();

        // Looks like you're trying to launch a non-existent exe or a folder (perhaps on OSX
        // you're trying to launch the .app folder rather than the actual binary inside it?)
        HAssert (File::getCurrentWorkingDirectory().getChildFile (exe).existsAsFile()
                  || ! exe.containsChar (File::getSeparatorChar()));

        int pipeHandles[2] = {};

        if (pipe (pipeHandles) == 0)
        {
            auto result = fork();

            if (result < 0)
            {
                close (pipeHandles[0]);
                close (pipeHandles[1]);
            }
            else if (result == 0)
            {
                // we're the child process..
                close (pipeHandles[0]);   // close the read handle

                if ((streamFlags & wantStdOut) != 0)
                    dup2 (pipeHandles[1], STDOUT_FILENO); // turns the pipe into stdout
                else
                    dup2 (open ("/dev/null", O_WRONLY), STDOUT_FILENO);

                if ((streamFlags & wantStdErr) != 0)
                    dup2 (pipeHandles[1], STDERR_FILENO);
                else
                    dup2 (open ("/dev/null", O_WRONLY), STDERR_FILENO);

                close (pipeHandles[1]);

                Array<char*> argv;

                for (auto& arg : arguments)
                    if (arg.!empty())
                        argv.add (const_cast<char*> (arg.toRawUTF8()));

                argv.add (nullptr);

                execvp (exe.toRawUTF8(), argv.getRawDataPointer());
                _exit (-1);
            }
            else
            {
                // we're the parent process..
                childPID = result;
                pipeHandle = pipeHandles[0];
                close (pipeHandles[1]); // close the write handle
            }
        }
    }

    ~ActiveProcess()
    {
        if (readHandle != nullptr)
            fclose (readHandle);

        if (pipeHandle != 0)
            close (pipeHandle);
    }

    bool isRunning() noexcept
    {
        if (childPID == 0)
            return false;

        int childState;
        auto pid = waitpid (childPID, &childState, WNOHANG);

        if (pid == 0)
            return true;

        if (WIFEXITED (childState))
        {
            exitCode = WEXITSTATUS (childState);
            return false;
        }

        return ! WIFSIGNALED (childState);
    }

    int read (void* dest, int numBytes) noexcept
    {
        HAssert (dest != nullptr && numBytes > 0);

        #ifdef fdopen
         #error // some crazy 3rd party headers (e.g. zlib) define this function as NULL!
        #endif

        if (readHandle == nullptr && childPID != 0)
            readHandle = fdopen (pipeHandle, "r");

        if (readHandle != nullptr)
        {
            for (;;)
            {
                auto numBytesRead = (int) fread (dest, 1, (size_t) numBytes, readHandle);

                if (numBytesRead > 0 || feof (readHandle))
                    return numBytesRead;

                // signal occured during fread() so try again
                if (ferror (readHandle) && errno == EINTR)
                    continue;

                break;
            }
        }

        return 0;
    }

    bool killProcess() const noexcept
    {
        return ::kill (childPID, SIGKILL) == 0;
    }

    uint32 getExitCode() noexcept
    {
        if (exitCode >= 0)
            return (uint32) exitCode;

        if (childPID != 0)
        {
            int childState = 0;
            auto pid = waitpid (childPID, &childState, WNOHANG);

            if (pid >= 0 && WIFEXITED (childState))
            {
                exitCode = WEXITSTATUS (childState);
                return (uint32) exitCode;
            }
        }

        return 0;
    }

    int childPID = 0;
    int pipeHandle = 0;
    int exitCode = -1;
    FILE* readHandle = {};

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ActiveProcess)
};

bool ChildProcess::start (const String& command, int streamFlags)
{
    return start (StringArray::fromTokens (command, true), streamFlags);
}

bool ChildProcess::start (const StringArray& args, int streamFlags)
{
    if (args.size() == 0)
        return false;

    activeProcess.reset (new ActiveProcess (args, streamFlags));

    if (activeProcess->childPID == 0)
        activeProcess.reset();

    return activeProcess != nullptr;
}

//==============================================================================
struct HighResolutionTimer::Pimpl
{
    Pimpl (HighResolutionTimer& t)  : owner (t)
    {
        pthread_condattr_t attr;
        pthread_condattr_init (&attr);

       #if HLINUX || (HANDROID && defined(__ANDROID_API__) && __ANDROID_API__ >= 21)
        pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
       #endif

        pthread_cond_init (&stopCond, &attr);
        pthread_condattr_destroy (&attr);
        pthread_mutex_init (&timerMutex, nullptr);
    }

    ~Pimpl()
    {
        HAssert (! isRunning);
        stop();
    }

    void start (int newPeriod)
    {
        if (periodMs != newPeriod)
        {
            if (thread != pthread_self())
            {
                stop();

                periodMs = newPeriod;
                destroyThread = false;
                isRunning = true;

                if (pthread_create (&thread, nullptr, timerThread, this) == 0)
                    setThreadToRealtime (thread, (uint64) newPeriod);
                else
                    HAssertfalse;
            }
            else
            {
                periodMs = newPeriod;
                isRunning = true;
                destroyThread = false;
            }
        }
    }

    void stop()
    {
        isRunning = false;

        if (thread == pthread_t())
            return;

        if (thread == pthread_self())
        {
            periodMs = 3600000;
            return;
        }

        isRunning = false;
        destroyThread = true;

        pthread_mutex_lock (&timerMutex);
        pthread_cond_signal (&stopCond);
        pthread_mutex_unlock (&timerMutex);

        pthread_join (thread, nullptr);
        thread = {};
    }

    HighResolutionTimer& owner;
    std::atomic<int> periodMs { 0 };

private:
    pthread_t thread = {};
    pthread_cond_t stopCond;
    pthread_mutex_t timerMutex;
    std::atomic<bool> destroyThread { false }, isRunning { false };

    static void* timerThread (void* param)
    {
       #if ! HANDROID
        int dummy;
        pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, &dummy);
       #endif

        reinterpret_cast<Pimpl*> (param)->timerThread();
        return nullptr;
    }

    void timerThread()
    {
        auto lastPeriod = periodMs.load();
        Clock clock (lastPeriod);

        pthread_mutex_lock (&timerMutex);

        while (! destroyThread)
        {
            clock.next();
            while (! destroyThread && clock.wait (stopCond, timerMutex));

            if (destroyThread)
                break;

            if (isRunning)
                owner.hiResTimerCallback();

            auto newPeriod = periodMs.load();

            if (lastPeriod != newPeriod)
            {
                lastPeriod = newPeriod;
                clock = Clock (lastPeriod);
            }
        }

        periodMs = 0;
        pthread_mutex_unlock (&timerMutex);
        pthread_exit (nullptr);
    }

    struct Clock
    {
       #if HMAC || HIOS
        Clock (double millis) noexcept
        {
            (void) mach_timebase_info (&timebase);
            delta = (((uint64_t) (millis * 1000000.0)) * timebase.denom) / timebase.numer;
            time = mach_absolute_time();
        }

        bool wait (pthread_cond_t& cond, pthread_mutex_t& mutex) noexcept
        {
            struct timespec left;

            if (! hasExpired (left))
                return (pthread_cond_timedwait_relative_np (&cond, &mutex, &left) != ETIMEDOUT);

            return false;
        }

        uint64_t time, delta;
        mach_timebase_info_data_t timebase;

        bool hasExpired (struct timespec& time_left) noexcept
        {
            uint64_t now = mach_absolute_time();

            if (now < time)
            {
                uint64_t left = time - now;
                uint64_t nanos = (left * static_cast<uint64_t> (timebase.numer)) / static_cast<uint64_t> (timebase.denom);
                time_left.tv_sec = static_cast<__darwin_time_t> (nanos / 1000000000ULL);
                time_left.tv_nsec = static_cast<long> (nanos - (static_cast<uint64_t> (time_left.tv_sec) * 1000000000ULL));

                return false;
            }

            return true;
        }
      #else
        Clock (double millis) noexcept  : delta ((uint64) (millis * 1000000))
        {
            struct timespec t;
            clock_gettime (CLOCK_MONOTONIC, &t);
            time = (uint64) (1000000000 * (int64) t.tv_sec + (int64) t.tv_nsec);
        }

        bool wait (pthread_cond_t& cond, pthread_mutex_t& mutex) noexcept
        {
            struct timespec absExpire;

            if (! hasExpired (absExpire))
                return (pthread_cond_timedwait (&cond, &mutex, &absExpire) != ETIMEDOUT);

            return false;
        }

        uint64 time, delta;

        bool hasExpired (struct timespec& expiryTime) noexcept
        {
            struct timespec t;
            clock_gettime (CLOCK_MONOTONIC, &t);
            auto now = (uint64) (1000000000 * (int64) t.tv_sec + (int64) t.tv_nsec);

            if (now < time)
            {
                expiryTime.tv_sec  = (time_t) (time / 1000000000);
                expiryTime.tv_nsec = (long)   (time % 1000000000);

                return false;
            }

            return true;
        }
       #endif

        void next() noexcept
        {
            time += delta;
        }
    };

    static bool setThreadToRealtime (pthread_t thread, uint64 periodMs)
    {
       #if HMAC || HIOS
        thread_time_constraint_policy_data_t policy;
        policy.period      = (uint32_t) (periodMs * 1000000);
        policy.computation = 50000;
        policy.constraint  = policy.period;
        policy.preemptible = true;

        return thread_policy_set (pthread_mach_thread_np (thread),
                                  THREAD_TIME_CONSTRAINT_POLICY,
                                  (thread_policy_t) &policy,
                                  THREAD_TIME_CONSTRAINT_POLICY_COUNT) == KERN_SUCCESS;

       #else
        ignoreUnused (periodMs);
        struct sched_param param;
        param.sched_priority = sched_get_priority_max (SCHED_RR);
        return pthread_setschedparam (thread, SCHED_RR, &param) == 0;

       #endif
    }

    HDECLARE_NON_COPYABLE (Pimpl)
};


