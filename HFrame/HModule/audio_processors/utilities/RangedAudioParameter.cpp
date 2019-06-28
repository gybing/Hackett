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

RangedAudioParameter::RangedAudioParameter (const String& parameterID,
                                            const String& parameterName,
                                            const String& parameterLabel,
                                            Category parameterCategory)
    : AudioProcessorParameterWithID (parameterID, parameterName, parameterLabel, parameterCategory)
{
}

int RangedAudioParameter::getNumSteps() const
{
    const auto& range = getNormalisableRange();

    if (range.interval > 0)
        return (static_cast<int> ((range.end - range.start) / range.interval) + 1);

    return AudioProcessor::getDefaultNumParameterSteps();
}

float RangedAudioParameter::convertTo0to1 (float v) const noexcept
{
    const auto& range = getNormalisableRange();
    return range.convertTo0to1 (range.snapToLegalValue (v));
}

float RangedAudioParameter::convertFrom0to1 (float v) const noexcept
{
    const auto& range = getNormalisableRange();
    return range.snapToLegalValue (range.convertFrom0to1 (jlimit (0.0f, 1.0f, v)));
}


