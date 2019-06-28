

//==============================================================================
/* An internal message pump class used in OSX and iOS. */
class MessageQueue
{
public:
    MessageQueue()
    {
       #if HIOS
        runLoop = CFRunLoopGetCurrent();
       #else
        runLoop = CFRunLoopGetMain();
       #endif

        CFRunLoopSourceContext sourceContext;
        zerostruct (sourceContext); // (can't use "= { 0 }" on this object because it's typedef'ed as a C struct)
        sourceContext.info = this;
        sourceContext.perform = runLoopSourceCallback;
        runLoopSource = CFRunLoopSourceCreate (kCFAllocatorDefault, 1, &sourceContext);
        CFRunLoopAddSource (runLoop, runLoopSource, kCFRunLoopCommonModes);
    }

    ~MessageQueue() noexcept
    {
        CFRunLoopRemoveSource (runLoop, runLoopSource, kCFRunLoopCommonModes);
        CFRunLoopSourceInvalidate (runLoopSource);
        CFRelease (runLoopSource);
    }

    void post (MessageManager::MessageBase* const message)
    {
        messages.add (message);
        wakeUp();
    }

private:
    ReferenceCountedArray<MessageManager::MessageBase, CriticalSection> messages;
    CFRunLoopRef runLoop;
    CFRunLoopSourceRef runLoopSource;

    void wakeUp() noexcept
    {
        CFRunLoopSourceSignal (runLoopSource);
        CFRunLoopWakeUp (runLoop);
    }

    bool deliverNextMessage()
    {
        const MessageManager::MessageBase::Ptr nextMessage (messages.removeAndReturn (0));

        if (nextMessage == nullptr)
            return false;

        HAUTORELEASEPOOL
        {
            HTRY
            {
                nextMessage->messageCallback();
            }
            HCATCH_EXCEPTION
        }

        return true;
    }

    void runLoopCallback() noexcept
    {
        for (int i = 4; --i >= 0;)
            if (! deliverNextMessage())
                return;

        wakeUp();
    }

    static void runLoopSourceCallback (void* info) noexcept
    {
        static_cast<MessageQueue*> (info)->runLoopCallback();
    }
};


