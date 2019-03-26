#include "JuceHeader.h"
#include "LXSearchBar.h"

//==============================================================================
LXSearchBar::LXSearchBar()
{
    addAndMakeVisible(searchEditor = new TextEditor());
    searchEditor->setTextToShowWhenEmpty(L"ËÑË÷", Colours::white.contrasting(0.1f));
    searchEditor->setFont(Font(18));
    searchEditor->setIndents(30, 4);
    searchEditor->setColour(TextEditor::ColourIds::backgroundColourId, Colours::transparentWhite);
    searchEditor->setColour(TextEditor::ColourIds::outlineColourId, Colours::transparentWhite);
    searchEditor->setColour(TextEditor::ColourIds::focusedOutlineColourId, Colours::transparentWhite);
    searchEditor->setColour(TextEditor::ColourIds::textColourId, Colours::white);
    searchEditor->setColour(TextEditor::ColourIds::shadowColourId, Colours::transparentWhite);
    searchEditor->addAndMakeVisible(searchIcon = new ImageComponent, 1);
    searchEditor->addListener(this);
    Image image = LXLoadCacheImage(L"sprite.png");
    searchIcon->setImage(image.getClippedImage(Rectangle<int>(0, 0, 30, 30)));
    searchIcon->setBounds(5, 5, 20, 20);
}

LXSearchBar::~LXSearchBar()
{
}

void LXSearchBar::paint(Graphics& g)
{
    g.fillAll(Colour(0xFF3A3A3A));
    g.setColour((searchEditor && searchEditor->hasKeyboardFocus(true)) ? Colour(0xFF515151) : Colour(0xFF464545));
    g.fillRoundedRectangle(Rectangle<float>(10.0f, 10.0f, (getWidth()*1.0f) - 50.0f, 30.0f), 5.0f);
}

void LXSearchBar::resized()
{
    if (searchEditor)
    {
        searchEditor->setBounds(10, 10, getWidth() - 50, 30);
    }
}
