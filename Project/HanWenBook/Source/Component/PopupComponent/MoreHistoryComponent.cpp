#include "JuceHeader.h"
#include "MoreHistoryComponent.h"

class MoreHistoryComponent::MoreHistoryItem
    : public juce::Component
    , public juce::Button::Listener
    , public HWRequestor
{
public:
    MoreHistoryItem(MoreHistoryComponent& r)
        : owner(r)
        , rowNumber(-1)
    {
        addAndMakeVisible(buttonToggle = new juce::ToggleButton);
        addAndMakeVisible(labelTitle = new juce::Label);
        addAndMakeVisible(labelPage = new juce::Label);
        addAndMakeVisible(labelCreateTime = new juce::Label);
        addAndMakeVisible(bookFaceComponent = new juce::ImageComponent);
        bookFaceComponent->setSize(10,10);

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
        labelCreateTime->setJustificationType(juce::Justification::centredLeft);
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

        HWParaBookPic bookPicUI(strBook, BOOKRES_FRONT3);
        HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Pic_Download, &bookPicUI, 0);
    }

    ~MoreHistoryItem()
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
        bookFaceComponent->setBounds(45,8,95,135);
        buttonToggle->setBounds(10, 10, 25, 25);
        labelTitle->setBounds(165, 0, getWidth()-180, 25);
        labelPage->setBounds(165, 25, getWidth()-180, 25);
        labelCreateTime->setBounds(165, 50, getWidth()-180, 25);
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
                const HWParaBookInfo* paras = static_cast<const HWParaBookInfo*>(para);
                if (err == HWError_SUCCEED && paras && paras->m_strBook == strBook)
                {
                    if (const HWTBookInfo* info = HWDocBookData::getInstance()->GetBookInfo())
                    {
                        labelTitle->setText(info->strName, juce::dontSendNotification);
                    }
                }
                break;
            }

        case HWReq_Pic_Download:
            {
                const HWParaBookPic* bookPicUI = static_cast<const HWParaBookPic*>(para);
                juce::Image* pBitmap = HWDocBookData::getInstance()->GetBookImage();
                if (pBitmap && !pBitmap->isNull() && bookFaceComponent && bookPicUI && bookPicUI->m_strBook==strBook)
                {
                    bookFaceComponent->setImage(*pBitmap, juce::RectanglePlacement::yTop | juce::RectanglePlacement::xLeft);
                    repaint();
                }
                break;
            }
        default:
            break;
        }
    }

private:
    MoreHistoryComponent& owner;
    int rowNumber;
    juce::String strBook;
    juce::ScopedPointer<juce::ToggleButton> buttonToggle;
    juce::ScopedPointer<juce::Label> labelTitle;
    juce::ScopedPointer<juce::Label> labelPage;
    juce::ScopedPointer<juce::Label> labelCreateTime;
    juce::ScopedPointer<juce::ImageComponent> bookFaceComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MoreHistoryItem)
};

//////////////////////////////////////////////////////////////////////////
//
MoreHistoryComponent::MoreHistoryComponent()
{
    initDefaultComponent();
    setBounds(0, 0, 646, 460);
    HWDocRead::getInstance()->Register(this);
}

MoreHistoryComponent::~MoreHistoryComponent()
{
    HWDocRead::getInstance()->UnRegister(this);
}

void MoreHistoryComponent::showMoreHistoryWindow(juce::Component* parent)
{
    juce::CXDialogWindow::showDialog(L"阅读历史记录", new MoreHistoryComponent(), parent, true, juce::Justification::centred);
}

void MoreHistoryComponent::paint(juce::Graphics& g)
{
    g.fillAll(ProjectInfo::colorLightWhite);
}

void MoreHistoryComponent::initDefaultComponent()
{
    addAndMakeVisible(contentListBox = new juce::ListBox(juce::String::empty, this));
    contentListBox->setRowHeight(150);
    contentListBox->setColour(juce::ListBox::ColourIds::backgroundColourId, ProjectInfo::colorLightWhite);
    contentListBox->getViewport()->setScrollBarsShown(true, false, true, false);
    contentListBox->getViewport()->setScrollBarThickness(10);
    hoverSelector = new juce::ListBoxMouseHoverSelector(*contentListBox, this);

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
}

void MoreHistoryComponent::resized()
{
    if (contentListBox)
    {
        contentListBox->setBounds(0, 0, getWidth(), getHeight()-50);
        buttonDel->setBounds(515, getHeight() - 40, 110, 30);
    }
}

void MoreHistoryComponent::buttonClicked(juce::Button* b)
{
    if (booksToDelete.empty())
        return;

    HWParaReadDelHistory delHisUI(booksToDelete);
    HWDocRead::getInstance()->SubmitRequest(this, HWReq_Read_DelHistory, &delHisUI, 0);
}

void MoreHistoryComponent::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    switch (reqID)
    {
    case HWReq_Read_DelHistory:
        {
            booksToDelete.clear();
            contentListBox->updateContent();
        }
        break;
    default:
        break;
    }
}

int MoreHistoryComponent::getNumRows()
{
    return HWDocRead::getInstance()->GetHistoryCount();
}

void MoreHistoryComponent::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{

}

void MoreHistoryComponent::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    if (MoreHistoryItem* item = static_cast<MoreHistoryItem*>(contentListBox->getComponentForRowNumber(row)))
    {
        ReadWindowHelper::getInstance()->readBook(item->getBook());
        if (juce::Component* parent = getParentComponent())
            parent->exitModalState (1);
    }
}

void MoreHistoryComponent::listBoxRowHover()
{
    contentListBox->repaintRow(hoverSelector->rowNumber);
    contentListBox->repaintRow(hoverSelector->lastRowNumber);
}

juce::Component* MoreHistoryComponent::refreshComponentForRow(int rowNumber, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    HWTReadHistoryData* pHisData = HWDocRead::getInstance()->GetHistoryData(rowNumber);
    if (!pHisData) return existingComponentToUpdate;

    juce::Time time(pHisData->tLastTime);   
    MoreHistoryItem* item = existingComponentToUpdate ? (static_cast<MoreHistoryItem*>(existingComponentToUpdate)) : new MoreHistoryItem(*this);
    juce::String str;
    str << L"第" << pHisData->strShowPageNo << L"页";
    item->setContext(rowNumber, pHisData->strBook, time.formatted(L"%Y-%m-%d %H:%M"), str);
    item->setInterceptsMouseClicks(false, true);
    return item;

}

void MoreHistoryComponent::OnBookOpened()
{
    contentListBox->updateContent();
}

void MoreHistoryComponent::OnReadHistoryDel()
{
    contentListBox->updateContent();
}

bool MoreHistoryComponent::findBookfromDelList(const juce::String& book)
{
    for (unsigned int i=0; i<booksToDelete.size(); ++i)
    {
        if (booksToDelete[i] == book)
            return true;
    }
    return false;
}

void MoreHistoryComponent::deleteBookFromDelList(const juce::String& book)
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

void MoreHistoryComponent::historyItemClick(const juce::String& book)
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
