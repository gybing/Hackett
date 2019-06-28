/*
  ==============================================================================

   This file is part of the H examples.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projector and used to generate a H project.

 BEGIN_HPIP_METADATA

 name:             GainDemo
 version:          1.0.0
 vendor:           H
 website:          http://H.com
 description:      Gain demo using the DSP module.

 dependencies:     audio_basics, audio_devices, audio_formats,
                   audio_processors, audio_utils, core,
                   data_structures, dsp, events, graphics,
                   gui_basics, gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 moduleFlags:      HSTRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        GainDemo

 useLocalCopy:     1

 END_HPIP_METADATA

*******************************************************************************/

#pragma once

#include "../Assets/DemoUtilities.h"
#include "../Assets/DSPDemos_Common.h"

using namespace dsp;

//==============================================================================
struct GainDemoDSP
{
    void prepare (const ProcessSpec&)
    {
        gain.setGainDecibels (-6.0f);
    }

    void process (const ProcessContextReplacing<float>& context)
    {
        gain.process (context);
    }

    void reset()
    {
        gain.reset();
    }

    void updateParameters()
    {
        gain.setGainDecibels (static_cast<float> (gainParam.getCurrentValue()));
    }

    //==============================================================================
    Gain<float> gain;
    SliderParameter gainParam { { -100.0, 20.0 }, 3.0, -6.0, "Gain", "dB" };

    std::vector<DSPDemoParameterBase*> parameters { &gainParam };
};

struct GainDemo    : public Component
{
    GainDemo()
    {
        addAndMakeVisible (fileReaderComponent);
        setSize (750, 500);
    }

    void resized() override
    {
        fileReaderComponent.setBounds (getLocalBounds());
    }

    AudioFileReaderComponent<GainDemoDSP> fileReaderComponent;
};
