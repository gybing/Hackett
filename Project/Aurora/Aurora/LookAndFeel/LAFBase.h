#pragma once
#include "Component/DocumentWindow.h"

using namespace juce;
class LAFBase : public LookAndFeel_V3,
    public CXDocumentWindow::LookAndFeelMethods
{
public:
    void drawRoundThumb (Graphics& g, const float x, const float y,
        const float diameter, const Colour& colour, float outlineThickness);

    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const Slider::SliderStyle style, Slider& slider) override;
    void drawLinearSliderBackground (Graphics& g, int x, int y, int width, int height,
        float /*sliderPos*/,
        float /*minSliderPos*/,
        float /*maxSliderPos*/,
        const Slider::SliderStyle /*style*/, Slider& slider) override;

    void drawCXDocumentWindowTitleBar (CXDocumentWindow& window, Graphics& g,
        int w, int h, int titleSpaceX, int titleSpaceW,
        const Image* icon, bool drawTitleTextOnLeft);
    class GlassWindowButton;
    Button* createCXDocumentWindowButton (int buttonType);
    void positionCXDocumentWindowButtons (CXDocumentWindow&,
        int titleBarX, int titleBarY,
        int titleBarW, int titleBarH,
        Button* minimiseButton,
        Button* maximiseButton,
        Button* closeButton,
        bool positionTitleBarButtonsOnLeft);

    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override;
    void drawTickBox (Graphics& g, Component& component,
        float x, float y, float w, float h,
        bool ticked,
        bool isEnabled,
        bool /*isMouseOverButton*/,
        bool /*isButtonDown*/) override;
    void drawLinearSliderThumb (Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const Slider::SliderStyle style, Slider& slider) override;
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, Slider& slider) override;

public:
     static LookAndFeel* GetCustamLookAndFeel();
};


