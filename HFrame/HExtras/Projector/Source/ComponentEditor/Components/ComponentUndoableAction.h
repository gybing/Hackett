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

//==============================================================================
template <class ComponentType>
class ComponentUndoableAction    : public UndoableAction
{
public:
    ComponentUndoableAction (ComponentType* const comp,
                             ComponentLayout& layout_)
        : layout (layout_),
          componentIndex (layout_.indexOfComponent (comp))
    {
        HAssert (comp != nullptr);
        HAssert (componentIndex >= 0);
    }

    ComponentType* getComponent() const
    {
        ComponentType* const c = dynamic_cast<ComponentType*> (layout.getComponent (componentIndex));
        HAssert (c != nullptr);
        return c;
    }

    int getSizeInUnits()    { return 2; }

protected:
    ComponentLayout& layout;
    const int componentIndex;

    void changed() const
    {
        HAssert (layout.getDocument() != nullptr);
        layout.getDocument()->changed();
    }

    void showCorrectTab() const
    {
        if (JucerDocumentEditor* const ed = JucerDocumentEditor::getActiveDocumentHolder())
            ed->showLayout();

        if (layout.getSelectedSet().getNumSelected() == 0)
            if (ComponentType* const c = dynamic_cast<ComponentType*> (layout.getComponent (componentIndex)))
                layout.getSelectedSet().selectOnly (getComponent());
    }

private:
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentUndoableAction)
};
