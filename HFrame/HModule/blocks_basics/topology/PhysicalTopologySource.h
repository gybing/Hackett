

/**
    This topology source manages the topology of the physical Blocks devices
    that are currently connected. It maintains a list of them and tells
    listeners when physical devices are added or removed.

    @tags{Blocks}
*/
class PhysicalTopologySource  : public TopologySource
{
public:
    /** Constructor. */
    PhysicalTopologySource (bool startDetached = false);

    /** Destructor. */
    ~PhysicalTopologySource() override;

    /** Returns the current physical topology. */
    BlockTopology getCurrentTopology() const override;

    /** Reset all touches */
    void cancelAllActiveTouches() noexcept override;

    /** Sets the TopologySource as active, occupying the midi port and trying to connect to the block devices */
    void setActive (bool shouldBeActive) override;

    /** Returns true, if the TopologySource is currently trying to connect the block devices */
    bool isActive() const override;

    /** This method will tell, if an other PhysicalTopologySource has locked the Midi connection */
    bool isLockedFromOutside() const;

    //==========================================================================
    /** For custom transport systems, this represents a connected device */
    struct DeviceConnection
    {
        DeviceConnection();
        virtual ~DeviceConnection();

        virtual bool sendMessageToDevice (const void* data, size_t dataSize) = 0;
        std::function<void(const void* data, size_t dataSize)> handleMessageFromDevice;
    };

    /** For custom transport systems, this represents a connected device */
    struct DeviceDetector
    {
        DeviceDetector();
        virtual ~DeviceDetector();

        virtual StringArray scanForDevices() = 0;
        virtual DeviceConnection* openDevice (int index) = 0;
        virtual bool isLockedFromOutside() const { return false; }
    };

    /** Constructor for custom transport systems. */
    PhysicalTopologySource (DeviceDetector& detectorToUse, bool startDetached = false);

    static const char* const* getStandardLittleFootFunctions() noexcept;

protected:
    virtual bool hasOwnServiceTimer() const;
    virtual void handleTimerTick();

private:
    //==========================================================================
    DeviceDetector* customDetector = nullptr;
    friend struct Detector;
    struct DetectorHolder;
    std::unique_ptr<DetectorHolder> detector;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhysicalTopologySource)
};


