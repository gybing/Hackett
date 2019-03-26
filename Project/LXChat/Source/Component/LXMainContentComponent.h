#ifndef __LXCONTENT_COMPONENT_HH__
#define __LXCONTENT_COMPONENT_HH__

//==============================================================================
class LXMainContentComponent : public Component, public LXGridViewModel, public DragAndDropContainer, public LassoSource<Component*>, public ChangeListener
{
public:
    //==============================================================================
    LXMainContentComponent();
    ~LXMainContentComponent();

    void initDefaultState();

protected:
    virtual void paint(Graphics&) override;
    virtual void resized() override;

    virtual int getNumItems() override;
    virtual void paintGridViewItem(int index, Graphics& g, int width, int height, bool rowIsSelected) override;
    virtual void gridViewItemClicked(int index, const MouseEvent&) override;
    virtual var getDragSourceDescription(const SparseSet<int>& rowsToDescribe) override;
    virtual void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    virtual void findLassoItemsInArea(Array<Component*>& itemsFound, const juce::Rectangle<int>& area) override;
    virtual SelectedItemSet<Component*>& getLassoSelection() override;
    virtual void mouseDown(const MouseEvent& event) override;
    virtual void mouseDrag(const MouseEvent& event) override;
    virtual void mouseUp(const MouseEvent& event) override;

    void clearCurrentApp();
    void selectedApp(int index);
    int getCurrentAppIndex() const noexcept;
private:
    ScopedPointer<LXGridView> appGridView;
    ScopedPointer<Component> currentApp;
    LXComponentAnimator animator;
    LassoComponent<Component*> lasso;  // LassoComponent套索对象
    SelectedItemSet<Component*> selectItems;    // 所选择的项目集合

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LXMainContentComponent)
};

#endif // __LXCONTENT_COMPONENT_HH__