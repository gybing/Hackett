

/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projector to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 H Module Format.txt file.


 BEGIN_HMODULE_DECLARATION

  ID:                 blocks_basics
  vendor:             H
  version:            5.4.3
  name:               Provides low-level control over ROLI BLOCKS devices
  description:        H wrapper for low-level control over ROLI BLOCKS devices.
  website:            http://developer.roli.com
  license:            ISC
  minimumCppStandard: 14

  dependencies:     events audio_devices

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once

//==============================================================================
#include <events/events.h>
#include <audio_devices/audio_devices.h>

#if ! HHAS_CONSTEXPR
 #ifndef HDEMO_RUNNER
  #error "The blocks_basics module requires a compiler that supports constexpr"
 #endif
#else

namespace H
{
    class TouchSurface;
    class LEDGrid;
    class LEDRow;
    class StatusLight;
    class LightRing;
    class ControlButton;
}

#include "blocks/Block.h"
#include "blocks/TouchSurface.h"
#include "blocks/LEDGrid.h"
#include "blocks/LEDRow.h"
#include "blocks/ControlButton.h"
#include "blocks/TouchList.h"
#include "blocks/StatusLight.h"
#include "blocks/BlocksVersion.h"
#include "topology/Topology.h"
#include "topology/BlockGraph.h"
#include "topology/TopologySource.h"
#include "topology/PhysicalTopologySource.h"
#include "topology/RuleBasedTopologySource.h"
#include "visualisers/DrumPadLEDProgram.h"
#include "visualisers/BitmapLEDProgram.h"

namespace H
{
 #include "littlefoot/LittleFootRunner.h"
 #include "littlefoot/LittleFootCompiler.h"
}

#endif
