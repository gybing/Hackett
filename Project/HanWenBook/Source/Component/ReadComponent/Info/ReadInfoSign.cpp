#include "JuceHeader.h"
#include "ReadInfoSign.h"

class ReadInfoSign::BookSignItem
    : public juce::Component
    , public juce::Button::Listener
    , public HWRequestor
{
public:
    BookSignItem(ReadInfoSign& r)
        : owner(r)
        , rowNumber(-1)
    {
        addAndMakeVisible(buttonDelete = new juce::TextButton(L"删除"));
        addAndMakeVisible(labelTitle = new juce::Label);
        addAndMakeVisible(labelCreateTime = new juce::Label);
        labelTitle->setFont(juce::Font(18));
        labelTitle->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
        labelTitle->setJustificationType(juce::Justification::centredLeft);
        labelTitle->setMinimumHorizontalScale(1.0f);
        labelTitle->setInterceptsMouseClicks(false, false);

        labelCreateTime->setFont(juce::Font(18));
        labelCreateTime->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
        labelCreateTime->setJustificationType(juce::Justification::centredRight);
        labelCreateTime->setInterceptsMouseClicks(false, false);
        buttonDelete->setColour(juce::TextButton::ColourIds::textColourOnId, ProjectInfo::colorLightRed);
        buttonDelete->setColour(juce::TextButton::ColourIds::textColourOffId, ProjectInfo::colorLightBlack);
        buttonDelete->addListener(this);
    }

    void setContext(int row, const juce::String& title, const juce::String& t, int id, int pageno)
    {
        rowNumber = row;
        labelTitle->setText(title.removeCharacters(juce::String(L"\n")), juce::dontSendNotification);
        labelCreateTime->setText(t, juce::dontSendNotification);
        signID = id;
        signPageno = pageno;
    }

    ~BookSignItem()
    {

    }

    int getSignPageno() const
    {
        return signPageno;
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
        HWParaSignDel delBookSigl(HWDocRead::getInstance()->GetBook(), signID);
        HWDocBookSign::getInstance()->SubmitRequest(this, HWReq_Sign_Del, &delBookSigl, 0);
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
        case HWReq_Sign_Add:
        case HWReq_Sign_Del:
            {
                ReadWindowHelper::getInstance()->reload();
                owner.contentListBox->updateContent();
            }
        default:
            break;
        }
    }

private:
    ReadInfoSign& owner;
    int rowNumber, signID, signPageno;
    juce::ScopedPointer<juce::Label> labelTitle;
    juce::ScopedPointer<juce::Label> labelCreateTime;
    juce::ScopedPointer<juce::TextButton> buttonDelete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookSignItem)
};

//////////////////////////////////////////////////////////////////////////
// ReadInfoSign

ReadInfoSign::ReadInfoSign()
    : hasNeedInitData(false)
{
    addAndMakeVisible(titleLabel = new juce::Label(juce::String::empty, L"好词妙句"));
    addAndMakeVisible(contentListBox = new juce::ListBox(juce::String::empty, this));

    titleLabel->setFont(juce::Font(30));
    titleLabel->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    titleLabel->setJustificationType(juce::Justification::centredLeft);
    contentListBox->setRowHeight(35);
    contentListBox->setColour(juce::ListBox::ColourIds::backgroundColourId, ProjectInfo::colorLightWhite);

    hoverSelector = new juce::ListBoxMouseHoverSelector(*contentListBox, this);
    HWDocBookSign::getInstance()->Register(this);
}

ReadInfoSign::~ReadInfoSign()
{
    HWDocBookSign::getInstance()->UnRegister(this);
}

void ReadInfoSign::resized()
{
    const juce::Rectangle<int> area = getLocalBounds();
    titleLabel->setBounds(area.getX() + 18, area.getY() + 30, 120, 35);
    contentListBox->setBoundsInset(juce::BorderSize<int>(area.getY() + 80, 15, 15, 15));
}

void ReadInfoSign::visibilityChanged()
{
    if (isShowing())
        initData();
}

void ReadInfoSign::initData()
{
    if (hasNeedInitData && strBook == HWDocRead::getInstance()->GetBook())
        return;
    
    strBook = HWDocRead::getInstance()->GetBook();
    hasNeedInitData = true;

    HWParaSignList bookSignListUI(HWDocRead::getInstance()->GetBook());
    HWDocBookSign::getInstance()->SubmitRequest(this, HWReq_Sign_List, &bookSignListUI, 0);
}

void ReadInfoSign::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{

}

int ReadInfoSign::getNumRows()
{
    return HWDocBookSign::getInstance()->GetBookSignCount();
}

void ReadInfoSign::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
}

void ReadInfoSign::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    if (BookSignItem* item = static_cast<BookSignItem*>(contentListBox->getComponentForRowNumber(row)))
    {
        ReadWindowHelper::getInstance()->gotoPage(item->getSignPageno());
    }
}

void ReadInfoSign::listBoxRowHover()
{
    contentListBox->repaintRow(hoverSelector->rowNumber);
    contentListBox->repaintRow(hoverSelector->lastRowNumber);
}

juce::Component* ReadInfoSign::refreshComponentForRow(int rowNumber, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    const HWTBookSign* pBookSign = HWDocBookSign::getInstance()->GetBookSign(rowNumber);
    if (pBookSign)
    {
        juce::Time time(pBookSign->tTime);   
        BookSignItem* item = existingComponentToUpdate ? (static_cast<BookSignItem*>(existingComponentToUpdate)) : new BookSignItem(*this);
        item->setContext(rowNumber, pBookSign->strText, time.formatted(L"%Y.%m.%d 创建"), pBookSign->nId, pBookSign->nPageNo);
        item->setInterceptsMouseClicks(false, true);
        return item;
    }

    return existingComponentToUpdate;
}

void ReadInfoSign::OnBookOpened()
{
    hasNeedInitData = false;
    if (isShowing())
        initData();
}

void ReadInfoSign::OnBookSignAdd()
{
    hasNeedInitData = false;
    if (isShowing())
        initData();
}

void ReadInfoSign::OnBookSignDel()
{
    hasNeedInitData = false;
    if (isShowing())
        initData();
}

void ReadInfoSign::OnBookSignListChanged()
{
    contentListBox->updateContent();
}