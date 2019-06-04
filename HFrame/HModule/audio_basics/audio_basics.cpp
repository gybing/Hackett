

#ifdef HAUDIO_BASICS_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#include "audio_basics.h"

#if HMINGW && ! defined (alloca)
 #define alloca __builtin_alloca
#endif

#if HUSE_SSE_INTRINSICS
 #include <emmintrin.h>
#endif

#ifndef HUSE_VDSP_FRAMEWORK
 #define HUSE_VDSP_FRAMEWORK 1
#endif

#if (HMAC || HIOS) && HUSE_VDSP_FRAMEWORK
 #include <Accelerate/Accelerate.h>
#else
 #undef HUSE_VDSP_FRAMEWORK
#endif

#if HUSE_ARM_NEON
 #include <arm_neon.h>
#endif

#include "buffers/AudioDataConverters.cpp"
#include "buffers/FloatVectorOperations.cpp"
#include "buffers/AudioChannelSet.cpp"
#include "buffers/AudioProcessLoadMeasurer.cpp"
#include "utilities/IIRFilter.cpp"
#include "utilities/LagrangeInterpolator.cpp"
#include "utilities/CatmullRomInterpolator.cpp"
#include "utilities/SmoothedValue.cpp"
#include "midi/MidiBuffer.cpp"
#include "midi/MidiFile.cpp"
#include "midi/MidiKeyboardState.cpp"
#include "midi/MidiMessage.cpp"
#include "midi/MidiMessageSequence.cpp"
#include "midi/MidiRPN.cpp"
#include "mpe/MPEValue.cpp"
#include "mpe/MPENote.cpp"
#include "mpe/MPEZoneLayout.cpp"
#include "mpe/MPEInstrument.cpp"
#include "mpe/MPEMessages.cpp"
#include "mpe/MPESynthesiserBase.cpp"
#include "mpe/MPESynthesiserVoice.cpp"
#include "mpe/MPESynthesiser.cpp"
#include "mpe/MPEUtils.cpp"
#include "sources/BufferingAudioSource.cpp"
#include "sources/ChannelRemappingAudioSource.cpp"
#include "sources/IIRFilterAudioSource.cpp"
#include "sources/MemoryAudioSource.cpp"
#include "sources/MixerAudioSource.cpp"
#include "sources/ResamplingAudioSource.cpp"
#include "sources/ReverbAudioSource.cpp"
#include "sources/ToneGeneratorAudioSource.cpp"
#include "synthesisers/Synthesiser.cpp"
