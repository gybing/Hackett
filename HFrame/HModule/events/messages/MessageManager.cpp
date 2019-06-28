

MessageManager::MessageManager() noexcept
  : messageThreadId (Thread::getCurrentThreadId())
{
    if (HApplicationBase::isStandaloneApp())
        Thread::setCurrentThreadName ("H Message Thread");
}

MessageManager::~MessageManager() noexcept
{
    broadcaster.reset();

    doPlatformSpecificShutdown();

    HAssert (instance == this);
    instance = nullptr;  // do this last in case this instance is still needed by doPlatformSpecificShutdown()
}

MessageManager* MessageManager::instance = nullptr;

MessageManager* MessageManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new MessageManager();
        doPlatformSpecificInitialisation();
    }

    return instance;
}

MessageManager* MessageManager::getInstanceWithoutCreating() noexcept
{
    return instance;
}

void MessageManager::deleteInstance()
{
    deleteAndZero (instance);
}

//==============================================================================
bool MessageManager::MessageBase::post()
{
    auto* mm = MessageManager::instance;

    if (mm == nullptr || mm->quitMessagePosted.get() != 0 || ! postMessageToSystemQueue (this))
    {
        Ptr deleter (this); // (this will delete messages that were just created with a 0 ref count)
        return false;
    }

    return true;
}

//==============================================================================
#if HMODAL_LOOPS_PERMITTED && ! (HMAC || HIOS)
bool MessageManager::runDispatchLoopUntil (int millisecondsToRunFor)
{
    HAssert (isThisTheMessageThread()); // must only be called by the message thread

    auto endTime = Time::currentTimeMillis() + millisecondsToRunFor;

    while (quitMessageReceived.get() == 0)
    {
        HTRY
        {
            if (! dispatchNextMessageOnSystemQueue (millisecondsToRunFor >= 0))
                Thread::sleep (1);
        }
        HCATCH_EXCEPTION

        if (millisecondsToRunFor >= 0 && Time::currentTimeMillis() >= endTime)
            break;
    }

    return quitMessageReceived.get() == 0;
}
#endif

#if ! (HMAC || HIOS || HANDROID)
class MessageManager::QuitMessage   : public MessageManager::MessageBase
{
public:
    QuitMessage() {}

    void messageCallback() override
    {
        if (auto* mm = MessageManager::instance)
            mm->quitMessageReceived = true;
    }

    HDECLARE_NON_COPYABLE (QuitMessage)
};

void MessageManager::runDispatchLoop()
{
    HAssert (isThisTheMessageThread()); // must only be called by the message thread

    while (quitMessageReceived.get() == 0)
    {
        HTRY
        {
            if (! dispatchNextMessageOnSystemQueue (false))
                Thread::sleep (1);
        }
        HCATCH_EXCEPTION
    }
}

void MessageManager::stopDispatchLoop()
{
    (new QuitMessage())->post();
    quitMessagePosted = true;
}

#endif

//==============================================================================
class AsyncFunctionCallback   : public MessageManager::MessageBase
{
public:
    AsyncFunctionCallback (MessageCallbackFunction* const f, void* const param)
        : func (f), parameter (param)
    {}

    void messageCallback() override
    {
        result = (*func) (parameter);
        finished.signal();
    }

    WaitableEvent finished;
    std::atomic<void*> result { nullptr };

private:
    MessageCallbackFunction* const func;
    void* const parameter;

    HDECLARE_NON_COPYABLE (AsyncFunctionCallback)
};

void* MessageManager::callFunctionOnMessageThread (MessageCallbackFunction* const func, void* const parameter)
{
    if (isThisTheMessageThread())
        return func (parameter);

    // If this thread has the message manager locked, then this will deadlock!
    HAssert (! currentThreadHasLockedMessageManager());

    const ReferenceCountedObjectPtr<AsyncFunctionCallback> message (new AsyncFunctionCallback (func, parameter));

    if (message->post())
    {
        message->finished.wait();
        return message->result.load();
    }

    HAssertfalse; // the OS message queue failed to send the message!
    return nullptr;
}

//==============================================================================
void MessageManager::deliverBroadcastMessage (const String& value)
{
    if (broadcaster != nullptr)
        broadcaster->sendActionMessage (value);
}

void MessageManager::registerBroadcastListener (ActionListener* const listener)
{
    if (broadcaster == nullptr)
        broadcaster.reset (new ActionBroadcaster());

    broadcaster->addActionListener (listener);
}

void MessageManager::deregisterBroadcastListener (ActionListener* const listener)
{
    if (broadcaster != nullptr)
        broadcaster->removeActionListener (listener);
}

//==============================================================================
bool MessageManager::isThisTheMessageThread() const noexcept
{
    return Thread::getCurrentThreadId() == messageThreadId;
}

void MessageManager::setCurrentThreadAsMessageThread()
{
    auto thisThread = Thread::getCurrentThreadId();

    if (messageThreadId != thisThread)
    {
        messageThreadId = thisThread;

        // This is needed on windows to make sure the message window is created by this thread
        doPlatformSpecificShutdown();
        doPlatformSpecificInitialisation();
    }
}

bool MessageManager::currentThreadHasLockedMessageManager() const noexcept
{
    auto thisThread = Thread::getCurrentThreadId();
    return thisThread == messageThreadId || thisThread == threadWithLock.get();
}

bool MessageManager::existsAndIsLockedByCurrentThread() noexcept
{
    if (auto i = getInstanceWithoutCreating())
        return i->currentThreadHasLockedMessageManager();

    return false;
}

bool MessageManager::existsAndIsCurrentThread() noexcept
{
    if (auto i = getInstanceWithoutCreating())
        return i->isThisTheMessageThread();

    return false;
}

//==============================================================================
//==============================================================================
/*  The only safe way to lock the message thread while another thread does
    some work is by posting a special message, whose purpose is to tie up the event
    loop until the other thread has finished its business.

    Any other approach can get horribly deadlocked if the OS uses its own hidden locks which
    get locked before making an event callback, because if the same OS lock gets indirectly
    accessed from another thread inside a MM lock, you're screwed. (this is exactly what happens
    in Cocoa).
*/
struct MessageManager::Lock::BlockingMessage   : public MessageManager::MessageBase
{
    BlockingMessage (const MessageManager::Lock* parent) noexcept
    // need a const_cast here as VS2013 doesn't like a const pointer to be in an atomic
        : owner (const_cast<MessageManager::Lock*> (parent)) {}

    void messageCallback() override
    {
        {
            ScopedLock lock (ownerCriticalSection);

            if (auto* o = owner.get())
                o->messageCallback();
        }

        releaseEvent.wait();
    }

    CriticalSection ownerCriticalSection;
    Atomic<MessageManager::Lock*> owner;
    WaitableEvent releaseEvent;

    HDECLARE_NON_COPYABLE (BlockingMessage)
};

//==============================================================================
MessageManager::Lock::Lock()                            {}
MessageManager::Lock::~Lock()                           { exit(); }
void MessageManager::Lock::enter()    const noexcept    {        tryAcquire (true); }
bool MessageManager::Lock::tryEnter() const noexcept    { return tryAcquire (false); }

bool MessageManager::Lock::tryAcquire (bool lockIsMandatory) const noexcept
{
    auto* mm = MessageManager::instance;

    if (mm == nullptr)
    {
        HAssertfalse;
        return false;
    }

    if (! lockIsMandatory && (abortWait.get() != 0))
    {
        abortWait.set (0);
        return false;
    }

    if (mm->currentThreadHasLockedMessageManager())
        return true;

    try
    {
        blockingMessage = *new BlockingMessage (this);
    }
    catch (...)
    {
        HAssert (! lockIsMandatory);
        return false;
    }

    if (! blockingMessage->post())
    {
        // post of message failed while trying to get the lock
        HAssert (! lockIsMandatory);
        blockingMessage = nullptr;
        return false;
    }

    do
    {
        while (abortWait.get() == 0)
            lockedEvent.wait (-1);

        abortWait.set (0);

        if (lockGained.get() != 0)
        {
            mm->threadWithLock = Thread::getCurrentThreadId();
            return true;
        }

    } while (lockIsMandatory);

    // we didn't get the lock
    blockingMessage->releaseEvent.signal();

    {
        ScopedLock lock (blockingMessage->ownerCriticalSection);

        lockGained.set (0);
        blockingMessage->owner.set (nullptr);
    }

    blockingMessage = nullptr;
    return false;
}

void MessageManager::Lock::exit() const noexcept
{
    if (lockGained.compareAndSetBool (false, true))
    {
        auto* mm = MessageManager::instance;

        HAssert (mm == nullptr || mm->currentThreadHasLockedMessageManager());
        lockGained.set (0);

        if (mm != nullptr)
            mm->threadWithLock = {};

        if (blockingMessage != nullptr)
        {
            blockingMessage->releaseEvent.signal();
            blockingMessage = nullptr;
        }
    }
}

void MessageManager::Lock::messageCallback() const
{
    lockGained.set (1);
    abort();
}

void MessageManager::Lock::abort() const noexcept
{
    abortWait.set (1);
    lockedEvent.signal();
}

//==============================================================================
MessageManagerLock::MessageManagerLock (Thread* threadToCheck)
    : locked (attemptLock (threadToCheck, nullptr))
{}

MessageManagerLock::MessageManagerLock (ThreadPoolJob* jobToCheck)
    : locked (attemptLock (nullptr, jobToCheck))
{}

bool MessageManagerLock::attemptLock (Thread* threadToCheck, ThreadPoolJob* jobToCheck)
{
    HAssert (threadToCheck == nullptr || jobToCheck == nullptr);

    if (threadToCheck != nullptr)
        threadToCheck->addListener (this);

    if (jobToCheck != nullptr)
        jobToCheck->addListener (this);

    // tryEnter may have a spurious abort (return false) so keep checking the condition
    while ((threadToCheck == nullptr || ! threadToCheck->threadShouldExit())
             && (jobToCheck == nullptr || ! jobToCheck->shouldExit()))
    {
        if (mmLock.tryEnter())
            break;
    }

    if (threadToCheck != nullptr)
    {
        threadToCheck->removeListener (this);

        if (threadToCheck->threadShouldExit())
            return false;
    }

    if (jobToCheck != nullptr)
    {
        jobToCheck->removeListener (this);

        if (jobToCheck->shouldExit())
            return false;
    }

    return true;
}

MessageManagerLock::~MessageManagerLock()  { mmLock.exit(); }

void MessageManagerLock::exitSignalSent()
{
    mmLock.abort();
}

//==============================================================================
API void CALLTYPE initialiseJuce_GUI();
API void CALLTYPE initialiseJuce_GUI()
{
    HAUTORELEASEPOOL
    {
        MessageManager::getInstance();
    }
}

API void CALLTYPE shutdownJuce_GUI();
API void CALLTYPE shutdownJuce_GUI()
{
    HAUTORELEASEPOOL
    {
        DeletedAtShutdown::deleteAll();
        MessageManager::deleteInstance();
    }
}

static int numScopedInitInstances = 0;

ScopedJuceInitialiser_GUI::ScopedJuceInitialiser_GUI()  { if (numScopedInitInstances++ == 0) initialiseJuce_GUI(); }
ScopedJuceInitialiser_GUI::~ScopedJuceInitialiser_GUI() { if (--numScopedInitInstances == 0) shutdownJuce_GUI(); }


