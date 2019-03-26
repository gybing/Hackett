#ifndef __LXCHATLIST_COMPONENT_HH__
#define __LXCHATLIST_COMPONENT_HH__

//==============================================================================

class LXSideChatList : public ListBox, public ListBoxModel
{
public:
    //==============================================================================
    LXSideChatList();
    ~LXSideChatList();

protected:
    virtual int getNumRows() override;
    virtual void paintListBoxItem(int rowNumber,
        Graphics& g,
        int width, int height,
        bool rowIsSelected) override;
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LXSideChatList)
};

#endif // __LXCHATLIST_COMPONENT_HH__