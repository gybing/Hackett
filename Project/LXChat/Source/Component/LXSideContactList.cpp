#include "JuceHeader.h"
#include "LXSideContactList.h"

//==============================================================================
LXSideContactList::LXSideContactList()
{
    setModel(this);
    setColour(ListBox::ColourIds::backgroundColourId, Colour(0xFF343434));
    setRowHeight(50);
}

LXSideContactList::~LXSideContactList()
{
}

int LXSideContactList::getNumRows()
{
    return 500;
}

void LXSideContactList::paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{
    g.setColour(rowIsSelected ? Colour(0xFF515050) : Colour(0xFF3A3A3A));
    g.fillRect(Rectangle<float>(1, 1, width - 2, height - 2));

    Image image = LXLoadCacheImage(L"sprite.png").getClippedImage(Rectangle<int>(0, 1433, 150, 150));
    g.drawImage(image, 10, 10, 30, 30, 0, 0, image.getWidth(), image.getHeight());

    g.setFont(height*0.4);
    g.setColour(Colours::white);
    g.drawFittedText(String(L"”√ªß ") + String(rowNumber), 50, 10, width - 60, 30, Justification::centredLeft, 1, 1.0f);
}
