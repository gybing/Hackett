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

  ID:               audio_formats
  vendor:           H
  version:          5.4.3
  name:             H audio file format codecs
  description:      Classes for reading and writing various audio file formats.
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     audio_basics
  OSXFrameworks:    CoreAudio CoreMIDI QuartzCore AudioToolbox
  iOSFrameworks:    AudioToolbox QuartzCore

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HAUDIO_FORMATS_H_INCLUDED

#include <audio_basics/audio_basics.h>

//==============================================================================
/** Config: HUSE_FLAC
    Enables the FLAC audio codec classes (available on all platforms).
    If your app doesn't need to read FLAC files, you might want to disable this to
    reduce the size of your codebase and build time.
*/
#ifndef HUSE_FLAC
 #define HUSE_FLAC 1
#endif

/** Config: HUSE_OGGVORBIS
    Enables the Ogg-Vorbis audio codec classes (available on all platforms).
    If your app doesn't need to read Ogg-Vorbis files, you might want to disable this to
    reduce the size of your codebase and build time.
*/
#ifndef HUSE_OGGVORBIS
 #define HUSE_OGGVORBIS 1
#endif

/** Config: HUSE_MP3AUDIOFORMAT
    Enables the software-based MP3AudioFormat class.
    IMPORTANT DISCLAIMER: By choosing to enable the HUSE_MP3AUDIOFORMAT flag and to compile
    this MP3 code into your software, you do so AT YOUR OWN RISK! By doing so, you are agreeing
    that ROLI Ltd. is in no way responsible for any patent, copyright, or other legal issues
    that you may suffer as a result.

    The code in MP3AudioFormat.cpp is NOT guaranteed to be free from infringements of 3rd-party
    intellectual property. If you wish to use it, please seek your own independent advice about the
    legality of doing so. If you are not willing to accept full responsibility for the consequences
    of using this code, then do not enable this setting.
*/
#ifndef HUSE_MP3AUDIOFORMAT
 #define HUSE_MP3AUDIOFORMAT 0
#endif

/** Config: HUSE_LAME_AUDIO_FORMAT
    Enables the LameEncoderAudioFormat class.
*/
#ifndef HUSE_LAME_AUDIO_FORMAT
 #define HUSE_LAME_AUDIO_FORMAT 0
#endif

/** Config: HUSE_WINDOWS_MEDIA_FORMAT
    Enables the Windows Media SDK codecs.
*/
#ifndef HUSE_WINDOWS_MEDIA_FORMAT
 #define HUSE_WINDOWS_MEDIA_FORMAT 1
#endif

#if ! HMSVC
 #undef HUSE_WINDOWS_MEDIA_FORMAT
 #define HUSE_WINDOWS_MEDIA_FORMAT 0
#endif

//==============================================================================
#include "format/AudioFormatReader.h"
#include "format/AudioFormatWriter.h"
#include "format/MemoryMappedAudioFormatReader.h"
#include "format/AudioFormat.h"
#include "format/AudioFormatManager.h"
#include "format/AudioFormatReaderSource.h"
#include "format/AudioSubsectionReader.h"
#include "format/BufferingAudioFormatReader.h"
#include "codecs/AiffAudioFormat.h"
#include "codecs/CoreAudioFormat.h"
#include "codecs/FlacAudioFormat.h"
#include "codecs/LAMEEncoderAudioFormat.h"
#include "codecs/MP3AudioFormat.h"
#include "codecs/OggVorbisAudioFormat.h"
#include "codecs/WavAudioFormat.h"
#include "codecs/WindowsMediaAudioFormat.h"
#include "sampler/Sampler.h"
