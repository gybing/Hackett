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

/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projector to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 H Module Format.txt file.


 BEGIN_HMODULE_DECLARATION

  ID:               audio_utils
  vendor:           H
  version:          5.4.3
  name:             H extra audio utility classes
  description:      Classes for audio-related GUI and miscellaneous tasks.
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     gui_extra, audio_processors, audio_formats, audio_devices
  OSXFrameworks:    CoreAudioKit DiscRecording
  iOSFrameworks:    CoreAudioKit

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HAUDIO_UTILS_H_INCLUDED

#include <gui_basics/gui_basics.h>
#include <audio_devices/audio_devices.h>
#include <audio_formats/audio_formats.h>
#include <audio_processors/audio_processors.h>

//==============================================================================
/** Config: HUSE_CDREADER
    Enables the AudioCDReader class (on supported platforms).
*/
#ifndef HUSE_CDREADER
#define HUSE_CDREADER 0
#endif

/** Config: HUSE_CDBURNER
    Enables the AudioCDBurner class (on supported platforms).
*/
#ifndef HUSE_CDBURNER
#define HUSE_CDBURNER 0
#endif

//==============================================================================
#include "gui/AudioDeviceSelectorComponent.h"
#include "gui/AudioThumbnailBase.h"
#include "gui/AudioThumbnail.h"
#include "gui/AudioThumbnailCache.h"
#include "gui/AudioVisualiserComponent.h"
#include "gui/MidiKeyboardComponent.h"
#include "gui/AudioAppComponent.h"
#include "gui/BluetoothMidiDevicePairingDialogue.h"
#include "players/SoundPlayer.h"
#include "players/AudioProcessorPlayer.h"
#include "audio_cd/AudioCDBurner.h"
#include "audio_cd/AudioCDReader.h"
