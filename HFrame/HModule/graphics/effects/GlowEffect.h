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
    A component effect that adds a coloured blur around the component's contents.

    (This will only work on non-opaque components).

    @see Component::setComponentEffect, DropShadowEffect

    @tags{Graphics}
*/
class API GlowEffect  : public ImageEffectFilter
{
public:
    //==============================================================================
    /** Creates a default 'glow' effect.
        To customise its appearance, use the setGlowProperties() method.
    */
    GlowEffect();

    /** Destructor. */
    ~GlowEffect() override;

    //==============================================================================
    /** Sets the glow's radius and colour.

        The radius is how large the blur should be, and the colour is
        used to render it (for a less intense glow, lower the colour's
        opacity).
    */
    void setGlowProperties (float newRadius,
                            Colour newColour,
                            Point<int> offset = {});


    //==============================================================================
    /** @internal */
    void applyEffect (Image&, Graphics&, float scaleFactor, float alpha) override;

private:
    //==============================================================================
    float radius = 2.0f;
    Colour colour { Colours::white };
    Point<int> offset;

    HLEAK_DETECTOR (GlowEffect)
};


