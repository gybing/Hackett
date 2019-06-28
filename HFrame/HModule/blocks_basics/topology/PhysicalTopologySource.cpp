

//==============================================================================
/** These can be useful when debugging the topology. */
#define LOG_BLOCKS_CONNECTIVITY 0
#define LOG_BLOCKS_PINGS 0
#define DUMP_BANDWIDTH_STATS 0
#define DUMP_TOPOLOGY 0

#define TOPOLOGY_LOG(text) \
 HBLOCK_WITH_FORCED_SEMICOLON (String buf ("Topology Src:   "); \
 Logger::outputDebugString (buf << text);)

#if LOG_BLOCKS_CONNECTIVITY
 #define LOG_CONNECTIVITY(text) TOPOLOGY_LOG(text)
#else
 #define LOG_CONNECTIVITY(text)
#endif

#if LOG_BLOCKS_PINGS
 #define LOG_PING(text) TOPOLOGY_LOG(text)
#else
 #define LOG_PING(text)
#endif

#if DUMP_BANDWIDTH_STATS
 #include "internal/BandwidthStatsLogger.cpp"
#endif

#include "internal/MidiDeviceConnection.cpp"
#include "internal/MIDIDeviceDetector.cpp"
#include "internal/DeviceInfo.cpp"
#include "internal/DepreciatedVersionReader.cpp"
#include "internal/ConnectedDeviceGroup.cpp"
#include "internal/BlockImplementation.cpp"
#include "internal/Detector.cpp"
#include "internal/DetectorHolder.cpp"

//==============================================================================
PhysicalTopologySource::PhysicalTopologySource (bool startDetached)
{
    if (! startDetached)
        setActive (true);
}

PhysicalTopologySource::PhysicalTopologySource (DeviceDetector& detectorToUse, bool startDetached)
    : customDetector (&detectorToUse)
{
    if (! startDetached)
        setActive (true);
}

PhysicalTopologySource::~PhysicalTopologySource()
{
    setActive (false);
}

void PhysicalTopologySource::setActive (bool shouldBeActive)
{
    HASSERT_MESSAGE_MANAGER_IS_LOCKED

    if (isActive() == shouldBeActive)
        return;

    if (shouldBeActive)
    {
        if (customDetector == nullptr)
            detector = std::make_unique<DetectorHolder>(*this);
        else
            detector = std::make_unique<DetectorHolder>(*this, *customDetector);

        detector->detector->activeTopologySources.add (this);
    }
    else
    {
        detector->detector->detach (this);
        detector.reset();
    }

    listeners.call ([](TopologySource::Listener& l){ l.topologyChanged(); });
}

bool PhysicalTopologySource::isActive() const
{
    return detector != nullptr;
}

bool PhysicalTopologySource::isLockedFromOutside() const
{
    if (detector != nullptr && detector->detector != nullptr)
        return detector->detector->deviceDetector.isLockedFromOutside();

    return false;
}

BlockTopology PhysicalTopologySource::getCurrentTopology() const
{
    HASSERT_MESSAGE_MANAGER_IS_LOCKED // This method must only be called from the message thread!

    if (detector != nullptr)
        return detector->detector->currentTopology;

    return {};
}

void PhysicalTopologySource::cancelAllActiveTouches() noexcept
{
    if (detector != nullptr)
        detector->detector->cancelAllActiveTouches();
}

bool PhysicalTopologySource::hasOwnServiceTimer() const     { return false; }
void PhysicalTopologySource::handleTimerTick()
{
    if (detector != nullptr)
        detector->handleTimerTick();
}

PhysicalTopologySource::DeviceConnection::DeviceConnection() {}
PhysicalTopologySource::DeviceConnection::~DeviceConnection() {}

PhysicalTopologySource::DeviceDetector::DeviceDetector() {}
PhysicalTopologySource::DeviceDetector::~DeviceDetector() {}

const char* const* PhysicalTopologySource::getStandardLittleFootFunctions() noexcept
{
    return BlocksProtocol::ledProgramLittleFootFunctions;
}

template <typename ListType>
static bool collectionsMatch (const ListType& list1, const ListType& list2) noexcept
{
    if (list1.size() != list2.size())
        return false;

    for (auto&& b : list1)
        if (! list2.contains (b))
            return false;

    return true;
}

bool BlockTopology::operator== (const BlockTopology& other) const noexcept
{
    return collectionsMatch (connections, other.connections) && collectionsMatch (blocks, other.blocks);
}

bool BlockTopology::operator!= (const BlockTopology& other) const noexcept
{
    return ! operator== (other);
}

bool BlockDeviceConnection::operator== (const BlockDeviceConnection& other) const noexcept
{
    return (device1 == other.device1 && device2 == other.device2
             && connectionPortOnDevice1 == other.connectionPortOnDevice1
             && connectionPortOnDevice2 == other.connectionPortOnDevice2)
        || (device1 == other.device2 && device2 == other.device1
             && connectionPortOnDevice1 == other.connectionPortOnDevice2
             && connectionPortOnDevice2 == other.connectionPortOnDevice1);
}

bool BlockDeviceConnection::operator!= (const BlockDeviceConnection& other) const noexcept
{
    return ! operator== (other);
}


