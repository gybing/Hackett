#pragma once

class ReadInfoSearch 
    : public juce::Component
    , public HWRequestor
    , public juce::TextEditor::Listener
    , public juce::PacketListBox::Listener
    , public juce::Button::Listener
{
public:
    ReadInfoSearch();
    ~ReadInfoSearch();

protected:
    void resized() override;
    virtual void visibilityChanged();
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override;
    virtual void textEditorReturnKeyPressed (juce::TextEditor&) override;
    virtual int getNumRows() override;
    virtual void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    virtual void expandSelectChanged() override;
    virtual void buttonClicked (juce::Button*) override;

    void initData();
    void initComboboxInfo();
    void search();

private:
    class BookSignItem;
    friend class BookSignItem;

    bool hasNeedInitData;
    juce::String strBook;
    juce::ScopedPointer<juce::ComboBox> comboArea;
    juce::ScopedPointer<juce::TextEditor> editorSearch;
    juce::ScopedPointer<juce::ImageButton> buttonSearch;
    juce::ScopedPointer<juce::PacketListBox> contentPanel;

    std::vector<int> vecComboItemAreas;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadInfoSearch)
};