

namespace
{
    struct PortIOStats
    {
        PortIOStats (const char* nm) : name (nm) {}

        const char* const name;
        int byteCount           = 0;
        int messageCount        = 0;
        int bytesPerSec         = 0;
        int largestMessageBytes = 0;
        int lastMessageBytes    = 0;

        void update (double elapsedSec)
        {
            if (byteCount > 0)
            {
                bytesPerSec = (int) (byteCount / elapsedSec);
                byteCount = 0;
                Logger::writeToLog (getString());
            }
        }

        String getString() const
        {
            return String (name) + ": "
            + "count="    + String (messageCount).paddedRight (' ', 7)
            + "rate="     + (String (bytesPerSec / 1024.0f, 1) + " Kb/sec").paddedRight (' ', 11)
            + "largest="  + (String (largestMessageBytes) + " bytes").paddedRight (' ', 11)
            + "last="     + (String (lastMessageBytes) + " bytes").paddedRight (' ', 11);
        }

        void registerMessage (int numBytes) noexcept
        {
            byteCount += numBytes;
            ++messageCount;
            lastMessageBytes = numBytes;
            largestMessageBytes = jmax (largestMessageBytes, numBytes);
        }
    };

    static PortIOStats inputStats { "Input" }, outputStats { "Output" };
    static uint32 startTime = 0;

    static inline void resetOnSecondBoundary()
    {
        auto now = Time::getMillisecondCounter();
        double elapsedSec = (now - startTime) / 1000.0;

        if (elapsedSec >= 1.0)
        {
            inputStats.update (elapsedSec);
            outputStats.update (elapsedSec);
            startTime = now;
        }
    }

    static inline void registerBytesOut (int numBytes)
    {
        outputStats.registerMessage (numBytes);
        resetOnSecondBoundary();
    }

    static inline void registerBytesIn (int numBytes)
    {
        inputStats.registerMessage (numBytes);
        resetOnSecondBoundary();
    }
}

String getMidiIOStats()
{
    return inputStats.getString() + "   " + outputStats.getString();
}


