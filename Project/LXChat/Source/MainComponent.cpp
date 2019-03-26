#include "JuceHeader.h"
#include "MainComponent.h"
#include "Component/LXMainContentComponent.h"
#include "Component/LXPanelChatComponent.h"
#include "Component/LXPanelContactComponent.h"
#include "Component/LXTitleCtrlBar.h"

//==============================================================================
const int nSideMenuWidth = 50;
const int nSideViewWidth = 250;
const int nSideContentWidth = nSideViewWidth + nSideMenuWidth;
const int nSearchBarHeight = 50;
const int nTitleBarHeight = 35;
const int nDefaultChatEditorHeight = 200;

//==============================================================================
class MainContentComponent::LXTabBarButton : public juce::TabBarButton
{
public:
    explicit LXTabBarButton(const String& name, TabbedButtonBar& ownerBar)
        : TabBarButton(name, ownerBar)
    {
    }

    ~LXTabBarButton()
    {
    }

    virtual int getBestTabLength(int depth) override
    {
        return 35;
    }

    void setImages(const Image& normalImage_, 
        const Image& overImage_, const Image& downImage_)
    {
        normalImage = normalImage_;
        overImage = overImage_;
        downImage = downImage_;

        repaint();
    }

    Image getNormalImage() const
    {
        return normalImage;
    }

    Image getOverImage() const
    {
        return overImage.isValid() ? overImage
            : normalImage;
    }

    Image getDownImage() const
    {
        return downImage.isValid() ? downImage
            : getOverImage();
    }

protected:
    void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown) override
    {
        if (!isEnabled())
        {
            isMouseOverButton = false;
            isButtonDown = false;
        }

        Image im(getCurrentImage());

        if (im.isValid())
        {
            const int iw = im.getWidth();
            const int ih = im.getHeight();
            int w = getWidth();
            int h = getHeight();
            int x = (w - iw) / 2;
            int y = (h - ih) / 2;

            {
                {
                    int newW, newH;
                    const float imRatio = ih / (float)iw;
                    const float destRatio = h / (float)w;

                    if (imRatio > destRatio)
                    {
                        newW = roundToInt(h / imRatio);
                        newH = h;
                    }
                    else
                    {
                        newW = w;
                        newH = roundToInt(w * imRatio);
                    }

                    x = (w - newW) / 2;
                    y = (h - newH) / 2;
                    w = newW;
                    h = newH;
                }
            }

            imageBounds.setBounds(x, y, w, h);

            const bool useDownImage = isButtonDown || getToggleState();

            drawImageButton(g, &im, x, y, w, h);
        }
    }

    void drawImageButton(Graphics& g, Image* image,
        int imageX, int imageY, int imageW, int imageH)
    {
        float imageOpacity = 1.0f;

        if (!isEnabled())
            imageOpacity *= 0.3f;

        AffineTransform t = RectanglePlacement(RectanglePlacement::stretchToFit)
            .getTransformToFit(image->getBounds().toFloat(),
            Rectangle<int>(imageX, imageY, imageW, imageH).toFloat());

        g.setOpacity(imageOpacity);
        g.drawImageTransformed(*image, t, false);
    }

private:
    Rectangle<int> imageBounds;
    Image normalImage, overImage, downImage;

    Image getCurrentImage() const
    {
        if (isDown() || getToggleState())
            return getDownImage();

        if (isOver())
            return getOverImage();

        return getNormalImage();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LXTabBarButton)
};

//==============================================================================
MainContentComponent::MainContentComponent()
    : TabbedComponent(juce::TabbedButtonBar::TabsAtLeft)
{
    setSize (850, 600);

    setColour(TabbedComponent::ColourIds::backgroundColourId, Colour(0xFF2F2F2F));
    setTabBarDepth(50);
    setOutline(0);
    setIndent(0);

    addTab(L"main", Colours::transparentBlack, new LXMainContentComponent, true);
    addTab(L"chat", Colours::transparentBlack, new LXPanelChatComponent, true);
    addTab(L"contact", Colours::transparentBlack, new LXPanelContactComponent, true);

    setCurrentTabIndex(0);

    // addAndMakeVisible(titleCtrlBar = new LXTitleCtrlBar);
    // resized();
}

MainContentComponent::~MainContentComponent()
{
}

juce::TabBarButton* MainContentComponent::createTabButton(const String& tabName, int tabIndex)
{
    LXTabBarButton* button = new LXTabBarButton(tabName, getTabbedButtonBar());
    Image image = LXLoadCacheImage(L"sprite.png");

    const int pos[] = { 2232, 2048, 2140 };

    button->setImages(
        image.getClippedImage(Rectangle<int>(0, pos[tabIndex], 35, 35)),
        image.getClippedImage(Rectangle<int>(0, pos[tabIndex], 35, 35)),
        image.getClippedImage(Rectangle<int>(0, pos[tabIndex] + 35, 35, 35)));
    return button;
}

void MainContentComponent::resized()
{
    TabbedComponent::resized();
}
