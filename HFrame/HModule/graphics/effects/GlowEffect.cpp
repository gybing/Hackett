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

GlowEffect::GlowEffect() {}
GlowEffect::~GlowEffect() {}

void GlowEffect::setGlowProperties (float newRadius, Colour newColour, Point<int> pos)
{
    radius = newRadius;
    colour = newColour;
    offset = pos;
}

void GlowEffect::applyEffect (Image& image, Graphics& g, float scaleFactor, float alpha)
{
    Image temp (image.getFormat(), image.getWidth(), image.getHeight(), true);

    ImageConvolutionKernel blurKernel (roundToInt (radius * scaleFactor * 2.0f));

    blurKernel.createGaussianBlur (radius);
    blurKernel.rescaleAllValues (radius);

    blurKernel.applyToImage (temp, image, image.getBounds());

    g.setColour (colour.withMultipliedAlpha (alpha));
    g.drawImageAt (temp, offset.x, offset.y, true);

    g.setOpacity (alpha);
    g.drawImageAt (image, offset.x, offset.y, false);
}


