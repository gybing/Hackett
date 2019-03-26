#pragma once

class ReadInfoSign 
    : public juce::Component
    , public HWRequestor
    , public juce::ListBoxModel
    , public juce::ListBoxMouseHoverSelector::Listener
    , public IHWSubjectRead
    , public IHWSubjectBookSign
{
public:
    ReadInfoSign();
    ~ReadInfoSign();

protected:
    void resized() override;
    virtual void visibilityChanged();
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override;
    virtual int getNumRows() override;
    virtual void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    virtual void listBoxItemClicked (int row, const juce::MouseEvent&) override;
    virtual void listBoxRowHover() override; 
    virtual juce::Component* refreshComponentForRow (int rowNumber, bool isRowSelected, juce::Component* existingComponentToUpdate) override;
    virtual void OnBookOpened() override;
    virtual void OnBookSignAdd() override;
    virtual void OnBookSignDel() override;
    virtual void OnBookSignListChanged() override;

    void initData();

private:
    class BookSignItem;
    friend class BookSignItem;

    bool hasNeedInitData;
    juce::String strBook;
    juce::ScopedPointer<juce::Label> titleLabel;
    juce::ScopedPointer<juce::ListBox> contentListBox;
    juce::ScopedPointer<juce::ListBoxMouseHoverSelector> hoverSelector;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadInfoSign)
};