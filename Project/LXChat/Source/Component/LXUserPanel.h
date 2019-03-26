#ifndef __LXUSERPANEL_COMPONENT_HH__
#define __LXUSERPANEL_COMPONENT_HH__

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================

class LXUserPanel : public Component
{
public:
    //==============================================================================
    LXUserPanel();
    ~LXUserPanel();

    void paint(Graphics&);
    void resized();

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LXUserPanel)
};

#endif // __LXUSERPANEL_COMPONENT_HH__