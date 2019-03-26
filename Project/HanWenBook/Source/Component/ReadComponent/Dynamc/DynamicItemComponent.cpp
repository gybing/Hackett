#include "JuceHeader.h"
#include "DynamicItemComponent.h"

#define REPLYLIST_PAGECOUNT 20

//////////////////////////////////////////////////////////////////////////
// DynamicItemComponent
DynamicItemComponent::DynamicItemComponent()
    : dynamicID(-1)
{
    addAndMakeVisible(userImage = new juce::ImageComponent);
    addAndMakeVisible(userTitle = new juce::TextButton);
    addAndMakeVisible(userFlower = new juce::ImageButton);
    addAndMakeVisible(userMagic = new juce::ImageButton);
    addAndMakeVisible(userFavour = new juce::ImageButton);
    addAndMakeVisible(readThisPage = new juce::TextButton);
    addAndMakeVisible(showDetails = new juce::TextButton);
    addAndMakeVisible(labelDate = new juce::Label);
    addAndMakeVisible(labelFrom = new juce::Label);

    showDetails->setButtonText(L"显示详细");
    showDetails->setColour(juce::TextButton::ColourIds::textColourOnId, ProjectInfo::colorBlueButton);
    showDetails->setColour(juce::TextButton::ColourIds::textColourOffId, ProjectInfo::colorBlueButton.contrasting(0.2f));
    readThisPage->setButtonText(L"阅读该页");
    readThisPage->setColour(juce::TextButton::ColourIds::textColourOnId, ProjectInfo::colorBlueButton);
    readThisPage->setColour(juce::TextButton::ColourIds::textColourOffId, ProjectInfo::colorBlueButton.contrasting(0.2f));
    labelDate->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightGrey);
    labelFrom->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorBlueButton);
}

DynamicItemComponent::~DynamicItemComponent()
{

}

void DynamicItemComponent::paint(juce::Graphics& g)
{
}

void DynamicItemComponent::resized()
{
    userImage->setBounds(10, 10, 55, 55);
    int width = userTitle->getBestWidthForHeight(18);
    userTitle->setBounds(75, 10, width, 18);
    userFlower->setBounds(userTitle->getRight() + 10, 10, 40, 18);
    userFavour->setBounds(userFlower->getRight() + 10, 20, 40, 18);
    width = readThisPage->getBestWidthForHeight(18);
    readThisPage->setBounds(getRight() - width, 20, width, 18);
    showDetails;
}

void DynamicItemComponent::buttonClicked(juce::Button* b)
{

}

void DynamicItemComponent::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    
}


void DynamicItemComponent::requestDynamicInfo()
{
    if (dynamicID != -1)
    {
        HWParaDynamicInfo dycInfoUI(dynamicID);
        HWDocDynamic::getInstance()->SubmitRequest(this, HWReq_Dynamic_Info, &dycInfoUI, 0);
    }
}

void DynamicItemComponent::setDynamicPanelData(HWTDynamicInfo* pInfo)
{
    // 内容
    HWDict& dict = pInfo->data.dictContent;
    textContent.append(juce::CharPointer_UTF8(HWDictElemString(&dict, "text")), juce::Font(18), ProjectInfo::colorLightBlack);

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
                textContent.append(strFmt, juce::Font(18), ProjectInfo::colorLightGrey);
            }
        }

        if (canGotoPage && dict.find("page") != dict.end())
        {
            int nPageNo = HWDictElemInt(&dict, "page");
            readThisPage->setVisible(true);
            gotoPageNo = nPageNo;
            gotoPageZone = HWDictElemInt(&dict, "zone");
        }
    }

    // 请求头像
    requestUserHead(pInfo->user.strShelfNo);

    // 创建时间
    juce::Time time(pInfo->data.tTime);
    labelDate->setText(time.formatted(L"%m月%日") << L" 来自", juce::dontSendNotification);

    // 设备来源
    labelFrom->setText(CXOption::DeviceString(pInfo->data.enDeviceType), juce::dontSendNotification);

    userID = pInfo->user.strShelfNo;

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

        userMagic->setVisible(true);
        userMagic->setButtonText(juce::String(pInfo->data.nReviewCount));
    }

    updateFlowerView(pInfo->data.nFlowerCount, pInfo->op.nFlower);
    updateFavour(pInfo->data.nFavourCount, pInfo->op.nFavour);

    replyListCount = pInfo->data.nReplyCount;
    updateReplyCountUI();

    // op

    //UpdateWindowSize();
}

void DynamicItemComponent::setCanGotoPage(bool can)
{

}

void DynamicItemComponent::reqestReplyList()
{
    replayListView->clearItems();
    mapReplyListPage.clear();
    requestReplyList(1);
}

void DynamicItemComponent::requestUserHead(const juce::String& szUserID)
{
    HWParaUserHead userHeadUI(szUserID);
    HWDocUserData::getInstance()->SubmitRequest(this, HWReq_Pic_Download, &userHeadUI, 0);
}

void DynamicItemComponent::updateFlowerView(int nCnt, int nOP/*=-1*/)
{
    userFlower->setButtonText(juce::String(nCnt));
    int nSetOP = (nOP == -1) ? (userFlower->getToggleState() ? 0:1) : nOP;
    const wchar_t* strImage = (nSetOP == 1 || HWDocAuth::getInstance()->GetUserType()==HWUserType_Student) ? L"dynamic_flower.png" : L"dynamic_flower_n.png";
    userFlower->setImages(false, false, false,
        CXOption::GetImage(strImage, 0, 3, false), 1.0f, juce::Colours::transparentBlack,
        CXOption::GetImage(strImage, 1, 3, false), 1.0f, juce::Colours::transparentBlack,
        CXOption::GetImage(strImage, 2, 3, false), 1.0f, juce::Colours::transparentBlack);
    
    userFlower->setToggleState(nSetOP, juce::dontSendNotification);
}

void DynamicItemComponent::updateFavour(int nCnt, int nOP/*=-1*/)
{
    userFavour->setButtonText(juce::String(nCnt));
    int nSetOP = (nOP == -1) ? (userFavour->getToggleState() ? 0:1) : nOP;
    const wchar_t* strImage = (nSetOP == 1 || HWDocAuth::getInstance()->GetUserType()==HWUserType_Student) ? L"comment_icon_3stat.png" : L"comment_icon_3stat_f.png";
    userFavour->setImages(false, false, false,
        CXOption::GetImage(strImage, 0, 3, false), 1.0f, juce::Colours::transparentBlack,
        CXOption::GetImage(strImage, 1, 3, false), 1.0f, juce::Colours::transparentBlack,
        CXOption::GetImage(strImage, 2, 3, false), 1.0f, juce::Colours::transparentBlack);

    userFavour->setToggleState(nSetOP, juce::dontSendNotification);
}

void DynamicItemComponent::updateReplyCountUI()
{
    juce::String strFmt;
    strFmt << L"评论 " << replyListCount;
    replayButton->setButtonText(strFmt);
}

void DynamicItemComponent::requestReplyList(int begin)
{
    if (mapReplyListPage.find(begin/REPLYLIST_PAGECOUNT) == mapReplyListPage.end())
    {
        HWParaDynamicReplyList replyListUI(dynamicID, begin, REPLYLIST_PAGECOUNT); 
        HWDocDynamic::getInstance()->SubmitRequest(this, HWReq_Dynamic_ReplyList, &replyListUI, (begin/REPLYLIST_PAGECOUNT));
    }
}

int DynamicItemComponent::caculateExpandContentSize()
{
    return 0;
}

void DynamicItemComponent::setShowDetailsButton(bool show)
{

}
