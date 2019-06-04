

InterprocessConnectionServer::InterprocessConnectionServer() : Thread ("H IPC server")
{
}

InterprocessConnectionServer::~InterprocessConnectionServer()
{
    stop();
}

//==============================================================================
bool InterprocessConnectionServer::beginWaitingForSocket (const int portNumber, const String& bindAddress)
{
    stop();

    socket.reset (new StreamingSocket());

    if (socket->createListener (portNumber, bindAddress))
    {
        startThread();
        return true;
    }

    socket.reset();
    return false;
}

void InterprocessConnectionServer::stop()
{
    signalThreadShouldExit();

    if (socket != nullptr)
        socket->close();

    stopThread (4000);
    socket.reset();
}

int InterprocessConnectionServer::getBoundPort() const noexcept
{
    return (socket == nullptr) ? -1 : socket->getBoundPort();
}

void InterprocessConnectionServer::run()
{
    while ((! threadShouldExit()) && socket != nullptr)
    {
        std::unique_ptr<StreamingSocket> clientSocket (socket->waitForNextConnection());

        if (clientSocket != nullptr)
            if (auto* newConnection = createConnectionObject())
                newConnection->initialiseWithSocket (clientSocket.release());
    }
}


