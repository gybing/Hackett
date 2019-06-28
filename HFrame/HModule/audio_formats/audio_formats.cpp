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

#ifdef HAUDIO_FORMATS_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of H cpp file"
#endif

#define HCORE_INCLUDE_COM_SMART_PTR 1
#define HCORE_INCLUDE_JNI_HELPERS 1
#define HCORE_INCLUDE_NATIVE_HEADERS 1

#include "audio_formats.h"

//==============================================================================
#if HMAC
 #include <AudioToolbox/AudioToolbox.h>

#elif HIOS
 #import <AudioToolbox/AudioToolbox.h>
 #import <AVFoundation/AVFoundation.h>

//==============================================================================
#elif HWINDOWS && HUSE_WINDOWS_MEDIA_FORMAT
 #include <wmsdk.h>
#endif

//==============================================================================
#include "format/AudioFormat.cpp"
#include "format/AudioFormatManager.cpp"
#include "format/AudioFormatReader.cpp"
#include "format/AudioFormatReaderSource.cpp"
#include "format/AudioFormatWriter.cpp"
#include "format/AudioSubsectionReader.cpp"
#include "format/BufferingAudioFormatReader.cpp"
#include "sampler/Sampler.cpp"
#include "codecs/AiffAudioFormat.cpp"
#include "codecs/CoreAudioFormat.cpp"
#include "codecs/FlacAudioFormat.cpp"
#include "codecs/MP3AudioFormat.cpp"
#include "codecs/OggVorbisAudioFormat.cpp"
#include "codecs/WavAudioFormat.cpp"
#include "codecs/LAMEEncoderAudioFormat.cpp"

#if HWINDOWS && HUSE_WINDOWS_MEDIA_FORMAT
 #include "codecs/WindowsMediaAudioFormat.cpp"
#endif
