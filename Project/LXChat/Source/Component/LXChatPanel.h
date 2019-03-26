#ifndef __LXCHATPANEL_COMPONENT_HH__
#define __LXCHATPANEL_COMPONENT_HH__

//==============================================================================

class LXChatPanel : public Component
{
public:
    //==============================================================================
    LXChatPanel();
    ~LXChatPanel();

    void paint(Graphics&);
    void resized();

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LXChatPanel)
};

#endif // __LXCHATPANEL_COMPONENT_HH__