#include "MainComponent.h"

//==============================================================================
MainContentComponent::MainContentComponent()
{
    isLastInputChangeHasText = false;

    addAndMakeVisible(resultBox = new AltQResultComponent);
    addAndMakeVisible(searchBox = new AltQSearchBox);
    searchBox->addListener(this);

    setMouseClickGrabsKeyboardFocus(false);
    setWantsKeyboardFocus(false);
    setInterceptsMouseClicks(false, true);

    setSize(AltQConstWidth, AltQConstSearchBarHeight);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::resized()
{
    searchBox->setBounds(0, 0, getWidth(), AltQConstSearchBarHeight);
    resultBox->setBounds(0, AltQConstSearchBarHeight, getWidth(), AltQConstHeight - AltQConstSearchBarHeight);
}

void MainContentComponent::searchInputChanged(TextEditor& e)
{
    bool hastext = !e.isEmpty();
    if (isLastInputChangeHasText != hastext)
    {
        isLastInputChangeHasText = hastext;
        Desktop::getInstance().getAnimator().animateComponent(this, Rectangle<int>(0, 0, getWidth(), hastext ? AltQConstHeight : AltQConstSearchBarHeight), 1.0f, 300, false, 0.0f, 1.0f);
    }

    if (hastext)
    {
        AltQModuleCore::getInstance()->Search(e.getText());
    }
}

void MainContentComponent::paint(Graphics& g)
{
    g.setColour(Colour(213, 214, 216));
    g.drawLine(0.0f, (AltQConstSearchBarHeight*1.0f) - 2.0f, getWidth()*1.0f, (AltQConstSearchBarHeight*1.0f));
}
