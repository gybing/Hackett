

/*
    Note that a lot of methods that you'd expect to find in this file actually
    live in posix_SharedCode.h!
*/

#if HIOS
bool isIOSAppActive = true;
#endif

//==============================================================================
API bool CALLTYPE Process::isForegroundProcess()
{
   if (SystemStats::isRunningInAppExtensionSandbox())
       return true;

   #if HMAC
    return [NSApp isActive];
   #else
    return isIOSAppActive;
   #endif
}

API void CALLTYPE Process::makeForegroundProcess()
{
   #if HMAC
    if (! SystemStats::isRunningInAppExtensionSandbox())
        [NSApp activateIgnoringOtherApps: YES];
   #endif
}

API void CALLTYPE Process::hide()
{
    if (! SystemStats::isRunningInAppExtensionSandbox())
    {
       #if HMAC
        [NSApp hide: nil];
       #elif HIOS
        [[UIApplication sharedApplication] performSelector: @selector(suspend)];
       #endif
    }
}

API void CALLTYPE Process::raisePrivilege()
{
    HAssertfalse;
}

API void CALLTYPE Process::lowerPrivilege()
{
    HAssertfalse;
}

API void CALLTYPE Process::setPriority (ProcessPriority)
{
    // xxx
}

//==============================================================================
API bool CALLTYPE isRunningUnderDebugger() noexcept
{
    struct kinfo_proc info;
    int m[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };
    size_t sz = sizeof (info);
    sysctl (m, 4, &info, &sz, nullptr, 0);
    return (info.kp_proc.p_flag & P_TRACED) != 0;
}


