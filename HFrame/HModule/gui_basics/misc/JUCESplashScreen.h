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

/*
  ==============================================================================

   In accordance with the terms of the H 5 End-Use License Agreement, the
   H Code in SECTION A cannot be removed, changed or otherwise rendered
   ineffective unless you have a H Indie or Pro license, or are using H
   under the GPL v3 license.

   End User License Agreement: www.H.com/H-5-licence

  ==============================================================================
*/

// BEGIN SECTION A

/**
    The standard H splash screen component.

    @tags{GUI}
*/
class API HSplashScreen  : public Component,
                                    private Timer,
                                    private DeletedAtShutdown
{
public:
    HSplashScreen (Component& parentToAddTo);
    ~HSplashScreen() override;

    static std::unique_ptr<Drawable> getSplashScreenLogo();

private:
    void paint (Graphics&) override;
    void timerCallback() override;
    void parentSizeChanged() override;
    void parentHierarchyChanged() override;
    bool hitTest (int, int) override;
    void mouseUp (const MouseEvent&) override;

    std::unique_ptr<Drawable> content;
    CriticalSection appUsageReporting;
    ComponentAnimator fader;

    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HSplashScreen)
};

// END SECTION A


