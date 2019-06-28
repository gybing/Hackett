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

AudioProcessorEditor::AudioProcessorEditor (AudioProcessor& p) noexcept  : processor (p)
{
    initialise();
}

AudioProcessorEditor::AudioProcessorEditor (AudioProcessor* p) noexcept  : processor (*p)
{
    // the filter must be valid..
    HAssert (p != nullptr);
    initialise();
}

AudioProcessorEditor::~AudioProcessorEditor()
{
    splashScreen.deleteAndZero();

    // if this fails, then the wrapper hasn't called editorBeingDeleted() on the
    // filter for some reason..
    HAssert (processor.getActiveEditor() != this);
    removeComponentListener (resizeListener.get());
}

void AudioProcessorEditor::setControlHighlight (ParameterControlHighlightInfo) {}
int AudioProcessorEditor::getControlParameterIndex (Component&)                { return -1; }

bool AudioProcessorEditor::supportsHostMIDIControllerPresence (bool)           { return true; }
void AudioProcessorEditor::hostMIDIControllerIsAvailable (bool)                {}

void AudioProcessorEditor::initialise()
{
    /*
      ==========================================================================
       In accordance with the terms of the H 5 End-Use License Agreement, the
       H Code in SECTION A cannot be removed, changed or otherwise rendered
       ineffective unless you have a H Indie or Pro license, or are using
       H under the GPL v3 license.

       End User License Agreement: www.H.com/H-5-licence
      ==========================================================================
    */

    // BEGIN SECTION A

    splashScreen = new HSplashScreen (*this);

    // END SECTION A

    resizable = false;

    attachConstrainer (&defaultConstrainer);
    resizeListener.reset (new AudioProcessorEditorListener (*this));
    addComponentListener (resizeListener.get());
}

//==============================================================================
void AudioProcessorEditor::setResizable (const bool shouldBeResizable, const bool useBottomRightCornerResizer)
{
    if (shouldBeResizable != resizable)
    {
        resizable = shouldBeResizable;

        if (! resizable && constrainer == &defaultConstrainer)
        {
            auto width = getWidth();
            auto height = getHeight();

            if (width > 0 && height > 0)
                defaultConstrainer.setSizeLimits (width, height, width, height);
        }
    }

    bool shouldHaveCornerResizer = (useBottomRightCornerResizer && shouldBeResizable);

    if (shouldHaveCornerResizer != (resizableCorner != nullptr))
    {
        if (shouldHaveCornerResizer)
        {
            resizableCorner.reset (new ResizableCornerComponent (this, constrainer));
            Component::addChildComponent (resizableCorner.get());
            resizableCorner->setAlwaysOnTop (true);
        }
        else
        {
            resizableCorner.reset();
        }
    }
}

void AudioProcessorEditor::setResizeLimits (int newMinimumWidth,
                                            int newMinimumHeight,
                                            int newMaximumWidth,
                                            int newMaximumHeight) noexcept
{
    // if you've set up a custom constrainer then these settings won't have any effect..
    HAssert (constrainer == &defaultConstrainer || constrainer == nullptr);

    const bool shouldEnableResize      = (newMinimumWidth != newMaximumWidth || newMinimumHeight != newMaximumHeight);
    const bool shouldHaveCornerResizer = (shouldEnableResize != resizable    || resizableCorner != nullptr);

    setResizable (shouldEnableResize, shouldHaveCornerResizer);

    if (constrainer == nullptr)
        setConstrainer (&defaultConstrainer);

    defaultConstrainer.setSizeLimits (newMinimumWidth, newMinimumHeight,
                                      newMaximumWidth, newMaximumHeight);

    setBoundsConstrained (getBounds());
}

void AudioProcessorEditor::setConstrainer (ComponentBoundsConstrainer* newConstrainer)
{
    if (constrainer != newConstrainer)
    {
        if (newConstrainer != nullptr)
            resizable = (newConstrainer->getMinimumWidth()  != newConstrainer->getMaximumWidth()
                      || newConstrainer->getMinimumHeight() != newConstrainer->getMaximumHeight());

        attachConstrainer (newConstrainer);
    }
}

void AudioProcessorEditor::attachConstrainer (ComponentBoundsConstrainer* newConstrainer)
{
    if (constrainer != newConstrainer)
    {
        constrainer = newConstrainer;
        updatePeer();
    }
}

void AudioProcessorEditor::setBoundsConstrained (Rectangle<int> newBounds)
{
    if (constrainer != nullptr)
        constrainer->setBoundsForComponent (this, newBounds, false, false, false, false);
    else
        setBounds (newBounds);
}

void AudioProcessorEditor::editorResized (bool wasResized)
{
    if (wasResized)
    {
        bool resizerHidden = false;

        if (auto* peer = getPeer())
            resizerHidden = peer->isFullScreen() || peer->isKioskMode();

        if (resizableCorner != nullptr)
        {
            resizableCorner->setVisible (! resizerHidden);

            const int resizerSize = 18;
            resizableCorner->setBounds (getWidth() - resizerSize,
                                        getHeight() - resizerSize,
                                        resizerSize, resizerSize);
        }

        if (! resizable)
            if (auto w = getWidth())
                if (auto h = getHeight())
                    defaultConstrainer.setSizeLimits (w, h, w, h);
    }
}

void AudioProcessorEditor::updatePeer()
{
    if (isOnDesktop())
        if (auto* peer = getPeer())
            peer->setConstrainer (constrainer);
}

void AudioProcessorEditor::setScaleFactor (float newScale)
{
    setTransform (AffineTransform::scale (newScale));
    editorResized (true);
}

//==============================================================================
#if HMODULE_AVAILABLE_audio_plugin_client && JucePlugin_Build_Unity
 typedef ComponentPeer* (*createUnityPeerFunctionType) (Component&);
 createUnityPeerFunctionType createUnityPeerFn = nullptr;
#endif

ComponentPeer* AudioProcessorEditor::createNewPeer (int styleFlags, void* nativeWindow)
{
   #if HMODULE_AVAILABLE_audio_plugin_client && JucePlugin_Build_Unity
    if (createUnityPeerFn != nullptr)
    {
        ignoreUnused (styleFlags, nativeWindow);
        return createUnityPeerFn (*this);
    }
   #endif

    return Component::createNewPeer (styleFlags, nativeWindow);
}


