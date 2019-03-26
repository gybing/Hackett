#include "JuceHeader.h"
#include "BookPage.h"
#include "BookPageSign.h"
#include "BookPageCtrl.h"

BookPage::BookPage()
    : bookWidth(0)
    , bookHeight(0)
    , bookPageNo(ProjectInfo::PagenoInvalid)
    , isDrawSmooth(false)
    , isDrawColorEye(false)
    , isDrawCompact(false)
{
    pageInfos.push_back(new BookPageSign(*this));
    addAndMakeVisible(pageInfos.back());
    pageInfos.back()->setInterceptsMouseClicks(false, false);

    pageInfos.push_back(new BookPageCtrl(*this));
    addAndMakeVisible(pageInfos.back());

    setMouseClickGrabsKeyboardFocus(true);
}

BookPage::~BookPage()
{

}

void BookPage::paint(juce::Graphics& g)
{
    g.fillAll(ProjectInfo::colorDarkLightGray);

    if (!imageBookPage.isNull())
    {
        g.drawImage(imageBookPage, 0, 5, bookWidth, bookHeight, 0, 0, imageBookPage.getWidth(), imageBookPage.getHeight(), false);
    }
    else
    {
        g.setColour(isDrawColorEye ? ProjectInfo::colorEyeGreen : ProjectInfo::colorLightWhite);
        g.fillRect(0, 5, bookWidth, bookHeight);
    }

    if (CXOption::getInstance()->GetReadBrowserTypeHor())
    {
        if (bookPageNo % 2 == 0)
        {
            g.drawImage(CXIMAGE_CACHE_LOAD(L"book_shadow_left.png"), getWidth()-60, 5, 60, getHeight(), 0, 0, 60, 1);
        }
        else
        {
            g.drawImage(CXIMAGE_CACHE_LOAD(L"book_shadow_right.png"), 0, 5, 60, getHeight(), 0, 0, 60, 1);
        }
    }
    
#ifdef JUCE_DEBUG
    static int i = 0;
    i++;
    juce::String strLog;
    strLog<<"page paint: "<<i<<" times , page no:"<<bookPageNo;
    DBG(strLog);
#endif // JUCE_DEBUG

}

void BookPage::resized()
{
    const int w = getWidth();
    const int h = getHeight();
    if (w != bookWidth || h != bookHeight)
    {
        bookWidth = w;
        bookHeight = h - 10;

        for (unsigned int i=0; i<pageInfos.size(); ++i)
            pageInfos[i]->setBounds(0, 3, w, h);

        requestPageObjectIfNeeded();
    }
}

void BookPage::setPageNo(int pageno)
{
    const bool isSmooth = CXOption::getInstance()->GetReadFontSmooth();
    const bool isEye = CXOption::getInstance()->GetBookPageBackColor();
    const bool isCompact = CXOption::getInstance()->GetReadCompact();

    const bool bookOrPageChanged = pageno != bookPageNo || strBook != HWDocRead::getInstance()->GetBook();
    const bool bookPropertyChanged = isSmooth != isDrawSmooth || isEye != isDrawColorEye || isCompact != isDrawCompact;

    if (bookOrPageChanged)
    {
        strBook = HWDocRead::getInstance()->GetBook();
        bookPageNo = pageno;

        // clear book image
        imageBookPage = juce::Image::null;
    }
    
    if (bookPropertyChanged)
    {
        isDrawColorEye = isEye;
        isDrawCompact = isCompact;
        isDrawSmooth = isSmooth;
    }

    for (unsigned int i=0; i<pageInfos.size(); ++i)
        pageInfos[i]->setVisible(false);

    if (bookOrPageChanged || bookPropertyChanged)
    {
        requestPageObjectIfNeeded();
    }
}

void BookPage::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    if (reqID == HWReq_Draw_PageDraw)
    {
        if (err == HWError_SUCCEED)
        {
            if (const HWParaPageDraw* pageDrawUI = static_cast<HWParaPageDraw*>(para))
            {
                if (pageDrawUI->m_strBook != strBook ||
                    pageDrawUI->m_nPageNo != bookPageNo ||
                    pageDrawUI->m_nWidth != bookWidth ||
                    pageDrawUI->m_nHeight != bookHeight)
                {
                    return;
                }

                pageobject = HWDocDraw::getInstance()->GetPageObject();
                PageObject* pobj = pageobject->getPageObject();
                const PagePixmap* pPix = pobj->GetPagePixmap();
                if (pPix)
                {
                    imageBookPage = CXOption::loadImageFromMemory((const void*)pPix->samples, pPix->w, pPix->h);
                    repaint();
                    KnlDropPagePixmap(pPix);
                }

                for (unsigned int i=0; i<pageInfos.size(); ++i)
                {
                    pageInfos[i]->onPageComplate();
                    pageInfos[i]->setVisible(true);
                }
            }
        }
    }
    else if (reqID == HWReq_Pic_Download)
    {
        // 忽略早期请求
        if (err == HWError_SUCCEED && (bookPageNo == 0 || bookPageNo == HWDocRead::getInstance()->GetPDFPageCount()+1))
        {
            juce::Image* pBitmap = HWDocBookData::getInstance()->GetBookImage();
            if (pBitmap)
            {
                imageBookPage = *pBitmap;
                repaint();
            }
        }
    }
    else if (reqID == HWReq_Read_OnePage)
    {
        jassert(err == HWError_SUCCEED);
    }
}

void BookPage::requestPageObjectIfNeeded()
{
    if (1)
    {
        const int pageCount = HWDocRead::getInstance()->GetPDFPageCount();
        if (bookPageNo == 0)
        {
            HWParaBookPic bookPicUI(HWDocRead::getInstance()->GetBook(), BOOKRES_FRONT0);
            HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Pic_Download, &bookPicUI, 0);
        }
        else if (bookPageNo == pageCount)
        {
            HWParaBookPic bookPicUI(HWDocRead::getInstance()->GetBook(), BOOKRES_BACK0);
            HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Pic_Download, &bookPicUI, 0);
        }
        else if (bookWidth > 97 && bookHeight > 135 && bookPageNo<pageCount+1)
        {
            const int margin = isDrawCompact ? 30 : 0;

            HWParaPageDraw pageDrawUI(strBook, bookPageNo, bookWidth, bookHeight, 1.0f, 
                isDrawSmooth ? (isDrawCompact ? DRAWMODE_TERSE_GRAY:DRAWMODE_GRAY) :
                (isDrawCompact ? DARWMODE_TERSE_SINGLE : DARWMODE_SINGLE),
                isDrawColorEye ? 0xFC7EDCC : 0xFFF3F3F0, 
                DRAWALIGN_VCENTER|DRAWALIGN_HCENTER, DRAWBITORDER_RGBA,
                margin, margin, margin, margin);

             HWDocDraw::getInstance()->SubmitRequest(this, HWReq_Draw_PageDraw, &pageDrawUI, 0, true);

#if JUCE_DEBUG
            juce::String strDBG;
            strDBG << L"开始请求第 " << bookPageNo << L" 页数据 W:" << bookWidth << L" H:" << bookHeight;
            DBG(strDBG);
#endif
        }
    }
}

int BookPage::getPageNo() const
{
    return bookPageNo;
}

HWPageObjectUI::Ptr BookPage::getPageObject()
{
    return pageobject;
}

void BookPage::mouseDown(const juce::MouseEvent& event)
{

}
