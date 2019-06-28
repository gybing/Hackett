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

 name:             WaveShaperTanhDemo
 version:          1.0.0
 vendor:           H
 website:          http://H.com
 description:      Wave shaper tanh demo using the DSP module.

 dependencies:     audio_basics, audio_devices, audio_formats,
                   audio_processors, audio_utils, core,
                   data_structures, dsp, events, graphics,
                   gui_basics, gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 moduleFlags:      HSTRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        WaveShaperTanhDemo

 useLocalCopy:     1

 END_HPIP_METADATA

*******************************************************************************/

#pragma once

#include "../Assets/DemoUtilities.h"
#include "../Assets/DSPDemos_Common.h"

using namespace dsp;

//==============================================================================
struct WaveShaperTanhDemoDSP
{
    void prepare (const ProcessSpec&) {}

    void process (const ProcessContextReplacing<float>& context)
    {
        shapers[currentShaperIdx].process (context);
    }

    void reset()
    {
        for (auto&& shaper : shapers)
            shaper.reset();
    }

    void updateParameters()
    {
        currentShaperIdx = jmin (numElementsInArray (shapers), accuracy.getCurrentSelectedID() - 1);
    }

    //==============================================================================
    WaveShaper<float> shapers[2] { { std::tanh }, { FastMathApproximations::tanh } };
    int currentShaperIdx = 0;

    ChoiceParameter accuracy {{ "No Approximation", "Use fast-math approximation" }, 1, "Accuracy" };
    std::vector<DSPDemoParameterBase*> parameters { &accuracy };  // no params for this demo
};

struct WaveShaperTanhDemo    : public Component
{
    WaveShaperTanhDemo()
    {
        addAndMakeVisible (fileReaderComponent);
        setSize (750, 500);
    }

    void resized() override
    {
        fileReaderComponent.setBounds (getLocalBounds());
    }

    AudioFileReaderComponent<WaveShaperTanhDemoDSP> fileReaderComponent;
};
