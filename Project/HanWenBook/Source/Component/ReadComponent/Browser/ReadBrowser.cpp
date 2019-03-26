#include "JuceHeader.h"
#include "ReadBrowser.h"
#include "BookViewVer.h"
#include "BookViewHor.h"

//////////////////////////////////////////////////////////////////////////
//
BookView::BookView()
{
}

BookView::~BookView()
{   
}

//////////////////////////////////////////////////////////////////////////
//
BookBrowser::BookBrowser()
{
    setBookViewType(CXOption::getInstance()->GetReadBrowserTypeHor());
    addChildComponent(readInfoContent = new ReadInfoComponent());

    HWDocRead::getInstance()->Register(this);
}

BookBrowser::~BookBrowser()
{
    HWDocRead::getInstance()->UnRegister(this);
    CXOption::getInstance()->SetReadExpandShow(false);
}

void BookBrowser::paint(juce::Graphics& g)
{
    g.fillAll(ProjectInfo::colorDarkLightGray);
}

void BookBrowser::resized()
{
    juce::Rectangle<int> area = getLocalBounds();
    if (CXOption::getInstance()->GetReadExpandShow())
    {
        readInfoContent->setBounds(juce::Rectangle<int>(area.getX(), area.getY(), 410, area.getHeight()).reduced(0, 2));
        bookView->setBounds(juce::Rectangle<int>(area.getX() + 410, area.getY(), getWidth() - 410, area.getHeight()).reduced(10, 5));
    }
    else
    {
        readInfoContent->setBounds(juce::Rectangle<int>(area.getX(), area.getY(), 0, area.getHeight()).reduced(0, 2));
        bookView->setBounds(area.reduced(10, 5));
    }
}

void BookBrowser::onVToolMenuItemClicked(juce::VerticalToolMenu* menu, juce::Button* b)
{
    const juce::String btnID = b->getComponentID();
    if (btnID.isEmpty())
        return;

    const int ID = btnID.getIntValue();

    // right
    if (ID > 0x1001 && ID < 0x1110)
    {
        switch (btnID.getIntValue())
        {
        case 0x1002: bookView->zoomIn(); break;
        case 0x1003: bookView->zoomOut(); break;
        case 0x1004: bookView->pageNext(); break;
        case 0x1005: bookView->pagePrev(); break;
        case 0x1006:
            {
                if (juce::CXDocumentWindow* w = findParentComponentOfClass<juce::CXDocumentWindow>())
                {
                    if (b->getToggleState())
                    {
                        w->setBorderThickness(juce::BorderSize<int>(4,4,4,4));
                        w->setTitleBarHeight(31);
                        b->setToggleState(false, false);
                    }
                    else
                    {
                        w->setBorderThickness(juce::BorderSize<int>(0,0,0,0));
                        w->setTitleBarHeight(0);
                        b->setToggleState(true, false);
                    }
                }
                
                break;
            }
        case 0x1007: 
            {
                const bool protect = !CXOption::getInstance()->GetBookPageBackColor();
                bookView->setProtectEye(protect); 
                b->setToggleState(protect, false);
                break;
            }
        case 0x1008:
            {
                changeBookViewType();

                const bool bHor = CXOption::getInstance()->GetReadBrowserTypeHor();
                b->setButtonText(bHor ? L"竖翻" : L"横翻");
                break;
            }
        default:
            break;
        }
    }

    // left : 0x101+
    if (ID > 0x0101 && ID < 0x1001)
    {
        // 写批注\区域批注特殊处理
        if (b->getToggleState())
        {
            if (ID == 0x0110 || ID == 0x0111)
            {
                b->setVisible(false);
            }
        }
        else
        {
            juce::Component* btn = menu->getItemByID(0x0110);
            if (btn && btn->isVisible() && ID != 0x0110)
            {
                btn->setVisible(false);
            }

            btn = menu->getItemByID(0x0111);
            if (btn && btn->isVisible() && ID != 0x0111)
            {
                btn->setVisible(false);
            }
        }

        bool showExpand;
        b->setToggleState(showExpand = !b->getToggleState(), false);
        readInfoContent->setCurrentTabIndex(ID-0x0101-1);
        menu->showItemEdge(showExpand ? juce::String(ID) : juce::String::empty);
        showExpandWindow(showExpand);

        return;
    }

    // collect
    if (ID == 0x1110)
    {

    }
}

void BookBrowser::showExpandWindow(bool show)
{
    readInfoContent->setVisible(show);
    bookView->setExpandShow(show);
    resized();
//     juce::Rectangle<int> area = getLocalBounds();
//     juce::ComponentAnimator& ani = juce::Desktop::getInstance().getAnimator();
//     if (show)
//     {
//         ani.animateComponent(readInfoContent, juce::Rectangle<int>(area.getX(), area.getY(), 410, area.getHeight()).reduced(0, 2), 1.0f, 200, true, 1.0f, 1.0f);
//         ani.animateComponent(bookView, juce::Rectangle<int>(area.getX() + 410, area.getY(), getWidth() - 410, area.getHeight()).reduced(10, 2), 1.0f, 200, true, 1.0f, 1.0f);
//     }
//     else
//     {
//         ani.animateComponent(readInfoContent, juce::Rectangle<int>(area.getX(), area.getY(), 0, area.getHeight()).reduced(0, 2), 1.0f, 200, true, 1.0f, 1.0f);
//         ani.animateComponent(bookView, area.reduced(10, 2), 1.0f, 200, true, 1.0f, 1.0f);
//     }
}

BookView* BookBrowser::getBookView()
{
    return bookView.get();
}

void BookBrowser::changeBookViewType()
{
    setBookViewType(!CXOption::getInstance()->GetReadBrowserTypeHor());
}

void BookBrowser::setBookViewType(bool bHor)
{
    bool needInitView = (bookView != nullptr);

    if (bHor)
    {
        addAndMakeVisible(bookView = new BookViewHor());
    }
    else
    {
        addAndMakeVisible(bookView = new BookViewVer());
    }

    if (needInitView)
    {
        resized();
    }

    CXOption::getInstance()->SetReadBrowserTypeHor(bHor);
}

void BookBrowser::OnBookOpened()
{
    bookView->update();
    bookView->gotoPage(HWDocRead::getInstance()->GetHistoryPageNo());

    updateCollectButtonState();
}

void BookBrowser::updateCollectButtonState()
{
    const bool hasNotBook = HWDocTag::getInstance()->GetUserBook(HWDocRead::getInstance()->GetBook()) == nullptr;
    if (juce::ToolButton* b = ReadWindowHelper::getInstance()->findReadMenuByID(0x1110))
    {
        b->setVisible(hasNotBook);
        if (juce::VerticalToolMenu* menu = static_cast<juce::VerticalToolMenu*>(b->getParentComponent()))
        {
            menu->resized();
        }
    }
}

