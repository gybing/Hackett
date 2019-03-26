#include "JuceHeader.h"
#include "ReadComponent.h"
#include "Browser/ReadBrowser.h"

ReadAppWindow::ReadAppWindow()
: CXDocumentWindow (juce::JUCEApplication::getInstance()->getApplicationName(),
                    ProjectInfo::colorBlueBlack,
                    CXDocumentWindow::closeButton|CXDocumentWindow::minimiseButton,
                    true, juce::BorderSize<int>(4,4,4,4))
{
    setTitleBarHeight(31);
    setUsingNativeTitleBar (false);
    setResizable (true, true);
    //setFullScreen(true);
    m_pContent = new ReadComponent(this);
    setContentOwned(m_pContent, false);
}

ReadAppWindow::~ReadAppWindow()
{
}

void ReadAppWindow::closeButtonPressed()
{
    ReadWindowHelper::getInstance()->closeWindow();
}

ReadComponent* ReadAppWindow::getContent()
{
    return m_pContent;
}

ReadComponent::ReadComponent(juce::CXDocumentWindow* window)
    : bookInfoExpendIsShow(false)
{
    addAndMakeVisible(bookBrowser = new BookBrowser);
    addAndMakeVisible(toolMenuLeft = new juce::VerticalToolMenu(bookBrowser));
    addAndMakeVisible(toolMenuRight = new juce::VerticalToolMenu(bookBrowser));

    createLeftToolMenu();
    createRightToolMenu();
}

ReadComponent::~ReadComponent()
{
}

void ReadComponent::buttonClicked(juce::Button* button)
{
}

void ReadComponent::paint(juce::Graphics& g)
{
}

void ReadComponent::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    toolMenuLeft->setBounds (bounds.getX(), bounds.getY(), 50, bounds.getHeight());
    toolMenuRight->setBounds (bounds.getRight()-50, bounds.getY(), 50, bounds.getHeight());
    bookBrowser->setBounds(bounds.getX()+50, bounds.getY(), bounds.getWidth()-100, bounds.getHeight());
}

void ReadComponent::createLeftToolMenu()
{
    toolMenuLeft->appand(true, 0x0102, L"目录",
        CXOption::GetImage(L"read_b_r_c.png", 0, 3, false),
        CXOption::GetImage(L"read_b_r_c.png", 2, 3, false),
        CXOption::GetImage(L"read_b_r_c.png", 1, 3, false))->setRadioGroupId(0x10101);
    toolMenuLeft->appand(true, 0x0103, L"简介",
        CXOption::GetImage(L"read_b_l_i.png", 0, 3, false),
        CXOption::GetImage(L"read_b_l_i.png", 2, 3, false),
        CXOption::GetImage(L"read_b_l_i.png", 1, 3, false))->setRadioGroupId(0x10101);
    toolMenuLeft->appand(true, 0x0104, L"书签",
        CXOption::GetImage(L"read_b_r_m.png", 0, 3, false),
        CXOption::GetImage(L"read_b_r_m.png", 2, 3, false),
        CXOption::GetImage(L"read_b_r_m.png", 1, 3, false))->setRadioGroupId(0x10101);
    toolMenuLeft->appand(true, 0x0105, L"批注",
        CXOption::GetImage(L"read_b_r_p.png", 0, 3, false),
        CXOption::GetImage(L"read_b_r_p.png", 2, 3, false),
        CXOption::GetImage(L"read_b_r_p.png", 1, 3, false))->setRadioGroupId(0x10101);
    toolMenuLeft->appand(true, 0x0106, L"书评",
        CXOption::GetImage(L"read_b_l_p.png", 0, 3, false),
        CXOption::GetImage(L"read_b_l_p.png", 2, 3, false),
        CXOption::GetImage(L"read_b_l_p.png", 1, 3, false))->setRadioGroupId(0x10101);
    toolMenuLeft->appand(true, 0x0107, L"好词\n妙句",
        CXOption::GetImage(L"read_b_r_s.png", 0, 3, false),
        CXOption::GetImage(L"read_b_r_s.png", 2, 3, false),
        CXOption::GetImage(L"read_b_r_s.png", 1, 3, false))->setRadioGroupId(0x10101);
    toolMenuLeft->appand(true, 0x0108, L"历史\n记录",
        CXOption::GetImage(L"read_b_l_h.png", 0, 3, false),
        CXOption::GetImage(L"read_b_l_h.png", 2, 3, false),
        CXOption::GetImage(L"read_b_l_h.png", 1, 3, false))->setRadioGroupId(0x10101);
    toolMenuLeft->appand(true, 0x0109, L"全文\n搜索", 
        CXOption::GetImage(L"read_b_l_s.png", 0, 3, false),
        CXOption::GetImage(L"read_b_l_s.png", 2, 3, false),
        CXOption::GetImage(L"read_b_l_s.png", 1, 3, false))->setRadioGroupId(0x10101);

    juce::Button* button =  toolMenuLeft->appand(true, 0x0110, L"区域\n批注", 
        CXOption::GetImage(L"read_b_r_p.png", 0, 3, false),
        CXOption::GetImage(L"read_b_r_p.png", 2, 3, false),
        CXOption::GetImage(L"read_b_r_p.png", 1, 3, false));
    button->setRadioGroupId(0x10101);
    button->setVisible(false);
    button =  toolMenuLeft->appand(true, 0x0111, L"写批注", 
        CXOption::GetImage(L"read_b_r_p.png", 0, 3, false),
        CXOption::GetImage(L"read_b_r_p.png", 2, 3, false),
        CXOption::GetImage(L"read_b_r_p.png", 1, 3, false));
    button->setRadioGroupId(0x10101);
    button->setVisible(false);
    juce::ColorButton* btnCollect = new juce::ColorButton;
    btnCollect->setColour(juce::ColorButton::ColourIds::buttonBackgroundId, juce::Colours::transparentBlack);
    btnCollect->setColour(juce::ColorButton::ColourIds::buttonColourNormalId, ProjectInfo::colorLightBlue);
    btnCollect->setColour(juce::ColorButton::ColourIds::buttonColourHoverId, ProjectInfo::colorLightBlue.contrasting(0.2f));
    btnCollect->setColour(juce::ColorButton::ColourIds::buttonColourDownId, ProjectInfo::colorLightBlue.contrasting(0.4f));
    btnCollect->setColour(juce::ColorButton::ColourIds::textColourNormalId, juce::Colours::white);
    btnCollect->setColour(juce::ColorButton::ColourIds::textColourHoverId, juce::Colours::white);
    btnCollect->setColour(juce::ColorButton::ColourIds::textColourDownId, juce::Colours::white);
    btnCollect->setButtonBorder(0);
    btnCollect->setButtonText(L"收\n藏");
    btnCollect->setBounds(10,0,30,80);
    toolMenuLeft->appand(false, 0x1110, btnCollect);
    const bool hasNotBook = HWDocTag::getInstance()->GetUserBook(HWDocRead::getInstance()->GetBook()) == nullptr;
    btnCollect->setVisible(hasNotBook);
}

void ReadComponent::createRightToolMenu()
{
    toolMenuRight->appand(false, 0x1002, L"缩小",
        CXOption::GetImage(L"read_b_r_zi.png", 0, 3, false),
        CXOption::GetImage(L"read_b_r_zi.png", 2, 3, false),
        CXOption::GetImage(L"read_b_r_zi.png", 1, 3, false));

    toolMenuRight->appand(false, 0x1003, L"放大",
        CXOption::GetImage(L"read_b_r_zo.png", 0, 3, false),
        CXOption::GetImage(L"read_b_r_zo.png", 2, 3, false),
        CXOption::GetImage(L"read_b_r_zo.png", 1, 3, false));
    toolMenuRight->appand(false);

    juce::ImageButton* btnNext = new juce::ImageButton;
    btnNext->setImages(false, false, false, 
        CXOption::GetImage(L"read_desk_page_next.png",0, 3, true), 1.0f, juce::Colours::transparentBlack,
        CXOption::GetImage(L"read_desk_page_next.png",1, 3, true), 1.0f, juce::Colours::transparentBlack,
        CXOption::GetImage(L"read_desk_page_next.png",2, 3, true), 1.0f, juce::Colours::transparentBlack);
    btnNext->setBounds(10,0,30,100);
    toolMenuRight->appand(false, 0x1004, btnNext);
    juce::ImageButton* btnPrev = new juce::ImageButton;
    btnPrev->setImages(false, false, false, 
        CXOption::GetImage(L"read_desk_page_prev.png",0, 3, true), 1.0f, juce::Colours::transparentBlack,
        CXOption::GetImage(L"read_desk_page_prev.png",1, 3, true), 1.0f, juce::Colours::transparentBlack,
        CXOption::GetImage(L"read_desk_page_prev.png",2, 3, true), 1.0f, juce::Colours::transparentBlack);
    btnPrev->setBounds(10,0,30,100);
    toolMenuRight->appand(false, 0x1005, btnPrev);

    toolMenuRight->appand(false, 0x1006, L"全屏", 
        CXOption::GetImage(L"read_b_r_sf.png", 0, 3, false),
        CXOption::GetImage(L"read_b_r_sf.png", 2, 3, false),
        CXOption::GetImage(L"read_b_r_sf.png", 1, 3, false));
    
    toolMenuRight->appand(false, 0x1007, L"护眼",
        CXOption::GetImage(L"read_b_l_eye.png", 0, 3, false),
        CXOption::GetImage(L"read_b_l_eye.png", 2, 3, false),
        CXOption::GetImage(L"read_b_l_eye.png", 1, 3, false))->setToggleState(CXOption::getInstance()->GetBookPageBackColor(), false);

    toolMenuRight->appand(false, 0x1008, L"横翻",
        CXOption::GetImage(L"read_b_r_f.png", 0, 3, false),
        CXOption::GetImage(L"read_b_r_f.png", 2, 3, false),
        CXOption::GetImage(L"read_b_r_f.png", 1, 3, false));
    
    toolMenuRight->appand(false, 0x1009, L"评分",
        CXOption::GetImage(L"read_b_l_r.png", 0, 3, false),
        CXOption::GetImage(L"read_b_l_r.png", 2, 3, false),
        CXOption::GetImage(L"read_b_l_r.png", 1, 3, false));
}

BookBrowser* ReadComponent::getReadBrowser()
{
    return bookBrowser.get();
}

juce::VerticalToolMenu* ReadComponent::getLeftToolMenu()
{
    return toolMenuLeft;
}

juce::VerticalToolMenu* ReadComponent::getRightToolMenu()
{
    return toolMenuRight;
}

//////////////////////////////////////////////////////////////////////////
// ReadWindowHelper
juce_ImplementSingleton(ReadWindowHelper)

ReadWindowHelper::ReadWindowHelper()
{
    
}

ReadWindowHelper::~ReadWindowHelper()
{
    
}

void ReadWindowHelper::showWindow()
{
    if (!readContentComponent)
    {
        readContentComponent = new ReadAppWindow;
    }
    
    readContentComponent->setVisible(true);
    readContentComponent->setFullScreen(true);
    readContentComponent->toFront(true);
}

void ReadWindowHelper::closeWindow()
{
    if (readContentComponent)
    {
        readContentComponent->setVisible(false);
        readContentComponent = nullptr;
    }
}

ReadAppWindow* ReadWindowHelper::getWindow()
{
    return readContentComponent.get();
}

void ReadWindowHelper::setMinimised(bool bmin)
{
    if (readContentComponent)
    {
        readContentComponent->setMinimised(bmin);
    }
}

void ReadWindowHelper::readBook(const juce::String& strBook, int pageno /*= ProjectInfo::PagenoInvalid*/)
{
    HWParaOpenBook pParam(strBook);
    HWDocRead::getInstance()->SubmitRequest(this, HWReq_Read_OpenBook, &pParam, pageno);
}

void ReadWindowHelper::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err,
                                  HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    if (reqID == HWReq_Read_OpenBook)
    {
        if (err == HWError_SUCCEED)
        {
            if (HWDocRead::getInstance()->IsValidBookData())
            {
                showWindow();

                // 页跳转
                if (ctx != ProjectInfo::PagenoInvalid)
                {
                    gotoPage(ctx);
                }

                HWParaBookInfo bookInfoUI(HWDocRead::getInstance()->GetBook());
                HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Book_Info, &bookInfoUI, 0);

                HWParaBookMarkList bookMarkListUI(HWDocRead::getInstance()->GetBook());
                HWDocBookMark::getInstance()->SubmitRequest(this, HWReq_Mark_List, &bookMarkListUI, 0);

                HWParaSignList signListUI(HWDocRead::getInstance()->GetBook());
                HWDocBookSign::getInstance()->SubmitRequest(this, HWReq_Sign_List, &signListUI, 0);

            }
            else
            {
                juce::AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::WarningIcon,
                                                  L"图书数据错误", L"打开图书");
            }
        }
        else if (err==HWError_CUSTOMINFO && busi == HWBUSI_NO_READ_POWER)
        {
            juce::AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::InfoIcon,
                                              L"无阅读权限，请购买会员", L"购买会员");
        }
    }
    else if (HWReq_Book_Info == reqID)
    {
        if (err == HWError_SUCCEED)
        {
            const HWTBookInfo* bookInfo = HWDocBookData::getInstance()->GetBookInfo();
            if (bookInfo)
            {
                readContentComponent->setTitle(bookInfo->strName);
            }
        }
    }
}

void ReadWindowHelper::gotoPage(int pageno)
{
    if (BookView* view = getBookView())
    {
        view->gotoPage(pageno);
    }
}

void ReadWindowHelper::reload()
{
    if (BookView* view = getBookView())
    {
        view->reload();
    }
}

BookBrowser* ReadWindowHelper::getBookBrowser()
{
    if (!HWDocRead::getInstance()->IsBookOpened())
        return nullptr;

    if (readContentComponent == nullptr)
        return nullptr;

    ReadComponent* comp = readContentComponent->getContent();
    if (!comp)
        return nullptr;

    return comp->getReadBrowser();
}

BookView* ReadWindowHelper::getBookView()
{
    BookBrowser* browser = getBookBrowser();
    if (!browser)
        return nullptr;

    return browser->getBookView();
}

juce::ToolButton* ReadWindowHelper::findReadMenuByID(int id)
{
    juce::ToolButton* button = nullptr;

    if (readContentComponent == nullptr)
        return nullptr;

    ReadComponent* c = readContentComponent->getContent();
    if (c == nullptr)
        return button;

    juce::VerticalToolMenu* tool = c->getLeftToolMenu();
    if (tool == nullptr)
        return button;

    button = static_cast<juce::ToolButton*>(tool->getItemByID(id));
    if (button)
        return button;
    
    tool = c->getRightToolMenu();
    if (tool == nullptr)
        return button;

    button = static_cast<juce::ToolButton*>(tool->getItemByID(id));
    if (button)
        return button;

    return button;
}
