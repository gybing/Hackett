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

  ID:               audio_processors
  vendor:           H
  version:          5.4.3
  name:             H audio processor classes
  description:      Classes for loading and playing VST, AU, LADSPA, or internally-generated audio processors.
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     gui_extra, audio_basics
  OSXFrameworks:    CoreAudio CoreMIDI AudioToolbox
  iOSFrameworks:    AudioToolbox

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once
#define HAUDIO_PROCESSORS_H_INCLUDED

#include <gui_basics/gui_basics.h>
#include <audio_basics/audio_basics.h>

//==============================================================================
/** Config: HPLUGINHOST_VST
    Enables the VST audio plugin hosting classes. You will need to have the VST2 SDK files in your header search paths. You can obtain the VST2 SDK files from on older version of the VST3 SDK.

    @see VSTPluginFormat, VST3PluginFormat, AudioPluginFormat, AudioPluginFormatManager, HPLUGINHOST_AU, HPLUGINHOST_VST3, HPLUGINHOST_LADSPA
*/
#ifndef HPLUGINHOST_VST
 #define HPLUGINHOST_VST 0
#endif

/** Config: HPLUGINHOST_VST3
    Enables the VST3 audio plugin hosting classes.

    @see VSTPluginFormat, VST3PluginFormat, AudioPluginFormat, AudioPluginFormatManager, HPLUGINHOST_VST, HPLUGINHOST_AU, HPLUGINHOST_LADSPA
*/
#ifndef HPLUGINHOST_VST3
 #define HPLUGINHOST_VST3 0
#endif

/** Config: HPLUGINHOST_AU
    Enables the AudioUnit plugin hosting classes. This is Mac-only, of course.

    @see AudioUnitPluginFormat, AudioPluginFormat, AudioPluginFormatManager, HPLUGINHOST_VST, HPLUGINHOST_VST3, HPLUGINHOST_LADSPA
*/
#ifndef HPLUGINHOST_AU
 #define HPLUGINHOST_AU 0
#endif

/** Config: HPLUGINHOST_LADSPA
    Enables the LADSPA plugin hosting classes. This is Linux-only, of course.

    @see LADSPAPluginFormat, AudioPluginFormat, AudioPluginFormatManager, HPLUGINHOST_VST, HPLUGINHOST_VST3, HPLUGINHOST_AU
 */
#ifndef HPLUGINHOST_LADSPA
 #define HPLUGINHOST_LADSPA 0
#endif

#if ! (HPLUGINHOST_AU || HPLUGINHOST_VST || HPLUGINHOST_VST3 || HPLUGINHOST_LADSPA)
// #error "You need to set either the HPLUGINHOST_AU and/or HPLUGINHOST_VST and/or HPLUGINHOST_VST3 and/or HPLUGINHOST_LADSPA flags if you're using this module!"
#endif

#if ! (defined (HSUPPORT_CARBON) || H64BIT || HIOS)
 #define HSUPPORT_CARBON 1
#endif

#ifndef HSUPPORT_LEGACY_AUDIOPROCESSOR
 #define HSUPPORT_LEGACY_AUDIOPROCESSOR 1
#endif

//==============================================================================
#include "processors/AudioProcessorEditor.h"
#include "processors/AudioProcessorListener.h"
#include "processors/AudioProcessorParameter.h"
#include "processors/AudioProcessorParameterGroup.h"
#include "processors/AudioProcessor.h"
#include "processors/PluginDescription.h"
#include "processors/AudioPluginInstance.h"
#include "processors/AudioProcessorGraph.h"
#include "processors/GenericAudioProcessorEditor.h"
#include "format/AudioPluginFormat.h"
#include "format/AudioPluginFormatManager.h"
#include "scanning/KnownPluginList.h"
#include "format_types/AudioUnitPluginFormat.h"
#include "format_types/LADSPAPluginFormat.h"
#include "format_types/VSTMidiEventList.h"
#include "format_types/VSTPluginFormat.h"
#include "format_types/VST3PluginFormat.h"
#include "scanning/PluginDirectoryScanner.h"
#include "scanning/PluginListComponent.h"
#include "utilities/AudioProcessorParameterWithID.h"
#include "utilities/RangedAudioParameter.h"
#include "utilities/AudioParameterFloat.h"
#include "utilities/AudioParameterInt.h"
#include "utilities/AudioParameterBool.h"
#include "utilities/AudioParameterChoice.h"
#include "utilities/AudioProcessorValueTreeState.h"
