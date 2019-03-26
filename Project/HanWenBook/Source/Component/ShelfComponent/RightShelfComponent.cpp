#include "JuceHeader.h"
#include "RightShelfComponent.h"
#include "BookCellComponent.h"

#define PANELBOOKCOUNT 16

const char* const BookCellDescripton = "_bookcellitem_";

RightShelfComponent::RightShelfComponent()
{
    addAndMakeVisible(m_pShelfPanelComponent = new ShelfPanelComponent);
}

RightShelfComponent::~RightShelfComponent()
{
}

void RightShelfComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFFb4b4b4));
}

void RightShelfComponent::resized()
{
    m_pShelfPanelComponent->setBounds(5, 0, getWidth() - 50, getHeight()+1);
}

RightContentNotify* RightShelfComponent::getRightContentNotify()
{
    return m_pShelfPanelComponent;
}

//////////////////////////////////////////////////////////////////////////
// ShelfPanelComponent

ShelfPanelComponent::ShelfPanelComponent()
    : m_imageColBackground(CXIMAGE_CACHE_LOAD(L"shelf_bk.png"))
{
    HWDocTag::getInstance()->Register(this);
    addAndMakeVisible (m_pShelfPanel = new ShelfViewComponent (juce::String::empty, this));
    m_pShelfPanel->getViewport()->setSingleStepSizes(100, 100);
    m_pShelfPanel->setMultipleSelectionEnabled(true);
}

ShelfPanelComponent::~ShelfPanelComponent()
{
    HWDocTag::getInstance()->UnRegister(this);
}

//////////////////////////////////////////////////////////////////////////
// ListBoxModule


void ShelfPanelComponent::resized()
{
    const int nWidth = getWidth();
    const int nHeight = getHeight();

    m_pShelfPanel->setBounds(0, 0, nWidth, nHeight);

    const int nPanelWidth = (int)((nHeight*ProjectInfo::DefaultBookPanelWidth) / ProjectInfo::DefaultBookPanelHeight);
    if (nPanelWidth != m_pShelfPanel->getColWidth())
    {
        m_pShelfPanel->setColWidth(nPanelWidth);    // 设置列表框中每一行的行高

        // 计算图书位置
        float marginX = 0.12f * nPanelWidth;
        const float stepX = (nPanelWidth - marginX) / 8.0f;
        marginX = marginX/2.0f;

        const float posX[4] = {marginX + stepX, marginX + (stepX*3.0f), marginX + (stepX*5.0f), marginX + (stepX*7.0f)};
        const float posY[4] = {0.267f*nHeight, 0.497f*nHeight, 0.728f*nHeight, 0.959f*nHeight};

        const float bookW = 0.192f * nPanelWidth;
        const float bookH = 0.1837f * getHeight();
        CXOption::getInstance()->CaculateBookPosition(posX, posY, bookW, bookH);
    }
}

void ShelfPanelComponent::OnTagListSelectedChanged(const HWTUserTag* tag)
{
    m_arrBookList.clear();
    if (tag)
    {
        m_UserTag = *tag;
        HWDocTag::getInstance()->GetUserBookListFromTag(m_UserTag.nId, m_arrBookList);
    }
    else
    {
        m_UserTag.nId = -1;
        m_UserTag.strName.clear();
    }
    m_pShelfPanel->updateContent();
}

juce::Component* ShelfPanelComponent::refreshItemComponentForIdx (int idx, bool isSelected, juce::Component* existingComponentToUpdate)
{
    BookCellComponent* pItem = existingComponentToUpdate ? static_cast<BookCellComponent*>(existingComponentToUpdate) : new BookCellComponent(*m_pShelfPanel);
    jassert(pItem);

    if (idx >=0 && idx < m_arrBookList.size())
    {
        pItem->setData(m_arrBookList[idx], idx);
    }
    else
    {
        pItem->setData(nullptr, -1);
    }

    pItem->repaint();
    pItem->setInterceptsMouseClicks(false, true);
    return pItem;
}

void ShelfPanelComponent::shelfItemClicked (int idx, const juce::MouseEvent& e)
{
    BookCellComponent* item = static_cast<BookCellComponent*>(m_pShelfPanel->getComponentForItemIdx(idx));
    if (item)
    {
        if (e.mods.isLeftButtonDown())
            PreviewComponent::showPreviewWindow(this, item->getUserBook().strBook);
    }
}

void ShelfPanelComponent::shelfItemDoubleClicked (int idx, const juce::MouseEvent&)
{
    BookCellComponent* item = static_cast<BookCellComponent*>(m_pShelfPanel->getComponentForItemIdx(idx));
    if (item)
    {
        ReadWindowHelper::getInstance()->readBook(item->getUserBook().strBook);
    }
}

juce::String ShelfPanelComponent::getTooltipForItem (int idx)
{
    return juce::String::empty;
}

void ShelfPanelComponent::shelfWasScrolled()
{

}

int ShelfPanelComponent::getNumItems()
{
    return m_arrBookList.size();
}

void ShelfPanelComponent::selectedItemsChanged(int lastSelectedItem)
{

}

void ShelfPanelComponent::deleteKeyPressed (int lastSelectedItem)
{

}

void ShelfPanelComponent::backgroundClicked (const juce::MouseEvent&)
{

}

void ShelfPanelComponent::paintCellItem (int idx, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
}

juce::MouseCursor ShelfPanelComponent::getMouseCursorForItem(int idx)
{
    return juce::MouseCursor::NormalCursor;
}

juce::var ShelfPanelComponent::getDragSourceDescription(const juce::SparseSet<int>& rowsToDescribe)
{
    return BookCellDescripton;
}

void ShelfPanelComponent::returnKeyPressed(int lastSelectedItem)
{

}

void ShelfPanelComponent::paintColPanel(int col, juce::Graphics& g, int x, int y, int width, int height)
{
    g.drawImage(m_imageColBackground, x, y, width, height, 0, 0, m_imageColBackground.getWidth(), m_imageColBackground.getHeight());

    int size20 = width*0.04f;
    int size50 = width*0.1f;
    g.setFont(juce::Font(size20));
    g.setColour(juce::Colour(0xFFa0a0a0));
    g.drawText(juce::String(col), x+size20, y+size20, size50, size20, juce::Justification::centredLeft);
    g.drawText(m_UserTag.strName, x, y+size20, width, size20, juce::Justification::centred);
}

bool ShelfPanelComponent::isInterestedInDragSource (const SourceDetails& dragSourceDetails)
{
    return dragSourceDetails.description == BookCellDescripton;
}

void ShelfPanelComponent::itemDragEnter (const SourceDetails& dragSourceDetails)
{

}

void ShelfPanelComponent::itemDragMove (const SourceDetails& dragSourceDetails)
{
   m_pShelfPanel->moveSelectedItemAnimate(dragSourceDetails);
}

void ShelfPanelComponent::itemDragExit (const SourceDetails& dragSourceDetails)
{

}

void ShelfPanelComponent::itemDropped (const SourceDetails& dragSourceDetails)
{
    jassert(m_arrBookList.size() >= m_vecDragBookIds.size());

    int dest = m_pShelfPanel->getLastDragOverItem();
    if (dest == -1)
        return;

    if (dest >= m_arrBookList.size())
        dest = m_arrBookList.size() - m_vecDragBookIds.size();

    jassert(!m_arrBookList.empty() && dest<m_arrBookList.size() && m_arrBookList[dest]);

    const int nDestID = m_arrBookList[dest]->nId;
    for (unsigned int i=0; i<m_vecDragBookIds.size(); ++i)
    {
        if (nDestID == m_vecDragBookIds[i])
            return;
    }

    int nDestPosInfo = (dest<m_setItemsToDrag[0]) ? 0 : ((dest>m_setItemsToDrag[m_setItemsToDrag.size()-1]) ? 1 : 2);
    HWDocTag::getInstance()->ChangeBookPos(m_vecDragBookIds, nDestID, nDestPosInfo);
    HWDocTag::getInstance()->SubmitChangeBookPos(nullptr);
}

void ShelfPanelComponent::dragOperationStarted()
{
    m_vecDragBookIds.clear();
    m_setItemsToDrag = m_pShelfPanel->getSelectedItems();
    for (unsigned int i=0; i<m_setItemsToDrag.size(); ++i)
    {
        const int idx = m_setItemsToDrag[i];
        juce::Component* comp = m_pShelfPanel->getComponentForItemIdx(idx);
        if (comp) comp->setVisible(false);
        m_vecDragBookIds.push_back(m_arrBookList[idx]->nId);
    }
}

void ShelfPanelComponent::dragOperationEnded()
{
    for (int i=0; i<m_setItemsToDrag.size(); ++i)
    {
        juce::Component* comp = m_pShelfPanel->getComponentForItemIdx(m_setItemsToDrag[i]);
        if (comp) comp->setVisible(true);
    }
}

void ShelfPanelComponent::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
}

void ShelfPanelComponent::OnBookPosChange()
{
    m_arrBookList.clear();
    HWDocTag::getInstance()->GetUserBookListFromTag(m_UserTag.nId, m_arrBookList);
    m_pShelfPanel->updateContent();
}
