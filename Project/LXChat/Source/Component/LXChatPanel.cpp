#include "JuceHeader.h"
#include "LXChatPanel.h"

//==============================================================================
LXChatPanel::LXChatPanel()
{
    setWantsKeyboardFocus(true);
}

LXChatPanel::~LXChatPanel()
{
    
}

void LXChatPanel::paint(Graphics& g)
{
    g.fillAll(Colour(0xFFF2F2F2));
}

void LXChatPanel::resized()
{
}
