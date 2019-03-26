#include "JuceHeader.h"
#include "BookCellComponent.h"
#include "RightShelfComponent.h"

BookCellComponent::BookCellComponent(ShelfViewComponent& sv)
    : owner(sv)
{
    setData(NULL, -1);
}

BookCellComponent::~BookCellComponent()
{

}

void BookCellComponent::setData(const HWTUserBook* pUserBook, int idx)
{
    itemIndex = idx;
    if (pUserBook)
    {
        if (pUserBook->nId != m_UserBook.nId || pUserBook->strName != m_UserBook.strName)
        {
            m_UserBook = *pUserBook;
            HWParaBookPic bookPicUI(m_UserBook.strBook, BOOKRES_FRONT3);
            HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Pic_Download, &bookPicUI, 0);
        }
    }
    else
    {
        m_UserBook.nId = -1;
        m_UserBook.nTag = -1;
        m_UserBook.strBook = L"";
        m_UserBook.tTime = 0;
        m_imgBookFace = juce::Image();
        m_imgBookShadow = CXIMAGE_CACHE_LOAD(L"shelf_book_shadow.png");
    }
}

void BookCellComponent::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    if (reqID == HWReq_Pic_Download)
    {
        HWParaBookPic* par = (HWParaBookPic*)para;
        if (par->m_strBook == m_UserBook.strBook)
        {
            juce::Image* pBitmap = HWDocBookData::getInstance()->GetBookImage();
            m_imgBookFace = pBitmap ? *pBitmap : juce::Image();
            repaint();
        }
    }
}

void BookCellComponent::paint (juce::Graphics& g)
{
    if (!m_imgBookFace.isNull())
    {
        g.drawImage(m_imgBookFace, 0, 0, getWidth()-5, getHeight(), 0, 0, m_imgBookFace.getWidth(), m_imgBookFace.getHeight());
        g.drawImage(m_imgBookShadow, getWidth()-6, 0, 6, getHeight(), 0, 0, m_imgBookShadow.getWidth(), m_imgBookShadow.getHeight());
    }

    if (owner.isItemSelected(itemIndex) && owner.getNumSelectedItems() > 1)
    {
        g.setColour(juce::Colour(0x3F000000));
        g.fillRect(0, 0, getWidth()-5, getHeight());
    }
}

bool BookCellComponent::isValidBookCell() const
{
    return m_UserBook.strBook.isNotEmpty() && m_imgBookFace.isValid();
}

const HWTUserBook& BookCellComponent::getUserBook() const
{
    return m_UserBook;
}
