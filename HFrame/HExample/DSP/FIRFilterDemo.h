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

 name:             FIRFilterDemo
 version:          1.0.0
 vendor:           H
 website:          http://H.com
 description:      FIR filter demo using the DSP module.

 dependencies:     audio_basics, audio_devices, audio_formats,
                   audio_processors, audio_utils, core,
                   data_structures, dsp, events, graphics,
                   gui_basics, gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 moduleFlags:      HSTRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        FIRFilterDemo

 useLocalCopy:     1

 END_HPIP_METADATA

*******************************************************************************/

#pragma once

#include "../Assets/DemoUtilities.h"
#include "../Assets/DSPDemos_Common.h"

using namespace dsp;

//==============================================================================
struct FIRFilterDemoDSP
{
    void prepare (const ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        fir.state = FilterDesign<float>::designFIRLowpassWindowMethod (440.0f, sampleRate, 21,
                                                                       WindowingFunction<float>::blackman);
        fir.prepare (spec);
    }

    void process (const ProcessContextReplacing<float>& context)
    {
        fir.process (context);
    }

    void reset()
    {
        fir.reset();
    }

    void updateParameters()
    {
        if (sampleRate != 0.0)
        {
            auto cutoff = static_cast<float> (cutoffParam.getCurrentValue());
            auto windowingMethod = static_cast<WindowingFunction<float>::WindowingMethod> (typeParam.getCurrentSelectedID() - 1);

            *fir.state = *FilterDesign<float>::designFIRLowpassWindowMethod (cutoff, sampleRate, 21, windowingMethod);
        }
    }

    //==============================================================================
    ProcessorDuplicator<FIR::Filter<float>, FIR::Coefficients<float>> fir;

    double sampleRate = 0.0;

    SliderParameter cutoffParam { { 20.0, 20000.0 }, 0.4, 440.0f, "Cutoff", "Hz" };
    ChoiceParameter typeParam { { "Rectangular", "Triangular", "Hann", "Hamming", "Blackman", "Blackman-Harris", "Flat Top", "Kaiser" },
                                5, "Windowing Function" };

    std::vector<DSPDemoParameterBase*> parameters { &cutoffParam, &typeParam };
};

struct FIRFilterDemo    : public Component
{
    FIRFilterDemo()
    {
        addAndMakeVisible (fileReaderComponent);
        setSize (750, 500);
    }

    void resized() override
    {
        fileReaderComponent.setBounds (getLocalBounds());
    }

    AudioFileReaderComponent<FIRFilterDemoDSP> fileReaderComponent;
};
