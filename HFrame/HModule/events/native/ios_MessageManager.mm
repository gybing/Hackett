

void MessageManager::runDispatchLoop()
{
    HAssert (isThisTheMessageThread()); // must only be called by the message thread

    while (quitMessagePosted.get() == 0)
    {
        HAUTORELEASEPOOL
        {
            [[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode
                                     beforeDate: [NSDate dateWithTimeIntervalSinceNow: 0.001]];
        }
    }
}

void MessageManager::stopDispatchLoop()
{
    if (! SystemStats::isRunningInAppExtensionSandbox())
       [[[UIApplication sharedApplication] delegate] applicationWillTerminate: [UIApplication sharedApplication]];

    exit (0); // iOS apps get no mercy..
}

#if HMODAL_LOOPS_PERMITTED
bool MessageManager::runDispatchLoopUntil (int millisecondsToRunFor)
{
    HAUTORELEASEPOOL
    {
        HAssert (isThisTheMessageThread()); // must only be called by the message thread

        uint32 startTime = Time::getMillisecondCounter();
        NSDate* endDate = [NSDate dateWithTimeIntervalSinceNow: millisecondsToRunFor * 0.001];

        while (quitMessagePosted.get() == 0)
        {
            HAUTORELEASEPOOL
            {
                [[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode
                                         beforeDate: endDate];

                if (millisecondsToRunFor >= 0
                     && Time::getMillisecondCounter() >= startTime + (uint32) millisecondsToRunFor)
                    break;
            }
        }

        return quitMessagePosted.get() == 0;
    }
}
#endif

//==============================================================================
static std::unique_ptr<MessageQueue> messageQueue;

void MessageManager::doPlatformSpecificInitialisation()
{
    if (messageQueue == nullptr)
        messageQueue.reset (new MessageQueue());
}

void MessageManager::doPlatformSpecificShutdown()
{
    messageQueue = nullptr;
}

bool MessageManager::postMessageToSystemQueue (MessageManager::MessageBase* const message)
{
    if (messageQueue != nullptr)
        messageQueue->post (message);

    return true;
}

void MessageManager::broadcastMessage (const String&)
{
    // N/A on current iOS
}


