

#include "blocks_basics.h"

#if ! HHAS_CONSTEXPR
 #ifndef HDEMO_RUNNER
  #error "The blocks_basics module requires a compiler that supports constexpr"
 #endif
#else

#include <regex>


 #include "littlefoot/LittleFootRemoteHeap.h"
}

#include "protocol/BitPackingUtilities.h"
#include "protocol/BlocksProtocolDefinitions.h"
#include "protocol/HostPacketDecoder.h"
#include "protocol/HostPacketBuilder.h"
#include "protocol/BlockModels.h"

#include "blocks/BlockConfigManager.h"
#include "blocks/Block.cpp"
#include "blocks/BlocksVersion.cpp"
#include "topology/BlockGraph.cpp"
#include "topology/PhysicalTopologySource.cpp"
#include "topology/RuleBasedTopologySource.cpp"
#include "visualisers/DrumPadLEDProgram.cpp"
#include "visualisers/BitmapLEDProgram.cpp"

#endif
