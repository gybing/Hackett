#include "JuceHeader.h"
#include "LXSideChatList.h"

//==============================================================================
LXSideChatList::LXSideChatList()
{
    setModel(this);
    setColour(ListBox::ColourIds::backgroundColourId, Colour(0xFF343434));

    setRowHeight(50);
}

LXSideChatList::~LXSideChatList()
{
}

int LXSideChatList::getNumRows()
{
    return 500;
}

void LXSideChatList::paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{
    g.setColour(rowIsSelected ? Colour(0xFF515050) : Colour(0xFF3A3A3A));
    g.fillRect(Rectangle<float>(1, 1, width - 2, height - 2));

    if (rowNumber > 5)
    {
        Image image = LXLoadCacheImage(L"sprite.png").getClippedImage(Rectangle<int>(0, 1433, 150, 150));
        g.drawImage(image, 10, 10, 30, 30, 0, 0, image.getWidth(), image.getHeight());

        g.setFont(height*0.4);
        g.setColour(Colours::white);
        g.drawFittedText(String(L"用户 ") + String(rowNumber), 50, 10, width - 60, 30, Justification::centredLeft, 1, 1.0f);
    }
    else
    {
        g.setColour(Colour(0xFFDDDDDD));
        g.fillRoundedRectangle(Rectangle<int>(10, 10, 30, 30).toFloat(), 5.0f);
        g.setColour(Colour(0xFF000000));
        g.fillRoundedRectangle(Rectangle<int>(11, 11, 28, 28).toFloat(), 5.0f);
        Image image = LXLoadCacheImage(L"sprite.png").getClippedImage(Rectangle<int>(0, 1433, 150, 150));

        int x;
        int y;
        const int w = (30 - 4) / 3;
        const int h = (30 - 4) / 3;

        String strName;
        for (unsigned int i = 0; i < 9; ++i)
        {
            x = 12 + ((i % 3) * (w+1));
            y = 12 + ((i / 3) * (h+1));
            g.drawImage(image, x, y, w, h, 0, 0, image.getWidth(), image.getHeight());

            strName += String(L"用户 ") + String(rowNumber) + (i < 8 ? L"、" : L"");
        }

        g.setFont(height*0.4);
        g.setColour(Colours::white);
        g.drawFittedText(strName, 50, 10, width - 60, 30, Justification::centredLeft, 1, 1.0f);
    }
    
}