

#ifdef HAUDIO_DEVICES_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#define HCORE_INCLUDE_OBJC_HELPERS 1
#define HCORE_INCLUDE_COM_SMART_PTR 1
#define HCORE_INCLUDE_JNI_HELPERS 1
#define HCORE_INCLUDE_NATIVE_HEADERS 1
#define HEVENTS_INCLUDE_WIN32_MESSAGE_WINDOW 1

#ifndef HUSE_WINRT_MIDI
 #define HUSE_WINRT_MIDI 0
#endif

#if HUSE_WINRT_MIDI
 #define HEVENTS_INCLUDE_WINRT_WRAPPER 1
#endif

#include "audio_devices.h"

//==============================================================================
#if HMAC
 #define Point CarbonDummyPointName
 #define Component CarbonDummyCompName
 #import <CoreAudio/AudioHardware.h>
 #import <CoreMIDI/MIDIServices.h>
 #import <AudioToolbox/AudioServices.h>
 #undef Point
 #undef Component

#elif HIOS
 #import <AudioToolbox/AudioToolbox.h>
 #import <AVFoundation/AVFoundation.h>
 #import <CoreMIDI/MIDIServices.h>

 #if TARGET_OS_SIMULATOR
  #import <CoreMIDI/MIDINetworkSession.h>
 #endif

//==============================================================================
#elif HWINDOWS
 #if HWASAPI
  #include <mmreg.h>
 #endif

 #if HUSE_WINRT_MIDI && HMSVC
  /* If you cannot find any of the header files below then you are probably
     attempting to use the Windows 10 Bluetooth Low Energy API. For this to work you
     need to install version 10.0.14393.0 of the Windows Standalone SDK and you may
     need to add the path to the WinRT headers to your build system. This path should
     have the form "C:\Program Files (x86)\Windows Kits\10\Include\10.0.14393.0\winrt".

     Also please note that Microsoft's Bluetooth MIDI stack has multiple issues, so
     this API is EXPERIMENTAL - use at your own risk!
  */
  #include <windows.devices.h>
  #include <windows.devices.midi.h>
  #include <windows.devices.enumeration.h>

  #pragma warning (push)
  #pragma warning (disable: 4265)
  #include <wrl/event.h>
  #pragma warning (pop)

  #pragma warning (push)
  #pragma warning (disable: 4467)
  #include <robuffer.h>
  #pragma warning (pop)
 #endif

 #if HASIO
  /* This is very frustrating - we only need to use a handful of definitions from
     a couple of the header files in Steinberg's ASIO SDK, and it'd be easy to copy
     about 30 lines of code into this cpp file to create a fully stand-alone ASIO
     implementation...

     ..unfortunately that would break Steinberg's license agreement for use of
     their SDK, so I'm not allowed to do this.

     This means that anyone who wants to use H's ASIO abilities will have to:

     1) Agree to Steinberg's licensing terms and download the ASIO SDK
         (see http://www.steinberg.net/en/company/developers.html).

     2) Enable this code with a global definition #define HASIO 1.

     3) Make sure that your header search path contains the iasiodrv.h file that
        comes with the SDK. (Only about a handful of the SDK header files are actually
        needed - so to simplify things, you could just copy these into your H directory).
  */
  #include <iasiodrv.h>
 #endif

//==============================================================================
#elif HLINUX
 #if HALSA
  /* Got an include error here? If so, you've either not got ALSA installed, or you've
     not got your paths set up correctly to find its header files.

     The package you need to install to get ASLA support is "libasound2-dev".

     If you don't have the ALSA library and don't want to build H with audio support,
     just set the HALSA flag to 0.
  */
  #include <alsa/asoundlib.h>
 #endif

 #if HJACK
  /* Got an include error here? If so, you've either not got jack-audio-connection-kit
     installed, or you've not got your paths set up correctly to find its header files.

     The package you need to install to get JACK support is "libjack-dev".

     If you don't have the jack-audio-connection-kit library and don't want to build
     H with low latency audio support, just set the HJACK flag to 0.
  */
  #include <jack/jack.h>
 #endif

 #if HBELA
  /* Got an include error here? If so, you've either not got the bela headers
     installed, or you've not got your paths set up correctly to find its header
     files.
  */
  #include <rtdk.h>
  #include <Bela.h>
  #include <Midi.h>
 #endif

 #undef SIZEOF

//==============================================================================
#elif HANDROID

 #if HUSE_ANDROID_OPENSLES
  #include <SLES/OpenSLES.h>
  #include <SLES/OpenSLES_Android.h>
  #include <SLES/OpenSLES_AndroidConfiguration.h>
 #endif

 #if HUSE_ANDROID_OBOE
  #if HUSE_ANDROID_OPENSLES
   #error "Oboe cannot be enabled at the same time as openSL! Please disable HUSE_ANDROID_OPENSLES"
  #endif

  #include <oboe/Oboe.h>
 #endif

#endif

#include "audio_io/AudioDeviceManager.cpp"
#include "audio_io/AudioIODevice.cpp"
#include "audio_io/AudioIODeviceType.cpp"
#include "midi_io/MidiMessageCollector.cpp"
#include "midi_io/MidiDevices.cpp"
#include "sources/AudioSourcePlayer.cpp"
#include "sources/AudioTransportSource.cpp"
#include "native/MidiDataConcatenator.h"

//==============================================================================
#if HMAC
 #include "native/mac_CoreAudio.cpp"
 #include "native/mac_CoreMidi.cpp"

//==============================================================================
#elif HIOS
 #include "native/ios_Audio.cpp"
 #include "native/mac_CoreMidi.cpp"

//==============================================================================
#elif HWINDOWS

 #if HWASAPI
  #include "native/win32_WASAPI.cpp"
 #endif

 #if HDIRECTSOUND
  #include "native/win32_DirectSound.cpp"
 #endif

 #include "native/win32_Midi.cpp"

 #if HASIO
  #include "native/win32_ASIO.cpp"
 #endif

//==============================================================================
#elif HLINUX
 #if HALSA
  #include "native/linux_ALSA.cpp"
 #endif

 #if HJACK
  #include "native/linux_JackAudio.cpp"
 #endif

 #if HBELA
  #include "native/linux_Bela.cpp"
 #else
  #include "native/linux_Midi.cpp"
 #endif

//==============================================================================
#elif HANDROID
 #include "native/android_Audio.cpp"
 #include "native/android_Midi.cpp"

 #if HUSE_ANDROID_OPENSLES
  #include "native/android_OpenSL.cpp"
 #endif

 #if HUSE_ANDROID_OBOE
  #include "native/android_Oboe.cpp"
 #endif
#endif

#if ! HSYSTEMAUDIOVOL_IMPLEMENTED

    // None of these methods are available. (On Windows you might need to enable WASAPI for this)
    float CALLTYPE SystemAudioVolume::getGain()         { HAssertfalse; return 0.0f; }
    bool  CALLTYPE SystemAudioVolume::setGain (float)   { HAssertfalse; return false; }
    bool  CALLTYPE SystemAudioVolume::isMuted()         { HAssertfalse; return false; }
    bool  CALLTYPE SystemAudioVolume::setMuted (bool)   { HAssertfalse; return false; }
}
#endif
