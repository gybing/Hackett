#pragma once
#include "AltQHeader.h"
#include "AltQSearchBox.h"
#include "AltQResultComponent.h"

//==============================================================================
class MainContentComponent
    : public Component
    , public AltQSearchBox::Listener
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

protected:
    virtual void paint(Graphics&) override;
    virtual void resized() override;

    // AQSearchBox
    virtual void searchInputChanged(TextEditor&) override;

private:
    bool isLastInputChangeHasText;
    ScopedPointer<AltQSearchBox> searchBox;
    ScopedPointer<AltQResultComponent> resultBox;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
