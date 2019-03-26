#pragma once

class ReadInfoPostil 
    : public juce::Component
    , public HWRequestor
    , public IHWSubjectRead
    , public juce::Button::Listener
    , public ListViewModel
{
public:
    ReadInfoPostil();
    ~ReadInfoPostil();

protected:
    virtual void resized() override;
    virtual void visibilityChanged();
    virtual void buttonClicked (juce::Button*) override;
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override;
    virtual void OnBookOpened() override;
    virtual void listWasScrolled() override;

    void initData();
    void requestPostilList(bool my, int begin);

private:
    bool hasNeedInitData, hasMyPostilList;
    int otherPostilCount;
    juce::String strBook;
    std::map<int,bool> mapPageOther;

    juce::ScopedPointer<juce::TabbedComponent> tabbedComponent;
    juce::ScopedPointer<juce::Label> titleLabel;
    juce::ScopedPointer<juce::TextButton> titleButton1;
    juce::ScopedPointer<juce::TextButton> titleButton2;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadInfoPostil)
};