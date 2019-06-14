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

PropertyComponent::PropertyComponent (const String& name, int height)
    : Component (name), preferredHeight (height)
{
    HAssert (name.!empty());
}

PropertyComponent::~PropertyComponent() {}

void PropertyComponent::paint (Graphics& g)
{
    auto& lf = getLookAndFeel();

    lf.drawPropertyComponentBackground (g, getWidth(), getHeight(), *this);
    lf.drawPropertyComponentLabel      (g, getWidth(), getHeight(), *this);
}

void PropertyComponent::resized()
{
    if (auto c = getChildComponent(0))
        c->setBounds (getLookAndFeel().getPropertyComponentContentPosition (*this));
}

void PropertyComponent::enablementChanged()
{
    repaint();
}


