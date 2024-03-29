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
class CurrentActivitiesComp   : public Component,
                                private ChangeListener,
                                private ListBoxModel,
                                private Timer
{
public:
    CurrentActivitiesComp (ActivityList& activities)
        : Component ("Activities"), activityList (activities)
    {
        addAndMakeVisible (&list);
        list.setColour (ListBox::backgroundColourId, Colours::transparentBlack);
        list.setRowHeight (16);
        list.setModel (this);

        activityList.addChangeListener (this);
    }

    ~CurrentActivitiesComp() override
    {
        activityList.removeChangeListener (this);
    }

    void resized() override         { list.setBounds (getLocalBounds()); }

    int getNumRows() override
    {
        return activityList.getNumActivities();
    }

    void paintListBoxItem (int rowNumber, Graphics& g,
                           int width, int height, bool /*rowIsSelected*/) override
    {
        const StringArray activities (activityList.getActivities());

        if (rowNumber >= 0 && rowNumber < activities.size())
        {
            g.setColour (findColour (defaultTextColourId));

            g.setFont (height * 0.7f);
            g.drawText (activities [rowNumber],
                        4, 0, width - 5, height, Justification::centredLeft, true);
        }
    }

    void paint (Graphics& g) override
    {
        if (getNumRows() == 0)
            TreePanelBase::drawEmptyPanelMessage (*this, g, "(No activities)");
    }

    static int getMaxPanelHeight()      { return 200; }

private:
    ActivityList& activityList;
    ListBox list;
    int panelHeightToSet;

    void timerCallback() override
    {
        stopTimer();

        if (ConcertinaPanel* cp = findParentComponentOfClass<ConcertinaPanel>())
            cp->setPanelSize (this, panelHeightToSet, true);
    }

    void changeListenerCallback (ChangeBroadcaster*) override
    {
        list.updateContent();

        panelHeightToSet = jmax (3, getNumRows()) * list.getRowHeight() + 15;

        if (! isTimerRunning())
            startTimer (100);

        repaint();
    }
};
