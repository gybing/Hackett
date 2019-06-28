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

  ID:               audio_plugin_client
  vendor:           H
  version:          5.4.3
  name:             H audio plugin wrapper classes
  description:      Classes for building VST, VST3, AudioUnit, AAX and RTAS plugins.
  website:          http://www.H.com/H
  license:          GPL/Commercial

  dependencies:     gui_basics, audio_basics, audio_processors

 END_HMODULE_DECLARATION

*******************************************************************************/


#pragma once

#include <gui_basics/gui_basics.h>
#include <audio_basics/audio_basics.h>
#include <audio_processors/audio_processors.h>

/** Config: HVST3_CAN_REPLACE_VST2

    Enable this if you want your VST3 plug-in to load and save VST2 compatible
    state. This allows hosts to replace VST2 plug-ins with VST3 plug-ins. If
    you change this option then your VST3 plug-in will be incompatible with
    previous versions.
*/
#ifndef HVST3_CAN_REPLACE_VST2
 #define HVST3_CAN_REPLACE_VST2 1
#endif

/** Config: HFORCE_USE_LEGACY_PARAM_IDS

    Enable this if you want to force H to use a continuous parameter
    index to identify a parameter in a DAW (this was the default in old
    versions of H). This is index is usually used by the DAW to save
    automation data and enabling this may mess up user's DAW projects.
*/
#ifndef HFORCE_USE_LEGACY_PARAM_IDS
 #define HFORCE_USE_LEGACY_PARAM_IDS 0
#endif

/** Config: HFORCE_LEGACY_PARAMETER_AUTOMATION_TYPE

    Enable this if you want to force H to use a legacy scheme for
    identifying plug-in parameters as either continuous or discrete.
    DAW projects with automation data written by an AudioUnit, VST3 or
    AAX plug-in built with H version 5.1.1 or earlier may load
    incorrectly when opened by an AudioUnit, VST3 or AAX plug-in built
    with H version 5.2.0 and later.
*/
#ifndef HFORCE_LEGACY_PARAMETER_AUTOMATION_TYPE
 #define HFORCE_LEGACY_PARAMETER_AUTOMATION_TYPE 0
#endif

/** Config: HUSE_STUDIO_ONE_COMPATIBLE_PARAMETERS

    Enable this if you want H to use parameter ids which are compatible
    with Studio One. Studio One ignores any parameter ids which are negative.
    Enabling this option will make H generate only positive parameter ids.
    Note that if you have already released a plug-in prior to H 4.3.0 then
    enabling this will change your parameter ids making your plug-in
    incompatible to old automation data.
*/
#ifndef HUSE_STUDIO_ONE_COMPATIBLE_PARAMETERS
 #define HUSE_STUDIO_ONE_COMPATIBLE_PARAMETERS 1
#endif

/** Config: HSTANDALONE_FILTER_WINDOW_USE_KIOSK_MODE

    Enable this if you want your standalone plugin window to use kiosk mode.
    By default, kiosk mode is enabled on iOS and Android.
*/

#ifndef HSTANDALONE_FILTER_WINDOW_USE_KIOSK_MODE
 #define HSTANDALONE_FILTER_WINDOW_USE_KIOSK_MODE (HIOS || HANDROID)
#endif

#include "utility/PluginHostType.h"
#include "VST/VSTCallbackHandler.h"
