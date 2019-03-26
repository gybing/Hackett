#include "JuceHeader.h"
#include "ReadInfoMark.h"

class ReadInfoMark::BookMarkItem
    : public juce::Component
    , public juce::Button::Listener
    , public HWRequestor
{
public:
    BookMarkItem(ReadInfoMark& r)
        : owner(r)
        , rowNumber(-1)
    {
        addAndMakeVisible(buttonDelete = new juce::TextButton(L"删除"));
        addAndMakeVisible(labelTitle = new juce::Label);
        addAndMakeVisible(labelCreateTime = new juce::Label);
        labelTitle->setFont(juce::Font(18));
        labelTitle->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
        labelTitle->setJustificationType(juce::Justification::centredLeft);
        labelTitle->setInterceptsMouseClicks(false, false);

        labelCreateTime->setFont(juce::Font(18));
        labelCreateTime->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
        labelCreateTime->setJustificationType(juce::Justification::centredRight);
        labelCreateTime->setInterceptsMouseClicks(false, false);
        buttonDelete->setColour(juce::TextButton::ColourIds::textColourOnId, ProjectInfo::colorLightRed);
        buttonDelete->setColour(juce::TextButton::ColourIds::textColourOffId, ProjectInfo::colorLightBlack);
        buttonDelete->addListener(this);
    }

    void setContext(int row, const juce::String& title, const juce::String& t, int pageno)
    {
        rowNumber = row;
        labelTitle->setText(title, juce::dontSendNotification);
        labelCreateTime->setText(t, juce::dontSendNotification);
        markPageNo = pageno;
    }

    ~BookMarkItem()
    {

    }

    int getMarkPageNo() const
    {
        return markPageNo;
    }

protected:
    virtual void paint(juce::Graphics& g) override
    {
        if (owner.hoverSelector->rowNumber == rowNumber)
        {
            g.setColour(juce::Colour(0xFFebe9e6));
            g.fillRect(1, 1, getWidth()-2, getHeight()-2);
        }
    }

    virtual void resized() override
    {
        labelTitle->setBounds(juce::Rectangle<int>(5, 0, getWidth()-160, getHeight()));
        labelCreateTime->setBounds(juce::Rectangle<int>(getRight()-165, 0, 125, getHeight()));
        buttonDelete->setBounds(juce::Rectangle<int>(getRight()-45, 8, 45, 20));
    }

    virtual void buttonClicked (juce::Button* b) override
    {
        HWParaBookMarkDel delBookMark(HWDocRead::getInstance()->GetBook(), markPageNo);
        HWDocBookMark::getInstance()->SubmitRequest(this, HWReq_Mark_Del, &delBookMark, 0);
    }

    virtual void mouseDown (const juce::MouseEvent& event) override
    {
        int a =0;
    }

    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override
    {
        switch (reqID)
        {
        case HWReq_Mark_Add:
        case HWReq_Mark_Del:
            {
                ReadWindowHelper::getInstance()->reload();
                owner.contentListBox->updateContent();
            }
        default:
            break;
        }
    }

private:
    ReadInfoMark& owner;
    int rowNumber, markPageNo;
    juce::ScopedPointer<juce::Label> labelTitle;
    juce::ScopedPointer<juce::Label> labelCreateTime;
    juce::ScopedPointer<juce::TextButton> buttonDelete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookMarkItem)
};

//////////////////////////////////////////////////////////////////////////
// ReadInfoMark

ReadInfoMark::ReadInfoMark()
    : hasNeedInitData(false)
{
    addAndMakeVisible(titleLabel = new juce::Label(juce::String::empty, L"书签"));
    addAndMakeVisible(contentListBox = new juce::ListBox(juce::String::empty, this));

    titleLabel->setFont(juce::Font(30));
    titleLabel->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    titleLabel->setJustificationType(juce::Justification::centredLeft);
    contentListBox->setRowHeight(35);
    contentListBox->setColour(juce::ListBox::ColourIds::backgroundColourId, ProjectInfo::colorLightWhite);

    hoverSelector = new juce::ListBoxMouseHoverSelector(*contentListBox, this);
    HWDocBookMark::getInstance()->Register(this);
    HWDocRead::getInstance()->Register(this);
}

ReadInfoMark::~ReadInfoMark()
{
    HWDocRead::getInstance()->UnRegister(this);
    HWDocBookMark::getInstance()->UnRegister(this);
}

void ReadInfoMark::resized()
{
    const juce::Rectangle<int> area = getLocalBounds();
    titleLabel->setBounds(area.getX() + 18, area.getY() + 30, 100, 35);
    contentListBox->setBoundsInset(juce::BorderSize<int>(area.getY() + 80, 15, 15, 15));
}

void ReadInfoMark::visibilityChanged()
{
    if (isShowing())
        initData();
}

void ReadInfoMark::initData()
{
    if (hasNeedInitData && strBook == HWDocRead::getInstance()->GetBook())
        return;
    
    strBook = HWDocRead::getInstance()->GetBook();
    hasNeedInitData = true;

    HWParaBookMarkList bookMarkListUI(HWDocRead::getInstance()->GetBook());
    HWDocBookMark::getInstance()->SubmitRequest(this, HWReq_Mark_List, &bookMarkListUI, 0);
}

void ReadInfoMark::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    if (reqID == HWReq_Mark_List)
    {
        if (err == HWError_SUCCEED)
        {
            contentListBox->updateContent();
        }
    }
}

int ReadInfoMark::getNumRows()
{
    return HWDocBookMark::getInstance()->GetBookMarkCount();
}

void ReadInfoMark::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
}

void ReadInfoMark::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    if (BookMarkItem* item = static_cast<BookMarkItem*>(contentListBox->getComponentForRowNumber(row)))
    {
        ReadWindowHelper::getInstance()->gotoPage(item->getMarkPageNo());
    }
}

void ReadInfoMark::listBoxRowHover()
{
    contentListBox->repaintRow(hoverSelector->rowNumber);
    contentListBox->repaintRow(hoverSelector->lastRowNumber);
}

juce::Component* ReadInfoMark::refreshComponentForRow(int rowNumber, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    const HWTBookMark* pBookMark = HWDocBookMark::getInstance()->GetBookMark(rowNumber);
    if (pBookMark)
    {
        juce::String strFmt;
        if (HWDocRead::getInstance()->GetShowPageNo(pBookMark->nPageNo, strFmt))
            strFmt = L"第" + strFmt + L"页";

        juce::Time time(pBookMark->tTime);   

        BookMarkItem* item = existingComponentToUpdate ? (static_cast<BookMarkItem*>(existingComponentToUpdate)) : new BookMarkItem(*this);
        item->setContext(rowNumber, strFmt, time.formatted(L"%Y.%m.%d 创建"), pBookMark->nPageNo);
        item->setInterceptsMouseClicks(false, true);
        return item;
    }

    return existingComponentToUpdate;
}

void ReadInfoMark::OnBookMarkAdd()
{
    hasNeedInitData = false;
    if (isShowing())
        initData();

}

void ReadInfoMark::OnBookMarkDel()
{
    hasNeedInitData = false;
    if (isShowing())
        initData();
}

void ReadInfoMark::OnBookOpened()
{
    hasNeedInitData = false;
    if (isShowing())
        initData();
}
