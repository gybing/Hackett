#include "JuceHeader.h"
#include "LXSideMenu.h"

//==============================================================================
LXSideMenu::LXSideMenu()
{
    setUserFace();
    loadSideMenu();
    resized();
}

LXSideMenu::~LXSideMenu()
{
}

void LXSideMenu::paint(Graphics& g)
{
    g.fillAll(Colour(0xFF2F2F2F));
}

void LXSideMenu::resized()
{
    for (int i = 0; i < buttonMenus.size(); ++i)
    {
        ImageButton* btn = buttonMenus.getUnchecked(i);
        btn->setBounds(7, 20 + ((i + 1) * 60), 35, 35);
    }
}

void LXSideMenu::loadSideMenu()
{
    Image image = LXLoadCacheImage(L"sprite.png");
    ImageButton* imageButton;
    
    addAndMakeVisible(imageButton = new ImageButton());
    imageButton->setImages(false, true, true,
        image.getClippedImage(Rectangle<int>(0, 2232, 35, 35)), 1.0f, Colours::transparentBlack,
        image.getClippedImage(Rectangle<int>(0, 2232, 35, 35)), 1.0f, Colours::transparentBlack,
        image.getClippedImage(Rectangle<int>(0, 2232 + 35, 35, 35)), 1.0f, Colours::transparentBlack);
    imageButton->setRadioGroupId(0x1F);
    imageButton->setClickingTogglesState(false);
    imageButton->addListener(this);
    imageButton->setToggleState(true, dontSendNotification);
    buttonMenus.add(imageButton);

    addAndMakeVisible(imageButton = new ImageButton());
    imageButton->setImages(false, true, true,
        image.getClippedImage(Rectangle<int>(0, 2048, 35, 35)), 1.0f, Colours::transparentBlack,
        image.getClippedImage(Rectangle<int>(0, 2048, 35, 35)), 1.0f, Colours::transparentBlack,
        image.getClippedImage(Rectangle<int>(0, 2048 + 35, 35, 35)), 1.0f, Colours::transparentBlack);
    imageButton->setRadioGroupId(0x1F);
    imageButton->setClickingTogglesState(false);
    imageButton->addListener(this);
    buttonMenus.add(imageButton);

    addAndMakeVisible(imageButton = new ImageButton());
    imageButton->setImages(false, true, true,
        image.getClippedImage(Rectangle<int>(0, 2140, 35, 35)), 1.0f, Colours::transparentBlack,
        image.getClippedImage(Rectangle<int>(0, 2140, 35, 35)), 1.0f, Colours::transparentBlack,
        image.getClippedImage(Rectangle<int>(0, 2140 + 35, 35, 35)), 1.0f, Colours::transparentBlack);
    imageButton->setRadioGroupId(0x1F);
    imageButton->setClickingTogglesState(false);
    imageButton->addListener(this);
    buttonMenus.add(imageButton);
}

void LXSideMenu::buttonClicked(Button* button)
{
    if (button == buttonUserFace)
    {
        return;
    }

    for (int i = 0; i < buttonMenus.size(); ++i)
    {
        ImageButton* btn = buttonMenus.getUnchecked(i);
        if (btn == button)
        {
            btn->setToggleState(true, dontSendNotification);

            setCurrentMenu(i);
        }
        else if (btn->getToggleState())
        {
            btn->setToggleState(false, dontSendNotification);
        }
    }

}

void LXSideMenu::setUserFace(Image face)
{
    if (buttonUserFace == nullptr)
    {
        addAndMakeVisible(buttonUserFace = new ImageButton());
        buttonUserFace->setBounds(10, 10, 30, 30);
        buttonUserFace->addListener(this);
    }

    if (face.isNull())
    {
        face = LXLoadCacheImage(L"sprite.png").getClippedImage(Rectangle<int>(0, 1433, 150, 150));
    }

    buttonUserFace->setImages(false, true, true,
        face, 1.0f, Colours::transparentBlack,
        face, 1.0f, Colours::transparentBlack,
        face, 1.0f, Colours::transparentBlack);
}
