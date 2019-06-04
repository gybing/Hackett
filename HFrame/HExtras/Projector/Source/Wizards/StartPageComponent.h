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
class StartPageComponent    : public Component
{
public:
    StartPageComponent()
    {
        setSize (900, 650);

        WizardComp* projectWizard = new WizardComp();

        panel.addTab ("Create New Project", new TemplateTileBrowser (projectWizard), true);
        panel.addTab ("New Project Options", projectWizard, true);

        addAndMakeVisible (panel);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (findColour (backgroundColourId));
    }

    void resized() override
    {
        panel.setBounds (getLocalBounds());
    }

private:
    SlidingPanelComponent panel;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StartPageComponent)
};
