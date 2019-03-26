#pragma once
#include "BookPage.h"

class BookPageCtrl
    : public HWRequestor
    , public IBookPageInfo
    , public juce::Button::Listener
    , public juce::Timer
{
public:
    BookPageCtrl(BookPage& o);
    ~BookPageCtrl();

    struct SelectLines
    {
        SelectLines() { rcBlockX0 = rcBlockY0 = rcBlockX1 = rcBlockY1 = 0.0f; }
        float rcBlockX0, rcBlockY0, rcBlockX1, rcBlockY1;
    };

    struct SelectBlock : public std::list<SelectLines>
    {
        int nPageNo, nZone, nScopePos, nScopeNum;
    };

    struct PostilArea
    {
        int nZone;
        juce::Rectangle<int> rcArea;
        int nMy;
        int nOther;
        unsigned int dwState;

        PostilArea(int nz, const juce::Rectangle<int>& rc, int nm, int no, unsigned int dw) : nZone(nz), rcArea(rc), nMy(nm), nOther(no), dwState(dw) {}
    };

protected:
    virtual void paint(juce::Graphics& g) override;
    virtual void resized() override;
    virtual void mouseExit(const juce::MouseEvent& event) override;
    virtual void mouseMove(const juce::MouseEvent& event) override;
    virtual void mouseDown(const juce::MouseEvent& event) override;
    virtual void mouseUp(const juce::MouseEvent& event) override;
    virtual void mouseDrag(const juce::MouseEvent& event) override;
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para);
    virtual void onPageComplate() override;
    virtual void buttonClicked (juce::Button*) override;
    virtual void timerCallback() override;

    void moveSelectBlock(const juce::Point<int>& down, const juce::Point<int>& pos);
    void cleanSelectResult();
    bool hasSelectResult() const;
    void showMenuSelected();
    void showMenuNormal();
    void copyTextComplate(int ID);
    void clickAreaPostil(PostilArea*);
    void setButtonImage(bool hasMark);
    void initPostilArea();
private:
    BookPage& owner;

    // select
    static SelectBlock listSelectRect;			// 当前选择的区域
    HWPageObjectUI::Ptr pageObject;
    bool newSelected;

    // mark
    juce::ScopedPointer<juce::ImageButton> buttonMark;

    // postil
    std::vector<PostilArea*> postilAreas;
    bool willFlashPostil;
    int flashPostilZone;
    int flashPostilCount;
    PostilArea* flashPostil;
    PostilArea* hoverArea;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookPageCtrl)
};