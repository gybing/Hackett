

struct PhysicalTopologySource::DetectorHolder  : private Timer
{
    DetectorHolder (PhysicalTopologySource& pts)
        : topologySource (pts),
          detector (Detector::getDefaultDetector())
    {
        startTimerHz (30);
    }

    DetectorHolder (PhysicalTopologySource& pts, DeviceDetector& dd)
        : topologySource (pts),
          detector (new Detector (dd))
    {
        startTimerHz (30);
    }

    void timerCallback() override
    {
        if (! topologySource.hasOwnServiceTimer())
            handleTimerTick();
    }

    void handleTimerTick()
    {
        for (auto& b : detector->currentTopology.blocks)
            if (auto bi = BlockImplementation<Detector>::getFrom (*b))
                bi->handleTimerTick();
    }

    PhysicalTopologySource& topologySource;
    Detector::Ptr detector;
};


