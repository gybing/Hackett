#include "JuceHeader.h"
#include "BookPageCtrl.h"

BookPageCtrl::SelectBlock BookPageCtrl::listSelectRect;			// µ±Ç°Ñ¡ÔñµÄÇøÓò

BookPageCtrl::BookPageCtrl(BookPage& o)
    : owner(o)
    , newSelected(true)
    , willFlashPostil(false)
    , flashPostilZone(-1)
    , flashPostilCount(-1)
    , hoverArea(nullptr)
{
    addAndMakeVisible(buttonMark = new juce::ImageButton);
    buttonMark->addListener(this);
    setButtonImage(false);

}

BookPageCtrl::~BookPageCtrl()
{

}

void BookPageCtrl::resized()
{
    buttonMark->setBounds(getRight() - 70, getY(), 26, 35);
}

void BookPageCtrl::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    if (reqID == HWReq_Sign_Add)
    {
        HWParaSignList signListUI(HWDocRead::getInstance()->GetBook());
        HWDocBookSign::getInstance()->SubmitRequest(this, HWReq_Sign_List, &signListUI, 0);
    }
    else if (HWReq_Text_Get == reqID)
    {
        if (err == HWError_SUCCEED)
        {
            copyTextComplate(ctx);
        }
    }
    else if (reqID == HWReq_Sign_List)
    {
    }
    else if (reqID == HWReq_Dynamic_PageArea)
    {
        if (err == HWError_SUCCEED)
        {
            initPostilArea();
        }
    }
}

void BookPageCtrl::onPageComplate()
{
    pageObject = owner.getPageObject();
    setButtonImage(HWDocBookMark::getInstance()->HasBookMark(owner.getPageNo()));

    postilAreas.clear();

    HWParaPostilPageArea pagePostilUI(HWDocRead::getInstance()->GetBook(), owner.getPageNo());
    HWDocBookPostil::getInstance()->SubmitRequest(this, HWReq_Dynamic_PageArea, &pagePostilUI, 0);
}

void BookPageCtrl::paint(juce::Graphics& g)
{
    if (!pageObject)
        return;

    if (hasSelectResult())
    {
        g.setColour(juce::Colour(0x7f8f8f8f));

        int nX0, nY0, nX1, nY1;
        juce::Rectangle<int> rc;
        FOR_EACH_LIST(SelectLines, listSelectRect)
        {
            SelectLines& sb = *it;

            pageObject->getPageObject()->Transfer(sb.rcBlockX0,sb.rcBlockY0, nX0, nY0);
            pageObject->getPageObject()->Transfer(sb.rcBlockX1,sb.rcBlockY1, nX1, nY1);

            rc.setBounds(juce::jmin(nX0, nX1), juce::jmin(nY0, nY1), abs(nX0 - nX1), abs(nY0 - nY1));
            g.fillRect(rc);
        }
    }

    if (!postilAreas.empty())
    {
        g.setColour(juce::Colours::white);

        FOR_EACH_VECTOR(i, postilAreas)
        {
            PostilArea* pPostilArea = postilAreas[i];
            if (flashPostilCount%2!=0 && pPostilArea == flashPostil)
            {
                continue;
            }
            else 
            {
                if (pPostilArea->nMy > 0)
                {
                    g.setColour((hoverArea == postilAreas[i]) ? ProjectInfo::colorLightRed.contrasting(0.1f) : ProjectInfo::colorLightRed);
                    g.fillRect(pPostilArea->rcArea);
                }
                else
                {
                    g.setColour((hoverArea == postilAreas[i]) ? ProjectInfo::colorBlueBlack.contrasting(0.1f) : ProjectInfo::colorBlueBlack);
                    g.fillRect(pPostilArea->rcArea);
                }

                g.setFont(18);
                g.setColour(juce::Colours::white);
                g.drawFittedText(juce::String(pPostilArea->nMy+pPostilArea->nOther), pPostilArea->rcArea,juce::Justification::centred, 1);                
            }
        }
    }
}

void BookPageCtrl::mouseDrag(const juce::MouseEvent& e)
{
    if (pageObject)
    {
        const juce::MouseEvent e2 (e.getEventRelativeTo (this));
        moveSelectBlock(e2.getMouseDownPosition(), e2.getPosition());
    }
}

void BookPageCtrl::moveSelectBlock(const juce::Point<int>& down, const juce::Point<int>& pos)
{
    if (down.getDistanceFrom(pos) > 10)
    {
        const PageBlockGroup* pPageBlockGroup = pageObject->getPageObject()->LineSelect(down.x, down.y, pos.x, pos.y, newSelected);
        newSelected = false;

        if (!pageObject->getPageObject()->SelectChanged())
            return;

        cleanSelectResult();

        float fX = 0.0f, fY = 0.0f;
        SelectLines sb;
        const PageBlock* pb = NULL;

        for (unsigned int i = 0 ; i < pPageBlockGroup->size(); ++i)
        {
            pb = (*pPageBlockGroup)[i];
            pageObject->getPageObject()->Inverse(pb->x0, pb->y0, fX, fY);
            sb.rcBlockX0 = fX;
            sb.rcBlockY0 = fY;
            pageObject->getPageObject()->Inverse(pb->x1, pb->y1, fX, fY);
            sb.rcBlockX1 = fX;
            sb.rcBlockY1 = fY;

            listSelectRect.push_back(sb);
        }
        if (!listSelectRect.empty())
        {
            // ¼ÇÂ¼ÎÄ±¾
            pageObject->getPageObject()->GetTextScope(listSelectRect.nScopePos, listSelectRect.nScopeNum);
            listSelectRect.nPageNo = owner.getPageNo();
            listSelectRect.nZone = pos.y / (getHeight() / 20);
        }

        repaint();
    }
}

void BookPageCtrl::cleanSelectResult()
{
    listSelectRect.clear();
    listSelectRect.nScopePos = listSelectRect.nScopeNum = listSelectRect.nPageNo = listSelectRect.nZone = 0;
}

bool BookPageCtrl::hasSelectResult() const
{
    return !listSelectRect.empty() && listSelectRect.nPageNo == owner.getPageNo();
}

void BookPageCtrl::mouseDown(const juce::MouseEvent& e)
{
    const juce::MouseEvent e2 (e.getEventRelativeTo (this));
    if (e2.mods.isLeftButtonDown())
    {
        cleanSelectResult();
        repaint();
    }
}

void BookPageCtrl::mouseUp(const juce::MouseEvent& e)
{
    const juce::MouseEvent e2 (e.getEventRelativeTo (this));
    const juce::Point<int> pos = e2.getPosition();
    for (unsigned int i=0; i<postilAreas.size(); ++i)
    {
        if (postilAreas[i]->rcArea.contains(pos))
        {
            clickAreaPostil(postilAreas[i]);
            return;
        }
    }

    if (e2.mods.isLeftButtonDown())
    {
        newSelected = true;
        if (hasSelectResult() && CXOption::getInstance()->GetReadDeskRightMenuAutoPop())
        {
            showMenuSelected();
        }
    }
    else if (e2.mods.isRightButtonDown())
    {
        if (hasSelectResult())
        {
            showMenuSelected();
        }
        else
        {
            showMenuNormal();
        }
    }
}

void BookPageCtrl::showMenuSelected()
{
    juce::PopupMenu m;

    m.addItem(0x00101, L"好词妙句");
    m.addItem(0x00102, L"复制");
    m.addItem(0x00103, L"批注");
    m.addSeparator();
    m.addItem(0x00104, L"搜索");

    class ReadSelectMenuCallBack : public juce::ModalComponentManager::Callback
    {
    public:
        explicit ReadSelectMenuCallBack(BookPageCtrl& m) : content(m) { }

    private:
        virtual void modalStateFinished (int returnValue) override
        {
            switch (returnValue)
            {
            case 0x00101:
                {
                    if (content.hasSelectResult())
                    {
                        HWParaSignAdd signAddUI(HWDocRead::getInstance()->GetBook(), content.owner.getPageNo(), 0xffF56863, listSelectRect.nScopePos, listSelectRect.nScopeNum);
                        HWDocBookSign::getInstance()->SubmitRequest(&content, HWReq_Sign_Add, &signAddUI, 0);
                        content.cleanSelectResult();
                        content.repaint();
                    }
                    break;
                }
                
            case 0x00102:
            case 0x00103:
            case 0x00104:
                {
                    if (content.hasSelectResult())
                    {
                        int nBex = listSelectRect.nScopePos;
                        int nBec = listSelectRect.nScopeNum;

                        HWParaBookTextGet textUI(HWDocRead::getInstance()->GetBook(), content.owner.getPageNo(), nBex, nBec);
                        HWDocBookData::getInstance()->SubmitRequest(&content, HWReq_Text_Get, &textUI, returnValue);
                    }
                    break;
                }
            default:
                break;
            }
        }

        BookPageCtrl& content;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadSelectMenuCallBack)
    };

    m.showMenuAsync(juce::PopupMenu::Options(), new ReadSelectMenuCallBack(*this));
}

void BookPageCtrl::copyTextComplate(int ID)
{
    switch (ID)
    {
    case 0x00102: // 复制文本
        {
            const juce::String* pstrText = HWDocBookData::getInstance()->GetTextSrc();
            if (pstrText && pstrText->isNotEmpty())
            {
                juce::SystemClipboard::copyTextToClipboard(*pstrText);
                juce::AlertWindow::showMessageBox(juce::AlertWindow::InfoIcon, L"复制文本", L"您选择的文字已经放入剪贴板", L"确定");
            }
            break;
        }
    case 0x00103: // 批注
        {
            // ShowWritePostilWnd(HWDocBookData::getInstance()->GetTextSrc());
            break;
        }
    case 0x00104:
        {
            const juce::String* pstrText = HWDocBookData::getInstance()->GetTextSrc();
            if (pstrText && pstrText->isNotEmpty())
            {
                juce::String str;
                str << L"www.baidu.com/s?wd=" << *pstrText;
#ifdef JUCE_WIN32
                ::ShellExecuteW(NULL, L"open", str.toWideCharPointer(), NULL, NULL, SW_SHOWNORMAL);
#elif JUCE_MAC
                // system(str.toWideCharPointer());
#endif
            }
            break;
        }
    default:
        break;
    }
}

void BookPageCtrl::showMenuNormal()
{
    juce::PopupMenu m;

    m.addItem(0x0110, L"平滑字体显示", CXOption::getInstance()->GetReadFontSmooth());
    m.addItem(0x0111, L"锐化字体显示", !CXOption::getInstance()->GetReadFontSmooth());
    m.addSeparator();
    m.addItem(0x0112, CXOption::getInstance()->GetReadCompact() ? L"切换到非紧凑模式" : L"切换到紧凑模式");
    m.addSeparator();
    m.addItem(0x0113, L"单页竖翻模式", CXOption::getInstance()->GetReadBrowserTypeHor());
    m.addItem(0x0114, L"双页横翻模式", !CXOption::getInstance()->GetReadBrowserTypeHor());
    m.addItem(0x0115, L"放大");
    m.addItem(0x0116, L"缩小");
    m.addSeparator();
    if (HWDocTag::getInstance()->GetUserBook(HWDocRead::getInstance()->GetBook()) == nullptr)
    {
        m.addItem(0x0117, L"收藏");
    }
    m.addItem(0x0118, L"批注");
    m.addSeparator();
    m.addItem(0x0119, L"关闭图书");

    class ReadNormalMenuCallBack : public juce::ModalComponentManager::Callback
    {
    public:
        explicit ReadNormalMenuCallBack(BookPageCtrl& m) : content(m) { }

    private:
        virtual void modalStateFinished (int returnValue) override
        {
            switch (returnValue)
            {
            case 0x0110: // 平滑字体显示
            case 0x0111: // 锐化字体显示
                {
                    CXOption::getInstance()->SetReadFontSmooth(!CXOption::getInstance()->GetReadFontSmooth());
                    ReadWindowHelper::getInstance()->reload();
                }
                break;
            case 0x0112: // 切换到紧凑模式
                {
                    CXOption::getInstance()->SetReadCompact(!CXOption::getInstance()->GetReadCompact());
                    ReadWindowHelper::getInstance()->reload();
                }
                break;
            case 0x0113: // 单页竖翻模式
            case 0x0114: // 双页横翻模式
                {
                    if (BookBrowser* browser = ReadWindowHelper::getInstance()->getBookBrowser())
                    {
                        browser->changeBookViewType();
                        if (juce::ToolButton* button = ReadWindowHelper::getInstance()->findReadMenuByID(0x1008))
                        {
                            const bool bHor = CXOption::getInstance()->GetReadBrowserTypeHor();
                            button->setButtonText(bHor ? L"竖翻" : L"横翻");
                        }
                    }
                }
                break;
            case 0x0115: // 放大
                {
                    if (BookView* view = ReadWindowHelper::getInstance()->getBookView())
                    {
                        view->zoomOut();
                    }
                }
                break;
            case 0x0116: // 缩小
                {
                    if (BookView* view = ReadWindowHelper::getInstance()->getBookView())
                    {
                        view->zoomIn();
                    }
                }
                break;
            case 0x0117: // 收藏
                break;
            case 0x0118: // 批注
                {
                    if (juce::ToolButton* b = ReadWindowHelper::getInstance()->findReadMenuByID(0x0111))
                    {
                        if (!b->isVisible())
                        {
                            b->setVisible(true);
                            b->triggerClick();
                        }
                    }
                }
                break;
            case 0x0119: // 关闭图书
                ReadWindowHelper::getInstance()->closeWindow();
                break;
            default:
                break;
            }
        }

        BookPageCtrl& content;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadNormalMenuCallBack)
    };

    m.showMenuAsync(juce::PopupMenu::Options(), new ReadNormalMenuCallBack(*this));
}

void BookPageCtrl::setButtonImage(bool hasMark)
{
    const wchar_t* szImagePath = hasMark ? L"read_desk_book_mark_h.png" : L"read_desk_book_mark_n.png";
    buttonMark->setImages(false, false, false,
        CXOption::getInstance()->GetImage(szImagePath, 0, 3, false), 1.0f, juce::Colours::transparentBlack,
        CXOption::getInstance()->GetImage(szImagePath, 1, 3, false), 1.0f, juce::Colours::transparentBlack,
        CXOption::getInstance()->GetImage(szImagePath, 2, 3, false), 1.0f, juce::Colours::transparentBlack);

}

void BookPageCtrl::buttonClicked(juce::Button* b)
{
    if (b == buttonMark)
    {
        const int pageno = owner.getPageNo();
        const bool hasmark = HWDocBookMark::getInstance()->HasBookMark(owner.getPageNo());
        setButtonImage(!hasmark);
        if (!hasmark)
        {
            HWParaBookMarkAdd addMarkUI(HWDocRead::getInstance()->GetBook(), pageno);
            HWDocBookMark::getInstance()->SubmitRequest(this, HWReq_Mark_Add, &addMarkUI, 0);
        }
        else
        {
            HWParaBookMarkDel delMarkUI(HWDocRead::getInstance()->GetBook(), pageno);
            HWDocBookMark::getInstance()->SubmitRequest(this, HWReq_Mark_Del, &delMarkUI, 0);
        }
    }
}

void BookPageCtrl::initPostilArea()
{
    hoverArea = nullptr;

    std::map<int, std::pair<int,int>>* mapPostilArea = HWDocBookPostil::getInstance()->GetPostilPageArea();
    if (pageObject && mapPostilArea&& !mapPostilArea->empty())
    {
        PageBlock pageBlack = pageObject->getPageObject()->GetPageBlock();
        int nPostilHeight = pageBlack.Height()/20;
        juce::Rectangle<int> rcAreaBase;

        int nPageLeft = 0;
        int nPageRight = getRight();

        
        if (CXOption::getInstance()->GetReadBrowserTypeHor() && owner.getPageNo() % 2 != 0)
        {
            rcAreaBase.setX(nPageRight - 29);
            rcAreaBase.setY(pageBlack.y0 - 14);
            rcAreaBase.setRight(nPageRight-1);
            rcAreaBase.setBottom(pageBlack.y0 + 14);
        }
        else 
        {
            rcAreaBase.setX(nPageLeft + 1);
            rcAreaBase.setY(pageBlack.y0 - 14);
            rcAreaBase.setRight(nPageLeft+29);
            rcAreaBase.setBottom(pageBlack.y0 + 14);
        }

        std::map<int, std::pair<int,int>>::iterator it = mapPostilArea->begin();
        std::map<int, std::pair<int,int>>::iterator itEnd = mapPostilArea->end();
        for (; it != itEnd; ++it)
        {
            int nOffsetY = it->first*nPostilHeight;
            postilAreas.push_back(new PostilArea(it->first, 
                juce::Rectangle<int>(rcAreaBase.getX(), rcAreaBase.getY()+nOffsetY, 
                rcAreaBase.getWidth(), rcAreaBase.getHeight()),
                it->second.first, it->second.second, 0));
        }
    }
    
    repaint();

    if (willFlashPostil)
    {
        willFlashPostil = FALSE;
        for (unsigned int i=0; i<postilAreas.size(); ++i)
        {
            PostilArea* pPostilArea = postilAreas[i];
            if (pPostilArea && pPostilArea->nZone == flashPostilZone)
            {
                flashPostil = pPostilArea;
                flashPostilCount = 0;
                startTimer(500);
                return;
            }
        }
    }
}

void BookPageCtrl::timerCallback()
{
    if (++flashPostilCount > 6)
    {
        stopTimer();
        flashPostilCount =0;
        repaint();
        return;
    }

    repaint();
}

void BookPageCtrl::clickAreaPostil(PostilArea*)
{
    if (juce::ToolButton* b = ReadWindowHelper::getInstance()->findReadMenuByID(0x0110))
    {
        if (!b->isVisible())
        {
            b->setVisible(true);
            b->triggerClick();
        }
    }
}

void BookPageCtrl::mouseMove(const juce::MouseEvent& e)
{
    const juce::MouseEvent e2 (e.getEventRelativeTo (this));
    const juce::Point<int> pos = e2.getPosition();
    for (unsigned int i=0; i<postilAreas.size(); ++i)
    {
        if (postilAreas[i]->rcArea.contains(pos))
        {
            if (hoverArea != postilAreas[i])
            {
                hoverArea = postilAreas[i];
                repaint();
            }
            return;
        }
    }

    mouseExit(e);
}

void BookPageCtrl::mouseExit(const juce::MouseEvent& event)
{
    if (hoverArea != nullptr)
    {
        hoverArea = nullptr;
        repaint();
    }
}
