#pragma once

class PostilItemComponent;

class BookPostilItem : public ListViewItem
{
public:
    BookPostilItem(ListView& l);
    ~BookPostilItem();

    void updatePosisitons();
    
    void setContent(bool my, int id, bool canGotoPage = true);
protected:
    virtual juce::Component* createItemComponent(int rowNumber) override;
    virtual int getItemHeight(int rowNumber) override;

private:
    friend class PostilItemComponent;
    ListView& owner;
    bool ismy, canGotoPage;
    int dynamicID;
    juce::WeakReference<juce::Component> postilComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookPostilItem)
};

class PostilItemComponent
    : public juce::Component
    , public juce::Button::Listener
    , public HWRequestor
{
public:
    PostilItemComponent(BookPostilItem& postil);
    ~PostilItemComponent();

    void initData();

protected:
    virtual void paint(juce::Graphics& g) override;
    virtual void resized() override;
    virtual void mouseDown(const juce::MouseEvent& event) override;
    virtual void visibilityChanged() override;
    virtual void buttonClicked (juce::Button* b) override;
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override;

    void setPostilPanelData( HWTBookPostil* pInfo );
    void setDynamicPanelData(HWTDynamicInfo* pInfo);
    void requestDynamicInfo();
    void requestUserHead(const juce::String& szUserID);
    void reqestReplyList();
    void requestReplyList(int begin);
    void updateFlowerView(int nCnt, int nOP=-1);
    void updateFavour(int nCnt, int nOP=-1);
    void updateMagic(int nCnt);
    void updateReplyCountUI();

private:
    BookPostilItem& postilItem;

    int replyListCount;
    juce::AttributedString textContent;
    juce::ScopedPointer<ListView> replayListView;
    std::map<int,bool> mapReplyListPage;

    juce::ScopedPointer<juce::ImageComponent> userImage;
    juce::ScopedPointer<juce::TextButton> userTitle;
    juce::ScopedPointer<juce::ToolButton> userFlower;
    juce::ScopedPointer<juce::ToolButton> userMagic;
    juce::ScopedPointer<juce::ToolButton> userFavour;
    juce::ScopedPointer<juce::TextButton> readThisPage;

    juce::ScopedPointer<juce::Label> labelDate;
    juce::ScopedPointer<juce::Label> labelFrom;
    juce::ScopedPointer<juce::TextButton> showDetails;
    juce::ScopedPointer<juce::TextButton> replayButton;

    struct PostilItemInfo
    {
        juce::String strUserID;
        juce::String strReadThePageBook;
        int readThePageNo, readThePageZone;
    };
    PostilItemInfo postilItemInfo;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PostilItemComponent)
};