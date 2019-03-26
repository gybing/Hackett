#include "JuceHeader.h"
#include "PreviewComponent.h"

PreviewComponent::PreviewComponent(const juce::String& strBook)
    : m_strBook(strBook)
{
    setBounds(0, 0, 800,500);
    initDefaultComponent();
    RequestBookInfo();
}

PreviewComponent::~PreviewComponent()
{
}

void PreviewComponent::showPreviewWindow(juce::Component* parent, const juce::String& strBook)
{
    if (strBook.isEmpty())
        return;

    juce::CXDialogWindow::showDialog(juce::String::empty, new PreviewComponent(strBook), parent, true, juce::Justification::centredLeft);
}

void PreviewComponent::paint(juce::Graphics& g)
{
    g.fillAll(ProjectInfo::colorLightWhite);

    // 书名
    if (m_strBookName.isNotEmpty())
    {
        g.setFont(juce::Font(26));
        g.setColour(ProjectInfo::colorLightBlack);
        g.drawText(m_strBookName, 210, 15, getWidth() - 200, 30, juce::Justification::centredLeft);
    }

    juce::Rectangle<float> rcDraw(m_rcBookInfo.getX(), m_rcBookInfo.getY(), m_rcBookInfo.getWidth(), 20);
    for (unsigned int i=0; i<6; ++i)
    {
        rcDraw.setY(rcDraw.getY() + 30);
        m_attrBookInfo[i].draw(g, rcDraw);
    }
}

void PreviewComponent::RequestBookInfo()
{
    if (m_strBook.isEmpty())
        return;

    // 图书信息
    HWParaBookInfo bookInfoUI(m_strBook);
    HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Book_Info, &bookInfoUI, 0);

    // 图书图片
    HWParaBookPic bookPicUI(m_strBook, BOOKRES_FRONT2);
    HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Pic_Download, &bookPicUI, 0);

    // 图书评分
    HWParaBookScoreGet bookScoreUI(m_strBook);
    HWDocBookData::getInstance()->SubmitRequest(this, HWReq_BookScore_Get, &bookScoreUI, 0);

    HWParaBookStat bookStat(m_strBook);
    HWDocSpace::getInstance()->SubmitRequest(this, HWReq_Space_BookStatistics, &bookStat, 0);

    RequestBookBrief();
}

void PreviewComponent::RequestBookBrief()
{
    // 图书简介
    HWParaBookRes bookResUI(m_strBook);
    HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Book_Res, &bookResUI, 0);
}

void PreviewComponent::bookFaceClicked(juce::Button* button)
{
    juce::ImageButton* btn = static_cast<juce::ImageButton*>(button);
    if (btn)
    {
        class LargeBookFaceComponent : public juce::ImageComponent, public HWRequestor
        {
        public:
            LargeBookFaceComponent(juce::Image img, const juce::String& strBook)
            {
                if (!img.isNull())
                {
                    setImage(img);
                    const float fStep = img.getWidth()*1.0f / img.getHeight()*1.0f;
                    const int height = CXOption::getInstance()->GetMainWndSize().getY();
                    setBounds(0,0, fStep * height, height);
                }

                HWParaBookPic bookPicUI(strBook, BOOKRES_FRONT0);
                HWDocBookData::getInstance()->SubmitRequest(this, HWReq_Pic_Download, &bookPicUI, 1);
            }
        private:
            virtual void paint(juce::Graphics& g) override
            {
                g.fillAll(ProjectInfo::colorLightWhite);
                juce::ImageComponent::paint(g);
            }

            virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err,
                HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
            {
                if (reqID == HWReq_Pic_Download && err == HWError_SUCCEED) // bookface
                {
                    juce::Image* pBitmap = HWDocBookData::getInstance()->GetBookImage();
                    if (pBitmap && !pBitmap->isNull())
                    {
                        setImage(*pBitmap);
                        repaint();
                    }
                }
            }

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LargeBookFaceComponent)
        };

        LargeBookFaceComponent* pCompImage = new LargeBookFaceComponent(btn->getNormalImage(), m_strBook);
        juce::CXDialogWindow::showDialog(m_strBookName, pCompImage, this, true, juce::Justification::centred, true);
    }
}

void PreviewComponent::buttonClicked(juce::Button* button)
{
    if (!button)
        return;

    // 点击图书封面
    if (m_pImageBookFaceView && button == m_pImageBookFaceView)
    {
        bookFaceClicked(button);
        return;
    }

    // 点击阅读按钮
    if (m_pButtonCtrl[0] == button)
    {
        ReadWindowHelper::getInstance()->readBook(m_strBook);
        juce::CXDialogWindow* dialog = findParentComponentOfClass<juce::CXDialogWindow>();
        if (dialog) dialog->setVisible(false);
        return;
    }
}

void PreviewComponent::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    switch (reqID)
    {
    case HWReq_Pic_Download:
        {
            juce::Image* pBitmap = HWDocBookData::getInstance()->GetBookImage();
            if (pBitmap && !pBitmap->isNull())
            {
                float fStep = pBitmap->getHeight()*1.0f / pBitmap->getWidth()*1.0f;

                juce::Rectangle<int> rcBookFace(15,15,165,205);
                rcBookFace.setHeight(fStep * rcBookFace.getWidth());
                m_pImageBookFaceView->setImages(false, true, true,
                    *pBitmap, 1.0f, juce::Colours::transparentBlack,
                    *pBitmap, 1.0f, juce::Colours::transparentBlack,
                    *pBitmap, 1.0f, juce::Colours::transparentBlack);
                m_pImageBookFaceView->setBounds(rcBookFace);

                // 移动图书信息
                m_rcBookInfo.setBounds(rcBookFace.getX(), rcBookFace.getBottom(), rcBookFace.getWidth(), getHeight()-rcBookFace.getBottom());
                repaint();
            }
        }
        break;
    case HWReq_Book_Info:
        {
            const HWTBookInfo* pBookInfo = HWDocBookData::getInstance()->GetBookInfo();
            if (pBookInfo)
            {
                m_strBookName = pBookInfo->strName;

                juce::Font defaultAttrFont(17);
                juce::String strFmt;

                // 标题
                juce::String strCategory = HWDocBookType::getInstance()->GetBookCategory(pBookInfo->nTypeId);
                juce::CXDocumentWindow* window = findParentComponentOfClass<juce::CXDocumentWindow>();
                if (window)
                    window->setTitle(strCategory.isEmpty() ? pBookInfo->strName : strCategory, juce::Justification::centredLeft);

                // 作者：
                strFmt.clear();
                for (unsigned int i = 0; i < pBookInfo->arrAuthors.size(); ++i)
                {
                    if (!strFmt.isEmpty())
                        strFmt += L"，";
                    strFmt += pBookInfo->arrAuthors[i];
                }
                m_attrBookInfo[1].clear();
                m_attrBookInfo[1].append(L"作者：", defaultAttrFont, juce::Colours::grey);
                m_attrBookInfo[1].append(strFmt, defaultAttrFont, juce::Colours::darkgrey);

                // 出版社
                m_attrBookInfo[2].clear();
                m_attrBookInfo[2].append(L"出版社：", defaultAttrFont, juce::Colours::grey);
                if (!pBookInfo->strPress.isEmpty())
                {
                    m_attrBookInfo[2].append(pBookInfo->strPress, defaultAttrFont, juce::Colours::darkgrey);
                }

                // 出版日期
                m_attrBookInfo[3].clear();
                m_attrBookInfo[3].append(L"出版日期：", defaultAttrFont, juce::Colours::grey);
                HWDocHelper::getInstance()->GetTimeFormat(pBookInfo->tPressTime, strFmt);
                m_attrBookInfo[3].append(strFmt, defaultAttrFont, juce::Colours::darkgrey);

                // 页数
                m_attrBookInfo[4].clear();
                m_attrBookInfo[4].append(L"页数：", defaultAttrFont, juce::Colours::grey);
                m_attrBookInfo[4].append(juce::String(pBookInfo->nPageCount), defaultAttrFont, juce::Colours::darkgrey);

                // 字数
                m_attrBookInfo[5].clear();
                m_attrBookInfo[5].append(L"页数：", defaultAttrFont, juce::Colours::grey);
                m_attrBookInfo[5].append(juce::String(pBookInfo->nWordCount), defaultAttrFont, juce::Colours::darkgrey);
            }
        }
        break;
    case HWReq_Book_Res:
        {
            juce::String* pstrBrief = HWDocBookData::getInstance()->GetBookBrief();
            if (pstrBrief && m_pTextBrief)
            {
                m_pTextBrief->setText(*pstrBrief, juce::dontSendNotification);
            }
        }
        break;
    case HWReq_BookScore_Get:
        {
            HWTBookScore* pBookScore = HWDocBookData::getInstance()->GetBookScore();
            if (pBookScore)
            {
                // 评分
                juce::Font& defaultAttrFont = juce::Font(16);
                juce::String strText;
                m_attrBookInfo[0].clear();
                m_attrBookInfo[0].append(L"评分：", defaultAttrFont, juce::Colours::grey);
                m_attrBookInfo[0].append(juce::String((int)pBookScore->fStars), defaultAttrFont.withPointHeight(defaultAttrFont.getHeightInPoints()*1.8f), ProjectInfo::colorGoldYello);
                strText << L"." << juce::String((int)((pBookScore->fStars - (float)(int)pBookScore->fStars) * 10));
                m_attrBookInfo[0].append(strText, defaultAttrFont, ProjectInfo::colorGoldYello);
                strText.clear();
                strText << L"（" << pBookScore->nCount << L"人评价）";
                m_attrBookInfo[0].append(strText, defaultAttrFont, juce::Colours::grey);
            }
        }
        break;
    case HWReq_Dynamic_BookNote:
        {
            if (err != HWError_SUCCEED) return;

            int nType = HIWORD(ctx);
            int nPage = LOWORD(ctx);

            if (nType == HWDyicType_Comment)
            {
                //                 std::vector<std::pair<int,int>> vecIds;
                //                 m_nReviewListCount = HWDocDynamic::Instance()->GetBookNoteCount();
                //                 HWDocDynamic::Instance()->GetBookNoteList(vecIds);
                //                 for (unsigned int i=0; i<vecIds.size(); ++i)
                //                 {
                //                     int nItem = m_pReviewListView->InsertItem(-1, NULL, vecIds[i].first, DEFAULT_ITEM_HEIGHT);
                //                     CXItemPanelEx* pPanel = m_pReviewListView->GetItemPanel(nItem);
                //                     CXReviewItemPanel* pItem = pPanel->FindChildByName2<CXReviewItemPanel>(L"dynamic_panel");
                //                     pItem->SetUserData(vecIds[i].first);
                //                     pItem->Init();
                //                 }
                //
                //                 m_mapReviewListPage[nPage] = true;
            }
            else if (nType == HWDyicType_Postil)
            {
                //                 std::vector<std::pair<int,int>> vecIds;
                //                 m_nPostilListCount = HWDocDynamic::Instance()->GetBookNoteCount();
                //                 HWDocDynamic::Instance()->GetBookNoteList(vecIds);
                //                 for (unsigned int i=0; i<vecIds.size(); ++i)
                //                 {
                //                     int nItem = m_pPostilListView->InsertItem(-1, NULL, vecIds[i].first, DEFAULT_ITEM_HEIGHT);
                //                     CXItemPanelEx* pPanel = m_pPostilListView->GetItemPanel(nItem);
                //                     CXPostilItemPanel* pItem = pPanel->FindChildByName2<CXPostilItemPanel>(L"dynamic_panel");
                //                     pItem->SetUserData(vecIds[i].first);
                //                     pItem->Init();
                //                 }
                //
                //                 m_mapPostilListPage[nPage] = true;
            }
        }
        break;
    case HWReq_Commend_Class:
    case HWReq_Commend_Book:
        {
            //             CXMessageBox(this, L"推荐成功！",L"图书推荐", MB_OK);
            //             OnBtnRecommendCancel();
            //             EndDialog(IDOK, false);
        }
        break;
    case HWReq_Space_ClassStatistics:
        {
            // SetClassStatInfo();
            break;
        }
    case HWReq_School_UsrInfo:
        {
            //SetClassStatUserInfo(ctx);
            break;
        }
    case HWReq_Space_BookStatistics:
        {
            HWTBookStat* pBookStat = HWDocSpace::getInstance()->GetBookStat(m_strBook);
            if (pBookStat)
            {
                //                 juce::String strFmt;
                //                 CXTabPage* pReview = (CXTabPage*)m_pTabCtrl->GetPage(1);
                //                 strFmt.Format(L"书评(%d)", pBookStat->nComment);
                //                 pReview->SetTitle(strFmt);
                //
                //                 CXTabPage* pPostil = (CXTabPage*)m_pTabCtrl->GetPage(2);
                //                 strFmt.Format(L"批注(%d)", pBookStat->nPostil);
                //                 pPostil->SetTitle(strFmt);
            }
            break;
        }
    case HWReq_School_StudentList:
        {
            //             CXListBoxEx* pListStudents = FindChildByName2<CXListBoxEx>(L"preview_recommend_stulist");
            //             const HWDocSchool::HWSchoolUserList* pStudentList = HWDocSchool::Instance()->GetStudentList();
            //             int nStudentCount = pStudentList->size();
            //             if (pStudentList && nStudentCount > 0)
            //             {
            //                 int nItemCount = (nStudentCount/5) + (nStudentCount%5!=0);
            //
            //                 CXItemPanel* pPanel;
            //                 for (unsigned int nItem=0; nItem<nItemCount; ++nItem)
            //                 {
            //                     int nNewItem = pListStudents->InsertItem(-1, NULL, 0);
            //                     pPanel = pListStudents->GetItemPanel(nNewItem);
            //                     int nFirst = nItem * 5;
            //                     for (int uIdx = nFirst; uIdx < nFirst+5 && uIdx < nStudentCount; ++uIdx)
            //                     {
            //                         const HWDocSchool::HWSchoolUserInfo* pStudent = (*pStudentList)[uIdx];
            //                         CXCheckBox* pStu = pPanel->FindChildByID2<CXCheckBox>(10 + uIdx-nFirst);
            //                         pStu->SetVisible(true);
            //                         pStu->SetAttribute(L"tip", pStudent->strName);
            //                         pStu->SetName(CX_U2T(pStudent->strShelfNo));
            //                         pStu->SetWindowText(pStudent->strName);
            //                     }
            //                 }
            //             }
            break;
        }
    case HWReq_School_ClassInfo:
        {
            if (ctx)
            {
                //                 CXWindow* pCheckBox = (CXWindow*)ctx;
                //                 const HWTClassInfo* pClassInfo = HWDocSchool::Instance()->GetClassInfo();
                //                 if (pClassInfo)
                //                 {
                //                     pCheckBox->SetWindowText(pClassInfo->strName);
                //                     pCheckBox->SetVisible(true);
                //                     if (pCheckBox->SetUserData(pClassInfo->nClassid) == -1)
                //                     {
                //                         pCheckBox->SetCheck(true);
                //                         ClearSelectStudentClassesWithout(pCheckBox);
                //                         RequestRecommendStudentStudents(pCheckBox->GetUserData());
                //                     }
                //                 }
            }

            break;
        }
    default:
        break;
    }
}

void PreviewComponent::initDefaultComponent()
{
    for (unsigned int i=0; i<6; ++i)
    {
        m_attrBookInfo[i].setWordWrap(juce::AttributedString::none);
        m_attrBookInfo[i].setJustification(juce::Justification::bottomLeft);
    }

    m_pImageBookFaceView = new juce::ImageButton;
    m_pImageBookFaceView->addListener(this);
    addAndMakeVisible(m_pImageBookFaceView);

    int posY = 210;
    m_pButtonCtrl[0] = new juce::ColorButton;
    m_pButtonCtrl[0]->setColour(juce::ColorButton::ColourIds::buttonColourHoverId, ProjectInfo::colorGoldYello);
    m_pButtonCtrl[0]->setColour(juce::ColorButton::ColourIds::buttonColourDownId, ProjectInfo::colorGoldYello.contrasting(0.1f));
    m_pButtonCtrl[0]->setColour(juce::ColorButton::ColourIds::textColourHoverId, ProjectInfo::colorGoldYello);
    m_pButtonCtrl[0]->setColour(juce::ColorButton::ColourIds::textColourDownId, ProjectInfo::colorGoldYello.contrasting(0.1f));
    m_pButtonCtrl[0]->setButtonBorder(1);
    m_pButtonCtrl[0]->setButtonText(L"阅读");
    m_pButtonCtrl[0]->setBounds(posY,60,95,30);
    m_pButtonCtrl[0]->addListener(this);
    addAndMakeVisible(m_pButtonCtrl[0]);
    posY += 115;

    m_pButtonCtrl[1] = new juce::ColorButton;
    m_pButtonCtrl[1]->setColour(juce::ColorButton::ColourIds::buttonColourNormalId, juce::Colours::grey);
    m_pButtonCtrl[1]->setColour(juce::ColorButton::ColourIds::buttonColourHoverId, ProjectInfo::colorBlueBlack);
    m_pButtonCtrl[1]->setColour(juce::ColorButton::ColourIds::buttonColourDownId, ProjectInfo::colorBlueBlack.contrasting(0.1f));
    m_pButtonCtrl[1]->setColour(juce::ColorButton::ColourIds::textColourNormalId, juce::Colours::grey);
    m_pButtonCtrl[1]->setColour(juce::ColorButton::ColourIds::textColourHoverId, ProjectInfo::colorBlueBlack);
    m_pButtonCtrl[1]->setColour(juce::ColorButton::ColourIds::textColourDownId, ProjectInfo::colorBlueBlack.contrasting(0.1f));
    m_pButtonCtrl[1]->setButtonBorder(1);
    m_pButtonCtrl[1]->setButtonText(L"移动");
    m_pButtonCtrl[1]->setBounds(posY,60,95,30);
    m_pButtonCtrl[1]->addListener(this);
    addAndMakeVisible(m_pButtonCtrl[1]);
    posY += 115;

    m_pButtonCtrl[2] = new juce::ColorButton;
    m_pButtonCtrl[2]->setColour(juce::ColorButton::ColourIds::buttonColourNormalId, juce::Colours::grey);
    m_pButtonCtrl[2]->setColour(juce::ColorButton::ColourIds::buttonColourHoverId, ProjectInfo::colorLightRed);
    m_pButtonCtrl[2]->setColour(juce::ColorButton::ColourIds::buttonColourDownId, ProjectInfo::colorLightRed.contrasting(0.1f));
    m_pButtonCtrl[2]->setColour(juce::ColorButton::ColourIds::textColourNormalId, juce::Colours::grey);
    m_pButtonCtrl[2]->setColour(juce::ColorButton::ColourIds::textColourHoverId, ProjectInfo::colorLightRed);
    m_pButtonCtrl[2]->setColour(juce::ColorButton::ColourIds::textColourDownId, ProjectInfo::colorLightRed.contrasting(0.1f));
    m_pButtonCtrl[2]->setButtonBorder(1);
    m_pButtonCtrl[2]->setButtonText(L"删除");
    m_pButtonCtrl[2]->setBounds(posY,60,95,30);
    m_pButtonCtrl[2]->addListener(this);
    addAndMakeVisible(m_pButtonCtrl[2]);
    posY += 115;

    m_pButtonCtrl[3] = new juce::ColorButton;
    m_pButtonCtrl[3]->setColour(juce::ColorButton::ColourIds::buttonColourNormalId, juce::Colours::grey);
    m_pButtonCtrl[3]->setColour(juce::ColorButton::ColourIds::buttonColourHoverId, ProjectInfo::colorBlueBlack);
    m_pButtonCtrl[3]->setColour(juce::ColorButton::ColourIds::buttonColourDownId, ProjectInfo::colorBlueBlack.contrasting(0.1f));
    m_pButtonCtrl[3]->setColour(juce::ColorButton::ColourIds::textColourNormalId, juce::Colours::grey);
    m_pButtonCtrl[3]->setColour(juce::ColorButton::ColourIds::textColourHoverId, ProjectInfo::colorBlueBlack);
    m_pButtonCtrl[3]->setColour(juce::ColorButton::ColourIds::textColourDownId, ProjectInfo::colorBlueBlack.contrasting(0.1f));
    m_pButtonCtrl[3]->setButtonBorder(1);
    m_pButtonCtrl[3]->setButtonText(L"推荐给班级");
    m_pButtonCtrl[3]->setBounds(posY,60,100,30);
    m_pButtonCtrl[3]->addListener(this);
    addAndMakeVisible(m_pButtonCtrl[3]);
    posY += 120;

    m_pButtonCtrl[4] = new juce::ColorButton;
    m_pButtonCtrl[4]->setColour(juce::ColorButton::ColourIds::buttonColourNormalId, juce::Colours::grey);
    m_pButtonCtrl[4]->setColour(juce::ColorButton::ColourIds::buttonColourHoverId, ProjectInfo::colorBlueBlack);
    m_pButtonCtrl[4]->setColour(juce::ColorButton::ColourIds::buttonColourDownId, ProjectInfo::colorBlueBlack.contrasting(0.1f));
    m_pButtonCtrl[4]->setColour(juce::ColorButton::ColourIds::textColourNormalId, juce::Colours::grey);
    m_pButtonCtrl[4]->setColour(juce::ColorButton::ColourIds::textColourHoverId, ProjectInfo::colorBlueBlack);
    m_pButtonCtrl[4]->setColour(juce::ColorButton::ColourIds::textColourDownId, ProjectInfo::colorBlueBlack.contrasting(0.1f));
    m_pButtonCtrl[4]->setButtonBorder(1);
    m_pButtonCtrl[4]->setButtonText(L"推荐给学生");
    m_pButtonCtrl[4]->setBounds(posY,60,100,30);
    m_pButtonCtrl[4]->addListener(this);
    addAndMakeVisible(m_pButtonCtrl[4]);

    m_pTextBrief = new juce::TextEditor;
    m_pTextBrief->setFont(juce::Font(18));
    m_pTextBrief->setColour(juce::TextEditor::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    m_pTextBrief->setMultiLine(true);
    m_pTextBrief->setBorder(juce::BorderSize<int>(0,0,0,0));
    m_pTextBrief->setReadOnly(true);
    m_pTextBrief->setCaretVisible(false);
    m_pTextBrief->setColour(juce::TextEditor::ColourIds::backgroundColourId, ProjectInfo::colorLightWhite);
    m_pTextBrief->setColour(juce::TextEditor::ColourIds::outlineColourId, ProjectInfo::colorLightWhite);
    m_pTextBrief->setColour(juce::TextEditor::ColourIds::shadowColourId, ProjectInfo::colorLightWhite);

    m_pTabbedComponent = new juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop);
    m_pTabbedComponent->setBounds(210,110,getWidth()-220, getHeight()-125);
    m_pTabbedComponent->setOutline(0);
    m_pTabbedComponent->setTabBarDepth(35);
    m_pTabbedComponent->setIndent(juce::BorderSize<int>(10, 0, 10, 0));
    m_pTabbedComponent->addTab(L"简介", ProjectInfo::colorLightWhite, m_pTextBrief, true);
    m_pTabbedComponent->addTab(L"书评", ProjectInfo::colorLightWhite, new juce::Label, true);
    m_pTabbedComponent->addTab(L"批注", ProjectInfo::colorLightWhite, new juce::Label, true);
    addAndMakeVisible(m_pTabbedComponent);
}

void PreviewComponent::resized()
{
}

