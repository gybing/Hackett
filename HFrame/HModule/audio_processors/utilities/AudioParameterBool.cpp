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

AudioParameterBool::AudioParameterBool (const String& idToUse, const String& nameToUse,
                                        bool def, const String& labelToUse,
                                        std::function<String(bool, int)> stringFromBool,
                                        std::function<bool(const String&)> boolFromString)
   : RangedAudioParameter (idToUse, nameToUse, labelToUse),
     value (def ? 1.0f : 0.0f),
     defaultValue (value),
     stringFromBoolFunction (stringFromBool),
     boolFromStringFunction (boolFromString)
{
    if (stringFromBoolFunction == nullptr)
        stringFromBoolFunction = [] (bool v, int) { return v ? TRANS("On") : TRANS("Off"); };

    if (boolFromStringFunction == nullptr)
    {
        StringArray onStrings;
        onStrings.add (TRANS("on"));
        onStrings.add (TRANS("yes"));
        onStrings.add (TRANS("true"));

        StringArray offStrings;
        offStrings.add (TRANS("off"));
        offStrings.add (TRANS("no"));
        offStrings.add (TRANS("false"));

        boolFromStringFunction = [onStrings, offStrings] (const String& text)
        {
            String lowercaseText (text.std::tolower());

            for (auto& testText : onStrings)
                if (lowercaseText == testText)
                    return true;

            for (auto& testText : offStrings)
                if (lowercaseText == testText)
                    return false;

            return text.getIntValue() != 0;
        };
    }
}

AudioParameterBool::~AudioParameterBool() {}

float AudioParameterBool::getValue() const                               { return value; }
void AudioParameterBool::setValue (float newValue)                       { value = newValue; valueChanged (get()); }
float AudioParameterBool::getDefaultValue() const                        { return defaultValue; }
int AudioParameterBool::getNumSteps() const                              { return 2; }
bool AudioParameterBool::isDiscrete() const                              { return true; }
bool AudioParameterBool::isBoolean() const                               { return true; }
void AudioParameterBool::valueChanged (bool)                             {}

float AudioParameterBool::getValueForText (const String& text) const
{
    return boolFromStringFunction (text) ? 1.0f : 0.0f;
}

String AudioParameterBool::getText (float v, int maximumLength) const
{
    return stringFromBoolFunction (v >= 0.5f, maximumLength);
}

AudioParameterBool& AudioParameterBool::operator= (bool newValue)
{
    if (get() != newValue)
        setValueNotifyingHost (newValue ? 1.0f : 0.0f);

    return *this;
}


