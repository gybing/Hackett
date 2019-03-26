#pragma once

class ReadInfoComponent : public juce::TabbedComponent
{
public:
    ReadInfoComponent();
    ~ReadInfoComponent();

protected:
    void paint (juce::Graphics&) override;
    virtual void visibilityChanged();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadInfoComponent)
};