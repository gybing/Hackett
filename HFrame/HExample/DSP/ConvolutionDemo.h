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

 name:             ConvolutionDemo
 version:          1.0.0
 vendor:           H
 website:          http://H.com
 description:      Convolution demo using the DSP module.

 dependencies:     audio_basics, audio_devices, audio_formats,
                   audio_processors, audio_utils, core,
                   data_structures, dsp, events, graphics,
                   gui_basics, gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 moduleFlags:      HSTRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        ConvolutionDemo

 useLocalCopy:     1

 END_HPIP_METADATA

*******************************************************************************/

#pragma once

#include "../Assets/DemoUtilities.h"
#include "../Assets/DSPDemos_Common.h"

using namespace dsp;

//==============================================================================
struct ConvolutionDemoDSP
{
    void prepare (const ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        convolution.prepare (spec);
        updateParameters();
    }

    void process (ProcessContextReplacing<float> context)
    {
        context.isBypassed = bypass;
        convolution.process (context);
    }

    void reset()
    {
        convolution.reset();
    }

    void updateParameters()
    {
        if (auto* cabinetTypeParameter = dynamic_cast<ChoiceParameter*> (parameters[0]))
        {
            if (cabinetTypeParameter->getCurrentSelectedID() == 1)
            {
                bypass = true;
            }
            else
            {
                bypass = false;

                auto maxSize = static_cast<size_t> (roundToInt (sampleRate * (8192.0 / 44100.0)));
                auto selectedType = cabinetTypeParameter->getCurrentSelectedID();
                auto assetName = (selectedType == 2 ? "guitar_amp.wav" : "cassette_recorder.wav");

                std::unique_ptr<InputStream> assetInputStream (createAssetInputStream (assetName));
                if (assetInputStream != nullptr)
                {
                    currentCabinetData.reset();
                    assetInputStream->readIntoMemoryBlock (currentCabinetData);

                    convolution.loadImpulseResponse (currentCabinetData.getData(), currentCabinetData.getSize(),
                                                     false, true, maxSize);
                }
            }
        }
    }

    //==============================================================================
    double sampleRate = 0.0;
    bool bypass = false;

    MemoryBlock currentCabinetData;
    Convolution convolution;

    ChoiceParameter cabinetParam { { "Bypass", "Guitar amplifier 8''", "Cassette recorder" }, 1, "Cabinet Type" };

    std::vector<DSPDemoParameterBase*> parameters { &cabinetParam };
};

struct ConvolutionDemo    : public Component
{
    ConvolutionDemo()
    {
        addAndMakeVisible (fileReaderComponent);
        setSize (750, 500);
    }

    void resized() override
    {
        fileReaderComponent.setBounds (getLocalBounds());
    }

    AudioFileReaderComponent<ConvolutionDemoDSP> fileReaderComponent;
};
