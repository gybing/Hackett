#include "JuceHeader.h"
#include "PostilItemComponent.h"

#define REPLYLIST_PAGECOUNT 20

//////////////////////////////////////////////////////////////////////////
// BookPostilItem 

juce::Component* BookPostilItem::createItemComponent(int rowNumber)
{
    if (postilComponent == nullptr)
    {
        postilComponent = new PostilItemComponent(*this);
    }
    return postilComponent;
}

int BookPostilItem::getItemHeight(int rowNumber)
{
    return 130;
}

void BookPostilItem::updatePosisitons()
{
    owner.updatePosition();
}

BookPostilItem::BookPostilItem(ListView& l)
 : owner(l)
{
}

BookPostilItem::~BookPostilItem()
{

}

void BookPostilItem::setContent(bool my, int id, bool c /*= true*/)
{
    ismy = my;
    canGotoPage = c;
    dynamicID = id;
}

//////////////////////////////////////////////////////////////////////////
// PostilItemComponent
PostilItemComponent::PostilItemComponent(BookPostilItem& postil)
    : postilItem(postil)
{
    userImage = new juce::ImageComponent;
    userTitle = new juce::TextButton;
    userFlower = new juce::ToolButton(juce::String::empty, false);
    userMagic = new juce::ToolButton(juce::String::empty, false);
    userFavour = new juce::ToolButton(juce::String::empty, false);
    readThisPage = new juce::TextButton;
    labelDate = new juce::Label;
    labelFrom = new juce::Label;
    showDetails = new juce::TextButton;
    replayButton = new juce::TextButton;

    userTitle->setColour(juce::TextButton::ColourIds::textColourOffId, ProjectInfo::colorLightBlack);
    userTitle->setColour(juce::TextButton::ColourIds::textColourOnId, ProjectInfo::colorLightBlack.contrasting(0.2f));

    userFlower->setColour(juce::ToolButton::ColourIds::textColourId, ProjectInfo::colorDarkGray);
    userFlower->setColour(juce::ToolButton::ColourIds::textColourOnId, ProjectInfo::colorDarkGray.contrasting(0.1f));

    userMagic->setColour(juce::ToolButton::ColourIds::textColourId, ProjectInfo::colorDarkGray);
    userMagic->setColour(juce::ToolButton::ColourIds::textColourOnId, ProjectInfo::colorDarkGray.contrasting(0.1f));
    updateMagic(0);

    userFavour->setColour(juce::ToolButton::ColourIds::textColourId, ProjectInfo::colorDarkGray);
    userFavour->setColour(juce::ToolButton::ColourIds::textColourOnId, ProjectInfo::colorDarkGray.contrasting(0.1f));

    showDetails->setButtonText(L"显示详细");
    showDetails->setColour(juce::TextButton::ColourIds::textColourOnId, ProjectInfo::colorBlueButton);
    showDetails->setColour(juce::TextButton::ColourIds::textColourOffId, ProjectInfo::colorBlueButton.contrasting(0.2f));
    readThisPage->setButtonText(L"阅读该页");
    readThisPage->setColour(juce::TextButton::ColourIds::textColourOnId, ProjectInfo::colorBlueButton);
    readThisPage->setColour(juce::TextButton::ColourIds::textColourOffId, ProjectInfo::colorBlueButton.contrasting(0.2f));
    labelDate->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightGrey);
    labelFrom->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorBlueButton);

    addAndMakeVisible(userImage);
    addAndMakeVisible(userTitle);
    addAndMakeVisible(userFlower);
    addAndMakeVisible(userMagic);
    addAndMakeVisible(userFavour);
    addAndMakeVisible(readThisPage);
    addAndMakeVisible(labelDate);
    addAndMakeVisible(labelFrom);
    addAndMakeVisible(showDetails);
    addAndMakeVisible(replayButton);

    userTitle->addListener(this);
    userFlower->addListener(this);
    userMagic->addListener(this);
    userFavour->addListener(this);
    readThisPage->addListener(this);
    showDetails->addListener(this);
    replayButton->addListener(this);
    resized();
}

PostilItemComponent::~PostilItemComponent()
{

}

void PostilItemComponent::paint(juce::Graphics& g)
{
    juce::Rectangle<float> textArea(75.0f, 40.0f, getWidth() - 85.0f, getHeight() - 60.0f);
    textContent.draw(g, textArea);
}

void PostilItemComponent::resized()
{
    userImage->setBounds(10, 10, 55, 55);
    int width = userTitle->getBestWidthForHeight(18);
    userTitle->setBounds(70, 10, width, 18);
    userFlower->setBounds(userTitle->getRight() + 10, 10, 45, 18);
    userMagic->setBounds(userFlower->getRight() + 10, 10, 45, 18);
    width = readThisPage->getBestWidthForHeight(18);
    readThisPage->setBounds(getRight() - width - 5, 10, width, 18);
    userFavour->setBounds(getRight() - width - 5, getHeight()-30, 45, 18);
}

void PostilItemComponent::buttonClicked(juce::Button* b)
{
    if (b == readThisPage)
    {
        ReadWindowHelper::getInstance()->gotoPage(postilItemInfo.readThePageNo);
        //ReadWindowHelper::getInstance()->showPostil(postilItemInfo.readThePageZone);
    }
}

void PostilItemComponent::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    switch (reqID)
    {
    case HWReq_Dynamic_Info:
        {
            if (err == HWError_SUCCEED)
            {
                HWTDynamicInfo* pDycInfo = HWDocDynamic::getInstance()->GetDynamicInfo();
                if (pDycInfo)
                {
                    setDynamicPanelData(pDycInfo);
                }
            }
            break;
        }
    case HWReq_Pic_Download:
        {
            juce::Image* pBitmap = HWDocUserData::getInstance()->GetUserHead();
            if (pBitmap && !pBitmap->isNull())
            {
                userImage->setImage(*pBitmap);
            }
            break;
        }
    case HWReq_Dynamic_Reply:
        {
            if (err == HWError_SUCCEED)
            {
                // 重新请求数据
                reqestReplyList();
            }
            break;
        }
    case HWReq_Dynamic_Favour:
        {
            if (err == HWError_SUCCEED)
            {
                std::pair<int,int>* pFavourRes = HWDocDynamic::getInstance()->GetFavourResult();
                if (pFavourRes)
                {
                    updateFavour(pFavourRes->second);
                }
            }
            break;
        }
    case HWReq_Dynamic_Flower:
        {
            if (err == HWError_SUCCEED)
            {
                std::pair<int,int>* pFlowerRes = HWDocDynamic::getInstance()->GetFlowerResult();
                if (pFlowerRes)
                {
                    updateFlowerView(pFlowerRes->second);
                }
            }
            break;
        }
    case HWReq_Postil_Del:
    case HWReq_Comment_Del:
        {
            if (err == HWError_SUCCEED)
            {
                postilItem.updatePosisitons();
            }
            break;
        }
    default:
        break;
    }
}

void PostilItemComponent::mouseDown(const juce::MouseEvent& event)
{
}

void PostilItemComponent::initData()
{
    if (postilItem.ismy)
    {
        if (HWTBookPostil* pPostil = HWDocBookPostil::getInstance()->GetPostil(postilItem.dynamicID)) 
            setPostilPanelData(pPostil);
    }
    else
    {
        requestDynamicInfo();
    }
}

void PostilItemComponent::setPostilPanelData(HWTBookPostil* pInfo)
{

}

void PostilItemComponent::requestDynamicInfo()
{
    HWParaDynamicInfo dycInfoUI(postilItem.dynamicID);
    HWDocDynamic::getInstance()->SubmitRequest(this, HWReq_Dynamic_Info, &dycInfoUI, 0);
}

void PostilItemComponent::setDynamicPanelData(HWTDynamicInfo* pInfo)
{
    // 内容
    HWDict& dict = pInfo->data.dictContent;
    textContent.append(juce::CharPointer_UTF8(HWDictElemString(&dict, "text")), juce::Font(16), ProjectInfo::colorLightBlack);
    textContent.append(L"\n");

    if (pInfo->data.nType == HWDyicType_Postil)
    {
        if (dict.find("original") != dict.end())
        {
            juce::String strSrc = juce::CharPointer_UTF8(HWDictElemString(&dict, "original"));
            if (strSrc.isNotEmpty())
            {
                strSrc.replace(juce::String(L'\n'), juce::String(L' '));
                juce::String strFmt;
                strFmt << L"原文第" << juce::String(juce::CharPointer_UTF8(HWDictElemString(&dict, "showpage"))) << L"页：" << strSrc;
                textContent.append(strFmt, juce::Font(16), ProjectInfo::colorLightGrey);
            }
        }

        if (postilItem.canGotoPage && dict.find("page") != dict.end())
        {
            int nPageNo = HWDictElemInt(&dict, "page");
            readThisPage->setVisible(true);
            postilItemInfo.readThePageNo = nPageNo;
            postilItemInfo.readThePageZone = HWDictElemInt(&dict, "zone");
        }
    }

    // 请求头像
    requestUserHead(pInfo->user.strShelfNo);

    // 创建时间
    juce::Time time(pInfo->data.tTime);
    labelDate->setText(time.formatted(L"%m月%d日") << L" 来自", juce::dontSendNotification);

    // 设备来源
    labelFrom->setText(CXOption::DeviceString(pInfo->data.enDeviceType), juce::dontSendNotification);

    postilItemInfo.strUserID = pInfo->user.strShelfNo;

    juce::String strTitle = pInfo->user.strName;

    if (pInfo->user.enUserType == HWUserType_Teacher)
    {
        // 用户名
        userTitle->setButtonText(strTitle << L"(老师)");
        // userv FindChildByID2<CXImageView>(41)->SetVisible(TRUE);
    }
    else if (pInfo->user.enUserType == HWUserType_TeacherFamily)
    {
        // 用户名
        userTitle->setButtonText(strTitle << L"(老师 家长)");
        // userv FindChildByID2<CXImageView>(41)->SetVisible(TRUE);
    }
    else if (pInfo->user.enUserType == HWUserType_Family)
    {
        // 用户名
        userTitle->setButtonText(strTitle << L"(家长)");
    }
    else if (pInfo->user.enUserType == HWUserType_Student)
    {
        userTitle->setButtonText(strTitle);

        if (HWDocAuth::getInstance()->GetUserType() == HWUserType_Student)
        {
            if (pInfo->data.nFlowerCount > 0)
            {
                userFlower->setVisible(true);
            }
        }
        else
        {
            userFlower->setVisible(true);
        }

        updateMagic(pInfo->data.nReviewCount);
    }

    updateFlowerView(pInfo->data.nFlowerCount, pInfo->op.nFlower);
    updateFavour(pInfo->data.nFavourCount, pInfo->op.nFavour);

    replyListCount = pInfo->data.nReplyCount;
    updateReplyCountUI();
    resized();
}

void PostilItemComponent::requestUserHead(const juce::String& szUserID)
{
    HWParaUserHead userHeadUI(szUserID);
    HWDocUserData::getInstance()->SubmitRequest(this, HWReq_Pic_Download, &userHeadUI, 0);
}

void PostilItemComponent::updateFlowerView(int nCnt, int nOP/*=-1*/)
{
    userFlower->setButtonText(juce::String(nCnt));
    int nSetOP = (nOP == -1) ? (userFlower->getToggleState() ? 0:1) : nOP;
    const wchar_t* strImage = (nSetOP == 1 || HWDocAuth::getInstance()->GetUserType()==HWUserType_Student) ? L"dynamic_flower.png" : L"dynamic_flower_n.png";

    juce::DrawableImage normal, over, down;
    normal.setImage(CXOption::GetImage(strImage, 0, 3, false));
    over.setImage(CXOption::GetImage(strImage, 1, 3, false));
    down.setImage(CXOption::GetImage(strImage, 2, 3, false));
    userFlower->setImages(&normal, &over, &down);

    userFlower->setToggleState(nSetOP, juce::dontSendNotification);
}

void PostilItemComponent::updateFavour(int nCnt, int nOP/*=-1*/)
{
    userFavour->setButtonText(juce::String(nCnt));
    int nSetOP = (nOP == -1) ? (userFavour->getToggleState() ? 0:1) : nOP;
    const wchar_t* strImage = (nSetOP == 1 || HWDocAuth::getInstance()->GetUserType()==HWUserType_Student) ? L"comment_icon_3stat.png" : L"comment_icon_3stat_f.png";
    
    juce::DrawableImage normal, over, down;
    normal.setImage(CXOption::GetImage(strImage, 0, 3, false));
    over.setImage(CXOption::GetImage(strImage, 1, 3, false));
    down.setImage(CXOption::GetImage(strImage, 2, 3, false));
    userFavour->setImages(&normal,&over,&down);
    userFavour->setToggleState(nSetOP, juce::dontSendNotification);
}

void PostilItemComponent::updateMagic(int nCnt)
{
    userMagic->setButtonText(juce::String(nCnt));
    const wchar_t* strImage = L"dynamic_magic.png";
    juce::DrawableImage normal, over, down;
    normal.setImage(CXOption::GetImage(strImage, 0, 3, false));
    over.setImage(CXOption::GetImage(strImage, 1, 3, false));
    down.setImage(CXOption::GetImage(strImage, 2, 3, false));
    userMagic->setImages(&normal,&over,&down);
}

void PostilItemComponent::updateReplyCountUI()
{
    juce::String strFmt;
    strFmt << L"评论 " << replyListCount;
    replayButton->setButtonText(strFmt);
}

void PostilItemComponent::reqestReplyList()
{
    replayListView->clearItems();
    mapReplyListPage.clear();
    requestReplyList(1);
}

void PostilItemComponent::requestReplyList(int begin)
{
    if (mapReplyListPage.find(begin/REPLYLIST_PAGECOUNT) == mapReplyListPage.end())
    {
        HWParaDynamicReplyList replyListUI(postilItem.dynamicID, begin, REPLYLIST_PAGECOUNT); 
        HWDocDynamic::getInstance()->SubmitRequest(this, HWReq_Dynamic_ReplyList, &replyListUI, (begin/REPLYLIST_PAGECOUNT));
    }
}

void PostilItemComponent::visibilityChanged()
{
    {
        initData();
    }
}
