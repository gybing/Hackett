

String SystemStats::getHVersion()
{
    // Some basic tests, to keep an eye on things and make sure these types work ok
    // on all platforms. Let me know if any of these assertions fail on your system!
    static_assert (sizeof (pointer_sized_int) == sizeof (void*), "Basic sanity test failed: please report!");
    static_assert (sizeof (int8) == 1,                           "Basic sanity test failed: please report!");
    static_assert (sizeof (uint8) == 1,                          "Basic sanity test failed: please report!");
    static_assert (sizeof (int16) == 2,                          "Basic sanity test failed: please report!");
    static_assert (sizeof (uint16) == 2,                         "Basic sanity test failed: please report!");
    static_assert (sizeof (int32) == 4,                          "Basic sanity test failed: please report!");
    static_assert (sizeof (uint32) == 4,                         "Basic sanity test failed: please report!");
    static_assert (sizeof (int64) == 8,                          "Basic sanity test failed: please report!");
    static_assert (sizeof (uint64) == 8,                         "Basic sanity test failed: please report!");

    return "H v" HSTRINGIFY(HMAJOR_VERSION)
                "." HSTRINGIFY(HMINOR_VERSION)
                "." HSTRINGIFY(HBUILDNUMBER);
}

#if HANDROID && ! defined (HDISABLE_HVERSION_PRINTING)
 #define HDISABLE_HVERSION_PRINTING 1
#endif

#if HDEBUG && ! HDISABLE_HVERSION_PRINTING
 struct JuceVersionPrinter
 {
     JuceVersionPrinter()
     {
         DBG (SystemStats::getHVersion());
     }
 };

 static JuceVersionPrinter HVersionPrinter;
#endif

StringArray SystemStats::getDeviceIdentifiers()
{
    StringArray ids;

   #if HWINDOWS
    File f (File::getSpecialLocation (File::windowsSystemDirectory));
   #else
    File f ("~");
   #endif
    if (auto num = f.getFileIdentifier())
    {
        ids.add (String::toHexString ((int64) num));
    }
    else
    {
        for (auto& address : MACAddress::getAllAddresses())
            ids.add (address.toString());
    }

    HAssert (! ids.empty()); // Failed to create any IDs!
    return ids;
}

//==============================================================================
struct CPUInformation
{
    CPUInformation() noexcept    { initialise(); }

    void initialise() noexcept;

    int numLogicalCPUs = 0, numPhysicalCPUs = 0;

    bool hasMMX      = false, hasSSE        = false, hasSSE2       = false, hasSSE3 = false,
         has3DNow    = false, hasSSSE3      = false, hasSSE41      = false,
         hasSSE42    = false, hasAVX        = false, hasAVX2       = false,
         hasAVX512F  = false, hasAVX512BW   = false, hasAVX512CD   = false,
         hasAVX512DQ = false, hasAVX512ER   = false, hasAVX512IFMA = false,
         hasAVX512PF = false, hasAVX512VBMI = false, hasAVX512VL   = false,
         hasAVX512VPOPCNTDQ = false,
         hasNeon = false;
};

static const CPUInformation& getCPUInformation() noexcept
{
    static CPUInformation info;
    return info;
}

int SystemStats::getNumCpus() noexcept          { return getCPUInformation().numLogicalCPUs; }
int SystemStats::getNumPhysicalCpus() noexcept  { return getCPUInformation().numPhysicalCPUs; }
bool SystemStats::hasMMX() noexcept             { return getCPUInformation().hasMMX; }
bool SystemStats::has3DNow() noexcept           { return getCPUInformation().has3DNow; }
bool SystemStats::hasSSE() noexcept             { return getCPUInformation().hasSSE; }
bool SystemStats::hasSSE2() noexcept            { return getCPUInformation().hasSSE2; }
bool SystemStats::hasSSE3() noexcept            { return getCPUInformation().hasSSE3; }
bool SystemStats::hasSSSE3() noexcept           { return getCPUInformation().hasSSSE3; }
bool SystemStats::hasSSE41() noexcept           { return getCPUInformation().hasSSE41; }
bool SystemStats::hasSSE42() noexcept           { return getCPUInformation().hasSSE42; }
bool SystemStats::hasAVX() noexcept             { return getCPUInformation().hasAVX; }
bool SystemStats::hasAVX2() noexcept            { return getCPUInformation().hasAVX2; }
bool SystemStats::hasAVX512F() noexcept         { return getCPUInformation().hasAVX512F; }
bool SystemStats::hasAVX512BW() noexcept        { return getCPUInformation().hasAVX512BW; }
bool SystemStats::hasAVX512CD() noexcept        { return getCPUInformation().hasAVX512CD; }
bool SystemStats::hasAVX512DQ() noexcept        { return getCPUInformation().hasAVX512DQ; }
bool SystemStats::hasAVX512ER() noexcept        { return getCPUInformation().hasAVX512ER; }
bool SystemStats::hasAVX512IFMA() noexcept      { return getCPUInformation().hasAVX512IFMA; }
bool SystemStats::hasAVX512PF() noexcept        { return getCPUInformation().hasAVX512PF; }
bool SystemStats::hasAVX512VBMI() noexcept      { return getCPUInformation().hasAVX512VBMI; }
bool SystemStats::hasAVX512VL() noexcept        { return getCPUInformation().hasAVX512VL; }
bool SystemStats::hasAVX512VPOPCNTDQ() noexcept { return getCPUInformation().hasAVX512VPOPCNTDQ; }
bool SystemStats::hasNeon() noexcept            { return getCPUInformation().hasNeon; }


//==============================================================================
String SystemStats::getStackBacktrace()
{
    String result;

   #if HANDROID || HMINGW
    HAssertfalse; // sorry, not implemented yet!

   #elif HWINDOWS
    HANDLE process = GetCurrentProcess();
    SymInitialize (process, nullptr, TRUE);

    void* stack[128];
    int frames = (int) CaptureStackBackTrace (0, numElementsInArray (stack), stack, nullptr);

    HeapBlock<SYMBOL_INFO> symbol;
    symbol.calloc (sizeof (SYMBOL_INFO) + 256, 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof (SYMBOL_INFO);

    for (int i = 0; i < frames; ++i)
    {
        DWORD64 displacement = 0;

        if (SymFromAddr (process, (DWORD64) stack[i], &displacement, symbol))
        {
            result << i << ": ";

            IMAGEHLP_MODULE64 moduleInfo;
            zerostruct (moduleInfo);
            moduleInfo.SizeOfStruct = sizeof (moduleInfo);

            if (::SymGetModuleInfo64 (process, symbol->ModBase, &moduleInfo))
                result << moduleInfo.ModuleName << ": ";

            result << symbol->Name << " + 0x" << String::toHexString ((int64) displacement) << newLine;
        }
    }

   #else
    void* stack[128];
    int frames = backtrace (stack, numElementsInArray (stack));
    char** frameStrings = backtrace_symbols (stack, frames);

    for (int i = 0; i < frames; ++i)
        result << frameStrings[i] << newLine;

    ::free (frameStrings);
   #endif

    return result;
}

//==============================================================================
static SystemStats::CrashHandlerFunction globalCrashHandler = nullptr;

#if HWINDOWS
static LONG WINAPI handleCrash (LPEXCEPTION_POINTERS ep)
{
    globalCrashHandler (ep);
    return EXCEPTION_EXECUTE_HANDLER;
}
#else
static void handleCrash (int signum)
{
    globalCrashHandler ((void*) (pointer_sized_int) signum);
    ::kill (getpid(), SIGKILL);
}

int siginterrupt (int sig, int flag);
#endif

void SystemStats::setApplicationCrashHandler (CrashHandlerFunction handler)
{
    HAssert (handler != nullptr); // This must be a valid function.
    globalCrashHandler = handler;

   #if HWINDOWS
    SetUnhandledExceptionFilter (handleCrash);
   #else
    const int signals[] = { SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGABRT, SIGSYS };

    for (int i = 0; i < numElementsInArray (signals); ++i)
    {
        ::signal (signals[i], handleCrash);
        siginterrupt (signals[i], 1);
    }
   #endif
}

bool SystemStats::isRunningInAppExtensionSandbox() noexcept
{
   #if HMAC || HIOS
    static bool firstQuery = true;
    static bool isRunningInAppSandbox = false;

    if (firstQuery)
    {
        firstQuery = false;

        File bundle = File::getSpecialLocation (File::invokedExecutableFile).getParentDirectory();

       #if HMAC
        bundle = bundle.getParentDirectory().getParentDirectory();
       #endif

        if (bundle.isDirectory())
            isRunningInAppSandbox = (bundle.getFileExtension() == ".appex");
    }

    return isRunningInAppSandbox;
   #else
    return false;
   #endif
}


