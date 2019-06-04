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

SliderPropertyComponent::SliderPropertyComponent (const String& name,
                                                  const double rangeMin,
                                                  const double rangeMax,
                                                  const double interval,
                                                  const double skewFactor,
                                                  bool symmetricSkew)
    : PropertyComponent (name)
{
    addAndMakeVisible (slider);

    slider.setRange (rangeMin, rangeMax, interval);
    slider.setSkewFactor (skewFactor, symmetricSkew);
    slider.setSliderStyle (Slider::LinearBar);

    slider.onValueChange = [this]
    {
        if (getValue() != slider.getValue())
            setValue (slider.getValue());
    };
}

SliderPropertyComponent::SliderPropertyComponent (const Value& valueToControl,
                                                  const String& name,
                                                  const double rangeMin,
                                                  const double rangeMax,
                                                  const double interval,
                                                  const double skewFactor,
                                                  bool symmetricSkew)
    : PropertyComponent (name)
{
    addAndMakeVisible (slider);

    slider.setRange (rangeMin, rangeMax, interval);
    slider.setSkewFactor (skewFactor, symmetricSkew);
    slider.setSliderStyle (Slider::LinearBar);

    slider.getValueObject().referTo (valueToControl);
}

SliderPropertyComponent::~SliderPropertyComponent()
{
}

void SliderPropertyComponent::setValue (const double /*newValue*/)
{
}

double SliderPropertyComponent::getValue() const
{
    return slider.getValue();
}

void SliderPropertyComponent::refresh()
{
    slider.setValue (getValue(), dontSendNotification);
}


