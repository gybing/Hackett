#ifndef __LXSIDECONTACTLIST_COMPONENT_HH__
#define __LXSIDECONTACTLIST_COMPONENT_HH__

//==============================================================================

class LXSideContactList : public ListBox, public ListBoxModel
{
public:
    //==============================================================================
    LXSideContactList();
    ~LXSideContactList();

protected:
    virtual int getNumRows() override;
    virtual void paintListBoxItem(int rowNumber,
        Graphics& g,
        int width, int height,
        bool rowIsSelected) override;
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LXSideContactList)
};

#endif // __LXSIDECONTACTLIST_COMPONENT_HH__