#pragma once

class IBookPageInfo
    : public juce::Component
{
public:
    virtual void onPageComplate() = 0;
};

class BookPage
    : public juce::Component
    , public HWRequestor
{
public:
    BookPage();
    ~BookPage();

    void setPageNo(int pageno);
    int getPageNo() const;

    HWPageObjectUI::Ptr getPageObject();
protected:
    virtual void paint (juce::Graphics&) override;
    virtual void resized() override;
    virtual void mouseDown(const juce::MouseEvent& event);

protected:
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para);

    void requestPageObjectIfNeeded();

private:
    juce::Image imageBookPage;
    int bookWidth, bookHeight, bookPageNo;
    juce::String strBook;
    bool isDrawSmooth, isDrawColorEye, isDrawCompact;
    std::vector<juce::ScopedPointer<IBookPageInfo>> pageInfos;
    HWPageObjectUI::Ptr pageobject;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookPage)
};