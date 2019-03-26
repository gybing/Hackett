#include "JuceHeader.h"
#include "BookViewHor.h"
#include "../Page/BookPage.h"

BookViewHor::BookViewHor()
    : isLastExpandZoomed(false)
{
    addChildComponent(taglistbox = new ReadListView(juce::String::empty, this, false));
    taglistbox->getViewport()->setScrollBarsShown(false, true, false, false);
    gotoPage(HWDocRead::getInstance()->GetHistoryPageNo());
    taglistbox->setVisible(true);
}

BookViewHor::~BookViewHor()
{
}

void BookViewHor::paint(juce::Graphics&)
{

}

void BookViewHor::resized()
{
    updateBookPageArea();
}

void BookViewHor::gotoPage(int page)
{
    taglistbox->scrollToEnsureRowIsOnscreen(page);
}

void BookViewHor::zoomIn()
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

void BookViewHor::zoomOut()
{
    int zoom = CXOption::getInstance()->GetReadBookRate();
    if (zoom < (CXOption::getInstance()->GetReadExpandShow() ? 1 : 2))
    {
        CXOption::getInstance()->SetReadBookRate(++zoom);
        updateBookPageArea();
    }
}

void BookViewHor::pagePrev()
{
    if (juce::Viewport* vp = taglistbox->getViewport())
    {
        // vp->setViewPosition(vp->getViewPosition() - singleStepScroll - singleStepScroll - singleStepScroll);
    }
}

void BookViewHor::pageNext()
{
    if (juce::Viewport* vp = taglistbox->getViewport())
    {
        // vp->setViewPosition(vp->getViewPosition() + singleStepScroll + singleStepScroll + singleStepScroll);
    }
}

juce::Component* BookViewHor::refreshComponentForRow (int rowNumber, Component* existingComponentToUpdate)
{
    BookPage* page = existingComponentToUpdate ? static_cast<BookPage*>(existingComponentToUpdate) : new BookPage;
    jassert(page);
    page->setPageNo(rowNumber);
    return page;
}

void BookViewHor::listWasScrolled()
{
    animatePageCurl();
    requestReadOnePage();
}

void BookViewHor::updateBookPageArea()
{
    const int rate = CXOption::getInstance()->GetReadBookRate();

    if (rate >=0 && rate < 3)
    {
        float w = HWDocRead::getInstance()->GetPDFWidth();
        float h = HWDocRead::getInstance()->GetPDFHeight();

        const float fHScale = getHeight() / h;
        const float fWScale = getWidth() / (2.0f*w);
        const float rate = juce::jmin(fHScale, fWScale);

        w = HWDocRead::getInstance()->GetPDFWidth() * rate;
        h = (10 + w * HWDocRead::getInstance()->GetPDFAspectRatio());

        taglistbox->setVisible(false);
        taglistbox->setRowSize(w);

        const int x = (getWidth() - (2*w)) / 2;
        const int y = (getHeight() - h) / 2;
        taglistbox->setBoundsInset(juce::BorderSize<int>(y, x, y, x));
        taglistbox->getViewport()->setSingleStepSizes(w, w);
        taglistbox->setVisible(true);
    }
}

void BookViewHor::setProtectEye(bool eye)
{
    CXOption::getInstance()->SetBookPageBackColor(eye);
    reload();
}

void BookViewHor::setExpandShow(bool show)
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

void BookViewHor::requestReadOnePage()
{
    const int pageno = taglistbox->getCurrentWholeIndex() - 1;
    if (pageno >=0 && pageno < HWDocRead::getInstance()->GetPDFPageCount())
    {
        HWParaReadOnePage readOnePageUI(HWDocRead::getInstance()->GetBook(), pageno);
        HWDocRead::getInstance()->SubmitRequest(this, HWReq_Read_OnePage, &readOnePageUI, 0);
        DBG(juce::String(L"ReadOnePage:") << pageno);
    }
}

void BookViewHor::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    // if (reqID == HWReq_Read_OnePage)
}

void BookViewHor::reload()
{
    taglistbox->reloadData();
}

void BookViewHor::animatePageCurl()
{
    const int lastPageno = HWDocRead::getInstance()->GetHistoryPageNo();
    const int pageno = taglistbox->getCurrentWholeIndex() - 1;

    if (lastPageno < pageno)
    {

    }
    else if (lastPageno > pageno)
    {

    }
}

void BookViewHor::update()
{
    taglistbox->updateContent();
}

int BookViewHor::getNumRows()
{
    return HWDocRead::getInstance()->GetPDFPageCount() + 2;
}
