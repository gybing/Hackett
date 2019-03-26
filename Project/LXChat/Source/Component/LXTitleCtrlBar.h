#ifndef __LXTITLEBAR_COMPONENT_HH__
#define __LXTITLEBAR_COMPONENT_HH__

//==============================================================================

class LXTitleCtrlBar : public Component, public Button::Listener
{
public:
    //==============================================================================
    LXTitleCtrlBar();
    ~LXTitleCtrlBar();

protected:
    virtual void paint(Graphics&) override;
    virtual void resized() override;
    virtual void buttonClicked(Button*) override;

    DocumentWindow* getDocumentWindow();
private:
    DocumentWindow* documentWindow;
    ScopedPointer<Button> buttonClose;
    ScopedPointer<Button> buttonMaxsize;
    ScopedPointer<Button> buttonMinsize;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LXTitleCtrlBar)
};

#endif // __LXTITLEBAR_COMPONENT_HH__