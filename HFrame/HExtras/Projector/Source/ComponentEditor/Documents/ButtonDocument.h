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
class ButtonDocument   : public JucerDocument
{
public:
    ButtonDocument (SourceCodeDocument* cpp);
    ~ButtonDocument();

    //==============================================================================
    String getTypeName() const;

    JucerDocument* createCopy();
    Component* createTestComponent (const bool alwaysFillBackground);

    int getNumPaintRoutines() const;
    StringArray getPaintRoutineNames() const;
    PaintRoutine* getPaintRoutine (const int index) const;

    void setStatePaintRoutineEnabled (const int index, bool b);
    bool isStatePaintRoutineEnabled (const int index) const;

    int chooseBestEnabledPaintRoutine (int paintRoutineWanted) const;

    ComponentLayout* getComponentLayout() const                 { return nullptr; }

    void addExtraClassProperties (PropertyPanel&);

    //==============================================================================
    std::unique_ptr<XmlElement> createXml() const;
    bool loadFromXml (const XmlElement&);

    void fillInGeneratedCode (GeneratedCode& code) const;
    void fillInPaintCode (GeneratedCode& code) const;

    void getOptionalMethods (StringArray& baseClasses,
                             StringArray& returnValues,
                             StringArray& methods,
                             StringArray& initialContents) const;

    //==============================================================================
    std::unique_ptr<PaintRoutine> paintRoutines[7];
    bool paintStatesEnabled [7];
};
