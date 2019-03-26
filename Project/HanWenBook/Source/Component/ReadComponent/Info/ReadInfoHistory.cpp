#include "JuceHeader.h"
#include "ReadInfoHistory.h"

class ReadInfoHistory::BookHistoryItem
    : public juce::Component
    , public juce::Button::Listener
    , public HWRequestor
{
public:
    BookHistoryItem(ReadInfoHistory& r)
        : owner(r)
        , rowNumber(-1)
    {
        addAndMakeVisible(buttonToggle = new juce::ToggleButton);
        addAndMakeVisible(labelTitle = new juce::Label);
        addAndMakeVisible(labelPage = new juce::Label);
        addAndMakeVisible(labelCreateTime = new juce::Label);

        buttonToggle->addListener(this);

        labelTitle->setFont(juce::Font(18));
        labelTitle->setColour(juce::Label::ColourIds::textColourId, juce::Colour(0xFF6b5147));
        labelTitle->setJustificationType(juce::Justification::centredLeft);
        labelTitle->setMinimumHorizontalScale(1.0f);
        labelTitle->setInterceptsMouseClicks(false, false);

        labelPage->setFont(juce::Font(18));
        labelPage->setColour(juce::Label::ColourIds::textColourId, juce::Colours::grey);
        labelPage->setJustificationType(juce::Justification::centredLeft);
        labelPage->setMinimumHorizontalScale(1.0f);
        labelPage->setInterceptsMouseClicks(false, false);

        labelCreateTime->setFont(juce::Font(18));
        labelCreateTime->setColour(juce::Label::ColourIds::textColourId, juce::Colours::grey);
        labelCreateTime->setJustificationType(juce::Justification::centredRight);
        labelCreateTime->setInterceptsMouseClicks(false, false);
    }

    void setContext(int row, const juce::String& book, const juce::String& t, const juce::String& showpage)
    {
        rowNumber = row;
        buttonToggle->setToggleState(owner.findBookfromDelList(book), false);
        labelCreateTime->setText(t, juce::dontSendNotification);
        labelPage->setText(showpage, juce::dontSendNotification);
        strBook = book;

        HWParaBookInfo bookInfo(strBook);
        HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Book_Info, &bookInfo, 0);
    }

    ~BookHistoryItem()
    {

    }

    const juce::String& getBook() const
    {
        return strBook;
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
        buttonToggle->setBounds(0, 0, 25, 25);
        labelTitle->setBounds(juce::Rectangle<int>(25, 0, getWidth()-160, 25));
        labelPage->setBounds(25, 25, 100, 25);
        labelCreateTime->setBounds(juce::Rectangle<int>(getRight()-125, 25, 125, 25));
    }

    virtual void buttonClicked (juce::Button* b) override
    {
        owner.historyItemClick(strBook);
    }

    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para) override
    {
        switch (reqID)
        {
        case HWReq_Book_Info:
            {
                const HWParaBookInfo* paras = static_cast<HWParaBookInfo*>(para);
                if (err == HWError_SUCCEED && paras && paras->m_strBook == strBook)
                {
                    if (const HWTBookInfo* info = HWDocBookData::getInstance()->GetBookInfo())
                    {
                        labelTitle->setText(info->strName, juce::dontSendNotification);
                    }
                }
                break;
            }
        case HWReq_Sign_Add:
        case HWReq_Sign_Del:
            {
                ReadWindowHelper::getInstance()->reload();
                owner.contentListBox->updateContent();
                break;
            }
        default:
            break;
        }
    }

private:
    ReadInfoHistory& owner;
    int rowNumber;
    juce::String strBook;
    juce::ScopedPointer<juce::ToggleButton> buttonToggle;
    juce::ScopedPointer<juce::Label> labelTitle;
    juce::ScopedPointer<juce::Label> labelPage;
    juce::ScopedPointer<juce::Label> labelCreateTime;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BookHistoryItem)
};

//////////////////////////////////////////////////////////////////////////
// ReadInfoHistory

ReadInfoHistory::ReadInfoHistory()
    : hasNeedInitData(false)
{
    addAndMakeVisible(titleLabel = new juce::Label(juce::String::empty, L"阅读历史记录"));
    addAndMakeVisible(contentListBox = new juce::ListBox(juce::String::empty, this));

    titleLabel->setFont(juce::Font(30));
    titleLabel->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    titleLabel->setJustificationType(juce::Justification::centredLeft);
    contentListBox->setRowHeight(50);
    contentListBox->setColour(juce::ListBox::ColourIds::backgroundColourId, ProjectInfo::colorLightWhite);
    contentListBox->getViewport()->setScrollBarsShown(true, false, true, false);
    contentListBox->getViewport()->setScrollBarThickness(10);

    buttonDel = new juce::ColorButton;
    buttonDel->setColour(juce::ColorButton::ColourIds::buttonColourNormalId, ProjectInfo::colorLightRed);
    buttonDel->setColour(juce::ColorButton::ColourIds::buttonColourHoverId, ProjectInfo::colorLightRed.contrasting(0.2f));
    buttonDel->setColour(juce::ColorButton::ColourIds::buttonColourDownId, ProjectInfo::colorLightRed.contrasting(0.4f));
    buttonDel->setColour(juce::ColorButton::ColourIds::textColourNormalId, juce::Colours::white);
    buttonDel->setColour(juce::ColorButton::ColourIds::textColourHoverId, juce::Colours::white);
    buttonDel->setColour(juce::ColorButton::ColourIds::textColourDownId, juce::Colours::white);
    buttonDel->setButtonBorder(0);
    buttonDel->setButtonText(L"删除选中记录");
    buttonDel->addListener(this);
    buttonDel->setEnabled(false);
    addAndMakeVisible(buttonDel);

    hoverSelector = new juce::ListBoxMouseHoverSelector(*contentListBox, this);
    HWDocRead::getInstance()->Register(this);
}

ReadInfoHistory::~ReadInfoHistory()
{
    HWDocRead::getInstance()->UnRegister(this);
}

void ReadInfoHistory::resized()
{
    const juce::Rectangle<int> area = getLocalBounds();
    titleLabel->setBounds(area.getX() + 18, area.getY() + 30, 200, 35);
    contentListBox->setBoundsInset(juce::BorderSize<int>(area.getY() + 80, 15, 50, contentListBox->getViewport()->isVerticalScrollBarShown() ? 10 : 15));
    buttonDel->setBounds(area.getCentreX() - 55, area.getBottom() - 40, 110, 30);
}

void ReadInfoHistory::visibilityChanged()
{
    if (isShowing())
        initData();
}

void ReadInfoHistory::initData()
{
    if (hasNeedInitData)
        return;
    
    hasNeedInitData = true;

    contentListBox->updateContent();
}

void ReadInfoHistory::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    switch (reqID)
    {
    case HWReq_Read_DelHistory:
        {
            booksToDelete.clear();
            hasNeedInitData = false;
            initData();
            buttonDel->setEnabled(false);
        }
        break;

    default:
        break;
    }
}

int ReadInfoHistory::getNumRows()
{
    return HWDocRead::getInstance()->GetHistoryCount();
}

void ReadInfoHistory::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
}

void ReadInfoHistory::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    if (BookHistoryItem* item = static_cast<BookHistoryItem*>(contentListBox->getComponentForRowNumber(row)))
    {
        ReadWindowHelper::getInstance()->readBook(item->getBook());
    }
}

void ReadInfoHistory::listBoxRowHover()
{
    contentListBox->repaintRow(hoverSelector->rowNumber);
    contentListBox->repaintRow(hoverSelector->lastRowNumber);
}

juce::Component* ReadInfoHistory::refreshComponentForRow(int rowNumber, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    HWTReadHistoryData* pHisData = HWDocRead::getInstance()->GetHistoryData(rowNumber);
    if (!pHisData) return existingComponentToUpdate;

    juce::Time time(pHisData->tLastTime);   
    BookHistoryItem* item = existingComponentToUpdate ? (static_cast<BookHistoryItem*>(existingComponentToUpdate)) : new BookHistoryItem(*this);
    juce::String str;
    str << L"第" << pHisData->strShowPageNo << L"页";
    item->setContext(rowNumber, pHisData->strBook, time.formatted(L"%Y-%m-%d %H:%M"), str);
    item->setInterceptsMouseClicks(false, true);
    return item;
}

bool ReadInfoHistory::findBookfromDelList(const juce::String& book)
{
    for (unsigned int i=0; i<booksToDelete.size(); ++i)
    {
        if (booksToDelete[i] == book)
            return true;
    }
    return false;
}

void ReadInfoHistory::deleteBookFromDelList(const juce::String& book)
{
    for (unsigned int i=0; i<booksToDelete.size(); ++i)
    {
        if (booksToDelete[i] == book)
        {
            booksToDelete.erase(booksToDelete.begin()+i);
            return;
        }
    }
}

void ReadInfoHistory::buttonClicked(juce::Button*)
{
    if (booksToDelete.empty())
        return;

    HWParaReadDelHistory delHisUI(booksToDelete);
    HWDocRead::getInstance()->SubmitRequest(this, HWReq_Read_DelHistory, &delHisUI, 0);
}

void ReadInfoHistory::historyItemClick(const juce::String& book)
{
    if (findBookfromDelList(book))
    {
        deleteBookFromDelList(book);
    }
    else
    {
        booksToDelete.push_back(book);
    }

    buttonDel->setEnabled(!booksToDelete.empty());
}

void ReadInfoHistory::OnReadHistoryDel()
{
    hasNeedInitData = false;
    if (isShowing())
        initData();
    repaint();
}

void ReadInfoHistory::OnBookOpened()
{
    hasNeedInitData = false;
    if (isShowing())
        initData();
    repaint();
}
