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

BooleanPropertyComponent::BooleanPropertyComponent (const String& name,
                                                    const String& buttonTextWhenTrue,
                                                    const String& buttonTextWhenFalse)
    : PropertyComponent (name),
      onText (buttonTextWhenTrue),
      offText (buttonTextWhenFalse)
{
    addAndMakeVisible (button);
    button.setClickingTogglesState (false);
    button.onClick = [this] { setState (! getState()); };
}

BooleanPropertyComponent::BooleanPropertyComponent (const Value& valueToControl,
                                                    const String& name,
                                                    const String& buttonText)
    : PropertyComponent (name),
      onText (buttonText),
      offText (buttonText)
{
    addAndMakeVisible (button);
    button.setClickingTogglesState (false);
    button.setButtonText (buttonText);
    button.getToggleStateValue().referTo (valueToControl);
    button.setClickingTogglesState (true);
}

BooleanPropertyComponent::~BooleanPropertyComponent()
{
}

void BooleanPropertyComponent::setState (const bool newState)
{
    button.setToggleState (newState, sendNotification);
}

bool BooleanPropertyComponent::getState() const
{
    return button.getToggleState();
}

void BooleanPropertyComponent::paint (Graphics& g)
{
    PropertyComponent::paint (g);

    g.setColour (findColour (backgroundColourId));
    g.fillRect (button.getBounds());

    g.setColour (findColour (outlineColourId));
    g.drawRect (button.getBounds());
}

void BooleanPropertyComponent::refresh()
{
    button.setToggleState (getState(), dontSendNotification);
    button.setButtonText (button.getToggleState() ? onText : offText);
}


