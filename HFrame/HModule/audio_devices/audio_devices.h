

/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projector to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 H Module Format.txt file.


 BEGIN_HMODULE_DECLARATION

  ID:               audio_devices
  vendor:           H
  version:          5.4.3
  name:             H audio and MIDI I/O device classes
  description:      Classes to play and record from audio and MIDI I/O devices
  website:          http://www.H.com/H
  license:          ISC

  dependencies:     audio_basics, events
  OSXFrameworks:    CoreAudio CoreMIDI AudioToolbox
  iOSFrameworks:    CoreAudio CoreMIDI AudioToolbox AVFoundation
  linuxPackages:    alsa
  mingwLibs:        winmm

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HAUDIO_DEVICES_H_INCLUDED

#include <events/events.h>
#include <audio_basics/audio_basics.h>

#include <graphics/graphics.h>

//==============================================================================
/** Config: HUSE_WINRT_MIDI
    Enables the use of the Windows Runtime API for MIDI, allowing connections
    to Bluetooth Low Energy devices on Windows 10 version 1809 (October 2018
    Update) and later. If you enable this flag then older, unsupported,
    versions of Windows will automatically fall back to using the regualar
    Win32 MIDI API.

    You will need version 10.0.14393.0 of the Windows Standalone SDK to compile
    and you may need to add the path to the WinRT headers. The path to the
    headers will be something similar to
    "C:\Program Files (x86)\Windows Kits\10\Include\10.0.14393.0\winrt".
*/
#ifndef HUSE_WINRT_MIDI
 #define HUSE_WINRT_MIDI 0
#endif

/** Config: HASIO
    Enables ASIO audio devices (MS Windows only).
    Turning this on means that you'll need to have the Steinberg ASIO SDK installed
    on your Windows build machine.

    See the comments in the ASIOAudioIODevice class's header file for more
    info about this.
*/
#ifndef HASIO
 #define HASIO 0
#endif

/** Config: HWASAPI
    Enables WASAPI audio devices (Windows Vista and above). See also the
    HWASAPI_EXCLUSIVE flag.
*/
#ifndef HWASAPI
 #define HWASAPI 1
#endif

/** Config: HWASAPI_EXCLUSIVE
    Enables WASAPI audio devices in exclusive mode (Windows Vista and above).
*/
#ifndef HWASAPI_EXCLUSIVE
 #define HWASAPI_EXCLUSIVE 0
#endif


/** Config: HDIRECTSOUND
    Enables DirectSound audio (MS Windows only).
*/
#ifndef HDIRECTSOUND
 #define HDIRECTSOUND 1
#endif

/** Config: HALSA
    Enables ALSA audio devices (Linux only).
*/
#ifndef HALSA
 #define HALSA 1
#endif

/** Config: HJACK
    Enables JACK audio devices (Linux only).
*/
#ifndef HJACK
 #define HJACK 0
#endif

/** Config: HBELA
    Enables Bela audio devices on Bela boards.
*/
#ifndef HBELA
 #define HBELA 0
#endif

/** Config: HUSE_ANDROID_OBOE
    ***
    DEVELOPER PREVIEW - Oboe is currently in developer preview and
    is in active development. This preview allows for early access
    and evaluation for developers targeting Android platform.
    ***

    Enables Oboe devices (Android only, API 16 or above). Requires
    Oboe repository path to be specified in Android exporter.
*/

#ifndef HUSE_ANDROID_OBOE
 #define HUSE_ANDROID_OBOE 0
#endif

#if HUSE_ANDROID_OBOE && HANDROID_API_VERSION < 16
 #undef HUSE_ANDROID_OBOE
 #define HUSE_ANDROID_OBOE 0
#endif

/** Config: HUSE_ANDROID_OPENSLES
    Enables OpenSLES devices (Android only).
*/
#ifndef HUSE_ANDROID_OPENSLES
 #if ! HUSE_ANDROID_OBOE && HANDROID_API_VERSION >= 9
  #define HUSE_ANDROID_OPENSLES 1
 #else
  #define HUSE_ANDROID_OPENSLES 0
 #endif
#endif

/** Config: HDISABLE_AUDIO_MIXING_WITH_OTHER_APPS
    Turning this on gives your app exclusive access to the system's audio
    on platforms which support it (currently iOS only).
*/
#ifndef HDISABLE_AUDIO_MIXING_WITH_OTHER_APPS
 #define HDISABLE_AUDIO_MIXING_WITH_OTHER_APPS 0
#endif

//==============================================================================
#include "midi_io/MidiDevices.h"
#include "midi_io/MidiMessageCollector.h"
#include "audio_io/AudioIODevice.h"
#include "audio_io/AudioIODeviceType.h"
#include "audio_io/SystemAudioVolume.h"
#include "sources/AudioSourcePlayer.h"
#include "sources/AudioTransportSource.h"
#include "audio_io/AudioDeviceManager.h"

#if HIOS
 #include "native/ios_Audio.h"
#endif
