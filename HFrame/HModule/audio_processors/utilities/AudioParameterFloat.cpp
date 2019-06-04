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

AudioParameterFloat::AudioParameterFloat (const String& idToUse, const String& nameToUse,
                                          NormalisableRange<float> r, float def,
                                          const String& labelToUse, Category categoryToUse,
                                          std::function<String(float, int)> stringFromValue,
                                          std::function<float(const String&)> valueFromString)
   : RangedAudioParameter (idToUse, nameToUse, labelToUse, categoryToUse),
     range (r), value (def), defaultValue (def),
     stringFromValueFunction (stringFromValue),
     valueFromStringFunction (valueFromString)
{
    if (stringFromValueFunction == nullptr)
    {
        auto numDecimalPlacesToDisplay = [this]
        {
            int numDecimalPlaces = 7;

            if (range.interval != 0.0f)
            {
                if (approximatelyEqual (std::abs (range.interval - (int) range.interval), 0.0f))
                    return 0;

                auto v = std::abs (roundToInt (range.interval * pow (10, numDecimalPlaces)));

                while ((v % 10) == 0 && numDecimalPlaces > 0)
                {
                    --numDecimalPlaces;
                    v /= 10;
                }
            }

            return numDecimalPlaces;
        }();

        stringFromValueFunction = [numDecimalPlacesToDisplay] (float v, int length)
        {
            String asText (v, numDecimalPlacesToDisplay);
            return length > 0 ? asText.substring (0, length) : asText;
        };
    }

    if (valueFromStringFunction == nullptr)
        valueFromStringFunction = [] (const String& text) { return text.getFloatValue(); };
}

AudioParameterFloat::AudioParameterFloat (String pid, String nm, float minValue, float maxValue, float def)
   : AudioParameterFloat (pid, nm, { minValue, maxValue, 0.01f }, def)
{
}

AudioParameterFloat::~AudioParameterFloat() {}

float AudioParameterFloat::getValue() const                              { return convertTo0to1 (value); }
void AudioParameterFloat::setValue (float newValue)                      { value = convertFrom0to1 (newValue); valueChanged (get()); }
float AudioParameterFloat::getDefaultValue() const                       { return convertTo0to1 (defaultValue); }
int AudioParameterFloat::getNumSteps() const                             { return AudioProcessorParameterWithID::getNumSteps(); }
String AudioParameterFloat::getText (float v, int length) const          { return stringFromValueFunction (convertFrom0to1 (v), length); }
float AudioParameterFloat::getValueForText (const String& text) const    { return convertTo0to1 (valueFromStringFunction (text)); }
void AudioParameterFloat::valueChanged (float)                           {}

AudioParameterFloat& AudioParameterFloat::operator= (float newValue)
{
    if (value != newValue)
        setValueNotifyingHost (convertTo0to1 (newValue));

    return *this;
}

