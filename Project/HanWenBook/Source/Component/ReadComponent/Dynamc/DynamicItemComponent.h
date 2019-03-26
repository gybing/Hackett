#pragma once

class DynamicItemComponent
    : public juce::Component
    , public juce::Button::Listener
    , public HWRequestor
{
public:
    DynamicItemComponent();
    ~DynamicItemComponent();

protected:
    virtual void paint(juce::Graphics& g) override;
    virtual void resized() override;
    virtual void buttonClicked (juce::Button* b) override;
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override;

    void requestDynamicInfo();
    void reqestReplyList();
    void requestUserHead(const juce::String& szUserID);
    void updateFlowerView(int nCnt, int nOP=-1);
    void updateFavour(int nCnt, int nOP=-1);
    void updateReplyCountUI();
    void requestReplyList(int begin);
    int caculateExpandContentSize();
    void setCanGotoPage(bool can);
    void setShowDetailsButton(bool show);
    void setDynamicPanelData(HWTDynamicInfo* pInfo);

    void expandContent(bool exp);
    void expandChildren(bool exp);
    bool isExpandChildren();
    int getExpandChildrenHeight();

private:
    bool canGotoPage;
    int dynamicID, replyListCount, gotoPageNo, gotoPageZone;
    std::map<int,bool> mapReplyListPage;
    juce::String userID;
    juce::TextLayout contentTextLayout;
    juce::AttributedString textContent;
    juce::ScopedPointer<ListView> replayListView;

    juce::ScopedPointer<juce::ImageComponent> userImage;
    juce::ScopedPointer<juce::TextButton> userTitle;
    juce::ScopedPointer<juce::ImageButton> userFlower;
    juce::ScopedPointer<juce::ImageButton> userMagic;
    juce::ScopedPointer<juce::ImageButton> userFavour;
    juce::ScopedPointer<juce::TextButton> readThisPage;

    juce::ScopedPointer<juce::Label> labelDate;
    juce::ScopedPointer<juce::Label> labelFrom;
    juce::ScopedPointer<juce::TextButton> showDetails;
    juce::ScopedPointer<juce::TextButton> replayButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DynamicItemComponent)
};