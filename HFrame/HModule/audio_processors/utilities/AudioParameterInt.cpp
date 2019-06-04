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

AudioParameterInt::AudioParameterInt (const String& idToUse, const String& nameToUse,
                                      int minValue, int maxValue, int def,
                                      const String& labelToUse,
                                      std::function<String(int, int)> stringFromInt,
                                      std::function<int(const String&)> intFromString)
   : RangedAudioParameter (idToUse, nameToUse, labelToUse),
     range ([minValue, maxValue]
            {
                NormalisableRange<float> rangeWithInterval { (float) minValue, (float) maxValue,
                                                             [](float start, float end, float v) { return jlimit (start, end, v * (end - start) + start); },
                                                             [](float start, float end, float v) { return jlimit (0.0f, 1.0f, (v - start) / (end - start)); },
                                                             [](float start, float end, float v) { return (float) roundToInt (jlimit (start, end, v)); } };
                 rangeWithInterval.interval = 1.0f;
                 return rangeWithInterval;
            }()),
     value ((float) def),
     defaultValue (convertTo0to1 ((float) def)),
     stringFromIntFunction (stringFromInt),
     intFromStringFunction (intFromString)
{
    HAssert (minValue < maxValue); // must have a non-zero range of values!

    if (stringFromIntFunction == nullptr)
        stringFromIntFunction = [] (int v, int) { return String (v); };

    if (intFromStringFunction == nullptr)
        intFromStringFunction = [] (const String& text) { return text.getIntValue(); };
}

AudioParameterInt::~AudioParameterInt() {}

float AudioParameterInt::getValue() const                                { return convertTo0to1 (value); }
void AudioParameterInt::setValue (float newValue)                        { value = convertFrom0to1 (newValue); valueChanged (get()); }
float AudioParameterInt::getDefaultValue() const                         { return defaultValue; }
int AudioParameterInt::getNumSteps() const                               { return ((int) getNormalisableRange().getRange().getLength()) + 1; }
float AudioParameterInt::getValueForText (const String& text) const      { return convertTo0to1 ((float) intFromStringFunction (text)); }
String AudioParameterInt::getText (float v, int length) const            { return stringFromIntFunction ((int) convertFrom0to1 (v), length); }
void AudioParameterInt::valueChanged (int)                               {}

AudioParameterInt& AudioParameterInt::operator= (int newValue)
{
    if (get() != newValue)
        setValueNotifyingHost (convertTo0to1 ((float) newValue));

    return *this;
}


//==============================================================================
//==============================================================================
#if HUNIT_TESTS

struct AudioParameterIntTests  : public UnitTest
{
    AudioParameterIntTests()
        : UnitTest ("AudioParameterInt", UnitTestCategories::audioProcessorParameters)
    {}

    void runTest() override
    {
        beginTest ("Three options switches at the correct points");
        {
            AudioParameterInt intParam ({}, {}, 1, 3, 1);

            intParam.setValueNotifyingHost (0.0f);
            expectEquals (intParam.get(), 1);

            intParam.setValueNotifyingHost (0.2f);
            expectEquals (intParam.get(), 1);

            intParam.setValueNotifyingHost (0.3f);
            expectEquals (intParam.get(), 2);

            intParam.setValueNotifyingHost (0.7f);
            expectEquals (intParam.get(), 2);

            intParam.setValueNotifyingHost (0.8f);
            expectEquals (intParam.get(), 3);

            intParam.setValueNotifyingHost (1.0f);
            expectEquals (intParam.get(), 3);
        }

        beginTest ("Out-of-bounds input");
        {
            AudioParameterInt intParam ({}, {}, -1, 2, 0);

            intParam.setValueNotifyingHost (-0.5f);
            expectEquals (intParam.get(), -1);

            intParam.setValueNotifyingHost (1.5f);
            expectEquals (intParam.get(), 2);

            intParam = -5;
            expectEquals (intParam.get(), -1);

            intParam = 5;
            expectEquals (intParam.get(), 2);
        }
    }
};

static AudioParameterIntTests audioParameterIntTests;

#endif

