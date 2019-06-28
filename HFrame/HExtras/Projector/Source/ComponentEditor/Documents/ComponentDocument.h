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

#pragma once

#include "../JucerDocument.h"

//==============================================================================
class ComponentDocument   : public JucerDocument
{
public:
    ComponentDocument (SourceCodeDocument* cpp);
    ~ComponentDocument();

    //==============================================================================
    String getTypeName() const;

    JucerDocument* createCopy();
    Component* createTestComponent (const bool alwaysFillBackground);

    int getNumPaintRoutines() const                             { return 1; }
    StringArray getPaintRoutineNames() const                    { return StringArray ("Graphics"); }
    PaintRoutine* getPaintRoutine (const int index) const       { return index == 0 ? backgroundGraphics.get() : nullptr; }

    ComponentLayout* getComponentLayout() const                 { return components.get(); }

    //==============================================================================
    std::unique_ptr<XmlElement> createXml() const;
    bool loadFromXml (const XmlElement& xml);

    void fillInGeneratedCode (GeneratedCode& code) const;
    void applyCustomPaintSnippets (StringArray&);

private:
    std::unique_ptr<ComponentLayout> components;
    std::unique_ptr<PaintRoutine> backgroundGraphics;
};
