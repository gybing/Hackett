/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#ifdef HAUDIO_UTILS_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#define HCORE_INCLUDE_NATIVE_HEADERS 1
#define HCORE_INCLUDE_JNI_HELPERS 1
#define HCORE_INCLUDE_OBJC_HELPERS 1
#define HCORE_INCLUDE_COM_SMART_PTR 1

#include "audio_utils.h"
#include <gui_extra/gui_extra.h>

#if HMAC
  #import <DiscRecording/DiscRecording.h>
  #import <CoreAudioKit/CABTLEMIDIWindowController.h>
#elif HWINDOWS
 #if HUSE_CDBURNER
  /* You'll need the Platform SDK for these headers - if you don't have it and don't
     need to use CD-burning, then you might just want to set the HUSE_CDBURNER flag
     to 0, to avoid these includes.
  */
  #include <imapi.h>
  #include <imapierror.h>
 #endif
#endif

#include "gui/AudioDeviceSelectorComponent.cpp"
#include "gui/AudioThumbnail.cpp"
#include "gui/AudioThumbnailCache.cpp"
#include "gui/AudioVisualiserComponent.cpp"
#include "gui/MidiKeyboardComponent.cpp"
#include "gui/AudioAppComponent.cpp"
#include "players/SoundPlayer.cpp"
#include "players/AudioProcessorPlayer.cpp"
#include "audio_cd/AudioCDReader.cpp"

#if HMAC
 #include "native/mac_BluetoothMidiDevicePairingDialogue.mm"

 #if HUSE_CDREADER
  #include "native/mac_AudioCDReader.mm"
 #endif

 #if HUSE_CDBURNER
  #include "native/mac_AudioCDBurner.mm"
 #endif

#elif HIOS
 #include "native/ios_BluetoothMidiDevicePairingDialogue.mm"

#elif HANDROID
 #include "native/android_BluetoothMidiDevicePairingDialogue.cpp"

#elif HLINUX
 #if HUSE_CDREADER
  #include "native/linux_AudioCDReader.cpp"
 #endif

 #include "native/linux_BluetoothMidiDevicePairingDialogue.cpp"

#elif HWINDOWS
 #include "native/win_BluetoothMidiDevicePairingDialogue.cpp"

 #if HUSE_CDREADER
  #include "native/win32_AudioCDReader.cpp"
 #endif

 #if HUSE_CDBURNER
  #include "native/win32_AudioCDBurner.cpp"
 #endif

#endif
