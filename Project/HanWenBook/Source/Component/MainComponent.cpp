#include "JuceHeader.h"
#include "MainComponent.h"
#include "StoreComponent.h"
#include "ShelfComponent.h"
#include "TrayIconComponent.h"
#include "PopupComponent/MoreHistoryComponent.h"

MainAppWindow* theMainApp = nullptr;

//////////////////////////////////////////////////////////////////////////
//

MainAppWindow::MainAppWindow()
: CXDocumentWindow (juce::JUCEApplication::getInstance()->getApplicationName(),
                    ProjectInfo::colorBlueBlack,
                    0,
                    true, juce::BorderSize<int>(4,4,4,4))
{
    theMainApp = this;
    
    setTitleBarHeight(56);
    setUsingNativeTitleBar (false);
    setResizable (true, true);

    setResizeLimits (ProjectInfo::DefaultFrameMinWidth, ProjectInfo::DefaultFrameMinHeight, ProjectInfo::DefaultFrameMaxWidth, ProjectInfo::DefaultFrameMaxHeight);
    const juce::Point<int> size = CXOption::getInstance()->GetMainWndSize();
    centreWithSize(size.x, size.y);
    menuBar = new MenuComponent;
    setContentOwned(mainContentComponent = new MainComponent(*menuBar), false);
    menuBar->setCallback(this, mainContentComponent);
    setMenuBarComponent(menuBar);
    taskbar = new CXTaskbarComponent;
}

MainAppWindow::~MainAppWindow()
{
    theMainApp = nullptr;
}

void MainAppWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}


//////////////////////////////////////////////////////////////////////////
class MainComponent::AsyncWebItemClick : public juce::AsyncUpdater
{
public:
    AsyncWebItemClick(MainComponent* c, int clickid, const std::vector<juce::String>& ps)
        : comp(c), id(clickid), paras(ps)
    {
    }

private:
    virtual void handleAsyncUpdate() override
    {
        if (comp)
        {
            comp->onAsyncWebItemClick(id, paras);
        }
        
        delete this;
    }

    MainComponent* comp;
    int id;
    std::vector<juce::String> paras;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AsyncWebItemClick)
};

//////////////////////////////////////////////////////////////////////////
//
MainComponent::MainComponent(MenuComponent& menu)
: TabbedComponent (juce::TabbedButtonBar::TabsAtTop)
, menuBar(menu)
{
    setOutline(0);
    setTabBarDepth(0);
    
#ifdef JUCE_WINDOWS
    class ClassComponent : public CEFBrowserControl
    {
        virtual const wchar_t* getDefaultUrl() { return L"http://c8test.hanwenbook.com/?f=classes"; }
        JUCE_LEAK_DETECTOR (ClassComponent)
    };
    class HomeComponent : public CEFBrowserControl
    {
        virtual const wchar_t* getDefaultUrl() { return L"http://c8test.hanwenbook.com/?f=space"; }
        JUCE_LEAK_DETECTOR (HomeComponent)
    };
    class HideComponent : public CEFBrowserControl
    {
        virtual const wchar_t* getDefaultUrl() { return L""; }
        JUCE_LEAK_DETECTOR (HideComponent)
    };
#else if JUCE_MAC
    class ClassComponent : public juce::WebBrowserComponent
    {
    public:
        ClassComponent() : juce::WebBrowserComponent(true)
        {
            goToURL(L"http://c8test.hanwenbook.com/?f=classes");
        }
        JUCE_LEAK_DETECTOR (ClassComponent)
    };
    class HomeComponent : public juce::WebBrowserComponent
    {
    public:
        HomeComponent() : juce::WebBrowserComponent(true)
        {
            goToURL(L"http://c8test.hanwenbook.com/?f=space");
        }
        JUCE_LEAK_DETECTOR (HomeComponent)
    };
    class HideComponent : public juce::WebBrowserComponent
    {
    public:
        HideComponent() : juce::WebBrowserComponent(true)
        {
            goToURL(juce::String::empty);
        }
        JUCE_LEAK_DETECTOR (HideComponent)
    };
    // http://c8test.hanwenbook.com/pcpay.html?i=%s&uf=%s
    // http://c8test.hanwenbook.com/?f=search&k=%s
#endif

    addTab (L" ", juce::Colours::transparentBlack, new StoreComponent, true);
    addTab (L" ", juce::Colours::transparentBlack,  new ShelfComponent, true);
    addTab (L" ", juce::Colours::transparentBlack,  new ClassComponent, true);
    addTab (L" ", juce::Colours::transparentBlack,  new HomeComponent, true);
    addTab (L" ", juce::Colours::transparentBlack,  new HideComponent, true);
    
    requestReadHistoryList();
    requestClassesInfos();
    HWDocRead::getInstance()->Register(this);
}

MainComponent::~MainComponent()
{
    // 发送退出请求。。。
    HWDocAuth::getInstance()->SubmitRequest(this, HWReq_Auth_Logout, NULL, 0,false, false);
    HWDocRead::getInstance()->UnRegister(this);
}

void MainComponent::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    if (reqID == HWReq_Type_Tree)
    {
        
    }
    else if (reqID == HWReq_Read_HistoryList)
    {
        updateSysInfoHistory();
    }
    else if (reqID == HWReq_Book_Info)
    {
        HWParaBookInfo* paras = (HWParaBookInfo*)para;
        const unsigned int index = ctx;
        const HWTBookInfo* bookInfo = HWDocBookData::getInstance()->GetBookInfo();

        if (bookInfo && paras && !vecSysInfoHostory.empty() && index<vecSysInfoHostory.size() &&
            vecSysInfoHostory[index].first == paras->m_strBook)
        {
            vecSysInfoHostory[index].second = bookInfo->strName;

            // 更新阅读历史按钮
            if (index == 0)
            {
                updateReadHistoryButtonInfo();
            }
        }
    }
    else if (reqID == HWReq_School_ClassInfo)
    {
        const HWTClassInfo* pClassInfo = HWDocSchool::getInstance()->GetClassInfo();
        if (pClassInfo)
        {
            vecClassInfos.push_back(*pClassInfo);
            if (vecClassInfos.size() == 2)
            {
                menuBar.buttonClassExp->setVisible(true);
            }
        }
    }
    else if (reqID == HWReq_School_UsrInfo)
    {
        const HWTUserInfo* pUserInfo = HWDocUserData::getInstance()->GetUserInfo();
        if (pUserInfo && pUserInfo->enUserType == HWUserType_Student)
        {
            HWParaClassInfo classInfoUI(pUserInfo->ExtClass());
            HWDocSchool::getInstance()->SubmitRequest(this, HWReq_School_ClassInfo, &classInfoUI, 0);
        }
    }
}

void MainComponent::setTabWebUrl(int idx, juce::String url)
{
#ifdef JUCE_WIN32
    CEFBrowserControl* browser = dynamic_cast<CEFBrowserControl*>(getTabContentComponent(idx));
#elif JUCE_MAC
    juce::WebBrowserComponent* browser = dynamic_cast<juce::WebBrowserComponent*>(getTabContentComponent(idx));
#endif
    if (browser)
    {
        browser->goToURL(url);
    }
}

void MainComponent::requestReadHistoryList()
{
    HWParaReadHistoryList hisListUI;
    HWDocRead::getInstance()->SubmitRequest(this, HWReq_Read_HistoryList, &hisListUI, 0);
}

void MainComponent::updateSysInfoHistory()
{
    vecSysInfoHostory.clear();

    std::vector<HWTReadHistoryData*>* history = HWDocRead::getInstance()->GetHistoryList();
    if (history)
    {
        for (unsigned int i=0; i<5, i<history->size(); ++i)
        {
            vecSysInfoHostory.push_back(std::pair<juce::String,juce::String>((*history)[i]->strBook, L""));
            HWParaBookInfo bookInfo((*history)[i]->strBook);
            HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Book_Info, &bookInfo, i);
        }
    }
}

void MainComponent::OnMainMenuCallBack(int nID)
{
    // search
    if (nID == 4)
    {
        setCurrentTabIndex(nID);
        juce::String search = L"http://c8test.hanwenbook.com/?f=search&k=";
        search << menuBar.searchTextEditor->getText();
        setTabWebUrl(4, search);
        return;
    }

    setCurrentTabIndex(nID);
}

void MainComponent::OnSysBtnCallBack(int nID)
{
    switch (nID)
    {
    case 0:
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
        break;
    case 1:
        theMainApp->setMinimised(true);
        break;
    default:
        break;
    }
}

void MainComponent::OnSysInfoCallBack(int nID)
{
    // message
    if (nID == 1)
    {
        setTabWebUrl(3, L"http://c8test.hanwenbook.com/?f=space&tab=2");
        changeMainTabPage(3);
        return;
    }

    // history
    if (nID == 4)
    {
        if (!vecSysInfoHostory.empty())
        {
            ReadWindowHelper::getInstance()->readBook(vecSysInfoHostory[0].first);
        }
        return;
    }

    OnSysInfoPopupMenu(nID);
}

void MainComponent::OnSysInfoPopupMenu(int nID)
{
    juce::PopupMenu m;
   
    juce::Point<int> pos = getScreenPosition();

    switch (nID)
    {
    case 0: // setting
        {
            m.addItem(0x000011, L"阅读设置");
            m.addItem(0x000012, L"系统设置");
            pos.setX(menuBar.btnInfo[0]->getRight() + pos.getX());
            break;
        }
    case 2: // home
        {
            m.addItem(0x001001, L"退出登录");
            pos.setX(menuBar.btnInfo[2]->getRight() + pos.getX());
            break;
        }
    case 3: // history
        {
            unsigned int ID = 0x010001;
            if (vecSysInfoHostory.size() > 1)
            {
                for (unsigned int i=1; i<6 && i<vecSysInfoHostory.size(); ++i)
                {
                    m.addItem(ID+i, vecSysInfoHostory[i].second);
                }
                m.addSeparator();
            }
            m.addItem(0x011001, L"更多");
            pos.setX(menuBar.btnInfo[3]->getRight() + pos.getX());
            break;
        }
    case 5:
        {
            for (unsigned int i=0; i<vecClassInfos.size(); ++i)
            {
                m.addItem(0x100001+i, vecClassInfos[i].strName);
            }
            pos.setX(menuBar.buttonMenu[2]->getRight() + pos.getX());
            break;
        }
    default:
        return;
    }

    class MainPopupMenuCallBack : public juce::ModalComponentManager::Callback
    {
    public:
        explicit MainPopupMenuCallBack(MainComponent& m)
            : content(m)
        {
        }

    private:
        virtual void modalStateFinished (int returnValue) override
        {
            switch (returnValue)
            {
            case 0x000011: // 阅读设置
                {
                    OptionComponent::showOptionWindow(theMainApp, 0);
                    break;
                }
            case 0x000012: // 系统设置
                {
                    OptionComponent::showOptionWindow(theMainApp, 1);
                    break;
                }
            case 0x001001: // 退出登录
                {
                    LoginWindowHelper::getInstance()->asyncRelogin();
                    break;
                }
            case 0x011001: // 更多阅读历史
                {
                    MoreHistoryComponent::showMoreHistoryWindow(theMainApp);
                    break;
                }
            default:
                {
                    // 点击阅读历史列表项

                    if (returnValue >= 0x010001 && returnValue < 0x011001)
                    {
                        unsigned int IDX = returnValue - 0x010001;
                        if (IDX > 0 && IDX < content.vecSysInfoHostory.size())
                        {
                            ReadWindowHelper::getInstance()->readBook(content.vecSysInfoHostory[IDX].first);
                        }
                        break;
                    }
                    
                    // 点击班级列表项
                    if (returnValue >= 0x100001)
                    {
                        const unsigned int ID = returnValue-0x100001;
                        if (ID < content.vecClassInfos.size())
                        {
                            juce::String strFmt;
                            strFmt << L"http://c8test.hanwenbook.com/?f=classes&classid=" << content.vecClassInfos[ID].nClassid;
                            content.setCurrentTabIndex(2);
                            content.setTabWebUrl(2, strFmt);
                            content.menuBar.buttonMenu[2]->setToggleState(true, juce::dontSendNotification);
                        }
                    }
                    break;
                }
            }
        }

        MainComponent& content;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPopupMenuCallBack)
    };

    
    m.showMenuAsync(juce::PopupMenu::Options().withLeft(true).withTargetScreenArea(juce::Rectangle<int>(pos.x, pos.y, 0, 0)), new MainPopupMenuCallBack(*this));
}

#if 1
void MainComponent::OnWebItemClick(int para, const std::vector<juce::String>& vecString)
{
    if (theMainApp)
    {
        (new AsyncWebItemClick(dynamic_cast<MainComponent*>(theMainApp->getContentComponent()), para, vecString))->triggerAsyncUpdate();
    }
}
#else
jsValue JS_CALL MainComponent::OnWebItemClick(jsExecState es)
{
    std::vector<juce::String> arrParams;

    const int nID = jsToInt(es, jsArg(es, 0));
    const jsValue jvArray = jsArg(es, 1);
    const int nLength = jsGetLength(es, jvArray);

    for (int idx = 0; idx < nLength; ++idx)
    {
        const wchar_t* szParam = jsToStringW(es, jsGetAt(es, jvArray, idx));
        arrParams.push_back(szParam);
    }

    if (theMainApp)
    {
        (new AsyncWebItemClick(dynamic_cast<MainComponent*>(theMainApp->getContentComponent()), nID, arrParams))->triggerAsyncUpdate();
    }
    return jsTrue();
}
#endif

void MainComponent::onAsyncWebItemClick(int id, const std::vector<juce::String>& paras)
{
    switch (id)
    {
    case 6001: // 图书阅读
        {
            if (!paras.empty() && paras[0].isNotEmpty())
            {
                ReadWindowHelper::getInstance()->readBook(paras[0]);
            }
            break;
        }
    case 6008: // 图书收藏
        {
            if (!paras.empty() && paras[0].isNotEmpty())
            {
                // juce::String strBook = paras[0];
            }
            break;
        }

    case 1006:// 1006	弹出图书预览窗口	BookGuid
        {
            if (!paras.empty())
            {
                if (paras.size() == 2 && paras[0].isNotEmpty() && paras[1].isNotEmpty())
                {
                    PreviewComponent::showPreviewWindow(theMainApp, paras[0] /*, paras[1].getIntValue(), CXEnumPreviewClass*/);
                }
                else if (!paras.empty() &&  paras[0].isNotEmpty())
                {
                    juce::String strBook = paras[0];
                    PreviewComponent::showPreviewWindow(theMainApp, strBook /*, CXEnumPreviewStore*/);
                }
            }
            break;
        }

    case 3011:
        {
            if (theMainApp && paras[0].isNotEmpty() && paras[1].isNotEmpty())
            {
                if (MainComponent* comp = static_cast<MainComponent*>(theMainApp->getContentComponent()))
                {
                    juce::String strFmt;
                    strFmt << L"http://c8test.hanwenbook.com/pcpay.html?i=" << paras[0] << L"&uf=" << paras[1];
                    comp->setCurrentTabIndex(4);
                    comp->setTabWebUrl(4, strFmt);
                    comp->menuBar.buttonMenu[4]->setToggleState(true, juce::dontSendNotification);
                }
            }
            break;
        }
    case 3012:
        {
            if (paras.size() == 2 && paras[0].isNotEmpty() && paras[1].isNotEmpty())
            {
                // juce::String strPageNo = paras[1];
                ReadWindowHelper::getInstance()->readBook(paras[0]);
            }

            break;
        }
    case 2001:
        {
            //             if (m_pMainWindow && pArrParam->GetCount() > 1)
            //             {
            //                 m_pMainWindow->GotoUserHome((*pArrParam)[0]);
            //             }
        }
        break;
    case 3013:
        {
            //             if (pArrParam->GetCount() == 2 && !(*pArrParam)[0].IsEmpty() && !(*pArrParam)[1].IsEmpty())
            //             {
            //                 CXBookBuyWindow::Show(NULL, FALSE);
            //                 CXString strFmt;
            //                 strFmt.Format(L"http://c8test.hanwenbook.com/pcpay.html?i=%s&uf=%s", (*pArrParam)[0],(*pArrParam)[1]);
            //                 CXWkeWebkit* pWeb = m_pMainWindow->FindChildByName2<CXWkeWebkit>(L"main_wke_search");
            //                 pWeb->SetAttribute(L"url", strFmt);
            //                 m_pMainWindow->SwitchToTab(4);
            //             }
            break;
        }
    case 3005: // 弹出浏览器
        {
            if (!paras.empty() && paras[0].isNotEmpty())
            {
                //::ShellExecuteW(NULL, L"open", paras[0], NULL, NULL, SW_SHOWNORMAL);
            }
        }
        break;
    case 3006: // 弹出IE浏览器
        {
            if (!paras.empty() && paras[0].isNotEmpty())
            {
                //::ShellExecuteW(NULL, L"open", L"iexplore.exe", paras[0], NULL, SW_SHOWNORMAL);
            }
        }
        break;
    default:
        {
#if DEBUG
            juce::String str, strTitle;
            for (unsigned int i=0; i<paras.size(); ++i)
            {
                str << paras[i];
                str << L", ";
            }

            strTitle << L"web msg: %d" << id;
            juce::AlertWindow::showMessageBox(juce::AlertWindow::InfoIcon, str, strTitle);
#endif
        }
        break;
    }
}

void MainComponent::changeMainTabPage(int nID)
{
    if (nID >= 0 && nID < 5)
    {
        menuBar.buttonMenu[nID]->triggerClick();
    }
}

void MainComponent::updateReadHistoryButtonInfo()
{
    if (!vecSysInfoHostory.empty() && 
        vecSysInfoHostory[0].second.isNotEmpty())
    {
        menuBar.buttonReadHistory->setButtonText(vecSysInfoHostory[0].second);
        menuBar.buttonReadHistory->setSize(menuBar.buttonReadHistory->getBestWidthForHeight(20), 20);
        menuBar.resized();
    }
}

void MainComponent::requestClassesInfos()
{
    vecClassInfos.clear();
    HWTUserInfo* pInfo = HWDocAuth::getInstance()->GetCurrUserInfo();

    if (pInfo->enUserType == HWUserType_Teacher || pInfo->enUserType == HWUserType_TeacherFamily)
    {
        for (unsigned int i=0; i<pInfo->ExtClasses().size(); ++i)
        {
            HWParaClassInfo classInfoUI(pInfo->ExtClasses()[i]);
            HWDocSchool::getInstance()->SubmitRequest(this, HWReq_School_ClassInfo, &classInfoUI, 0);
        }
    }

    if (pInfo->enUserType==HWUserType_TeacherFamily || pInfo->enUserType == HWUserType_Family)
    {
        for (unsigned int i=0; i<pInfo->ExtChildren().size(); ++i)
        {
            HWParaUserInfo userInfoUI(pInfo->ExtChildren()[i]);
            HWDocUserData::getInstance()->SubmitRequest(this, HWReq_School_UsrInfo, &userInfoUI, 0);
        }
    }
}

void MainComponent::OnReadHistoryDel()
{
    updateSysInfoHistory();
}

void MainComponent::OnBookOpened()
{
    updateSysInfoHistory();
}

//////////////////////////////////////////////////////////////////////////

MenuComponent::MenuComponent()
    : menuCallback(nullptr)
{    
    // 主菜单按钮
    const wchar_t* options[] = { L"图书馆", L"书架", L"班级", L"我的", nullptr };
    for (unsigned int i=0; i<4; ++i)
    {
        buttonMenu[i] = new juce::TextButton;
        buttonMenu[i]->setButtonText(options[i]);
        buttonMenu[i]->addListener(this);
        buttonMenu[i]->setRadioGroupId (235);
        buttonMenu[i]->setClickingTogglesState (false);
        addAndMakeVisible(buttonMenu[i]);
    }

    buttonMenu[0]->setToggleState(true, juce::NotificationType::dontSendNotification);
    buttonMenu[2]->addChildComponent (buttonClassExp = new juce::ImageButton);
    buttonClassExp->setImages(true, true, false,
        CXOption::GetImage(L"main_nick_normal.png", 0, 3, true)
        , 1.0f, juce::Colours::transparentWhite,
        CXOption::GetImage(L"main_nick_normal.png", 1, 3, true)
        , 1.0f, juce::Colours::transparentBlack,
        CXOption::GetImage(L"main_nick_normal.png", 2, 3, true)
        , 1.0f, juce::Colours::transparentBlack);
    buttonClassExp->addListener(this);

    // 搜索框
    addChildComponent(buttonMenu[4] = new juce::TextButton);
    buttonMenu[4]->setRadioGroupId(235);
    addAndMakeVisible(searchTextEditor = new juce::TextEditor());
    searchTextEditor->setTextToShowWhenEmpty(L"搜索", juce::Colours::white.contrasting(0.1f));
    searchTextEditor->setFont(juce::Font(18));
    searchTextEditor->setIndents(30, 4);
    searchTextEditor->setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentWhite);
    searchTextEditor->setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::white);
    searchTextEditor->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId, juce::Colours::white);
    searchTextEditor->setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::white);
    searchTextEditor->setColour(juce::TextEditor::ColourIds::shadowColourId, juce::Colours::transparentWhite);
    searchTextEditor->addAndMakeVisible(iconSearch = new juce::ImageComponent, 1);
    searchTextEditor->addListener(this);

    iconSearch->setImage(CXIMAGE_CACHE_LOAD(L"main_search_icon.png"));
    iconSearch->setBounds(9, 9, 12, 12);
    
    const wchar_t* imageName[6] = {
        L"sys_btn_close.png",
        L"sys_btn_minimize.png",
        L"main_setting_3stat.png",
        L"main_message_3stat.png",
        L"main_user_3stat.png",
        L"main_nick_normal.png",
    };
    
    for (unsigned int i=0; i<2; ++i)
    {
        addAndMakeVisible(btnSys[i] = new juce::ImageButton());
        btnSys[i]->setImages(true, true, false,
                             CXOption::GetImage(imageName[i], 0, 3, false)
                             , 1.0f, juce::Colours::transparentWhite,
                             CXOption::GetImage(imageName[i], 1, 3, false)
                             , 1.0f, juce::Colours::transparentBlack,
                             CXOption::GetImage(imageName[i], 2, 3, false)
                             , 1.0f, juce::Colours::transparentBlack);
        btnSys[i]->addListener(this);
    }
    
    for (unsigned int i=0; i<4; ++i)
    {
        addAndMakeVisible(btnInfo[i] = new juce::ImageButton());
        btnInfo[i]->setImages(true, true, false,
                              CXOption::GetImage(imageName[2+i], 0, 3, true)
                              , 1.0f, juce::Colours::transparentWhite,
                              CXOption::GetImage(imageName[2+i], 1, 3, true)
                              , 1.0f, juce::Colours::transparentBlack,
                              CXOption::GetImage(imageName[2+i], 2, 3, true)
                              , 1.0f, juce::Colours::transparentBlack);
        btnInfo[i]->addListener(this);
    }

    buttonReadHistory = new juce::TextButton();
    buttonReadHistory->addListener(this);
    buttonReadHistory->setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white.contrasting(0.2f));
    buttonReadHistory->setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
    addAndMakeVisible(buttonReadHistory);
}

MenuComponent::~MenuComponent()
{
}

void MenuComponent::paint(juce::Graphics& g)
{
    juce::Image image = CXIMAGE_CACHE_LOAD(L"main_logo.png");
    g.setOpacity (1.0f);
    g.drawImageWithin (image, 0, 0, image.getWidth(), image.getHeight(), juce::RectanglePlacement::centred, false);
}

void MenuComponent::resized()
{
    float width = getWidth();
    int btnX = 0.18f * width;
    int margin = juce::jmax(10.0f, 0.0625f * width);

    for (unsigned int i=0; i<4; ++i)
    {
        buttonMenu[i]->setBounds(btnX,13,80,25);
        btnX += margin;
    }
    buttonClassExp->setBounds(buttonMenu[2]->getWidth()-20, (buttonMenu[2]->getHeight()-20.0f)/2.0f, 20, 20);

    margin = juce::jmax(10.0f, 0.03125f * width);
    searchTextEditor->setBounds(btnX + margin,12,180,30);

    btnX = width - 58;
    for (unsigned int i=0; i<2; ++i)
    {
        btnSys[i]->setBounds(btnX, 10, 38, 25);
        btnX -= margin;
    }

    const int searchEditRight = searchTextEditor->getBoundsInParent().getRight();

    for (unsigned int i=0; i<4; ++i)
    {
        btnInfo[i]->setVisible(btnInfo[i]->getX() > searchEditRight);
        btnInfo[i]->setBounds(btnX, 15, 20, 20);
        btnX -= margin;
    }

    btnX += margin + 5;

    const int bestW = buttonReadHistory->getBestWidthForHeight(20);
    const int w = btnX-searchEditRight-10;
    if (w < bestW || buttonReadHistory->getWidth() != bestW)
    {
        buttonReadHistory->setSize(juce::jmax(0,juce::jmin(w, bestW)), 20);
    }

    buttonReadHistory->setTopRightPosition(btnX, 15);
}

void MenuComponent::buttonClicked(juce::Button* btn)
{
    if (menuCallback == nullptr)
        return;
    
    for (unsigned int i=0; i<5; ++i)
    {
        if (btn == buttonMenu[i])
        {
            // 用户有班级列表时特殊处理
            if (i == 2 && buttonClassExp->isVisible())
            {
                menuCallback->OnSysInfoCallBack(5);
                return;
            }

            btn->setToggleState(true, juce::dontSendNotification);
            menuCallback->OnMainMenuCallBack(i);
            return;
        }
    }
    
    for (unsigned int i=0; i<2; ++i)
    {
        if (btn == btnSys[i])
        {
            menuCallback->OnSysBtnCallBack(i);
            return;
        }
    }
    
    for (unsigned int i=0; i<4; ++i)
    {
        if (btn == btnInfo[i])
        {
            menuCallback->OnSysInfoCallBack(i);
            return;
        }
    }

    if (buttonReadHistory == btn)
    {
        menuCallback->OnSysInfoCallBack(4);
    }
    else if (buttonClassExp == btn)
    {
        menuCallback->OnSysInfoCallBack(5);
    }
}

void MenuComponent::textEditorTextChanged (juce::TextEditor&)
{
}

void MenuComponent::textEditorReturnKeyPressed (juce::TextEditor&)
{
    buttonMenu[4]->setToggleState(true, juce::dontSendNotification);
    menuCallback->OnMainMenuCallBack(4);
}

void MenuComponent::setCallback(juce::MouseListener* mouse, MainMenuCallBack* callback)
{
    menuCallback = callback;
    addMouseListener((juce::MouseListener*)mouse, false);
}
