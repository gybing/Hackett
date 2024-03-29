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
    A lowest-common-denominator implementation of LowLevelGraphicsContext that does all
    its rendering in memory.

    User code is not supposed to create instances of this class directly - do all your
    rendering via the Graphics class instead.

    @tags{Graphics}
*/
class API LowLevelGraphicsSoftwareRenderer    : public RenderingHelpers::StackBasedLowLevelGraphicsContext<RenderingHelpers::SoftwareRendererSavedState>
{
public:
    //==============================================================================
    /** Creates a context to render into an image. */
    LowLevelGraphicsSoftwareRenderer (const Image& imageToRenderOnto);

    /** Creates a context to render into a clipped subsection of an image. */
    LowLevelGraphicsSoftwareRenderer (const Image& imageToRenderOnto, Point<int> origin,
                                      const RectangleList<int>& initialClip);

    /** Destructor. */
    ~LowLevelGraphicsSoftwareRenderer() override;

private:
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LowLevelGraphicsSoftwareRenderer)
};


