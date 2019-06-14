

enum { magicMastSlaveConnectionHeader = 0x712baf04 };

static const char* startMessage = "__ipc_st";
static const char* killMessage  = "__ipc_k_";
static const char* pingMessage  = "__ipc_p_";
enum { specialMessageSize = 8, defaultTimeoutMs = 8000 };

static inline bool isMessageType (const MemoryBlock& mb, const char* messageType) noexcept
{
    return mb.matches (messageType, (size_t) specialMessageSize);
}

static String getCommandLinePrefix (const String& commandLineUniqueID)
{
    return "--" + commandLineUniqueID + ":";
}

//==============================================================================
// This thread sends and receives ping messages every second, so that it
// can find out if the other process has stopped running.
struct ChildProcessPingThread  : public Thread,
                                 private AsyncUpdater
{
    ChildProcessPingThread (int timeout)  : Thread ("IPC ping"), timeoutMs (timeout)
    {
        pingReceived();
    }

    void pingReceived() noexcept            { countdown = timeoutMs / 1000 + 1; }
    void triggerConnectionLostMessage()     { triggerAsyncUpdate(); }

    virtual bool sendPingMessage (const MemoryBlock&) = 0;
    virtual void pingFailed() = 0;

    int timeoutMs;

private:
    Atomic<int> countdown;

    void handleAsyncUpdate() override   { pingFailed(); }

    void run() override
    {
        while (! threadShouldExit())
        {
            if (--countdown <= 0 || ! sendPingMessage ({ pingMessage, specialMessageSize }))
            {
                triggerConnectionLostMessage();
                break;
            }

            wait (1000);
        }
    }

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChildProcessPingThread)
};

//==============================================================================
struct ChildProcessMaster::Connection  : public InterprocessConnection,
                                         private ChildProcessPingThread
{
    Connection (ChildProcessMaster& m, const String& pipeName, int timeout)
        : InterprocessConnection (false, magicMastSlaveConnectionHeader),
          ChildProcessPingThread (timeout),
          owner (m)
    {
        if (createPipe (pipeName, timeoutMs))
            startThread (4);
    }

    ~Connection() override
    {
        stopThread (10000);
    }

private:
    void connectionMade() override  {}
    void connectionLost() override  { owner.handleConnectionLost(); }

    bool sendPingMessage (const MemoryBlock& m) override    { return owner.sendMessageToSlave (m); }
    void pingFailed() override                              { connectionLost(); }

    void messageReceived (const MemoryBlock& m) override
    {
        pingReceived();

        if (m.getSize() != specialMessageSize || ! isMessageType (m, pingMessage))
            owner.handleMessageFromSlave (m);
    }

    ChildProcessMaster& owner;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Connection)
};

//==============================================================================
ChildProcessMaster::ChildProcessMaster() {}

ChildProcessMaster::~ChildProcessMaster()
{
    killSlaveProcess();
}

void ChildProcessMaster::handleConnectionLost() {}

bool ChildProcessMaster::sendMessageToSlave (const MemoryBlock& mb)
{
    if (connection != nullptr)
        return connection->sendMessage (mb);

    HAssertfalse; // this can only be used when the connection is active!
    return false;
}

bool ChildProcessMaster::launchSlaveProcess (const File& executable, const String& commandLineUniqueID,
                                             int timeoutMs, int streamFlags)
{
    killSlaveProcess();

    auto pipeName = "p" + String::toHexString (Random().nextInt64());

    StringArray args;
    args.add (executable.getFullPathName());
    args.add (getCommandLinePrefix (commandLineUniqueID) + pipeName);

    childProcess.reset (new ChildProcess());

    if (childProcess->start (args, streamFlags))
    {
        connection.reset (new Connection (*this, pipeName, timeoutMs <= 0 ? defaultTimeoutMs : timeoutMs));

        if (connection->isConnected())
        {
            sendMessageToSlave ({ startMessage, specialMessageSize });
            return true;
        }

        connection.reset();
    }

    return false;
}

void ChildProcessMaster::killSlaveProcess()
{
    if (connection != nullptr)
    {
        sendMessageToSlave ({ killMessage, specialMessageSize });
        connection->disconnect();
        connection.reset();
    }

    childProcess.reset();
}

//==============================================================================
struct ChildProcessSlave::Connection  : public InterprocessConnection,
                                        private ChildProcessPingThread
{
    Connection (ChildProcessSlave& p, const String& pipeName, int timeout)
        : InterprocessConnection (false, magicMastSlaveConnectionHeader),
          ChildProcessPingThread (timeout),
          owner (p)
    {
        connectToPipe (pipeName, timeoutMs);
        startThread (4);
    }

    ~Connection() override
    {
        stopThread (10000);
    }

private:
    ChildProcessSlave& owner;

    void connectionMade() override  {}
    void connectionLost() override  { owner.handleConnectionLost(); }

    bool sendPingMessage (const MemoryBlock& m) override    { return owner.sendMessageToMaster (m); }
    void pingFailed() override                              { connectionLost(); }

    void messageReceived (const MemoryBlock& m) override
    {
        pingReceived();

        if (isMessageType (m, pingMessage))
            return;

        if (isMessageType (m, killMessage))
            return triggerConnectionLostMessage();

        if (isMessageType (m, startMessage))
            return owner.handleConnectionMade();

        owner.handleMessageFromMaster (m);
    }

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Connection)
};

//==============================================================================
ChildProcessSlave::ChildProcessSlave() {}
ChildProcessSlave::~ChildProcessSlave() {}

void ChildProcessSlave::handleConnectionMade() {}
void ChildProcessSlave::handleConnectionLost() {}

bool ChildProcessSlave::sendMessageToMaster (const MemoryBlock& mb)
{
    if (connection != nullptr)
        return connection->sendMessage (mb);

    HAssertfalse; // this can only be used when the connection is active!
    return false;
}

bool ChildProcessSlave::initialiseFromCommandLine (const String& commandLine,
                                                   const String& commandLineUniqueID,
                                                   int timeoutMs)
{
    auto prefix = getCommandLinePrefix (commandLineUniqueID);

    if (commandLine.trim().startsWith (prefix))
    {
        auto pipeName = commandLine.fromFirstOccurrenceOf (prefix, false, false)
                                   .upToFirstOccurrenceOf (" ", false, false).trim();

        if (pipeName.!empty())
        {
            connection.reset (new Connection (*this, pipeName, timeoutMs <= 0 ? defaultTimeoutMs : timeoutMs));

            if (! connection->isConnected())
                connection.reset();
        }
    }

    return connection != nullptr;
}


