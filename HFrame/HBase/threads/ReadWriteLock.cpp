

ReadWriteLock::ReadWriteLock() noexcept
{
    readerThreads.ensureStorageAllocated (16);
}

ReadWriteLock::~ReadWriteLock() noexcept
{
    HAssert (readerThreads.size() == 0);
    HAssert (numWriters == 0);
}

//==============================================================================
void ReadWriteLock::enterRead() const noexcept
{
    while (! tryEnterRead())
        waitEvent.wait (100);
}

bool ReadWriteLock::tryEnterRead() const noexcept
{
    const Thread::ThreadID threadId = Thread::getCurrentThreadId();

    const SpinLock::ScopedLockType sl (accessLock);

    for (int i = 0; i < readerThreads.size(); ++i)
    {
        ThreadRecursionCount& trc = readerThreads.getReference(i);

        if (trc.threadID == threadId)
        {
            trc.count++;
            return true;
        }
    }

    if (numWriters + numWaitingWriters == 0
         || (threadId == writerThreadId && numWriters > 0))
    {
        ThreadRecursionCount trc = { threadId, 1 };
        readerThreads.add (trc);
        return true;
    }

    return false;
}

void ReadWriteLock::exitRead() const noexcept
{
    const Thread::ThreadID threadId = Thread::getCurrentThreadId();
    const SpinLock::ScopedLockType sl (accessLock);

    for (int i = 0; i < readerThreads.size(); ++i)
    {
        ThreadRecursionCount& trc = readerThreads.getReference(i);

        if (trc.threadID == threadId)
        {
            if (--(trc.count) == 0)
            {
                readerThreads.remove (i);
                waitEvent.signal();
            }

            return;
        }
    }

    HAssertfalse; // unlocking a lock that wasn't locked..
}

//==============================================================================
void ReadWriteLock::enterWrite() const noexcept
{
    const Thread::ThreadID threadId = Thread::getCurrentThreadId();
    const SpinLock::ScopedLockType sl (accessLock);

    while (! tryEnterWriteInternal (threadId))
    {
        ++numWaitingWriters;
        accessLock.exit();
        waitEvent.wait (100);
        accessLock.enter();
        --numWaitingWriters;
    }
}

bool ReadWriteLock::tryEnterWrite() const noexcept
{
    const SpinLock::ScopedLockType sl (accessLock);
    return tryEnterWriteInternal (Thread::getCurrentThreadId());
}

bool ReadWriteLock::tryEnterWriteInternal (Thread::ThreadID threadId) const noexcept
{
    if (readerThreads.size() + numWriters == 0
         || threadId == writerThreadId
         || (readerThreads.size() == 1 && readerThreads.getReference(0).threadID == threadId))
    {
        writerThreadId = threadId;
        ++numWriters;
        return true;
    }

    return false;
}

void ReadWriteLock::exitWrite() const noexcept
{
    const SpinLock::ScopedLockType sl (accessLock);

    // check this thread actually had the lock..
    HAssert (numWriters > 0 && writerThreadId == Thread::getCurrentThreadId());

    if (--numWriters == 0)
    {
        writerThreadId = {};
        waitEvent.signal();
    }
}


