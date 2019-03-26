#include "JuceHeader.h"
#include "BookViewVer.h"
#include "../Page/BookPage.h"

const juce::Point<int> singleStepScroll(0, 30);

BookViewVer::BookViewVer()
    : isLastExpandZoomed(false)
{
    addChildComponent(taglistbox = new ReadListView(juce::String::empty, this));
    taglistbox->getViewport()->setSingleStepSizes(singleStepScroll.x, singleStepScroll.y);
    taglistbox->getViewport()->setScrollBarsShown(true, false, true, true);
    
    gotoPage(HWDocRead::getInstance()->GetHistoryPageNo());
    taglistbox->setVisible(true);
}

BookViewVer::~BookViewVer()
{
}

void BookViewVer::paint(juce::Graphics&)
{

}

void BookViewVer::resized()
{
    bookPageZoomWidth[2] = getParentWidth() - 20;
    bookPageZoomWidth[1] = bookPageZoomWidth[2] - 410;
    bookPageZoomWidth[0] = bookPageZoomWidth[1] - 410;
    updateBookPageArea();
}

void BookViewVer::gotoPage(int page)
{
    taglistbox->scrollToEnsureRowIsOnscreen(page);
}

void BookViewVer::zoomIn()
{
    int zoom = CXOption::getInstance()->GetReadBookRate();
    if (zoom > 0)
    {
        CXOption::getInstance()->SetReadBookRate(--zoom);
        updateBookPageArea();

        if (isLastExpandZoomed)
            isLastExpandZoomed = false;
    }
}

void BookViewVer::zoomOut()
{
    int zoom = CXOption::getInstance()->GetReadBookRate();
    if (zoom < (CXOption::getInstance()->GetReadExpandShow() ? 1 : 2))
    {
        CXOption::getInstance()->SetReadBookRate(++zoom);
        updateBookPageArea();
    }
}

void BookViewVer::pagePrev()
{
    if (juce::Viewport* vp = taglistbox->getViewport())
    {
        vp->setViewPosition(vp->getViewPosition() - singleStepScroll - singleStepScroll - singleStepScroll);
    }
}

void BookViewVer::pageNext()
{
    if (juce::Viewport* vp = taglistbox->getViewport())
    {
        vp->setViewPosition(vp->getViewPosition() + singleStepScroll + singleStepScroll + singleStepScroll);
    }
}

juce::Component* BookViewVer::refreshComponentForRow (int rowNumber, Component* existingComponentToUpdate)
{
    BookPage* page = existingComponentToUpdate ? static_cast<BookPage*>(existingComponentToUpdate) : new BookPage;
    jassert(page);
    page->setPageNo(rowNumber);
    return page;
}

void BookViewVer::listWasScrolled()
{
    requestReadOnePage();
}

void BookViewVer::updateBookPageArea()
{
    const int rate = CXOption::getInstance()->GetReadBookRate();

    if (rate >=0 && rate < 3)
    {
        const int w = bookPageZoomWidth[rate];
        taglistbox->setVisible(false);
        taglistbox->setRowSize(10 + (w * HWDocRead::getInstance()->GetPDFAspectRatio()));

        if (!CXOption::getInstance()->GetReadExpandShow())
        {
            const int mw = (getWidth() - w) / 2;
            taglistbox->setBoundsInset(juce::BorderSize<int>(0, mw, 0, mw));
        }
        else
        {
            taglistbox->setBoundsInset(juce::BorderSize<int>(0, 0, 0, (getWidth() - w)));
        }
        taglistbox->setVisible(true);
    }
}

void BookViewVer::setProtectEye(bool eye)
{
    CXOption::getInstance()->SetBookPageBackColor(eye);
    reload();
}

void BookViewVer::setExpandShow(bool show)
{
    CXOption::getInstance()->SetReadExpandShow(show);
    if (CXOption::getInstance()->GetReadExpandShow() && CXOption::getInstance()->GetReadBookRate()==2)
    {
        zoomIn();
        isLastExpandZoomed = true;
    }
    else if (isLastExpandZoomed)
    {
        zoomOut();
    }
    else
    {
        updateBookPageArea();
    }
}

void BookViewVer::requestReadOnePage()
{
    const int pageno = taglistbox->getCurrentWholeIndex() - 1;
    if (pageno >=0 && pageno < HWDocRead::getInstance()->GetPDFPageCount())
    {
        HWParaReadOnePage readOnePageUI(HWDocRead::getInstance()->GetBook(), pageno);
        HWDocRead::getInstance()->SubmitRequest(this, HWReq_Read_OnePage, &readOnePageUI, 0);
        DBG(juce::String(L"ReadOnePage:") << pageno);
    }
}

void BookViewVer::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    // if (reqID == HWReq_Read_OnePage)
}

void BookViewVer::reload()
{
    taglistbox->reloadData();
}

void BookViewVer::update()
{
    taglistbox->updateContent();
}

int BookViewVer::getNumRows()
{
    return HWDocRead::getInstance()->GetPDFPageCount() + 2;
}
