
#if HMSVC
 #pragma warning (push)
 #pragma warning (disable : 4127 4389 4018)
#endif

#ifndef AI_NUMERICSERV  // (missing in older Mac SDKs)
 #define AI_NUMERICSERV 0x1000
#endif

#if HWINDOWS
 typedef int       socklen_t;
 typedef int       recvsend_size_t;
 typedef SOCKET    SocketHandle;
 static const SocketHandle invalidSocket = INVALID_SOCKET;
#elif HANDROID
 typedef socklen_t socklen_t;
 typedef size_t    recvsend_size_t;
 typedef int       SocketHandle;
 static const SocketHandle invalidSocket = -1;
#else
 typedef socklen_t socklen_t;
 typedef socklen_t recvsend_size_t;
 typedef int       SocketHandle;
 static const SocketHandle invalidSocket = -1;
#endif

//==============================================================================
namespace SocketHelpers
{
    static void initSockets()
    {
       #if HWINDOWS
        static bool socketsStarted = false;

        if (! socketsStarted)
        {
            socketsStarted = true;

            WSADATA wsaData;
            const WORD wVersionRequested = MAKEWORD (1, 1);
            WSAStartup (wVersionRequested, &wsaData);
        }
       #endif
    }

    inline bool isValidPortNumber (int port) noexcept
    {
        return port >= 0 && port < 65536;
    }

    template <typename Type>
    static bool setOption (SocketHandle handle, int mode, int property, Type value) noexcept
    {
        return setsockopt (handle, mode, property, reinterpret_cast<const char*> (&value), sizeof (value)) == 0;
    }

    template <typename Type>
    static bool setOption (SocketHandle handle, int property, Type value) noexcept
    {
        return setOption (handle, SOL_SOCKET, property, value);
    }

    static bool resetSocketOptions (SocketHandle handle, bool isDatagram, bool allowBroadcast) noexcept
    {
        return handle > 0
                && setOption (handle, SO_RCVBUF, (int) 65536)
                && setOption (handle, SO_SNDBUF, (int) 65536)
                && (isDatagram ? ((! allowBroadcast) || setOption (handle, SO_BROADCAST, (int) 1))
                               : setOption (handle, IPPROTO_TCP, TCP_NODELAY, (int) 1));
    }

    static void closeSocket (std::atomic<int>& handle, std::mutex& readLock,
                             bool isListener, int portNumber, std::atomic<bool>& connected) noexcept
    {
        const SocketHandle h = handle.load();
        handle = -1;

       #if HWINDOWS
        if (h != (unsigned) SOCKET_ERROR || connected)
            closesocket (h);

		
        // make sure any read process finishes before we delete the socket
		std::lock_guard<std::mutex> lock(readLock);
        connected = false;
       #else
        if (connected)
        {
            connected = false;

            if (isListener)
            {
                // need to do this to interrupt the accept() function..
                SocketTCP temp;
                temp.connect (IPAddress::local().toString(), portNumber, 1000);
            }
        }

        if (h != -1)
        {
            // unblock any pending read requests
            ::shutdown (h, SHUT_RDWR);

            {
                // see man-page of recv on linux about a race condition where the
                // shutdown command is lost if the receiving thread does not have
                // a chance to process before close is called. On Mac OS X shutdown
                // does not unblock a select call, so using a lock here will dead-lock
                // both threads.
               #if HLINUX || HANDROID
                std::mutex::ScopeLockType lock (readLock);
                ::close (h);
               #else
                ::close (h);
                std::mutex::ScopeLockType lock (readLock);
              #endif
            }
        }
       #endif
    }

    static bool bindSocket (SocketHandle handle, int port, const std::string& address) noexcept
    {
        if (handle <= 0 || ! isValidPortNumber (port))
            return false;

        struct sockaddr_in addr;
		memset((void*)&addr, 0, sizeof(addr)); // (can't use "= { 0 }" on this object because it's typedef'ed as a C struct)

        addr.sin_family = PF_INET;
        addr.sin_port = htons ((uint16) port);
        addr.sin_addr.s_addr = !address.empty() ? ::inet_addr (address.c_str())
                                                    : htonl (INADDR_ANY);

        return ::bind (handle, (struct sockaddr*) &addr, sizeof (addr)) >= 0;
    }

    static int getBoundPort (SocketHandle handle) noexcept
    {
        if (handle > 0)
        {
            struct sockaddr_in addr;
            socklen_t len = sizeof (addr);

            if (getsockname (handle, (struct sockaddr*) &addr, &len) == 0)
                return ntohs (addr.sin_port);
        }

        return -1;
    }

    static std::string getConnectedAddress (SocketHandle handle) noexcept
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof (addr);

        if (getpeername (handle, (struct sockaddr*) &addr, &len) >= 0)
            return inet_ntoa (addr.sin_addr);

        return std::string ("0.0.0.0");
    }

    static int readSocket (SocketHandle handle,
                           void* destBuffer, int maxBytesToRead,
                           std::atomic<bool>& connected,
                           bool blockUntilSpecifiedAmountHasArrived,
                           std::mutex& readLock,
                           std::string* senderIP = nullptr,
                           int* senderPort = nullptr) noexcept
    {
        int bytesRead = 0;

        while (bytesRead < maxBytesToRead)
        {
            long bytesThisTime = -1;
            auto buffer = static_cast<char*> (destBuffer) + bytesRead;
            auto numToRead = (recvsend_size_t) (maxBytesToRead - bytesRead);

            {
                // avoid race-condition
                std::lock_guard<std::mutex> lock (readLock);

				if (senderIP == nullptr || senderPort == nullptr)
				{
					bytesThisTime = ::recv(handle, buffer, numToRead, 0);
				}
				else
				{
					sockaddr_in client;
					socklen_t clientLen = sizeof(sockaddr);

					bytesThisTime = ::recvfrom(handle, buffer, numToRead, 0, (sockaddr*)&client, &clientLen);

					*senderIP = inet_ntoa(client.sin_addr);
					*senderPort = ntohs(client.sin_port);
				}
            }

            if (bytesThisTime <= 0 || ! connected)
            {
                if (bytesRead == 0 && blockUntilSpecifiedAmountHasArrived)
                    bytesRead = -1;

                break;
            }

            bytesRead += bytesThisTime;

            if (! blockUntilSpecifiedAmountHasArrived)
                break;
        }

        return (int) bytesRead;
    }

    static int waitForReadiness (std::atomic<int>& handle, std::mutex& readLock,
                                 bool forReading, int timeoutMsecs) noexcept
    {
        // avoid race-condition
        std::unique_lock<std::mutex> lock (readLock);

        if (!lock.try_lock())
            return -1;

        int h = handle.load();

        struct timeval timeout;
        struct timeval* timeoutp;

        if (timeoutMsecs >= 0)
        {
            timeout.tv_sec = timeoutMsecs / 1000;
            timeout.tv_usec = (timeoutMsecs % 1000) * 1000;
            timeoutp = &timeout;
        }
        else
        {
            timeoutp = nullptr;
        }

        fd_set rset, wset;
        FD_ZERO (&rset);
        FD_SET (h, &rset);
        FD_ZERO (&wset);
        FD_SET (h, &wset);

        fd_set* const prset = forReading ? &rset : nullptr;
        fd_set* const pwset = forReading ? nullptr : &wset;

       #if HWINDOWS
        if (select ((int) h + 1, prset, pwset, 0, timeoutp) < 0)
            return -1;
       #else
        {
            int result;

            while ((result = select (h + 1, prset, pwset, nullptr, timeoutp)) < 0
                    && errno == EINTR)
            {
            }

            if (result < 0)
                return -1;
        }
       #endif

        // we are closing
        if (handle.load() < 0)
            return -1;

        {
            int opt;
            socklen_t len = sizeof (opt);

            if (getsockopt (h, SOL_SOCKET, SO_ERROR, (char*) &opt, &len) < 0
                 || opt != 0)
                return -1;
        }

        return FD_ISSET (h, forReading ? &rset : &wset) ? 1 : 0;
    }

    static bool setSocketBlockingState (SocketHandle handle, bool shouldBlock) noexcept
    {
       #if HWINDOWS
        u_long nonBlocking = shouldBlock ? 0 : (u_long) 1;
        return ioctlsocket (handle, FIONBIO, &nonBlocking) == 0;
       #else
        int socketFlags = fcntl (handle, F_GETFL, 0);

        if (socketFlags == -1)
            return false;

        if (shouldBlock)
            socketFlags &= ~O_NONBLOCK;
        else
            socketFlags |= O_NONBLOCK;

        return fcntl (handle, F_SETFL, socketFlags) == 0;
       #endif
    }

    static addrinfo* getAddressInfo (bool isDatagram, const std::string& hostName, int portNumber)
    {
        struct addrinfo hints;
		memset((void*)&hints, 0, sizeof(hints));

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = isDatagram ? SOCK_DGRAM : SOCK_STREAM;
        hints.ai_flags = AI_NUMERICSERV;

        struct addrinfo* info = nullptr;

        if (getaddrinfo (hostName.c_str(), std::to_string(portNumber).c_str(), &hints, &info) == 0)
            return info;

        return nullptr;
    }

    static bool connectSocket (std::atomic<int>& handle,
                               std::mutex& readLock,
                               const std::string& hostName,
                               int portNumber,
                               int timeOutMillisecs) noexcept
    {
        bool success = false;

        if (auto* info = getAddressInfo (false, hostName, portNumber))
        {
            for (auto* i = info; i != nullptr; i = i->ai_next)
            {
                auto newHandle = socket (i->ai_family, i->ai_socktype, 0);

                if (newHandle != invalidSocket)
                {
                    auto result = ::connect (newHandle, i->ai_addr, (socklen_t) i->ai_addrlen);
                    success = (result >= 0);

                    if (! success)
                    {
                       #if HWINDOWS
                        if (result == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
                       #else
                        if (errno == EINPROGRESS)
                       #endif
                        {
                            std::atomic<int> cvHandle { (int) newHandle };

                            if (waitForReadiness (cvHandle, readLock, false, timeOutMillisecs) == 1)
                                success = true;
                        }
                    }

                    if (success)
                    {
                        handle = (int) newHandle;
                        break;
                    }

                   #if HWINDOWS
                    closesocket (newHandle);
                   #else
                    ::close (newHandle);
                   #endif
                }
            }

            freeaddrinfo (info);

            if (success)
            {
                setSocketBlockingState (handle, true);
                resetSocketOptions (handle, false, false);
            }
        }

        return success;
    }

    static void makeReusable (int handle) noexcept
    {
        setOption (handle, SO_REUSEADDR, (int) 1);
    }

    static bool multicast (int handle, const std::string& multicastIPAddress,
                           const std::string& interfaceIPAddress, bool join) noexcept
    {
        struct ip_mreq mreq;
		memset((void*)&mreq, 0, sizeof(mreq));

        mreq.imr_multiaddr.s_addr = inet_addr (multicastIPAddress.c_str());
        mreq.imr_interface.s_addr = INADDR_ANY;

        if (!interfaceIPAddress.empty())
            mreq.imr_interface.s_addr = inet_addr (interfaceIPAddress.c_str());

        return setsockopt (handle, IPPROTO_IP,
                           join ? IP_ADD_MEMBERSHIP
                                : IP_DROP_MEMBERSHIP,
                           (const char*) &mreq, sizeof (mreq)) == 0;
    }
}

//==============================================================================
SocketTCP::SocketTCP()
{
    SocketHelpers::initSockets();
}

SocketTCP::SocketTCP (const std::string& host, int portNum, int h)
    : hostName (host),
      portNumber (portNum),
      handle (h),
      connected (true)
{
    HAssert (SocketHelpers::isValidPortNumber (portNum));

    SocketHelpers::initSockets();
    SocketHelpers::resetSocketOptions (h, false, false);
}

SocketTCP::~SocketTCP()
{
    close();
}

//==============================================================================
int SocketTCP::read (void* destBuffer, int maxBytesToRead, bool shouldBlock)
{
    return (connected && ! isListener) ? SocketHelpers::readSocket (handle, destBuffer, maxBytesToRead,
                                                                    connected, shouldBlock, readLock)
                                       : -1;
}

int SocketTCP::write (const void* sourceBuffer, int numBytesToWrite)
{
    if (isListener || ! connected)
        return -1;

    return (int) ::send (handle, (const char*) sourceBuffer, (recvsend_size_t) numBytesToWrite, 0);
}

//==============================================================================
int SocketTCP::waitUntilReady (bool readyForReading, int timeoutMsecs)
{
    return connected ? SocketHelpers::waitForReadiness (handle, readLock, readyForReading, timeoutMsecs)
                     : -1;
}

//==============================================================================
bool SocketTCP::bindToPort (int port)
{
    return bindToPort (port, std::string());
}

bool SocketTCP::bindToPort (int port, const std::string& addr)
{
    HAssert (SocketHelpers::isValidPortNumber (port));

    return SocketHelpers::bindSocket (handle, port, addr);
}

int SocketTCP::getBoundPort() const noexcept
{
    return SocketHelpers::getBoundPort (handle);
}

bool SocketTCP::connect (const std::string& remoteHostName, int remotePortNumber, int timeOutMillisecs)
{
    HAssert (SocketHelpers::isValidPortNumber (remotePortNumber));

    if (isListener)
    {
        HAssert(false);    // a listener socket can't connect to another one!
        return false;
    }

    if (connected)
        close();

    hostName = remoteHostName;
    portNumber = remotePortNumber;
    isListener = false;

    connected = SocketHelpers::connectSocket (handle, readLock, remoteHostName,
                                              remotePortNumber, timeOutMillisecs);

    if (! (connected && SocketHelpers::resetSocketOptions (handle, false, false)))
    {
        close();
        return false;
    }

    return true;
}

void SocketTCP::close()
{
    SocketHelpers::closeSocket (handle, readLock, isListener, portNumber, connected);

    hostName.clear();
    portNumber = 0;
    handle = -1;
    isListener = false;
}

//==============================================================================
bool SocketTCP::createListener (int newPortNumber, const std::string& localHostName)
{
    HAssert (SocketHelpers::isValidPortNumber (newPortNumber));

    if (connected)
        close();

    hostName = "listener";
    portNumber = newPortNumber;
    isListener = true;

    handle = (int) socket (AF_INET, SOCK_STREAM, 0);

    if (handle < 0)
        return false;

   #if ! HWINDOWS // on windows, adding this option produces behaviour different to posix
    SocketHelpers::makeReusable (handle);
   #endif

    if (SocketHelpers::bindSocket (handle, portNumber, localHostName)
         && listen (handle, SOMAXCONN) >= 0)
    {
        connected = true;
        return true;
    }

    close();
    return false;
}

SocketTCP* SocketTCP::waitForNextConnection() const
{
    // To call this method, you first have to use createListener() to
    // prepare this socket as a listener.
    HAssert (isListener || ! connected);

    if (connected && isListener)
    {
        struct sockaddr_storage address;
        socklen_t len = sizeof (address);
        auto newSocket = (int) accept (handle, (struct sockaddr*) &address, &len);

        if (newSocket >= 0 && connected)
            return new SocketTCP (inet_ntoa (((struct sockaddr_in*) &address)->sin_addr),
                                        portNumber, newSocket);
    }

    return nullptr;
}

bool SocketTCP::isLocal() const noexcept
{
    if (! isConnected())
        return false;

    return hostName == "127.0.0.1" || hostName == "localhost";
}


//==============================================================================
//==============================================================================
SocketUDP::SocketUDP (bool canBroadcast)
{
    SocketHelpers::initSockets();

    handle = (int) socket (AF_INET, SOCK_DGRAM, 0);

    if (handle >= 0)
    {
        SocketHelpers::resetSocketOptions (handle, true, canBroadcast);
        SocketHelpers::makeReusable (handle);
    }
}

SocketUDP::~SocketUDP()
{
    if (lastServerAddress != nullptr)
        freeaddrinfo (static_cast<struct addrinfo*> (lastServerAddress));

    shutdown();
}

void SocketUDP::shutdown()
{
    if (handle < 0)
        return;

    std::atomic<int> handleCopy { handle.load() };
    handle = -1;
    std::atomic<bool> connected { false };
    SocketHelpers::closeSocket (handleCopy, readLock, false, 0, connected);
}

bool SocketUDP::bindToPort (int port)
{
    return bindToPort (port, std::string());
}

bool SocketUDP::bindToPort (int port, const std::string& addr)
{
    HAssert (SocketHelpers::isValidPortNumber (port));

    if (SocketHelpers::bindSocket (handle, port, addr))
    {
        isBound = true;
        lastBindAddress = addr;
        return true;
    }

    return false;
}

int SocketUDP::getBoundPort() const noexcept
{
    return (handle >= 0 && isBound) ? SocketHelpers::getBoundPort (handle) : -1;
}

//==============================================================================
int SocketUDP::waitUntilReady (bool readyForReading, int timeoutMsecs)
{
    if (handle < 0)
        return -1;

    return SocketHelpers::waitForReadiness (handle, readLock, readyForReading, timeoutMsecs);
}

int SocketUDP::read (void* destBuffer, int maxBytesToRead, bool shouldBlock)
{
    if (handle < 0 || ! isBound)
        return -1;

    std::atomic<bool> connected { true };

    SocketHelpers::setSocketBlockingState (handle, shouldBlock);
    return SocketHelpers::readSocket (handle, destBuffer, maxBytesToRead,
                                      connected, shouldBlock, readLock);
}

int SocketUDP::read (void* destBuffer, int maxBytesToRead, bool shouldBlock, std::string& senderIPAddress, int& senderPort)
{
    if (handle < 0 || ! isBound)
        return -1;

    std::atomic<bool> connected { true };

    SocketHelpers::setSocketBlockingState (handle, shouldBlock);
    return SocketHelpers::readSocket (handle, destBuffer, maxBytesToRead, connected,
                                      shouldBlock, readLock, &senderIPAddress, &senderPort);
}

int SocketUDP::write (const std::string& remoteHostname, int remotePortNumber,
                           const void* sourceBuffer, int numBytesToWrite)
{
    HAssert (SocketHelpers::isValidPortNumber (remotePortNumber));

    if (handle < 0)
        return -1;

    struct addrinfo*& info = reinterpret_cast<struct addrinfo*&> (lastServerAddress);

    // getaddrinfo can be quite slow so cache the result of the address lookup
    if (info == nullptr || remoteHostname != lastServerHost || remotePortNumber != lastServerPort)
    {
        if (info != nullptr)
            freeaddrinfo (info);

        if ((info = SocketHelpers::getAddressInfo (true, remoteHostname, remotePortNumber)) == nullptr)
            return -1;

        lastServerHost = remoteHostname;
        lastServerPort = remotePortNumber;
    }

    return (int) ::sendto (handle, (const char*) sourceBuffer,
                           (recvsend_size_t) numBytesToWrite, 0,
                           info->ai_addr, (socklen_t) info->ai_addrlen);
}

bool SocketUDP::joinMulticast (const std::string& multicastIPAddress)
{
    if (! isBound || handle < 0)
        return false;

    return SocketHelpers::multicast (handle, multicastIPAddress, lastBindAddress, true);
}

bool SocketUDP::leaveMulticast (const std::string& multicastIPAddress)
{
    if (! isBound || handle < 0)
        return false;

    return SocketHelpers::multicast (handle, multicastIPAddress, lastBindAddress, false);
}

bool SocketUDP::setMulticastLoopbackEnabled (bool enable)
{
    if (! isBound || handle < 0)
        return false;

    return SocketHelpers::setOption<bool> (handle, IPPROTO_IP, IP_MULTICAST_LOOP, enable);
}

bool SocketUDP::setEnablePortReuse (bool enabled)
{
   #if HANDROID
   #else
    if (handle >= 0)
        return SocketHelpers::setOption (handle,
                                        #if HWINDOWS || HLINUX
                                         SO_REUSEADDR,  // port re-use is implied by addr re-use on these platforms
                                        #else
                                         SO_REUSEPORT,
                                        #endif
                                         (int) (enabled ? 1 : 0));
   #endif

    return false;
}

#if HMSVC
 #pragma warning (pop)
#endif
