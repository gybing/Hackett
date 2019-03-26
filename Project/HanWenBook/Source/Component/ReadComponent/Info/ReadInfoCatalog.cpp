#include "JuceHeader.h"
#include "ReadInfoCatalog.h"

ReadInfoCatalog::ReadInfoCatalog()
    : hasNeedInitData(false)
    , justShowChapter(false)
{
    addAndMakeVisible(titleLabel = new juce::Label(juce::String::empty, L"目录"));
    addAndMakeVisible(contentListBox = new juce::ListBox(juce::String::empty, this));

    titleLabel->setFont(juce::Font(30));
    titleLabel->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    titleLabel->setJustificationType(juce::Justification::centredLeft);
    contentListBox->setRowHeight(35);
    contentListBox->setColour(juce::ListBox::ColourIds::backgroundColourId, ProjectInfo::colorLightWhite);
    contentListBox->getViewport()->setScrollBarsShown(true, false, true, false);
    contentListBox->getViewport()->setScrollBarThickness(10);
    hoverSelector = new juce::ListBoxMouseHoverSelector(*contentListBox, this);

    if (HWDocRead::getInstance()->HasChildCatalog())
    {
        buttonjustShowChapter = new juce::ColorButton;
        buttonjustShowChapter->setButtonBorder(0);
        buttonjustShowChapter->setButtonText(L"显示全部");
        buttonjustShowChapter->setColour(juce::ColorButton::ColourIds::buttonBackgroundId, juce::Colours::transparentBlack);
        buttonjustShowChapter->setColour(juce::ColorButton::ColourIds::buttonColourNormalId, ProjectInfo::colorLightBlue);
        buttonjustShowChapter->setColour(juce::ColorButton::ColourIds::buttonColourHoverId, ProjectInfo::colorLightBlue.contrasting(0.2f));
        buttonjustShowChapter->setColour(juce::ColorButton::ColourIds::buttonColourDownId, ProjectInfo::colorLightBlue.contrasting(0.4f));
        buttonjustShowChapter->setColour(juce::ColorButton::ColourIds::textColourNormalId, juce::Colours::white);
        buttonjustShowChapter->setColour(juce::ColorButton::ColourIds::textColourHoverId, juce::Colours::white);
        buttonjustShowChapter->setColour(juce::ColorButton::ColourIds::textColourDownId, juce::Colours::white);
        buttonjustShowChapter->addListener(this);
        addAndMakeVisible(buttonjustShowChapter);
    }
    HWDocRead::getInstance()->Register(this);
}

ReadInfoCatalog::~ReadInfoCatalog()
{
    HWDocRead::getInstance()->UnRegister(this);
}

void ReadInfoCatalog::resized()
{
    const juce::Rectangle<int> area = getLocalBounds();
    titleLabel->setBounds(area.getX() + 18, area.getY() + 30, 100, 35);
    contentListBox->setBoundsInset(juce::BorderSize<int>(area.getY() + 80, 15, 15, 15));
    if (buttonjustShowChapter)
    {
        buttonjustShowChapter->setBounds(area.getWidth()-130, 30, 100,31);
    }
}

void ReadInfoCatalog::visibilityChanged()
{
    if (isShowing())
        initData();
}

void ReadInfoCatalog::initData()
{
    if (hasNeedInitData && strBook == HWDocRead::getInstance()->GetBook())
        return;
    
    strBook = HWDocRead::getInstance()->GetBook();
    hasNeedInitData = true;

    showTextTip(getNumRows() == 0);
    contentListBox->updateContent();
}

void ReadInfoCatalog::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{

}

int ReadInfoCatalog::getNumRows()
{
    return HWDocRead::getInstance()->GetCatalogCount(!justShowChapter);
}

void ReadInfoCatalog::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    int nDeep = -1;
    const HWTCatalog* pCatalog = HWDocRead::getInstance()->GetCatalog(rowNumber, !justShowChapter, nDeep);
    if (pCatalog)
    {
        if (hoverSelector && hoverSelector->rowNumber == rowNumber)
        {
            g.setColour(juce::Colour(0xFFebe9e6));
            g.fillRect(1, 1, width-2, height-2);
        }

        g.setColour(ProjectInfo::colorLightBlack);
        g.setFont(juce::Font(18));

        juce::String strFmt;
        if (!justShowChapter)
        {
            for (int i = nDeep; i > 0; --i)
                strFmt << L"    ";
        }
        strFmt << pCatalog->strTitle;
        const int pos = width*0.7f;
        g.drawText(strFmt, 10, 0, pos, height, juce::Justification::centredLeft);

        strFmt.clear();
        strFmt << L"(" << pCatalog->strShowPage << L")";
        g.drawText(strFmt, pos, 0, width-pos-10, height, juce::Justification::centredRight);
    }
}

void ReadInfoCatalog::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    int nDeep = -1;
    const HWTCatalog* pCatalog = HWDocRead::getInstance()->GetCatalog(row, !justShowChapter, nDeep);
    if (pCatalog)
    {
        ReadWindowHelper::getInstance()->gotoPage(pCatalog->nPageNo);
    }
}

void ReadInfoCatalog::listBoxRowHover()
{
    contentListBox->repaintRow(hoverSelector->rowNumber);
    contentListBox->repaintRow(hoverSelector->lastRowNumber);
}

void ReadInfoCatalog::buttonClicked(juce::Button* b)
{
    if (justShowChapter = !justShowChapter)
        b->setButtonText(L"显示全部");
    else
        b->setButtonText(L"仅显示章");

    hasNeedInitData = false;
    initData();
}

void ReadInfoCatalog::OnBookOpened()
{
    hasNeedInitData = false;
    if (isShowing())
        initData();
}

void ReadInfoCatalog::showTextTip(bool s)
{
    if (s)
    {
        textTip = new juce::AttributedImageText;
        // textTip->append(CXOption::getInstance()->GetImage(L"read_desk_book_mark_n.png", 0, 3, false));
        // textTip->append(L"您还没有对这本书做过书签，您可以点击图书上方", juce::Font(21), ProjectInfo::colorLightBlack);
        // textTip->append(L"添加书签", juce::Font(21), ProjectInfo::colorLightBlack);
    }
    else if (textTip)
    {
        textTip = nullptr;
    }
}
