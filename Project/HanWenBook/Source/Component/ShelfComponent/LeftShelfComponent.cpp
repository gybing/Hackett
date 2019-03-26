#include "JuceHeader.h"
#include "LeftShelfComponent.h"
#include "RightShelfComponent.h"

LeftShelfComponent::LeftShelfComponent(RightContentNotify* notify)
    : rightContentNotify(notify)
{
    HWDocTag::getInstance()->Register(this);
    addAndMakeVisible(tagListView = new ExpandListView(this));
    tagListView->setHoverSelector(true);

    InitData();
}

LeftShelfComponent::~LeftShelfComponent()
{
    HWDocTag::getInstance()->UnRegister(this);
}

void LeftShelfComponent::InitData()
{
    headImageMask = CXIMAGE_CACHE_LOAD(L"home_head_mask.png");
    HWTUserInfo* userInfo = HWDocAuth::getInstance()->GetCurrUserInfo();
    if (userInfo)
    {
        userTitle = userInfo->strName;
        if (userInfo->enUserType == HWUserType_Teacher ||
            userInfo->enUserType == HWUserType_TeacherFamily)
        {
            userTitle += L"（老师）";
            userImageVIP = CXIMAGE_CACHE_LOAD(L"dynamic_user_vip.png");
        }
        else if (userInfo->enUserType == HWUserType_Family)
        {
            
        }
        else if (userInfo->enUserType == HWUserType_Student)
        {
            
        }
        
        // 取头像
        HWParaUserHead userHeadUI(userInfo->strShelfNo);
        HWDocUserData::getInstance()->SubmitRequest(this, HWReq_Pic_Download, &userHeadUI, 0);
        
        HWParaTagList tagListUI(juce::String::empty);
        HWDocTag::getInstance()->SubmitRequest(this, HWReq_Tag_List, &tagListUI, 0);
    }
    else
    {
        userTitle = L"测试 （老师）";
        userLevel = L"藏书： 0本";
        headImagePortrait = CXIMAGE_CACHE_LOAD(L"main_head_default_boy.png");
        userImageVIP = CXIMAGE_CACHE_LOAD(L"dynamic_user_vip.png");
    }
}

void LeftShelfComponent::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    switch (reqID)
    {
        case HWReq_Pic_Download:
        {
            juce::Image* pImage = HWDocUserData::getInstance()->GetUserHead();
            if (pImage)
            {
                headImagePortrait = *pImage;
                repaint(20, 15, headImageMask.getWidth(), headImageMask.getHeight());
            }
            
            break;
        }
        case HWReq_Tag_List:
        {
            HWParaBookList bookListUI(juce::String::empty);
            HWDocTag::getInstance()->SubmitRequest(this, HWReq_Book_List, &bookListUI, 0);
            break;
        }
        case HWReq_Book_List:
        {
            tagListView->clearItems();
            unsigned int count = HWDocTag::getInstance()->GetUserTagCount();
            if (count > 0)
            {
                for (unsigned int i=0; i<count; ++i)
                    tagListView->addItem(new TagListViewItem(*this, i));

                // 添加新建按钮
                tagListView->addItem(new TagListViewItem(*this, TagListItemNewID));

                if (TagListViewItem* item = static_cast<TagListViewItem*>(tagListView->getItem(0)))
                    item->selectedItem();
            }
            
            break;
        }
        default:
            break;
    }
}

void LeftShelfComponent::paint(juce::Graphics& g)
{
    g.fillAll(ProjectInfo::colorLightWhite);
    g.drawImage(headImagePortrait, 20, 15, headImageMask.getWidth(), headImageMask.getHeight(),
                0,0,headImagePortrait.getWidth(), headImagePortrait.getHeight());
    g.drawImageAt(headImageMask, 20, 15);
    g.setFont(18);
    g.setColour(ProjectInfo::colorLightBlack);
    g.drawSingleLineText(userTitle, 112, 32);
    g.setFont(16);
    g.drawSingleLineText(userLevel, 112, 80);
    juce::Component::paint(g);
}

void LeftShelfComponent::resized()
{
    tagListView->setBounds(0, 110, 220, getHeight() - 110);
}

void LeftShelfComponent::OnBookTagListChange()
{
    HWParaTagList tagListUI(juce::String::empty);
    HWDocTag::getInstance()->SubmitRequest(this, HWReq_Tag_List, &tagListUI, 0);
}

//////////////////////////////////////////////////////////////////////////
// TagListItem

juce::ScopedPointer<juce::ToolButton> TagListViewItem::TagListItem::buttonRename;
juce::ScopedPointer<juce::ToolButton> TagListViewItem::TagListItem::buttonDelete;
juce::ScopedPointer<juce::ToolButton> TagListViewItem::TagListItem::buttonRecomend;
juce::ScopedPointer<juce::ToolButton> TagListViewItem::TagListItem::buttonCfmOK;
juce::ScopedPointer<juce::ToolButton> TagListViewItem::TagListItem::buttonCfmCancel;

TagListViewItem::TagListItem::TagListItem(TagListViewItem& o)
    : owner(o)
{
    setInterceptsMouseClicks(false, true);

    labelName = new juce::Label;
    labelName->setFont(juce::Font(TagListItemDefaultHeight * 0.4f));
    labelName->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    labelName->setColour(juce::Label::ColourIds::outlineWhenEditingColourId, ProjectInfo::colorLightWhite);
    labelName->setColour(juce::TextEditor::ColourIds::shadowColourId, ProjectInfo::colorLightGrey);
    labelName->setMinimumHorizontalScale(1.0f);
    labelName->setBounds(12, 10, 170, 30);
    labelName->setInterceptsMouseClicks(false, false);

    if (owner.rowNumber != TagListItemNewID)
    {
        labelName->setJustificationType(juce::Justification::centredLeft);
        labelName->setText(owner.userTabInfo.strName, juce::NotificationType::dontSendNotification);
    }
    else
    {
        labelName->setJustificationType(juce::Justification::centredRight);
        labelName->setText(L"新建分类", juce::NotificationType::dontSendNotification);
    }
    addAndMakeVisible(labelName);
}

TagListViewItem::TagListItem::~TagListItem()
{
}

void TagListViewItem::TagListItem::showExpandMenu()
{
    closeExpand();

    buttonRename = new juce::ToolButton(juce::String::empty, false);
    buttonDelete = new juce::ToolButton(juce::String::empty, false);
    buttonRecomend = new juce::ToolButton(juce::String::empty, false);

    buttonRename->setColour(juce::ToolButton::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    buttonRename->setColour(juce::ToolButton::ColourIds::textColourOnId, ProjectInfo::colorLightBlack.contrasting(0.1f));
    buttonDelete->setColour(juce::ToolButton::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    buttonDelete->setColour(juce::ToolButton::ColourIds::textColourOnId, ProjectInfo::colorLightBlack.contrasting(0.1f));
    buttonRecomend->setColour(juce::ToolButton::ColourIds::textColourId, ProjectInfo::colorGoldYello);
    buttonRecomend->setColour(juce::ToolButton::ColourIds::textColourOnId, ProjectInfo::colorGoldYello.contrasting(0.1f));

    buttonRename->addListener(this);
    buttonDelete->addListener(this);
    buttonRecomend->addListener(this);

    buttonRename->setButtonHeight(18);
    buttonDelete->setButtonHeight(18);
    buttonRecomend->setButtonHeight(18);

    buttonRename->setButtonTextPos(28);
    buttonDelete->setButtonTextPos(30);
    buttonRecomend->setButtonTextPos(30);

    buttonRename->setButtonText(L"重命名");
    buttonDelete->setButtonText(L"删除");
    buttonRecomend->setButtonText(L"推荐");

    juce::DrawableImage normal, over, down;
    const wchar_t* szImage = nullptr;

    szImage = L"shelf_tag_ctrl_m.png";
    normal.setImage(CXOption::GetImage(szImage, 0, 4, false));
    over.setImage(CXOption::GetImage(szImage, 1, 4, false));
    down.setImage(CXOption::GetImage(szImage, 2, 4, false));
    buttonRename->setImages(&normal, &over, &down);

    szImage = L"shelf_tag_ctrl_d.png";
    normal.setImage(CXOption::GetImage(szImage, 0, 4, false));
    over.setImage(CXOption::GetImage(szImage, 1, 4, false));
    down.setImage(CXOption::GetImage(szImage, 2, 4, false));
    buttonDelete->setImages(&normal, &over, &down);

    szImage = L"shelf_tag_ctrl_r.png";
    normal.setImage(CXOption::GetImage(szImage, 0, 4, false));
    over.setImage(CXOption::GetImage(szImage, 1, 4, false));
    down.setImage(CXOption::GetImage(szImage, 2, 4, false));
    buttonRecomend->setImages(&normal, &over, &down);

    const int itemWidth = getWidth()/3;
    buttonRename->setBounds(0, TagListItemDefaultHeight, itemWidth, TagListItemDefaultHeight);
    buttonDelete->setBounds(itemWidth, TagListItemDefaultHeight, itemWidth, TagListItemDefaultHeight);
    buttonRecomend->setBounds(itemWidth * 2, TagListItemDefaultHeight, itemWidth, TagListItemDefaultHeight);

    addAndMakeVisible(buttonRename);
    addAndMakeVisible(buttonDelete);
    addAndMakeVisible(buttonRecomend);
}

bool TagListViewItem::TagListItem::isExpandMenuShow() const
{
    return expandItemRowNumber == &owner && buttonRename != nullptr;
}

bool TagListViewItem::TagListItem::isExpandConformShow() const
{
    return expandItemRowNumber == &owner && buttonCfmOK != nullptr;
}

void TagListViewItem::TagListItem::closeExpand()
{
    if (buttonCfmOK)
        buttonCfmOK = nullptr;
    if (buttonCfmCancel)
        buttonCfmCancel = nullptr;
    if (buttonRename)
        buttonRename = nullptr;
    if (buttonDelete)
        buttonDelete = nullptr;
    if (buttonRecomend)
        buttonRecomend = nullptr;
}

juce::TextEditor* TagListViewItem::TagListItem::createEditorComponent()
{
    juce::TextEditor* const ed = new juce::TextEditor (getName());
    ed->applyFontToAllText (getLookAndFeel().getLabelFont (*labelName));
    labelName->copyAllExplicitColoursTo (*ed);

    return ed;
}

void TagListViewItem::TagListItem::showEditor()
{
    if (editor == nullptr)
    {
        addAndMakeVisible (editor = createEditorComponent());
        editor->setText (labelName->getText(), false);
        editor->setKeyboardType (juce::TextEditor::textKeyboard);
        editor->addListener (this);
        editor->grabKeyboardFocus();

        if (editor == nullptr) // may be deleted by a callback
            return;

        editor->setHighlightedRegion (juce::Range<int> (0, labelName->getText().length()));

        resized();
        repaint();

        //editorShown (editor);

        enterModalState (false);
        editor->grabKeyboardFocus();
    }
}

void TagListViewItem::TagListItem::hideEditor()
{

}

void TagListViewItem::TagListItem::showExpandConform()
{
    closeExpand();

    buttonCfmOK = new juce::ToolButton(juce::String::empty, false);
    buttonCfmCancel = new juce::ToolButton(juce::String::empty, false);

    buttonCfmOK->setColour(juce::ToolButton::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    buttonCfmOK->setColour(juce::ToolButton::ColourIds::textColourOnId, ProjectInfo::colorLightBlack.contrasting(0.1f));
    buttonCfmCancel->setColour(juce::ToolButton::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    buttonCfmCancel->setColour(juce::ToolButton::ColourIds::textColourOnId, ProjectInfo::colorLightBlack.contrasting(0.1f));

    buttonCfmOK->addListener(this);
    buttonCfmCancel->addListener(this);

    buttonCfmOK->setButtonHeight(18);
    buttonCfmCancel->setButtonHeight(18);

    buttonCfmOK->setButtonTextPos(28);
    buttonCfmCancel->setButtonTextPos(30);

    buttonCfmOK->setButtonText(L"确定");
    buttonCfmCancel->setButtonText(L"取消");

    juce::DrawableImage normal, over, down;
    const wchar_t* szImage = nullptr;

    szImage = L"shelf_tag_ctrl_y.png";
    normal.setImage(CXOption::GetImage(szImage, 0, 4, false));
    over.setImage(CXOption::GetImage(szImage, 1, 4, false));
    down.setImage(CXOption::GetImage(szImage, 2, 4, false));
    buttonCfmOK->setImages(&normal, &over, &down);

    szImage = L"shelf_tag_ctrl_d.png";
    normal.setImage(CXOption::GetImage(szImage, 0, 4, false));
    over.setImage(CXOption::GetImage(szImage, 1, 4, false));
    down.setImage(CXOption::GetImage(szImage, 2, 4, false));
    buttonCfmCancel->setImages(&normal, &over, &down);

    buttonCfmOK->setBounds(30, TagListItemDefaultHeight, 60, TagListItemDefaultHeight);
    buttonCfmCancel->setBounds(120, TagListItemDefaultHeight, 60, TagListItemDefaultHeight);

    addAndMakeVisible(buttonCfmOK);
    addAndMakeVisible(buttonCfmCancel);
}

void TagListViewItem::TagListItem::paint(juce::Graphics& g)
{
    // 如果该行处于选中状态，则将其背景色绘制为浅蓝色
    if (owner.isItemExpanded())
    {
        g.setColour(juce::Colour(0xFFe2e0e0));
        g.fillRect(5, 1, getWidth()-10, TagListItemDefaultHeight-2);
        g.fillRect(5, TagListItemDefaultHeight + 1, getWidth()-10, (2 * TagListItemDefaultHeight)-2);
    }
    else if (owner.isItemMouseSelected())
    {
        g.setColour(juce::Colour(0xFFe2e0e0));
        g.fillRect(5, 1, getWidth()-10, TagListItemDefaultHeight-2);
    }
    else if (owner.isItemMouseHoverd())
    {
        g.setColour(juce::Colour(0xFFebe9e6));
        g.fillRect(5, 1, getWidth()-10, TagListItemDefaultHeight-2);
    }

    if (owner.rowNumber == TagListItemNewID)
    {
        g.drawImage(CXOption::getInstance()->GetImage(L"shelf_l_new.png", owner.isItemMouseHoverd() ? 2 : 0, 3, true),
            getWidth() - 40, 10, 30, 30, 0, 0, 30, 30);
    }

    if (owner.isItemExpanded() && owner.rowNumber != TagListItemNewID)
    {
        g.drawImage(CXIMAGE_CACHE_LOAD(L"shelf_tag_ctrl_up.png"), getWidth() - 30, 22, 10, 6, 0, 0, 10, 6);
    }
}

void TagListViewItem::TagListItem::buttonClicked(juce::Button* b)
{
    if (b == buttonRename)
    {
        labelName->showEditor();
        labelName->getCurrentTextEditor()->setBorder(juce::BorderSize<int>(0));
        showExpandConform();
    }
    else if (b == buttonDelete)
    {
        if (juce::AlertWindow::showOkCancelBox(juce::AlertWindow::QuestionIcon, L"删除书架", L"是否删除书架？", L"是", L"否"))
        {
            if (ExpandListView* listView = owner.owner.tagListView)
            {
                const int tagID = owner.userTabInfo.nId;
                listView->removeItem(owner.rowNumber, true);
                if (listView->getNumItems() > 0)
                {
                    if (TagListViewItem* item = static_cast<TagListViewItem*>(listView->getItem(0)))
                    {
                        item->selectedItem();
                    }
                }

                HWParaDelTag delTagUI(tagID);
                HWDocTag::getInstance()->SubmitRequest(nullptr, HWReq_Tag_Del, &delTagUI, 0, false, false);
            }
        }
    }
    else if (b == buttonRecomend)
    {

    }
    else if (b == buttonCfmOK)
    {
        if (owner.rowNumber != TagListItemNewID)
        {
            showExpandMenu();
            // HWParaTagName tagNameUI(owner.userTabInfo.nId, textLabelChangedName);
            // HWDocTag::getInstance()->SubmitRequest(nullptr, HWReq_Tag_ModName, &tagNameUI, 0, false, false);
        }
        else
        {
            owner.setItemExpand(true);

            // 发送新建分类请求
            // HWParaAddTag addTagUI(textLabelChangedName, false);
            // HWDocTag::getInstance()->SubmitRequest(nullptr, HWReq_Tag_Add, &addTagUI, 0, false, false);
        }
    }
    else if (b == buttonCfmCancel)
    {
        if (owner.rowNumber != TagListItemNewID)
            showExpandMenu();
        else
            owner.setItemExpand(true);
    }
}

void TagListViewItem::TagListItem::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{

}

void TagListViewItem::TagListItem::textEditorTextChanged (juce::TextEditor&)
{

}

void TagListViewItem::TagListItem::textEditorReturnKeyPressed (juce::TextEditor&)
{

}

void TagListViewItem::TagListItem::textEditorEscapeKeyPressed (juce::TextEditor&)
{

}

void TagListViewItem::TagListItem::textEditorFocusLost (juce::TextEditor&)
{

}

//////////////////////////////////////////////////////////////////////////
//
TagListViewItem* TagListViewItem::expandItemRowNumber = nullptr;
TagListViewItem* TagListViewItem::selectedItemRowNumber = nullptr;

TagListViewItem::TagListViewItem(LeftShelfComponent& o, int id)
    : owner(o)
    , tagListItem(nullptr)
    , rowNumber(id)
{
    if (rowNumber != TagListItemNewID)
    {
        if (const HWTUserTag* tag = HWDocTag::getInstance()->GetUserTag(id))
        {
            userTabInfo = *tag;
        }
        else
        {
            memset(&userTabInfo, 0, sizeof(HWTUserTag));
        }
    }
}

TagListViewItem::~TagListViewItem()
{

}

juce::Component* TagListViewItem::createItemComponent(int row)
{
    return tagListItem = new TagListItem(*this);
}

int TagListViewItem::getItemHeight(int row)
{
    return (tagListItem && isItemExpanded()) ? 2 * TagListItemDefaultHeight : TagListItemDefaultHeight;
}

void TagListViewItem::paintItem(int row, juce::Graphics& g)
{

}

void TagListViewItem::itemDoubleClicked(const juce::MouseEvent& e)
{
    jassert(tagListItem);
    if (e.getPosition().y < TagListItemDefaultHeight || 
        (expandItemRowNumber && expandItemRowNumber->tagListItem && !expandItemRowNumber->tagListItem->isExpandConformShow()))
    {
        selectedItem();
    }
}

void TagListViewItem::itemClicked(const juce::MouseEvent& e)
{
    jassert(tagListItem);
    selectedItem();
}

bool TagListViewItem::isItemExpanded() const
{
    return expandItemRowNumber && expandItemRowNumber == this;
}

void TagListViewItem::setItemExpand(bool collapse)
{
    TagListViewItem* willChangedItem = collapse || (expandItemRowNumber && expandItemRowNumber == this) ? nullptr : this;

    if (willChangedItem != expandItemRowNumber)
    {
        if (expandItemRowNumber && expandItemRowNumber->tagListItem)
        {
            expandItemRowNumber->tagListItem->closeExpand();
        }

        expandItemRowNumber = willChangedItem;
        owner.tagListView->updatePosition();

        if (willChangedItem)
        {
            if (rowNumber != TagListItemNewID)
            {
                if (!tagListItem->isExpandMenuShow())
                    tagListItem->showExpandMenu();
            }
            else
            {
                if (!tagListItem->isExpandConformShow())
                {
                    tagListItem->showExpandConform();
                    tagListItem->labelName->showEditor();
                }
            }
        }
    }
}

bool TagListViewItem::isItemMouseHoverd() const
{
    return owner.tagListView->getHoverdItem() == rowNumber;
}

bool TagListViewItem::isItemMouseSelected() const
{
     return selectedItemRowNumber && selectedItemRowNumber == this;
}

void TagListViewItem::selectedItem()
{
    if (tagListItem && tagListItem->labelName && 
        !tagListItem->labelName->isBeingEdited() && 
        tagListItem->isExpandConformShow())
    {
        tagListItem->labelName->showEditor();
        return;
    }

    if (rowNumber == 0)
    {
        setItemExpand(true);
    }
    else
    {
        setItemExpand();
    }

    if ( selectedItemRowNumber != this && rowNumber != TagListItemNewID)
    {
        selectedItemRowNumber = this;

        if (owner.rightContentNotify)
        {
            owner.rightContentNotify->OnTagListSelectedChanged(&userTabInfo);
        }
    }
}
