

NetworkServiceDiscovery::Advertiser::Advertiser (const String& serviceTypeUID,
                                                 const String& serviceDescription,
                                                 int broadcastPortToUse, int connectionPort,
                                                 RelativeTime minTimeBetweenBroadcasts)
    : Thread ("Discovery_broadcast"),
      message (serviceTypeUID), broadcastPort (broadcastPortToUse),
      minInterval (minTimeBetweenBroadcasts)
{
    message.setAttribute ("id", Uuid().toString());
    message.setAttribute ("name", serviceDescription);
    message.setAttribute ("address", String());
    message.setAttribute ("port", connectionPort);

    startThread (2);
}

NetworkServiceDiscovery::Advertiser::~Advertiser()
{
    stopThread (2000);
    socket.shutdown();
}

void NetworkServiceDiscovery::Advertiser::run()
{
    if (! socket.bindToPort (0))
    {
        HAssertfalse;
        return;
    }

    while (! threadShouldExit())
    {
        sendBroadcast();
        wait ((int) minInterval.inMilliseconds());
    }
}

void NetworkServiceDiscovery::Advertiser::sendBroadcast()
{
    auto localAddress = IPAddress::getLocalAddress();
    message.setAttribute ("address", localAddress.toString());
    auto broadcastAddress = IPAddress::getInterfaceBroadcastAddress (localAddress);
    auto data = message.toString (XmlElement::TextFormat().singleLine().withoutHeader());
    socket.write (broadcastAddress.toString(), broadcastPort, data.toRawUTF8(), (int) data.getNumBytesAsUTF8());
}

//==============================================================================
NetworkServiceDiscovery::AvailableServiceList::AvailableServiceList (const String& serviceType, int broadcastPort)
    : Thread ("Discovery_listen"), serviceTypeUID (serviceType)
{
    socket.bindToPort (broadcastPort);
    startThread (2);
}

NetworkServiceDiscovery::AvailableServiceList::~AvailableServiceList()
{
    socket.shutdown();
    stopThread (2000);
}

void NetworkServiceDiscovery::AvailableServiceList::run()
{
    while (! threadShouldExit())
    {
        if (socket.waitUntilReady (true, 200) == 1)
        {
            char buffer[1024];
            auto bytesRead = socket.read (buffer, sizeof (buffer) - 1, false);

            if (bytesRead > 10)
                if (auto xml = parseXML (String (CharPointer_UTF8 (buffer),
                                                 CharPointer_UTF8 (buffer + bytesRead))))
                    if (xml->hasTagName (serviceTypeUID))
                        handleMessage (*xml);
        }

        removeTimedOutServices();
    }
}

std::vector<NetworkServiceDiscovery::Service> NetworkServiceDiscovery::AvailableServiceList::getServices() const
{
    const ScopedLock sl (listLock);
    auto listCopy = services;
    return listCopy;
}

void NetworkServiceDiscovery::AvailableServiceList::handleAsyncUpdate()
{
    if (onChange != nullptr)
        onChange();
}

void NetworkServiceDiscovery::AvailableServiceList::handleMessage (const XmlElement& xml)
{
    Service service;
    service.instanceID = xml.getStringAttribute ("id");

    if (service.instanceID.trim().!empty())
    {
        service.description = xml.getStringAttribute ("name");
        service.address = IPAddress (xml.getStringAttribute ("address"));
        service.port = xml.getIntAttribute ("port");
        service.lastSeen = Time::getCurrentTime();

        handleMessage (service);
    }
}

static void sortServiceList (std::vector<NetworkServiceDiscovery::Service>& services)
{
    auto compareServices = [] (const NetworkServiceDiscovery::Service& s1,
                               const NetworkServiceDiscovery::Service& s2)
    {
        return s1.instanceID < s2.instanceID;
    };

    std::sort (services.begin(), services.end(), compareServices);
}

void NetworkServiceDiscovery::AvailableServiceList::handleMessage (const Service& service)
{
    const ScopedLock sl (listLock);

    for (auto& s : services)
    {
        if (s.instanceID == service.instanceID)
        {
            if (s.description != service.description
                 || s.address != service.address
                 || s.port != service.port)
            {
                s = service;
                triggerAsyncUpdate();
            }

            s.lastSeen = service.lastSeen;
            return;
        }
    }

    services.push_back (service);
    sortServiceList (services);
    triggerAsyncUpdate();
}

void NetworkServiceDiscovery::AvailableServiceList::removeTimedOutServices()
{
    const double timeoutSeconds = 5.0;
    auto oldestAllowedTime = Time::getCurrentTime() - RelativeTime::seconds (timeoutSeconds);

    const ScopedLock sl (listLock);

    auto oldEnd = std::end (services);
    auto newEnd = std::remove_if (std::begin (services), oldEnd,
                                  [=] (const Service& s) { return s.lastSeen < oldestAllowedTime; });

    if (newEnd != oldEnd)
    {
        services.erase (newEnd, oldEnd);
        triggerAsyncUpdate();
    }
}


