

MidiOutput::MidiOutput (const String& deviceName, const String& deviceIdentifier)
    : Thread ("midi out"), deviceInfo (deviceName, deviceIdentifier)
{
}

void MidiOutput::sendBlockOfMessagesNow (const MidiBuffer& buffer)
{
    MidiBuffer::Iterator i (buffer);
    MidiMessage message;
    int samplePosition; // Note: Not actually used, so no need to initialise.

    while (i.getNextEvent (message, samplePosition))
        sendMessageNow (message);
}

void MidiOutput::sendBlockOfMessages (const MidiBuffer& buffer,
                                      double millisecondCounterToStartAt,
                                      double samplesPerSecondForBuffer)
{
    // You've got to call startBackgroundThread() for this to actually work..
    HAssert (isThreadRunning());

    // this needs to be a value in the future - RTFM for this method!
    HAssert (millisecondCounterToStartAt > 0);

    auto timeScaleFactor = 1000.0 / samplesPerSecondForBuffer;

    const uint8* data;
    int len, time;

    for (MidiBuffer::Iterator i (buffer); i.getNextEvent (data, len, time);)
    {
        auto eventTime = millisecondCounterToStartAt + timeScaleFactor * time;
        auto* m = new PendingMessage (data, len, eventTime);

        const ScopedLock sl (lock);

        if (firstMessage == nullptr || firstMessage->message.getTimeStamp() > eventTime)
        {
            m->next = firstMessage;
            firstMessage = m;
        }
        else
        {
            auto* mm = firstMessage;

            while (mm->next != nullptr && mm->next->message.getTimeStamp() <= eventTime)
                mm = mm->next;

            m->next = mm->next;
            mm->next = m;
        }
    }

    notify();
}

void MidiOutput::clearAllPendingMessages()
{
    const ScopedLock sl (lock);

    while (firstMessage != nullptr)
    {
        auto* m = firstMessage;
        firstMessage = firstMessage->next;
        delete m;
    }
}

void MidiOutput::startBackgroundThread()
{
    startThread (9);
}

void MidiOutput::stopBackgroundThread()
{
    stopThread (5000);
}

void MidiOutput::run()
{
    while (! threadShouldExit())
    {
        auto now = Time::getMillisecondCounter();
        uint32 eventTime = 0;
        uint32 timeToWait = 500;

        PendingMessage* message;

        {
            const ScopedLock sl (lock);
            message = firstMessage;

            if (message != nullptr)
            {
                eventTime = (uint32) roundToInt (message->message.getTimeStamp());

                if (eventTime > now + 20)
                {
                    timeToWait = eventTime - (now + 20);
                    message = nullptr;
                }
                else
                {
                    firstMessage = message->next;
                }
            }
        }

        if (message != nullptr)
        {
            std::unique_ptr<PendingMessage> messageDeleter (message);

            if (eventTime > now)
            {
                Time::waitForMillisecondCounter (eventTime);

                if (threadShouldExit())
                    break;
            }

            if (eventTime > now - 200)
                sendMessageNow (message->message);
        }
        else
        {
            HAssert (timeToWait < 1000 * 30);
            wait ((int) timeToWait);
        }
    }

    clearAllPendingMessages();
}


