#ifndef __LXUPANELMAIN_COMPONENT_HH__
#define __LXUPANELMAIN_COMPONENT_HH__

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================

class LXPanelChatComponent : public Component
{
public:
    //==============================================================================
    LXPanelChatComponent();
    ~LXPanelChatComponent();

    void paint(Graphics&);
    void resized();

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LXPanelChatComponent)
};

#endif // __LXUPANELMAIN_COMPONENT_HH__