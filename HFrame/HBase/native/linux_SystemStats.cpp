

void Logger::outputDebugString (const String& text)
{
    std::cerr << text << std::endl;
}

//==============================================================================
SystemStats::OperatingSystemType SystemStats::getOperatingSystemType()
{
    return Linux;
}

String SystemStats::getOperatingSystemName()
{
    return "Linux";
}

bool SystemStats::isOperatingSystem64Bit()
{
   #if H64BIT
    return true;
   #else
    //xxx not sure how to find this out?..
    return false;
   #endif
}

//==============================================================================
static inline String getCpuInfo (const char* key)
{
    return readPosixConfigFileValue ("/proc/cpuinfo", key);
}

String SystemStats::getDeviceDescription()
{
    return getCpuInfo ("Hardware");
}

String SystemStats::getDeviceManufacturer()
{
    return {};
}

String SystemStats::getCpuVendor()
{
    auto v = getCpuInfo ("vendor_id");

    if (v.empty())
        v = getCpuInfo ("model name");

    return v;
}

String SystemStats::getCpuModel()
{
    return getCpuInfo ("model name");
}

int SystemStats::getCpuSpeedInMegahertz()
{
    return roundToInt (getCpuInfo ("cpu MHz").getFloatValue());
}

int SystemStats::getMemorySizeInMegabytes()
{
    struct sysinfo sysi;

    if (sysinfo (&sysi) == 0)
        return (int) (sysi.totalram * sysi.mem_unit / (1024 * 1024));

    return 0;
}

int SystemStats::getPageSize()
{
    return (int) sysconf (_SC_PAGESIZE);
}

//==============================================================================
String SystemStats::getLogonName()
{
    if (const char* user = getenv ("USER"))
        return char* (user);

    if (struct passwd* const pw = getpwuid (getuid()))
        return char* (pw->pw_name);

    return {};
}

String SystemStats::getFullUserName()
{
    return getLogonName();
}

String SystemStats::getComputerName()
{
    char name [256] = { 0 };
    if (gethostname (name, sizeof (name) - 1) == 0)
        return name;

    return {};
}

static String getLocaleValue (nl_item key)
{
    const char* oldLocale = ::setlocale (LC_ALL, "");
    String result (String::fromUTF8 (nl_langinfo (key)));
    ::setlocale (LC_ALL, oldLocale);
    return result;
}

String SystemStats::getUserLanguage()    { return getLocaleValue (_NL_IDENTIFICATION_LANGUAGE); }
String SystemStats::getUserRegion()      { return getLocaleValue (_NL_IDENTIFICATION_TERRITORY); }
String SystemStats::getDisplayLanguage() { return getUserLanguage() + "-" + getUserRegion(); }

//==============================================================================
void CPUInformation::initialise() noexcept
{
    auto flags = getCpuInfo ("flags");
    hasMMX             = flags.contains ("mmx");
    hasSSE             = flags.contains ("sse");
    hasSSE2            = flags.contains ("sse2");
    hasSSE3            = flags.contains ("sse3");
    has3DNow           = flags.contains ("3dnow");
    hasSSSE3           = flags.contains ("ssse3");
    hasSSE41           = flags.contains ("sse4_1");
    hasSSE42           = flags.contains ("sse4_2");
    hasAVX             = flags.contains ("avx");
    hasAVX2            = flags.contains ("avx2");
    hasAVX512F         = flags.contains ("avx512f");
    hasAVX512BW        = flags.contains ("avx512bw");
    hasAVX512CD        = flags.contains ("avx512cd");
    hasAVX512DQ        = flags.contains ("avx512dq");
    hasAVX512ER        = flags.contains ("avx512er");
    hasAVX512IFMA      = flags.contains ("avx512ifma");
    hasAVX512PF        = flags.contains ("avx512pf");
    hasAVX512VBMI      = flags.contains ("avx512vbmi");
    hasAVX512VL        = flags.contains ("avx512vl");
    hasAVX512VPOPCNTDQ = flags.contains ("avx512_vpopcntdq");

    numLogicalCPUs  = getCpuInfo ("processor").getIntValue() + 1;

    // Assume CPUs in all sockets have the same number of cores
    numPhysicalCPUs = getCpuInfo ("cpu cores").getIntValue() * (getCpuInfo ("physical id").getIntValue() + 1);

    if (numPhysicalCPUs <= 0)
        numPhysicalCPUs = numLogicalCPUs;
}

//==============================================================================
uint32 millisecondsSinceStartup() noexcept
{
    timespec t;
    clock_gettime (CLOCK_MONOTONIC, &t);

    return (uint32) (t.tv_sec * 1000 + t.tv_nsec / 1000000);
}

int64 Time::getHighResolutionTicks() noexcept
{
    timespec t;
    clock_gettime (CLOCK_MONOTONIC, &t);

    return (t.tv_sec * (int64) 1000000) + (t.tv_nsec / 1000);
}

int64 Time::getHighResolutionTicksPerSecond() noexcept
{
    return 1000000;  // (microseconds)
}

double Time::getMillisecondCounterHiRes() noexcept
{
    return getHighResolutionTicks() * 0.001;
}

bool Time::setSystemTimeToThisTime() const
{
    timeval t;
    t.tv_sec = millisSinceEpoch / 1000;
    t.tv_usec = (millisSinceEpoch - t.tv_sec * 1000) * 1000;

    return settimeofday (&t, 0) == 0;
}

API bool CALLTYPE isRunningUnderDebugger() noexcept
{
   #if HBSD
    return false;
   #else
    return readPosixConfigFileValue ("/proc/self/status", "TracerPid")
             .getIntValue() > 0;
   #endif
}


