#ifndef __LXSIDEMENU_COMPONENT_HH__
#define __LXSIDEMENU_COMPONENT_HH__

//==============================================================================

class LXSideMenu
    : public Component
    , public Button::Listener
    , public LXMainMenuBroadcaster
{
public:
    //==============================================================================
    LXSideMenu();
    ~LXSideMenu();

protected:
    virtual void paint(Graphics&) override;
    virtual void resized() override;
    virtual void buttonClicked(Button*) override;

protected:
    void loadSideMenu();
    void setUserFace(Image face = Image::null);

private:
    ScopedPointer<ImageButton> buttonUserFace;
    OwnedArray<ImageButton> buttonMenus;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LXSideMenu)
};

#endif // __LXSIDEMENU_COMPONENT_HH__