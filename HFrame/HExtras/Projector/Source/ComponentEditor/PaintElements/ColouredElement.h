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

#include "../PaintRoutine.h"
#include "../JucerDocument.h"
#include "StrokeType.h"

//==============================================================================
/**
    Base class for paint elements that have a fill colour and stroke.

*/
class ColouredElement   : public PaintElement
{
public:
    ColouredElement (PaintRoutine* owner,
                     const String& name,
                     const bool showOutline_,
                     const bool showJointAndEnd_);

    ~ColouredElement() override;

    //==============================================================================
    void getEditableProperties (Array<PropertyComponent*>& props, bool multipleSelected) override;
    void getColourSpecificProperties (Array<PropertyComponent*>& props);

    //==============================================================================
    const JucerFillType& getFillType() noexcept;
    void setFillType (const JucerFillType& newType, const bool undoable);

    bool isStrokeEnabled() const noexcept;
    void enableStroke (bool enable, const bool undoable);

    const StrokeType& getStrokeType() noexcept;
    void setStrokeType (const PathStrokeType& newType, const bool undoable);
    void setStrokeFill (const JucerFillType& newType, const bool undoable);

    //==============================================================================
    Rectangle<int> getCurrentBounds (const Rectangle<int>& parentArea) const override;
    void setCurrentBounds (const Rectangle<int>& newBounds, const Rectangle<int>& parentArea, const bool undoable) override;

    void createSiblingComponents() override;

    //==============================================================================
    void addColourAttributes (XmlElement* const e) const;
    bool loadColourAttributes (const XmlElement& xml);

protected:
    JucerFillType fillType;

    bool isStrokePresent;
    const bool showOutline, showJointAndEnd;
    StrokeType strokeType;

    void convertToNewPathElement (const Path& path);
};
