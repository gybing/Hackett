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

AudioProcessorParameterWithID::AudioProcessorParameterWithID (const String& idToUse,
                                                              const String& nameToUse,
                                                              const String& labelToUse,
                                                              AudioProcessorParameter::Category categoryToUse)
    : paramID (idToUse), name (nameToUse), label (labelToUse), category (categoryToUse) {}
AudioProcessorParameterWithID::~AudioProcessorParameterWithID() {}

String AudioProcessorParameterWithID::getName (int maximumStringLength) const        { return name.substr (0, maximumStringLength); }
String AudioProcessorParameterWithID::getLabel() const                               { return label; }
AudioProcessorParameter::Category AudioProcessorParameterWithID::getCategory() const { return category; }


