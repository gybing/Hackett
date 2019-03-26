#include "JuceHeader.h"
#include "LXPanelContactComponent.h"

//==============================================================================
LXPanelContactComponent::LXPanelContactComponent()
{

}

LXPanelContactComponent::~LXPanelContactComponent()
{
}

void LXPanelContactComponent::paint(Graphics& g)
{
    g.fillAll(ProjectInfo::getRandomBrightColour());
}

void LXPanelContactComponent::resized()
{
}
