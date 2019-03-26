#include "JuceHeader.h"
#include "ReadInfoPostil.h"
#include "../Dynamc/PostilItemComponent.h"

#define RDATA_PAGECOUNT 20
#define DEFAULT_ITEM_HEIGHT 130
#define DEFAULT_REPLY_HEIGHT 130

//////////////////////////////////////////////////////////////////////////
// ReadInfoPostil

ReadInfoPostil::ReadInfoPostil()
    : hasNeedInitData(false)
    , hasMyPostilList(false)
    , otherPostilCount(0)
{
    ListView* listView1 = new ListView(this);
    listView1->getViewport()->setScrollBarsShown(true, false, true, true);
    listView1->getViewport()->setScrollBarThickness(10);
    ListView* listView2 = new ListView(this);
    listView2->getViewport()->setScrollBarsShown(true, false, true, true);
    listView2->getViewport()->setScrollBarThickness(10);

    tabbedComponent = new juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop);
    tabbedComponent->setOutline(0);
    tabbedComponent->setTabBarDepth(0);
    tabbedComponent->addTab (L"我的批注", juce::Colours::transparentBlack, listView1, true);
    tabbedComponent->addTab (L"书友的批注", juce::Colours::transparentBlack, listView2, true);
    addAndMakeVisible(tabbedComponent);

    titleButton1 = new juce::TextButton();
    titleButton1->setButtonText(L"我的批注");
    titleButton1->setColour(juce::TextButton::ColourIds::textColourOnId, ProjectInfo::colorLightBlack);
    titleButton1->setColour(juce::TextButton::ColourIds::textColourOffId, ProjectInfo::colorLightGrey);
    titleButton1->setRadioGroupId (0x01059);
    titleButton1->setClickingTogglesState (false);
    titleButton1->addListener(this);
    titleButton1->setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(titleButton1);

    titleLabel = new juce::Label(juce::String::empty, L"|");
    titleLabel->setFont(juce::Font(30));
    titleLabel->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    titleLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    titleButton2 = new juce::TextButton();
    titleButton2->setButtonText(L"书友的批注");
    titleButton2->setColour(juce::TextButton::ColourIds::textColourOnId, ProjectInfo::colorLightBlack);
    titleButton2->setColour(juce::TextButton::ColourIds::textColourOffId, ProjectInfo::colorLightGrey);
    titleButton2->setRadioGroupId (0x01059);
    titleButton2->setClickingTogglesState (false);
    titleButton2->addListener(this);
    addAndMakeVisible(titleButton2);

    HWDocRead::getInstance()->Register(this);
}

ReadInfoPostil::~ReadInfoPostil()
{
    HWDocRead::getInstance()->UnRegister(this);
}

void ReadInfoPostil::resized()
{
    const juce::Rectangle<int> area = getLocalBounds();
    titleButton1->setBounds(area.getX() + 18, area.getY() + 30, 120, 35);
    titleButton2->setBounds(area.getX() + 138, area.getY() + 30, 150, 35);
    titleLabel->setBounds(area.getX() + 133, area.getY() + 30, 15, 35);
    tabbedComponent->setBoundsInset(juce::BorderSize<int>(area.getY() + 80, 15, 15, 15));
}

void ReadInfoPostil::visibilityChanged()
{
    if (isShowing())
        initData();
}

void ReadInfoPostil::initData()
{
    if (hasNeedInitData && strBook == HWDocRead::getInstance()->GetBook())
        return;
    
    strBook = HWDocRead::getInstance()->GetBook();
    hasNeedInitData = true;

    requestPostilList(true, 1);
    requestPostilList(false, 1);
}

void ReadInfoPostil::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    if (reqID == HWReq_Dynamic_BookNote)
    {
        if (err != HWError_SUCCEED) return;

        otherPostilCount = HWDocDynamic::getInstance()->GetBookNoteCount();
        if (ListView* listView = static_cast<ListView*>(tabbedComponent->getTabContentComponent(1)))
        {
            std::vector<std::pair<int,int>> vecIds;
            HWDocDynamic::getInstance()->GetBookNoteList(vecIds);
            for (unsigned int i=0; i<vecIds.size(); ++i)
            {
                BookPostilItem* postilItem = new BookPostilItem(*listView);
                listView->addItem(postilItem);
                postilItem->setContent(false, vecIds[i].first, true);
            }
        }
        mapPageOther[ctx] = true;
    }
    else if (reqID == HWReq_Postil_List)
    {
        if (err != HWError_SUCCEED) return;

        std::vector<HWTBookPostil*>* pVec = HWDocBookPostil::getInstance()->GetPostilList();
        if (pVec && !pVec->empty())
        {
            if (ListView* listView = static_cast<ListView*>(tabbedComponent->getTabContentComponent(1)))
            {
                listView->clearItems();
                FOR_EACH_VECTOR(i, (*pVec))
                {
                    BookPostilItem* postilItem = new BookPostilItem(*listView);
                    listView->addItem(postilItem);
                    postilItem->setContent(true, (*pVec)[i]->nID, true);
                }
            }
        }
    }
}

void ReadInfoPostil::OnBookOpened()
{
    hasNeedInitData = false;
    if (isShowing())
        initData();
}

void ReadInfoPostil::buttonClicked(juce::Button* btn)
{
    btn->setToggleState(true, juce::dontSendNotification);
    if (btn == titleButton1)
    {
        tabbedComponent->setCurrentTabIndex(0);
    }
    else
    {
        tabbedComponent->setCurrentTabIndex(1);
    }
}

void ReadInfoPostil::requestPostilList(bool my, int begin)
{
    if (my)
    {
        if (!hasMyPostilList)
        {
            hasMyPostilList = true;
            HWParaBookPostilList dycListUI(HWDocRead::getInstance()->GetBook());
            HWDocBookPostil::getInstance()->SubmitRequest(this, HWReq_Postil_List, &dycListUI, 0);
        }
    }
    else
    {
        if (mapPageOther.find(begin/RDATA_PAGECOUNT) == mapPageOther.end())
        {
            HWParaBookNoteList dycListUI(HWDocRead::getInstance()->GetBook(), HWDyicType_Postil, begin, RDATA_PAGECOUNT, FALSE);
            HWDocDynamic::getInstance()->SubmitRequest(this, HWReq_Dynamic_BookNote, &dycListUI, (begin/RDATA_PAGECOUNT));
        }
    }
}

void ReadInfoPostil::listWasScrolled()
{
    if ( tabbedComponent->getCurrentTabIndex() == 1 )
    {
        if (ListView* listView = static_cast<ListView*>(tabbedComponent->getCurrentContentComponent()))
        {
            const juce::Rectangle<int> b = listView->getViewport()->getViewedComponent()->getBounds();
            const int pos = listView->getViewport()->getViewPositionY() + listView->getViewport()->getMaximumVisibleHeight();
            if (pos + 5 >= b.getHeight())
            {
                int count = listView->getNumItems();
                if (count < otherPostilCount)
                {
                    requestPostilList(false, count+1);
                }
            }
        }
    }
}
