

/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projector to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 H Module Format.txt file.


 BEGIN_HMODULE_DECLARATION

  ID:               audio_basics
  vendor:           H
  version:          5.4.3
  name:             H audio and MIDI data classes
  description:      Classes for audio buffer manipulation, midi message handling, synthesis, etc.
  website:          http://www.H.com/H
  license:          ISC

  dependencies:     core
  OSXFrameworks:    Accelerate
  iOSFrameworks:    Accelerate

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HAUDIO_BASICS_H_INCLUDED

#include <core/core.h>

//==============================================================================
#undef Complex  // apparently some C libraries actually define these symbols (!)
#undef Factor

//==============================================================================
#if HMINGW && ! defined (__SSE2__)
 #define HUSE_SSE_INTRINSICS 0
#endif

#ifndef HUSE_SSE_INTRINSICS
 #define HUSE_SSE_INTRINSICS 1
#endif

#if ! HINTEL
 #undef HUSE_SSE_INTRINSICS
#endif

#if __ARM_NEON__ && ! (HUSE_VDSP_FRAMEWORK || defined (HUSE_ARM_NEON))
 #define HUSE_ARM_NEON 1
#endif

#if TARGET_IPHONE_SIMULATOR
 #ifdef HUSE_ARM_NEON
  #undef HUSE_ARM_NEON
 #endif
 #define HUSE_ARM_NEON 0
#endif

//==============================================================================
#include "buffers/AudioDataConverters.h"
#include "buffers/FloatVectorOperations.h"
#include "buffers/AudioSampleBuffer.h"
#include "buffers/AudioChannelSet.h"
#include "buffers/AudioProcessLoadMeasurer.h"
#include "utilities/Decibels.h"
#include "utilities/IIRFilter.h"
#include "utilities/LagrangeInterpolator.h"
#include "utilities/CatmullRomInterpolator.h"
#include "utilities/SmoothedValue.h"
#include "utilities/Reverb.h"
#include "utilities/ADSR.h"
#include "midi/MidiMessage.h"
#include "midi/MidiBuffer.h"
#include "midi/MidiMessageSequence.h"
#include "midi/MidiFile.h"
#include "midi/MidiKeyboardState.h"
#include "midi/MidiRPN.h"
#include "mpe/MPEValue.h"
#include "mpe/MPENote.h"
#include "mpe/MPEZoneLayout.h"
#include "mpe/MPEInstrument.h"
#include "mpe/MPEMessages.h"
#include "mpe/MPESynthesiserBase.h"
#include "mpe/MPESynthesiserVoice.h"
#include "mpe/MPESynthesiser.h"
#include "mpe/MPEUtils.h"
#include "sources/AudioSource.h"
#include "sources/PositionableAudioSource.h"
#include "sources/BufferingAudioSource.h"
#include "sources/ChannelRemappingAudioSource.h"
#include "sources/IIRFilterAudioSource.h"
#include "sources/MemoryAudioSource.h"
#include "sources/MixerAudioSource.h"
#include "sources/ResamplingAudioSource.h"
#include "sources/ReverbAudioSource.h"
#include "sources/ToneGeneratorAudioSource.h"
#include "synthesisers/Synthesiser.h"
#include "audio_play_head/AudioPlayHead.h"
