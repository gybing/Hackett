#pragma once

class ReadInfoCatalog 
    : public juce::Component
    , public HWRequestor
    , public juce::ListBoxModel
    , public juce::ListBoxMouseHoverSelector::Listener
    , public juce::Button::Listener
    , public IHWSubjectRead
{
public:
    ReadInfoCatalog();
    ~ReadInfoCatalog();

protected:
    void resized() override;
    virtual void visibilityChanged();
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override;
    virtual int getNumRows() override;
    virtual void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    virtual void listBoxItemClicked (int row, const juce::MouseEvent&) override;
    virtual void listBoxRowHover() override; 
    virtual void buttonClicked (juce::Button*) override;
    virtual void OnBookOpened() override;

    void initData();
    void showTextTip(bool show = true);
private:
    bool hasNeedInitData, justShowChapter;
    juce::String strBook;
    juce::ScopedPointer<juce::Label> titleLabel;
    juce::ScopedPointer<juce::ColorButton> buttonjustShowChapter;
    juce::ScopedPointer<juce::ListBox> contentListBox;
    juce::ScopedPointer<juce::ListBoxMouseHoverSelector> hoverSelector;
    juce::ScopedPointer<juce::AttributedImageText> textTip;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadInfoCatalog)
};