#include "JuceHeader.h"
#include "LXPanelChatComponent.h"

//==============================================================================
LXPanelChatComponent::LXPanelChatComponent()
{

}

LXPanelChatComponent::~LXPanelChatComponent()
{
}

void LXPanelChatComponent::paint(Graphics& g)
{
    g.fillAll(ProjectInfo::getRandomBrightColour());
}

void LXPanelChatComponent::resized()
{
}
