#include "JuceHeader.h"
#include "ReadInfoBrief.h"

ReadInfoBrief::ReadInfoBrief()
    : hasNeedInitData(false)
    , justShowChapter(false)
{
    addAndMakeVisible(bookFace = new juce::ImageComponent);
    bookFace->setBounds(15,30,96,130);

    addAndMakeVisible(bookName = new juce::Label);
    addAndMakeVisible(bookAuthor = new juce::Label);
    addAndMakeVisible(bookPublish = new juce::Label);
    addAndMakeVisible(bookPubData = new juce::Label);

    bookName->setColour(juce::Label::ColourIds::textColourId, juce::Colour(0xFF6b5147));
    bookName->setFont(juce::Font(20));
    bookAuthor->setColour(juce::Label::ColourIds::textColourId, juce::Colours::grey);
    bookAuthor->setFont(juce::Font(18));
    bookPublish->setColour(juce::Label::ColourIds::textColourId, juce::Colours::grey);
    bookPublish->setFont(juce::Font(18));
    bookPubData->setColour(juce::Label::ColourIds::textColourId, juce::Colours::grey);
    bookPubData->setFont(juce::Font(18));

    textBrief = new juce::TextEditor;
    textBrief->setFont(juce::Font(18));
    textBrief->setColour(juce::TextEditor::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    textBrief->setMultiLine(true);
    textBrief->setBorder(juce::BorderSize<int>(0,0,0,0));
    textBrief->setReadOnly(true);
    textBrief->setCaretVisible(false);
    textBrief->setColour(juce::TextEditor::ColourIds::backgroundColourId, ProjectInfo::colorLightWhite);
    textBrief->setColour(juce::TextEditor::ColourIds::outlineColourId, ProjectInfo::colorLightWhite);
    textBrief->setColour(juce::TextEditor::ColourIds::shadowColourId, ProjectInfo::colorLightWhite);
    addAndMakeVisible(textBrief);

    HWDocRead::getInstance()->Register(this);
}

ReadInfoBrief::~ReadInfoBrief()
{
    HWDocRead::getInstance()->UnRegister(this);
}

void ReadInfoBrief::resized()
{
    const juce::Rectangle<int> area = bookFace->getBounds();
    const int posX = area.getRight() + 15;
    const int posY = area.getY() + 5;
    const int w = getWidth() - posX - 10;

    textBrief->setBoundsInset(juce::BorderSize<int>(area.getBottom() + 20, 15, 15, 15));
    bookName->setBounds(posX, posY, w, 20);
    bookAuthor->setBounds(posX, posY + 30, w, 20);
    bookPublish->setBounds(posX, posY + 55, w, 20);
    bookPubData->setBounds(posX, posY + 80, w, 20);
}

void ReadInfoBrief::visibilityChanged()
{
    if (isShowing())
        initData();
}

void ReadInfoBrief::initData()
{
    if (hasNeedInitData && strBook == HWDocRead::getInstance()->GetBook())
        return;
    
    strBook = HWDocRead::getInstance()->GetBook();
    hasNeedInitData = true;

    HWParaBookInfo bookInfo(strBook);
    HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Book_Info, &bookInfo, 0);

    HWParaBookPic bookPicUI(strBook, BOOKRES_FRONT3);
    HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Pic_Download, &bookPicUI, 0);

    // 图书简介
    HWParaBookRes bookResUI(strBook);
    HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Book_Res, &bookResUI, 0);
}

void ReadInfoBrief::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    switch (reqID)
    {
    case HWReq_Pic_Download:
        {
            if (err == HWError_SUCCEED)
            {
                if (juce::Image* pBitmap = HWDocBookData::getInstance()->GetBookImage())
                {
                    const float fRate = pBitmap->getHeight()*1.0f/pBitmap->getWidth()*1.0f;
                    int nHeight = 96 * fRate;

                    bookFace->setImage(*pBitmap);
                    bookFace->setBounds(15, 30, 96, nHeight);
                }
            }
        }
        break;
    case HWReq_Book_Info:
        {
            if (err == HWError_SUCCEED )
            {
                const HWTBookInfo* pBookInfo = HWDocBookData::getInstance()->GetBookInfo();
                if (pBookInfo)
                {
                    juce::String strFmt;
                    for (unsigned int i = 0; i < pBookInfo->arrAuthors.size(); ++i)
                    {
                        if (!strFmt.isEmpty())
                            strFmt << L"，";

                        strFmt << pBookInfo->arrAuthors[i];
                    }

                    bookName->setText(pBookInfo->strName, juce::dontSendNotification);
                    bookAuthor->setText(strFmt, juce::dontSendNotification);
                    bookPublish->setText(pBookInfo->strPress, juce::dontSendNotification);

                    juce::Time tm(pBookInfo->tPressTime);
                    bookPubData->setText(tm.formatted(L"%Y年%m月"), juce::dontSendNotification);
                }
            }
        }
        break;
    case HWReq_Book_Res:
        {
            if (juce::String* pstrBrief = HWDocBookData::getInstance()->GetBookBrief())
            {
                textBrief->setText(*pstrBrief, juce::dontSendNotification);
            }
        }
        break;
    default:
        break;
    }
}

void ReadInfoBrief::OnBookOpened()
{
    hasNeedInitData = false;
    if (isShowing())
        initData();
}
