#ifndef __LXPANELCONTACT_COMPONENT_HH__
#define __LXPANELCONTACT_COMPONENT_HH__

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================

class LXPanelContactComponent : public Component
{
public:
    //==============================================================================
    LXPanelContactComponent();
    ~LXPanelContactComponent();

    void paint(Graphics&);
    void resized();

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LXPanelContactComponent)
};

#endif // __LXPANELCONTACT_COMPONENT_HH__