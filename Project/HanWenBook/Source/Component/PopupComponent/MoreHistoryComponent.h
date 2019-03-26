#pragma once

class MoreHistoryComponent
    : public juce::Component
    , public HWRequestor
    , public juce::ListBoxModel
    , public juce::ListBoxMouseHoverSelector::Listener
    , public juce::Button::Listener
    , public IHWSubjectRead

{
public:
    MoreHistoryComponent();
    ~MoreHistoryComponent();

    static void showMoreHistoryWindow(juce::Component* parent);

protected:
    virtual void paint(juce::Graphics& g) override;
    virtual void resized() override;
    virtual void buttonClicked (juce::Button*);
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override;
    virtual int getNumRows() override;
    virtual void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    virtual void listBoxItemClicked (int row, const juce::MouseEvent&) override;
    virtual void listBoxRowHover() override; 
    virtual juce::Component* refreshComponentForRow (int rowNumber, bool isRowSelected, juce::Component* existingComponentToUpdate) override;
    virtual void OnBookOpened() override;
    virtual void OnReadHistoryDel() override;

protected:
    void initDefaultComponent();
    inline bool findBookfromDelList(const juce::String& book);
    inline void deleteBookFromDelList(const juce::String& book);
    void historyItemClick(const juce::String& book);

private:
    class MoreHistoryItem;
    friend class MoreHistoryItem;

    juce::ScopedPointer<juce::Label> title;
    juce::ScopedPointer<juce::ListBox> contentListBox;
    juce::ScopedPointer<juce::ColorButton> buttonDel;
    juce::ScopedPointer<juce::ListBoxMouseHoverSelector> hoverSelector;
    std::vector<juce::String> booksToDelete;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MoreHistoryComponent)
};