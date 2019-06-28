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

#include "PointComponent.h"
#include "ColouredElement.h"

//==============================================================================
class GradientPointComponent    : public PointComponent
{
public:
    GradientPointComponent (ColouredElement* const owner_,
                            const bool isStroke_,
                            const bool isStart_)
        : PointComponent (owner_),
          isStroke (isStroke_),
          isStart (isStart_)
    {
    }

    RelativePositionedRectangle getPosition()
    {
        ColouredElement* e = dynamic_cast<ColouredElement*> (owner);

        if (isStroke)
            return isStart ? e->getStrokeType().fill.gradPos1
                           : e->getStrokeType().fill.gradPos2;

        return isStart ? e->getFillType().gradPos1
                       : e->getFillType().gradPos2;
    }

    void setPosition (const RelativePositionedRectangle& newPos)
    {
        ColouredElement* e = dynamic_cast<ColouredElement*> (owner);

        if (isStroke)
        {
            JucerFillType f (e->getStrokeType().fill);

            if (isStart)
                f.gradPos1 = newPos;
            else
                f.gradPos2 = newPos;

            e->setStrokeFill (f, true);
        }
        else
        {
            JucerFillType f (e->getFillType());

            if (isStart)
                f.gradPos1 = newPos;
            else
                f.gradPos2 = newPos;

            e->setFillType (f, true);
        }
    }

    void updatePosition()
    {
        PointComponent::updatePosition();

        ColouredElement* e = dynamic_cast<ColouredElement*> (owner);

        JucerFillType f (isStroke ? e->getStrokeType().fill
                                  : e->getFillType());

        setVisible (f.mode == JucerFillType::linearGradient
                     || f.mode == JucerFillType::radialGradient);
    }

private:
    bool isStroke, isStart;
};
