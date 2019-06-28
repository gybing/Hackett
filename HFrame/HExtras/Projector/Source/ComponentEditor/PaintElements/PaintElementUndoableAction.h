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

#include "../UI/JucerDocumentEditor.h"
#include "PaintElementGroup.h"

//==============================================================================
template <class ElementType>
class PaintElementUndoableAction    : public UndoableAction
{
public:
    PaintElementUndoableAction (ElementType* const element)
        : routine (*element->getOwner()),
          elementIndex (element->getOwner()->indexOfElement (element))
    {
        HAssert (element != nullptr);

        if (elementIndex < 0)
            findGroupIndices (element->getOwner(), element);

        HAssert (elementIndex >= 0);
    }

    ElementType* getElement() const
    {
        if (containerGroups.size() > 0)
        {
            auto group = dynamic_cast<PaintElementGroup*> (routine.getElement (containerGroups.getFirst()));

            if (group == nullptr)
                return nullptr;

            for (int i = 1; i < containerGroups.size(); ++i)
            {
                group = dynamic_cast<PaintElementGroup*> (group->getElement (containerGroups.getUnchecked(i)));

                if (group == nullptr)
                    return nullptr;
            }

            auto e = dynamic_cast<ElementType*> (group->getElement (elementIndex));
            HAssert (e != nullptr);
            return e;
        }
        else
        {
            auto e = dynamic_cast<ElementType*> (routine.getElement (elementIndex));
            HAssert (e != nullptr);
            return e;
        }
    }

    int getSizeInUnits()    { return 2; }

protected:
    PaintRoutine& routine;
    int elementIndex;
    Array <int> containerGroups;

    void changed() const
    {
        HAssert (routine.getDocument() != 0);
        routine.getDocument()->changed();
    }

    void showCorrectTab() const
    {
        if (JucerDocumentEditor* const docHolder = JucerDocumentEditor::getActiveDocumentHolder())
            docHolder->showGraphics (&routine);

        if (routine.getSelectedElements().getNumSelected() == 0)
            if (ElementType* const e = dynamic_cast<ElementType*> (routine.getElement (elementIndex)))
                routine.getSelectedElements().selectOnly (e);
    }

private:
    void findGroupIndices (PaintRoutine* const pr, PaintElement* const element)
    {
        for (int i = pr->getNumElements(); --i >= 0;)
        {
            if (auto pg = dynamic_cast<PaintElementGroup*> (pr->getElement (i)))
            {
                if (pg->containsElement (element))
                {
                    containerGroups.add (i);
                    findGroupIndices (pg, element);
                }
            }
        }
    }

    void findGroupIndices (PaintElementGroup* const group, PaintElement* const element)
    {
        elementIndex = group->indexOfElement (element);

        if (elementIndex < 0)
        {
            for (int i = group->getNumElements(); --i >= 0;)
            {
                if (auto pg = dynamic_cast<PaintElementGroup*> (group->getElement (i)))
                {
                    if (pg->containsElement (element))
                    {
                        containerGroups.add (i);
                        findGroupIndices (pg, element);
                    }
                }
            }
        }
    }

    PaintElementUndoableAction (const PaintElementUndoableAction&);
    PaintElementUndoableAction& operator= (const PaintElementUndoableAction&);
};
