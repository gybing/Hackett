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


//==============================================================================
struct Icon
{
    Icon() : path (nullptr) {}
    Icon (const Path& p, Colour c)  : path (&p), colour (c) {}
    Icon (const Path* p, Colour c)  : path (p),  colour (c) {}

    void draw (Graphics& g, const Rectangle<float>& area, bool isCrossedOut) const
    {
        if (path != nullptr)
        {
            g.setColour (colour);

            const RectanglePlacement placement (RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize);
            g.fillPath (*path, placement.getTransformToFit (path->getBounds(), area));

            if (isCrossedOut)
            {
                g.setColour (Colours::red.withAlpha (0.8f));
                g.drawLine ((float) area.getX(), area.getY() + area.getHeight() * 0.2f,
                            (float) area.getRight(), area.getY() + area.getHeight() * 0.8f, 3.0f);
            }
        }
    }

    Icon withContrastingColourTo (Colour background) const
    {
        return Icon (path, background.contrasting (colour, 0.6f));
    }

    Icon withColour (Colour newColour)
    {
        return Icon (path, newColour);
    }

    const Path* path;
    Colour colour;
};

//==============================================================================
class Icons
{
public:
    Icons();

    Path folder, document, imageDoc, config, HLogo, graph, jigsaw, info, warning, bug,
         code, box, mainJuceLogo, user, closedFolder, exporter, fileExplorer, file, buildTab,
         modules, openFolder, play, settings, singleModule, edit, plus, android, codeBlocks,
         linux, xcode, visualStudio, clion;

private:
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Icons)
};

#ifndef BUILDING_HCOMPILEENGINE
 const Icons& getIcons();
#endif
