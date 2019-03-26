#ifndef DEVICECOMPONENT_H_INCLUDED
#define DEVICECOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
class DeviceComponent   : public Component, public Button::Listener
{
public:
    //==============================================================================
    DeviceComponent();
    ~DeviceComponent();

protected:
    virtual void paint(Graphics& g) override;
    virtual void resized() override;
    virtual void buttonClicked(Button*) override;

private:
    ScopedPointer<Label> labelConnectCount;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceComponent)
};


#endif  // DEVICECOMPONENT_H_INCLUDED
