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
    A drawable object which acts as a container for a set of other Drawables.

    Note that although this is a Component, it takes ownership of its child components
    and will delete them, so that you can use it as a self-contained graphic object.
    The intention is that you should not add your own components to it, only add other
    Drawable objects.

    @see Drawable

    @tags{GUI}
*/
class API DrawableComposite  : public Drawable
{
public:
    //==============================================================================
    /** Creates a composite Drawable. */
    DrawableComposite();

    /** Creates a copy of a DrawableComposite. */
    DrawableComposite (const DrawableComposite&);

    /** Destructor. */
    ~DrawableComposite() override;

    //==============================================================================
    /** Sets the parallelogram that defines the target position of the content rectangle when the drawable is rendered.
        @see setContentArea
    */
    void setBoundingBox (Parallelogram<float> newBoundingBox);

    /** Sets the rectangle that defines the target position of the content rectangle when the drawable is rendered.
        @see setContentArea
    */
    void setBoundingBox (Rectangle<float> newBoundingBox);

    /** Returns the parallelogram that defines the target position of the content rectangle when the drawable is rendered.
        @see setBoundingBox
    */
    Parallelogram<float> getBoundingBox() const noexcept            { return bounds; }

    /** Changes the bounding box transform to match the content area, so that any sub-items will
        be drawn at their untransformed positions.
    */
    void resetBoundingBoxToContentArea();

    /** Returns the main content rectangle.
        @see contentLeftMarkerName, contentRightMarkerName, contentTopMarkerName, contentBottomMarkerName
    */
    Rectangle<float> getContentArea() const noexcept                { return contentArea; }

    /** Changes the main content area.
        @see setBoundingBox, contentLeftMarkerName, contentRightMarkerName, contentTopMarkerName, contentBottomMarkerName
    */
    void setContentArea (Rectangle<float> newArea);

    /** Resets the content area and the bounding transform to fit around the area occupied
        by the child components.
    */
    void resetContentAreaAndBoundingBoxToFitChildren();

    //==============================================================================
    /** @internal */
    std::unique_ptr<Drawable> createCopy() const override;
    /** @internal */
    Rectangle<float> getDrawableBounds() const override;
    /** @internal */
    void childBoundsChanged (Component*) override;
    /** @internal */
    void childrenChanged() override;
    /** @internal */
    void parentHierarchyChanged() override;
    /** @internal */
    Path getOutlineAsPath() const override;

private:
    //==============================================================================
    Parallelogram<float> bounds;
    Rectangle<float> contentArea;
    bool updateBoundsReentrant = false;

    void updateBoundsToFitChildren();

    DrawableComposite& operator= (const DrawableComposite&);
    HLEAK_DETECTOR (DrawableComposite)
};

