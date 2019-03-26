#pragma once
#include "ReadBrowser.h"

class BookViewHor
    : public BookView
    , public ReadListViewModel
    , public HWRequestor

{
public:
    BookViewHor();
    ~BookViewHor();

protected:
    // Component
    void paint (juce::Graphics&) override;
    void resized() override;

    // BookView
    virtual void gotoPage(int page) override;
    virtual void zoomIn() override;
    virtual void zoomOut() override;
    virtual void pagePrev() override;
    virtual void pageNext() override;
    virtual void setProtectEye(bool eye) override;
    virtual void setExpandShow(bool show) override;
    virtual void reload() override;
    virtual void update() override;

    // ReadListViewModel
    virtual int getNumRows() override;
    virtual juce::Component* refreshComponentForRow (int rowNumber, Component* existingComponentToUpdate) override;
    virtual void listWasScrolled() override;

    // ≈……˙¿‡÷ÿ‘ÿ
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override;

    void updateBookPageArea();
    void requestReadOnePage();
    void animatePageCurl();

private:
    bool isLastExpandZoomed;
    juce::ScopedPointer<ReadListView> taglistbox;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookViewHor)
};