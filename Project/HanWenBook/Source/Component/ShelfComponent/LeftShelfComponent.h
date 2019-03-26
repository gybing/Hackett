#pragma once

#define TagListItemDefaultHeight 50
#define TagListItemNewID -5

class RightContentNotify;
class TagListViewItem;

class LeftShelfComponent
    : public juce::Component
    , public HWRequestor
    , public ExpandListViewModel
    , public IHWSubjectTag
{
public:
    LeftShelfComponent(RightContentNotify* notify);
    ~LeftShelfComponent();

protected:
    void InitData();

    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para);

    void paint(juce::Graphics& g) override;
    void resized() override;

    virtual void listWasScrolled() override {}
    virtual void OnBookTagListChange() override;

private:
    friend class TagListViewItem;
    RightContentNotify* rightContentNotify;
    juce::ScopedPointer<ExpandListView> tagListView;
    juce::Image headImagePortrait;
    juce::Image headImageMask;
    juce::Image userImageVIP;
    juce::String userTitle;
    juce::String userLevel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LeftShelfComponent)
};

class TagListViewItem
    : public ExpandListViewItem
{
public:
    TagListViewItem(LeftShelfComponent& o, int id);
    ~TagListViewItem();

    void setItemExpand(bool collapse = false);
    bool isItemExpanded() const;
    bool isItemMouseHoverd() const;
    bool isItemMouseSelected() const;
    void selectedItem();

public:
    class TagListItem
        : public juce::Component
        , public juce::Button::Listener
        , public juce::TextEditor::Listener
        , public HWRequestor
    {
    public:
        TagListItem(TagListViewItem& o);
        ~TagListItem();

        const HWTUserTag* getUserTag() const;

        void showExpandMenu();
        bool isExpandMenuShow() const;
        void showExpandConform();
        bool isExpandConformShow() const;
        void closeExpand();
        void showEditor();
        void hideEditor();

    protected:
        virtual void paint(juce::Graphics& g) override;
        virtual void buttonClicked (juce::Button*) override;
        virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
            HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override;

        virtual void textEditorTextChanged (juce::TextEditor&) override;
        virtual void textEditorReturnKeyPressed (juce::TextEditor&) override;
        virtual void textEditorEscapeKeyPressed (juce::TextEditor&) override;
        virtual void textEditorFocusLost (juce::TextEditor&) override;

        juce::TextEditor* createEditorComponent();

    private:
        friend class TagListViewItem;
        int row;
        TagListViewItem& owner;
        juce::ScopedPointer<juce::Label> labelName;
        juce::ScopedPointer<juce::TextEditor> editor;
        static juce::ScopedPointer<juce::ToolButton> buttonRename;
        static juce::ScopedPointer<juce::ToolButton> buttonDelete;
        static juce::ScopedPointer<juce::ToolButton> buttonRecomend;
        static juce::ScopedPointer<juce::ToolButton> buttonCfmOK;
        static juce::ScopedPointer<juce::ToolButton> buttonCfmCancel;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TagListItem)
    };

protected:
    // ListViewItem
    virtual juce::Component* createItemComponent(int row) override;
    virtual int getItemHeight(int row) override;
    virtual void paintItem (int row, juce::Graphics& g) override;
    virtual void itemClicked (const juce::MouseEvent& e) override;
    virtual void itemDoubleClicked (const juce::MouseEvent& e) override;

private:
    LeftShelfComponent& owner;
    TagListItem* tagListItem;
    HWTUserTag userTabInfo;

    int rowNumber;
    static TagListViewItem* expandItemRowNumber, *selectedItemRowNumber;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TagListViewItem)
};
