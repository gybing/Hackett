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
class BuildStatusTabComp   : public Component,
                             private ChangeListener,
                             private Timer
{
public:
    BuildStatusTabComp (ErrorList& el, ActivityList& al)
        : errorList (el), activityList (al)
    {
        setInterceptsMouseClicks (false, false);
        addAndMakeVisible (&spinner);
        activityList.addChangeListener (this);
        errorList.addChangeListener (this);
    }

    ~BuildStatusTabComp() override
    {
        activityList.removeChangeListener (this);
        errorList.removeChangeListener (this);
    }

    enum { size = 20 };

    void updateStatus()
    {
        State newState = nothing;

        if (activityList.getNumActivities() > 0)    newState = busy;
        else if (errorList.getNumErrors() > 0)      newState = errors;
        else if (errorList.getNumWarnings() > 0)    newState = warnings;

        if (newState != state)
        {
            state = newState;
            setSize (state != nothing ? size : 0, size);
            spinner.setVisible (state == busy);
            repaint();
        }
    }

    void paint (Graphics& g) override
    {
        if (state == errors || state == warnings)
        {
            g.setColour (state == errors ? Colours::red : Colours::yellow);
            const Path& icon = (state == errors) ? getIcons().warning
                                                 : getIcons().info;

            g.fillPath (icon, RectanglePlacement (RectanglePlacement::centred)
                                .getTransformToFit (icon.getBounds(),
                                                    getCentralArea().reduced (1, 1).toFloat()));
        }
    }

    void resized() override
    {
        spinner.setBounds (getCentralArea());
    }

    Rectangle<int> getCentralArea() const
    {
        return getLocalBounds().withTrimmedRight (4);
    }

private:
    ErrorList& errorList;
    ActivityList& activityList;

    void changeListenerCallback (ChangeBroadcaster*) override   { if (! isTimerRunning()) startTimer (150); }
    void timerCallback() override                               { stopTimer(); updateStatus(); }

    enum State
    {
        nothing,
        busy,
        errors,
        warnings
    };

    State state;

    //==============================================================================
    struct Spinner  : public Component,
                      private Timer
    {
        Spinner()
        {
            setInterceptsMouseClicks (false, false);
        }

        void paint (Graphics& g) override
        {
            if (findParentComponentOfClass<TabBarButton>() != nullptr)
            {
                getLookAndFeel().drawSpinningWaitAnimation (g, findColour (treeIconColourId),
                                                            0, 0, getWidth(), getHeight());
                startTimer (1000 / 20);
            }
        }

        void timerCallback() override
        {
            if (isVisible())
                repaint();
            else
                stopTimer();
        }
    };

    Spinner spinner;
};
