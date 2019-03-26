#pragma once
#include "BookCellComponent.h"

class RightContentNotify
{
public:
    virtual void OnTagListSelectedChanged(const HWTUserTag* tag) = 0;
};

class ShelfPanelComponent;
class RightShelfComponent : public juce::Component
{
public:
    RightShelfComponent();
    ~RightShelfComponent();

    RightContentNotify* getRightContentNotify();

protected:
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::ScopedPointer<ShelfPanelComponent> m_pShelfPanelComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RightShelfComponent)
};

class ShelfPanelComponent : 
    public juce::Component, 
    public ShelfViewModel, 
    public RightContentNotify, 
    public juce::DragAndDropContainer,
    public juce::DragAndDropTarget, 
    public HWRequestor,
    public IHWSubjectTag
{
public:
    ShelfPanelComponent();
    ~ShelfPanelComponent();

protected:
    virtual void OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, 
        HWEnum_BusiID busi, HWContext ctx, HWParaUI* para);

    virtual void resized() override;

    // ShelfViewModel override
    virtual juce::Component* refreshItemComponentForIdx (int idx, bool isSelected, juce::Component* existingComponentToUpdate) override;
    virtual int getNumItems() override;
    virtual void shelfItemClicked (int idx, const juce::MouseEvent&) override;
    virtual void shelfItemDoubleClicked (int idx, const juce::MouseEvent&) override;
    virtual juce::var getDragSourceDescription (const juce::SparseSet<int>& rowsToDescribe) override;
    virtual juce::String getTooltipForItem (int idx) override;
    virtual void shelfWasScrolled() override;
    virtual void selectedItemsChanged(int lastSelectedItem) override;
    virtual void returnKeyPressed (int lastSelectedItem) override;
    virtual void deleteKeyPressed (int lastSelectedItem) override;
    virtual void backgroundClicked (const juce::MouseEvent&) override;
    virtual void paintCellItem (int idx, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    virtual juce::MouseCursor getMouseCursorForItem(int idx) override;
    virtual void paintColPanel (int col, juce::Graphics& g, int x, int y, int width, int height);

    virtual bool isInterestedInDragSource (const SourceDetails& dragSourceDetails);
    virtual void itemDragEnter (const SourceDetails& dragSourceDetails);
    virtual void itemDragMove (const SourceDetails& dragSourceDetails);
    virtual void itemDragExit (const SourceDetails& dragSourceDetails);
    virtual void itemDropped (const SourceDetails& dragSourceDetails);
    virtual void dragOperationStarted();
    virtual void dragOperationEnded();

protected:
    virtual void OnTagListSelectedChanged(const HWTUserTag* tag);
    virtual void OnBookPosChange();

    void dragMoveOnBook(int dest);
    void moveBook(int src, int dst);
private:
    HWTUserTag m_UserTag;
    HWDocTag::HWBookList m_arrBookList;
    juce::ScopedPointer<ShelfViewComponent> m_pShelfPanel;
    juce::Image m_imageColBackground;
    std::vector<int> m_vecDragBookIds;
    juce::SparseSet<int> m_setItemsToDrag;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShelfPanelComponent)
};
