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

//==============================================================================
/**
    A component that acts as one of the vertical or horizontal bars you see being
    used to resize panels in a window.

    One of these acts with a StretchableLayoutManager to resize the other components.

    @see StretchableLayoutManager

    @tags{GUI}
*/
class API StretchableLayoutResizerBar  : public Component
{
public:
    //==============================================================================
    /** Creates a resizer bar for use on a specified layout.

        @param layoutToUse          the layout that will be affected when this bar
                                    is dragged
        @param itemIndexInLayout    the item index in the layout that corresponds to
                                    this bar component. You'll need to set up the item
                                    properties in a suitable way for a divider bar, e.g.
                                    for an 8-pixel wide bar which, you could call
                                    myLayout->setItemLayout (barIndex, 8, 8, 8)
        @param isBarVertical        true if it's an upright bar that you drag left and
                                    right; false for a horizontal one that you drag up and
                                    down
    */
    StretchableLayoutResizerBar (StretchableLayoutManager* layoutToUse,
                                 int itemIndexInLayout,
                                 bool isBarVertical);

    /** Destructor. */
    ~StretchableLayoutResizerBar() override;

    //==============================================================================
    /** This is called when the bar is dragged.

        This method must update the positions of any components whose position is
        determined by the StretchableLayoutManager, because they might have just
        moved.

        The default implementation calls the resized() method of this component's
        parent component, because that's often where you're likely to apply the
        layout, but it can be overridden for more specific needs.
    */
    virtual void hasBeenMoved();

    //==============================================================================
    /** This abstract base class is implemented by LookAndFeel classes. */
    struct API  LookAndFeelMethods
    {
        virtual ~LookAndFeelMethods() = default;

        virtual void drawStretchableLayoutResizerBar (Graphics&, int w, int h,
                                                      bool isVerticalBar, bool isMouseOver, bool isMouseDragging) = 0;
    };

    //==============================================================================
    /** @internal */
    void paint (Graphics&) override;
    /** @internal */
    void mouseDown (const MouseEvent&) override;
    /** @internal */
    void mouseDrag (const MouseEvent&) override;


private:
    //==============================================================================
    StretchableLayoutManager* layout;
    int itemIndex, mouseDownPos;
    bool isVertical;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StretchableLayoutResizerBar)
};


