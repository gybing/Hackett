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

#ifndef DOXYGEN

#if HMAC

//==============================================================================
// Helper class to workaround windows not getting mouse-moves...
class FakeMouseMoveGenerator  : private Timer
{
public:
    FakeMouseMoveGenerator()
    {
        startTimer (1000 / 30);
    }

    static bool componentContainsAudioProcessorEditor (Component* comp) noexcept
    {
        if (dynamic_cast<AudioProcessorEditor*> (comp) != nullptr)
            return true;

        for (auto* child : comp->getChildren())
            if (componentContainsAudioProcessorEditor (child))
                return true;

        return false;
    }

    void timerCallback() override
    {
        // Workaround for windows not getting mouse-moves...
        auto screenPos = Desktop::getInstance().getMainMouseSource().getScreenPosition();

        if (screenPos != lastScreenPos)
        {
            lastScreenPos = screenPos;
            auto mods = ModifierKeys::currentModifiers;

            if (! mods.isAnyMouseButtonDown())
            {
                if (auto* comp = Desktop::getInstance().findComponentAt (screenPos.roundToInt()))
                {
                    if (componentContainsAudioProcessorEditor (comp->getTopLevelComponent()))
                    {
                        safeOldComponent = comp;

                        if (auto* peer = comp->getPeer())
                        {
                            if (! peer->isFocused())
                            {
                                peer->handleMouseEvent (MouseInputSource::InputSourceType::mouse, peer->globalToLocal (screenPos), mods,
                                                        MouseInputSource::invalidPressure, MouseInputSource::invalidOrientation, Time::currentTimeMillis());
                            }
                        }

                        return;
                    }
                }

                if (safeOldComponent != nullptr)
                {
                    if (auto* peer = safeOldComponent->getPeer())
                    {
                        peer->handleMouseEvent (MouseInputSource::InputSourceType::mouse, MouseInputSource::offscreenMousePos, mods,
                                                MouseInputSource::invalidPressure, MouseInputSource::invalidOrientation, Time::currentTimeMillis());
                    }
                }

                safeOldComponent = nullptr;
            }
        }
    }

private:
    Point<float> lastScreenPos;
    WeakReference<Component> safeOldComponent;
};

#else
struct FakeMouseMoveGenerator {};
#endif

#endif

