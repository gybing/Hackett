#include "DeviceComponent.h"

//==============================================================================
DeviceComponent::DeviceComponent()
{
    addAndMakeVisible(labelConnectCount = new Label(String::empty, L"0台设备安全上网中，无人蹭网"));
    labelConnectCount->setColour(Label::ColourIds::textColourId, Colours::white);
    labelConnectCount->setJustificationType(Justification::centred);
    labelConnectCount->setFont(25);
}

DeviceComponent::~DeviceComponent()
{
}

void DeviceComponent::paint (Graphics& g)
{
    g.setColour(ProjectInfo::colorLightGreen);
    g.fillRect(0.0f, 0.0f, getWidth()*1.0f, 70.0f);
}

void DeviceComponent::resized()
{
    labelConnectCount->setBounds(0, 0, getWidth(), 70);
}

void DeviceComponent::buttonClicked(Button* button)
{
    
}
