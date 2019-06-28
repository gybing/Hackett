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
#include "../PaintRoutine.h"
#include "SnapGridPainter.h"
class JucerDocumentEditor;

//==============================================================================
class PaintRoutineEditor  : public Component,
                            public ChangeListener,
                            public LassoSource <PaintElement*>,
                            public FileDragAndDropTarget
{
public:
    //==============================================================================
    PaintRoutineEditor (PaintRoutine& graphics,
                        JucerDocument& document,
                        JucerDocumentEditor* const docHolder);
    ~PaintRoutineEditor();

    //==============================================================================
    void paint (Graphics& g);
    void paintOverChildren (Graphics& g);
    void resized();
    void changeListenerCallback (ChangeBroadcaster*);

    void mouseDown (const MouseEvent& e);
    void mouseDrag (const MouseEvent& e);
    void mouseUp (const MouseEvent& e);
    void visibilityChanged();

    void findLassoItemsInArea (Array <PaintElement*>& results, const Rectangle<int>& area);

    SelectedItemSet <PaintElement*>& getLassoSelection();

    bool isInterestedInFileDrag (const StringArray& files);
    void filesDropped (const StringArray& filenames, int x, int y);

    Rectangle<int> getComponentArea() const;

    //==============================================================================
    void refreshAllElements();

private:
    PaintRoutine& graphics;
    JucerDocument& document;
    JucerDocumentEditor* const documentHolder;
    LassoComponent <PaintElement*> lassoComp;
    SnapGridPainter grid;
    Image componentOverlay;
    float componentOverlayOpacity;

    Colour currentBackgroundColour;

    void removeAllElementComps();
    void updateComponentOverlay();
    void updateChildBounds();
};
