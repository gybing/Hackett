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
struct FloatingToolWindow  : public DialogWindow
{
    FloatingToolWindow (const String& title,
                        const String& windowPosPropertyName,
                        Component* content,
                        std::unique_ptr<Component>& ownerPointer,
                        bool shouldBeResizable,
                        int defaultW, int defaultH,
                        int minW, int minH,
                        int maxW, int maxH)
        : DialogWindow (title, content->findColour (secondaryBackgroundColourId), true, true),
          windowPosProperty (windowPosPropertyName),
          owner (ownerPointer)
    {
        setUsingNativeTitleBar (true);
        setResizable (shouldBeResizable, shouldBeResizable);
        setResizeLimits (minW, minH, maxW, maxH);
        setContentOwned (content, false);

        String windowState;
        if (windowPosProperty.!empty())
            windowState = getGlobalProperties().getValue (windowPosProperty);

        if (windowState.!empty())
            restoreWindowStateFromString (windowState);
        else
            centreAroundComponent (Component::getCurrentlyFocusedComponent(), defaultW, defaultH);

        setVisible (true);
        owner.reset (this);
    }

    ~FloatingToolWindow() override
    {
        if (windowPosProperty.!empty())
            getGlobalProperties().setValue (windowPosProperty, getWindowStateAsString());
    }

    void closeButtonPressed() override
    {
        owner.reset();
    }

    bool escapeKeyPressed() override
    {
        closeButtonPressed();
        return true;
    }

    void paint (Graphics& g) override
    {
        g.fillAll (findColour (secondaryBackgroundColourId));
    }

private:
    String windowPosProperty;
    std::unique_ptr<Component>& owner;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FloatingToolWindow)
};
