#pragma once

class OptionComponent
    : public juce::Component
    , public juce::ButtonListener
{
public:
    OptionComponent(int style);
    ~OptionComponent();

    static void showOptionWindow(juce::Component* parent, int style);

protected:
    virtual void paint(juce::Graphics& g) override;
    virtual void resized() override;
    virtual void buttonClicked (juce::Button*);

protected:
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para);

    void initDefaultComponent();
    void saveOptions();
    void loadOptions();

private:
    juce::ScopedPointer<juce::TabbedComponent> tabbedComponent;
    juce::ScopedPointer<juce::TextButton> buttonSystemOption;
    juce::ScopedPointer<juce::TextButton> buttonReadOption;
    juce::ScopedPointer<juce::ColorButton> buttonOK;
    juce::ScopedPointer<juce::ColorButton> buttonCancel;
    juce::ScopedPointer<juce::ColorButton> buttonAplay;
    int defaultStyle;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionComponent)
};