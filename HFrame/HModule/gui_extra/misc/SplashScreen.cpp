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

SplashScreen::SplashScreen (const String& title, const Image& image, bool useDropShadow)
    : Component (title),
      backgroundImage (image),
      clickCountToDelete (0)
{
    // You must supply a valid image here!
    HAssert (backgroundImage.isValid());

    setOpaque (! backgroundImage.hasAlphaChannel());

   #if HIOS || HANDROID
    const bool useFullScreen = true;
   #else
    const bool useFullScreen = false;
   #endif

    makeVisible (image.getWidth(), image.getHeight(), useDropShadow, useFullScreen);
}

SplashScreen::SplashScreen (const String& title, int width, int height, bool useDropShadow)
    : Component (title),
      clickCountToDelete (0)
{
    makeVisible (width, height, useDropShadow, false);
}

void SplashScreen::makeVisible (int w, int h, bool useDropShadow, bool fullscreen)
{
    clickCountToDelete = Desktop::getInstance().getMouseButtonClickCounter();
    creationTime = Time::getCurrentTime();

    const Rectangle<int> screenSize = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    const int width  = (fullscreen ? screenSize.getWidth()   : w);
    const int height = (fullscreen ? screenSize.getHeight()  : h);

    setAlwaysOnTop (true);
    setVisible (true);
    centreWithSize (width, height);
    addToDesktop (useDropShadow ? ComponentPeer::windowHasDropShadow : 0);

    if (fullscreen)
        getPeer()->setFullScreen (true);

    toFront (false);
}

SplashScreen::~SplashScreen() {}

void SplashScreen::deleteAfterDelay (RelativeTime timeout, bool removeOnMouseClick)
{
    // Note that this method must be safe to call from non-GUI threads
    if (! removeOnMouseClick)
        clickCountToDelete = std::numeric_limits<int>::max();

    minimumVisibleTime = timeout;

    startTimer (50);
}

void SplashScreen::paint (Graphics& g)
{
    g.setOpacity (1.0f);
    g.drawImage (backgroundImage, getLocalBounds().toFloat(), RectanglePlacement (RectanglePlacement::fillDestination));
}

void SplashScreen::timerCallback()
{
    if (Time::getCurrentTime() > creationTime + minimumVisibleTime
         || Desktop::getInstance().getMouseButtonClickCounter() > clickCountToDelete)
        delete this;
}


