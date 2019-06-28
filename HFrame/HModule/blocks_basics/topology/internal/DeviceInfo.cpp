

struct DeviceInfo
{
    // VS2015 requires a constructor to avoid aggregate initialization
    DeviceInfo (Block::UID buid, BlocksProtocol::TopologyIndex tidx,
                BlocksProtocol::BlockSerialNumber s, BlocksProtocol::VersionNumber v,
                BlocksProtocol::BlockName n, BlocksProtocol::BatteryLevel level,
                BlocksProtocol::BatteryCharging charging, Block::UID master)
        : uid (buid), index (tidx), serial (s), version (v), name (n),
          batteryLevel (level), batteryCharging (charging), masterUid (master),
          isMaster (uid == master)
    {
    }

    Block::UID uid {};
    BlocksProtocol::TopologyIndex index;
    BlocksProtocol::BlockSerialNumber serial;
    BlocksProtocol::VersionNumber version;
    BlocksProtocol::BlockName name;
    BlocksProtocol::BatteryLevel batteryLevel;
    BlocksProtocol::BatteryCharging batteryCharging;
    Block::UID masterUid;
    bool isMaster {};
};


