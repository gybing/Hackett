#pragma once
#include "AltQHeader.h"
#include "AltQResultListBox.h"
#include "AltQResultPreview.h"

//==============================================================================
class AltQResultComponent : public Component
{
public:
    //==============================================================================
    AltQResultComponent();
    ~AltQResultComponent();

    virtual void paint(Graphics&) override;
    virtual void resized() override;

private:
    ScopedPointer<AltQResultListBox> resultListBox;
    ScopedPointer<AltQResultPreview> resultPreview;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AltQResultComponent)
};
