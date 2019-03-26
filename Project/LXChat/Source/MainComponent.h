#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

//==============================================================================
class LXTitleCtrlBar;

class MainContentComponent
    : public juce::TabbedComponent
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

protected:
    virtual juce::TabBarButton* createTabButton(const String& tabName, int tabIndex) override;
    virtual void resized() override;

private:
    class LXTabBarButton;

    ScopedPointer<LXTitleCtrlBar> titleCtrlBar;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
