

/*
    Note that a lot of methods that you'd expect to find in this file actually
    live in posix_SharedCode.h!
*/

//==============================================================================
API void CALLTYPE Process::setPriority (const ProcessPriority prior)
{
    const int policy = (prior <= NormalPriority) ? SCHED_OTHER : SCHED_RR;
    const int minp = sched_get_priority_min (policy);
    const int maxp = sched_get_priority_max (policy);

    struct sched_param param;

    switch (prior)
    {
        case LowPriority:
        case NormalPriority:    param.sched_priority = 0; break;
        case HighPriority:      param.sched_priority = minp + (maxp - minp) / 4; break;
        case RealtimePriority:  param.sched_priority = minp + (3 * (maxp - minp) / 4); break;
        default:                HAssertfalse; break;
    }

    pthread_setschedparam (pthread_self(), policy, &param);
}

static bool swapUserAndEffectiveUser()
{
    int result1 = setreuid (geteuid(), getuid());
    int result2 = setregid (getegid(), getgid());
    return result1 == 0 && result2 == 0;
}

API void CALLTYPE Process::raisePrivilege()  { if (geteuid() != 0 && getuid() == 0) swapUserAndEffectiveUser(); }
API void CALLTYPE Process::lowerPrivilege()  { if (geteuid() == 0 && getuid() != 0) swapUserAndEffectiveUser(); }


