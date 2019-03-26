#include "stdafx.h"

void LAFBase::drawRoundThumb (Graphics& g, const float x, const float y,
                     const float diameter, const Colour& colour, float outlineThickness)
{
    const juce::Rectangle<float> a (x, y, diameter, diameter);
    const float halfThickness = outlineThickness * 0.5f;

    Path p;
    p.addEllipse (x + halfThickness, y + halfThickness, diameter - outlineThickness, diameter - outlineThickness);

    const DropShadow ds (Colours::black, 1, Point<int> (0, 0));
    ds.drawForPath (g, p);

    g.setColour (colour);
    g.fillPath (p);

    g.setColour (colour.brighter());
    g.strokePath (p, PathStrokeType (outlineThickness));
}

void LAFBase::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                       float sliderPos, float minSliderPos, float maxSliderPos,
                       const Slider::SliderStyle style, Slider& slider)
{
    g.fillAll (slider.findColour (Slider::backgroundColourId));

    if (style == Slider::LinearBar || style == Slider::LinearBarVertical)
    {
        const float fx = (float) x, fy = (float) y, fw = (float) width, fh = (float) height;

        Path p;

        if (style == Slider::LinearBarVertical)
            p.addRectangle (fx, sliderPos, fw, 1.0f + fh - sliderPos);
        else
            p.addRectangle (fx, fy, sliderPos - fx, fh);


        Colour baseColour (slider.findColour (Slider::rotarySliderFillColourId)
            .withMultipliedSaturation (slider.isEnabled() ? 1.0f : 0.5f)
            .withMultipliedAlpha (0.8f));

        g.setColour (baseColour);
        g.fillPath (p);

        const float lineThickness = jmin (15.0f, jmin (width, height) * 0.45f) * 0.1f;
        g.drawRect (slider.getLocalBounds().toFloat(), lineThickness);
    }
    else
    {
        drawLinearSliderBackground (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        drawLinearSliderThumb (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}

void LAFBase::drawLinearSliderBackground (Graphics& g, int x, int y, int width, int height,
                                 float /*sliderPos*/,
                                 float /*minSliderPos*/,
                                 float /*maxSliderPos*/,
                                 const Slider::SliderStyle /*style*/, Slider& slider)
{
    const float sliderRadius = getSliderThumbRadius (slider) - 5.0f;
    Path on, off;

    if (slider.isHorizontal())
    {
        const float iy = x + width * 0.5f - sliderRadius * 0.5f;
        juce::Rectangle<float> r (x - sliderRadius * 0.5f, iy, width + sliderRadius, sliderRadius);
        const float onW = r.getWidth() * ((float) slider.valueToProportionOfLength (slider.getValue()));

        on.addRectangle (r.removeFromLeft (onW));
        off.addRectangle (r);
    }
    else
    {
        const float ix = x + width * 0.5f - sliderRadius * 0.5f;
        juce::Rectangle<float> r (ix, y - sliderRadius * 0.5f, sliderRadius, height + sliderRadius);
        const float onH = r.getHeight() * ((float) slider.valueToProportionOfLength (slider.getValue()));

        on.addRectangle (r.removeFromBottom (onH));
        off.addRectangle (r);
    }

    g.setColour (slider.findColour (Slider::rotarySliderFillColourId));
    g.fillPath (on);

    g.setColour (slider.findColour (Slider::trackColourId));
    g.fillPath (off);
}

void LAFBase::drawCXDocumentWindowTitleBar (CXDocumentWindow& window, Graphics& g,
                                   int w, int h, int titleSpaceX, int titleSpaceW,
                                   const Image* icon, bool drawTitleTextOnLeft)
{
    const bool isActive = window.isActiveWindow();

    g.setGradientFill (ColourGradient (window.getBackgroundColour(),
        0.0f, 0.0f,
        window.getBackgroundColour().contrasting (isActive ? 0.15f : 0.05f),
        0.0f, (float) h, false));
    g.fillAll();

    Font font (h * 0.65f, Font::bold);
    g.setFont (font);

    int textW = font.getStringWidth (window.getName());
    int iconW = 0;
    int iconH = 0;

    if (icon != nullptr)
    {
        iconH = (int) font.getHeight();
        iconW = icon->getWidth() * iconH / icon->getHeight() + 4;
    }

    textW = jmin (titleSpaceW, textW + iconW);
    int textX = drawTitleTextOnLeft ? titleSpaceX
        : jmax (titleSpaceX, (w - textW) / 2);

    if (textX + textW > titleSpaceX + titleSpaceW)
        textX = titleSpaceX + titleSpaceW - textW;

    if (icon != nullptr)
    {
        g.setOpacity (isActive ? 1.0f : 0.6f);
        g.drawImageWithin (*icon, textX, (h - iconH) / 2, iconW, iconH,
            RectanglePlacement::centred, false);
        textX += iconW;
        textW -= iconW;
    }

    if (window.isColourSpecified (CXDocumentWindow::textColourId) || isColourSpecified (CXDocumentWindow::textColourId))
        g.setColour (window.findColour (CXDocumentWindow::textColourId));
    else
        g.setColour (window.getBackgroundColour().contrasting (isActive ? 0.7f : 0.4f));

    g.drawText (window.getName(), textX, 0, textW, h, Justification::centredLeft, true);
}

//==============================================================================
class LAFBase::GlassWindowButton : public Button
{
public:
    GlassWindowButton (const String& name, Colour col,
        const Path& normalShape_,
        const Path& toggledShape_) noexcept
        : Button (name),
        colour (col),
        normalShape (normalShape_),
        toggledShape (toggledShape_)
    {
    }

    //==============================================================================
    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown)
    {
        float alpha = isMouseOverButton ? (isButtonDown ? 1.0f : 0.8f) : 0.55f;

        if (! isEnabled())
            alpha *= 0.5f;

        float x = 0, y = 0, diam;

        if (getWidth() < getHeight())
        {
            diam = (float) getWidth();
            y = (getHeight() - getWidth()) * 0.5f;
        }
        else
        {
            diam = (float) getHeight();
            y = (getWidth() - getHeight()) * 0.5f;
        }

        x += diam * 0.05f;
        y += diam * 0.05f;
        diam *= 0.9f;

        g.setGradientFill (ColourGradient (Colour::greyLevel (0.9f).withAlpha (alpha), 0, y + diam,
            Colour::greyLevel (0.6f).withAlpha (alpha), 0, y, false));
        g.fillEllipse (x, y, diam, diam);

        x += 2.0f;
        y += 2.0f;
        diam -= 4.0f;

        LookAndFeel_V2::drawGlassSphere (g, x, y, diam, colour.withAlpha (alpha), 1.0f);

        Path& p = getToggleState() ? toggledShape : normalShape;

        const AffineTransform t (p.getTransformToScaleToFit (x + diam * 0.3f, y + diam * 0.3f,
            diam * 0.4f, diam * 0.4f, true));

        g.setColour (Colours::black.withAlpha (alpha * 0.6f));
        g.fillPath (p, t);
    }

private:
    Colour colour;
    Path normalShape, toggledShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlassWindowButton)
};

Button* LAFBase::createCXDocumentWindowButton (int buttonType)
{
    Path shape;
    const float crossThickness = 0.25f;

    if (buttonType == CXDocumentWindow::closeButton)
    {
        shape.addLineSegment (Line<float> (0.0f, 0.0f, 1.0f, 1.0f), crossThickness * 1.4f);
        shape.addLineSegment (Line<float> (1.0f, 0.0f, 0.0f, 1.0f), crossThickness * 1.4f);

        return new LAFBase::GlassWindowButton ("close", Colour (0xffdd1100), shape, shape);
    }

    if (buttonType == CXDocumentWindow::minimiseButton)
    {
        shape.addLineSegment (Line<float> (0.0f, 0.5f, 1.0f, 0.5f), crossThickness);

        return new LAFBase::GlassWindowButton ("minimise", Colour (0xffaa8811), shape, shape);
    }

    if (buttonType == CXDocumentWindow::maximiseButton)
    {
        shape.addLineSegment (Line<float> (0.5f, 0.0f, 0.5f, 1.0f), crossThickness);
        shape.addLineSegment (Line<float> (0.0f, 0.5f, 1.0f, 0.5f), crossThickness);

        Path fullscreenShape;
        fullscreenShape.startNewSubPath (45.0f, 100.0f);
        fullscreenShape.lineTo (0.0f, 100.0f);
        fullscreenShape.lineTo (0.0f, 0.0f);
        fullscreenShape.lineTo (100.0f, 0.0f);
        fullscreenShape.lineTo (100.0f, 45.0f);
        fullscreenShape.addRectangle (45.0f, 45.0f, 100.0f, 100.0f);
        PathStrokeType (30.0f).createStrokedPath (fullscreenShape, fullscreenShape);

        return new LAFBase::GlassWindowButton ("maximise", Colour (0xff119911), shape, fullscreenShape);
    }

    jassertfalse;
    return nullptr;
}

void LAFBase::positionCXDocumentWindowButtons (CXDocumentWindow&,
                                      int titleBarX, int titleBarY,
                                      int titleBarW, int titleBarH,
                                      Button* minimiseButton,
                                      Button* maximiseButton,
                                      Button* closeButton,
                                      bool positionTitleBarButtonsOnLeft)
{
    const int buttonW = titleBarH - titleBarH / 8;

    int x = positionTitleBarButtonsOnLeft ? titleBarX + 4
        : titleBarX + titleBarW - buttonW - buttonW / 4;

    if (closeButton != nullptr)
    {
        closeButton->setBounds (x, titleBarY, buttonW, titleBarH);
        x += positionTitleBarButtonsOnLeft ? buttonW : -(buttonW + buttonW / 4);
    }

    if (positionTitleBarButtonsOnLeft)
        std::swap (minimiseButton, maximiseButton);

    if (maximiseButton != nullptr)
    {
        maximiseButton->setBounds (x, titleBarY, buttonW, titleBarH);
        x += positionTitleBarButtonsOnLeft ? buttonW : -buttonW;
    }

    if (minimiseButton != nullptr)
        minimiseButton->setBounds (x, titleBarY, buttonW, titleBarH);
}

void LAFBase::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                           bool isMouseOverButton, bool isButtonDown)
{
    Colour baseColour (backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
        .withMultipliedAlpha (button.isEnabled() ? 0.9f : 0.5f));

    if (isButtonDown || isMouseOverButton)
        baseColour = baseColour.contrasting (isButtonDown ? 0.2f : 0.1f);

    const float width  = button.getWidth() - 1.0f;
    const float height = button.getHeight() - 1.0f;

    if (width > 0 && height > 0)
    {
        g.setGradientFill (ColourGradient (baseColour, 0.0f, 0.0f,
            baseColour.darker (0.1f), 0.0f, height,
            false));

        g.fillRect (button.getLocalBounds());
    }
}

void LAFBase::drawTickBox (Graphics& g, Component& component,
                  float x, float y, float w, float h,
                  bool ticked,
                  bool isEnabled,
                  bool /*isMouseOverButton*/,
                  bool /*isButtonDown*/)
{
    const float boxSize = w * 0.7f;

    bool isDownOrDragging = component.isEnabled() && (component.isMouseOverOrDragging() || component.isMouseButtonDown());
    const Colour colour (component.findColour (TextButton::buttonOnColourId).withMultipliedSaturation ((component.hasKeyboardFocus (false) || isDownOrDragging) ? 1.3f : 0.9f)
        .withMultipliedAlpha (component.isEnabled() ? 1.0f : 0.7f));
    g.setColour (colour);

    juce::Rectangle<float> r (x, y + (h - boxSize) * 0.5f, boxSize, boxSize);
    g.fillRect (r);

    if (ticked)
    {
        const Path tick (LookAndFeel_V3::getTickShape (6.0f));
        g.setColour (isEnabled ? findColour (TextButton::buttonColourId) : Colours::grey);

        const AffineTransform trans (RectanglePlacement (RectanglePlacement::centred)
            .getTransformToFit (tick.getBounds(), r.reduced (r.getHeight() * 0.05f)));
        g.fillPath (tick, trans);
    }
}

void LAFBase::drawLinearSliderThumb (Graphics& g, int x, int y, int width, int height,
                            float sliderPos, float minSliderPos, float maxSliderPos,
                            const Slider::SliderStyle style, Slider& slider)
{
    const float sliderRadius = (float) getSliderThumbRadius (slider);

    bool isDownOrDragging = slider.isEnabled() && (slider.isMouseOverOrDragging() || slider.isMouseButtonDown());
    Colour knobColour (slider.findColour (Slider::rotarySliderFillColourId).withMultipliedSaturation ((slider.hasKeyboardFocus (false) || isDownOrDragging) ? 1.3f : 0.9f)
        .withMultipliedAlpha (slider.isEnabled() ? 1.0f : 0.7f));
    g.setColour (knobColour);

    if (style == Slider::LinearHorizontal || style == Slider::LinearVertical)
    {
        float kx, ky;

        if (style == Slider::LinearVertical)
        {
            kx = x + width * 0.5f;
            ky = sliderPos;
            g.fillRect (juce::Rectangle<float> (kx - sliderRadius, ky - 2.5f, sliderRadius * 2.0f, 5.0f));
        }
        else
        {
            kx = sliderPos;
            ky = y + height * 0.5f;
            g.fillRect (juce::Rectangle<float> (kx - 2.5f, ky - sliderRadius, 5.0f, sliderRadius * 2.0f));
        }
    }
    else
    {
        // Just call the base class for the demo
        LookAndFeel_V2::drawLinearSliderThumb (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}

void LAFBase::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                       float rotaryStartAngle, float rotaryEndAngle, Slider& slider)
{
    const float diameter = jmin (width, height) - 4.0f;
    const float radius = (diameter / 2.0f) * std::cos (float_Pi / 4.0f);
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

    const Colour baseColour (slider.isEnabled() ? slider.findColour (Slider::rotarySliderFillColourId).withAlpha (isMouseOver ? 0.8f : 1.0f)
        : Colour (0x80808080));

    juce::Rectangle<float> r (rx, ry, rw, rw);
    AffineTransform t (AffineTransform::rotation (angle, r.getCentreX(), r.getCentreY()));

    float x1 = r.getTopLeft().getX(), y1 = r.getTopLeft().getY(), x2 = r.getBottomLeft().getX(), y2 = r.getBottomLeft().getY();
    t.transformPoints (x1, y1, x2, y2);

    g.setGradientFill (ColourGradient (baseColour, x1, y1,
        baseColour.darker (0.1f), x2, y2,
        false));

    Path knob;
    knob.addRectangle (r);
    g.fillPath (knob, t);

    Path needle;
    juce::Rectangle<float> r2 (r * 0.1f);
    needle.addRectangle (r2.withPosition (Point<float> (r.getCentreX() - (r2.getWidth() / 2.0f), r.getY())));

    g.setColour (slider.findColour (Slider::rotarySliderOutlineColourId));
    g.fillPath (needle, AffineTransform::rotation (angle, r.getCentreX(), r.getCentreY()));
}

LookAndFeel* LAFBase::GetCustamLookAndFeel()
{
    static ScopedPointer<LAFBase> laf = new LAFBase();
    const Colour baseColour (Colours::red);
    laf->setColour (Slider::thumbColourId, Colour::greyLevel (0.95f));
    laf->setColour (Slider::textBoxOutlineColourId, Colours::transparentWhite);
    laf->setColour (Slider::rotarySliderFillColourId, baseColour);
    laf->setColour (Slider::rotarySliderOutlineColourId, Colours::white);
    laf->setColour (Slider::trackColourId, Colours::black);
    laf->setColour (TextButton::buttonColourId, Colours::white);
    laf->setColour (TextButton::textColourOffId, baseColour);
    laf->setColour (TextButton::buttonOnColourId, laf->findColour (TextButton::textColourOffId));
    laf->setColour (TextButton::textColourOnId, laf->findColour (TextButton::buttonColourId));
    return laf.get();
}
