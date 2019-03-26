#include "JuceHeader.h"
#include "LXTitleCtrlBar.h"

//==============================================================================
class ControlButton : public Button
{
public:
    ControlButton(const String& name, Colour c, const Path& normal, const Path& toggled)
        : Button(name), colour(c), normalShape(normal), toggledShape(toggled)
    {
    }

    void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown) override
    {
        if (isMouseOverButton)
            g.fillAll(colour);

        if (isButtonDown)
            g.fillAll(colour.contrasting(0.1f));

        float x = 0, y = 0;
        float diam = (getWidth() < getHeight()) ? ((float) getWidth() * 0.3f) : ((float) getHeight() * 0.3f);

        x = (getWidth()*0.5f) - (diam * 0.5f);
        y = (getHeight()*0.5f) - (diam * 0.5f);

        DocumentWindow* documentWindow = findParentComponentOfClass<DocumentWindow>();
        Path& p = documentWindow && documentWindow->isFullScreen() ? toggledShape : normalShape;

        const AffineTransform t(p.getTransformToScaleToFit(x, y,
            diam, diam, true));

        g.setColour((isMouseOverButton || isButtonDown) ? Colours::white : Colour(0xFF818181));
        g.fillPath(p, t);
    }

    static Button* createControlButton(int buttonType)
    {
        Path shape;
        const float crossThickness = 0.2f;

        if (buttonType == 0)
        {
            shape.addLineSegment(Line<float>(0.0f, 0.0f, 1.0f, 1.0f), crossThickness * 1.4f);
            shape.addLineSegment(Line<float>(1.0f, 0.0f, 0.0f, 1.0f), crossThickness * 1.4f);

            return new ControlButton("close", Colour(0xFFF25561), shape, shape);
        }

        if (buttonType == 2)
        {
            shape.addLineSegment(Line<float>(0.0f, 0.5f, 1.0f, 0.5f), crossThickness);

            return new ControlButton("minimise", Colour(0xFF03A601), shape, shape);
        }

        if (buttonType == 1)
        {
            shape.addLineSegment(Line<float>(0.0f, 0.2f, 1.0f, 0.2f), crossThickness);
            shape.addLineSegment(Line<float>(0.05f, 0.2f, 0.05f, 0.95f), crossThickness / 2.0f);
            shape.addLineSegment(Line<float>(0.95f, 0.2f, 0.95f, 0.95f), crossThickness / 2.0f);
            shape.addLineSegment(Line<float>(0.0f, 0.95f, 1.0f, 0.95f), crossThickness / 2.0f);

            Path toggle;
            float offset = 0.2f;
            toggle.addLineSegment(Line<float>(0.0f, 0.2f + offset, 1.0f, 0.2f + offset), crossThickness);
            toggle.addLineSegment(Line<float>(0.05f, 0.2f + offset, 0.05f, 0.95f), crossThickness / 2.0f);
            toggle.addLineSegment(Line<float>(0.95f, 0.2f + offset, 0.95f, 0.95f), crossThickness / 2.0f);
            toggle.addLineSegment(Line<float>(0.0f, 0.95f, 1.0f, 0.95f), crossThickness / 2.0f);

            toggle.addLineSegment(Line<float>(0.0f + offset, 0.2f, 1.0f, 0.2f), crossThickness);
            toggle.addLineSegment(Line<float>(0.05f + offset, 0.2f, 0.05f + offset, 0.95f), crossThickness / 2.0f);
            toggle.addLineSegment(Line<float>(0.95f, 0.2f, 0.95f, 0.95f), crossThickness / 2.0f);
            toggle.addLineSegment(Line<float>(0.0f + offset, 0.95f, 1.0f, 0.95f), crossThickness / 2.0f);

            return new ControlButton("maximise", Colour(0xFF03A601), shape, toggle);
        }

        jassertfalse;
        return nullptr;
    }

private:
    Colour colour;
    Path normalShape, toggledShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlButton)
};

//==============================================================================
LXTitleCtrlBar::LXTitleCtrlBar()
    : documentWindow(nullptr)
{
    addAndMakeVisible(buttonClose = ControlButton::createControlButton(0));
    addAndMakeVisible(buttonMaxsize = ControlButton::createControlButton(1));
    addAndMakeVisible(buttonMinsize = ControlButton::createControlButton(2));

    buttonClose->addListener(this);
    buttonMaxsize->addListener(this);
    buttonMinsize->addListener(this);
}

LXTitleCtrlBar::~LXTitleCtrlBar()
{
}

void LXTitleCtrlBar::paint(Graphics& g)
{
    if (DocumentWindow* window = getDocumentWindow())
    {
        const bool isActive = window->isActiveWindow();

        Colour colorBackground(0xFFF2F2F2);

        const int height = getHeight();
        const int width = getWidth();

        g.setGradientFill(ColourGradient(colorBackground,
            0.0f, 0.0f,
            colorBackground.contrasting(isActive ? 0.01f : 0.01f),
            0.0f, (float)height, false));
        g.fillAll();

        Font font(height * 0.65f, Font::bold);
        g.setFont(font);

        int textW = font.getStringWidth(window->getName());
        int textX = jmax(0, (width - textW) / 2);

        g.setColour(ProjectInfo::colorLightBlack);
        g.drawText(window->getName(), textX, 0, textW, height, Justification::centredLeft, true);
    }
}

void LXTitleCtrlBar::resized()
{
    if (buttonClose)
    {
        buttonClose->setBounds(getWidth() - 35, 0, 35, getHeight());
        buttonMaxsize->setBounds(getWidth() - 65, 0, 30, getHeight());
        buttonMinsize->setBounds(getWidth() - 95, 0, 30, getHeight());
    }
}

void LXTitleCtrlBar::buttonClicked(Button* button)
{
    if (DocumentWindow* window = getDocumentWindow())
    {
        if (button == buttonClose)
            window->closeButtonPressed();

        else if (button == buttonMaxsize)
            window->maximiseButtonPressed();

        else if (button == buttonMinsize)
            window->minimiseButtonPressed();
    }
}

DocumentWindow* LXTitleCtrlBar::getDocumentWindow()
{
    if (documentWindow == nullptr)
    {
        documentWindow = findParentComponentOfClass<DocumentWindow>();
    }

    return documentWindow;
}
