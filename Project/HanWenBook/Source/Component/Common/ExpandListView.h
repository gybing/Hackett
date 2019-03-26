#ifndef JUCE_EXPANDLISTVIEW_H_INCLUDED
#define JUCE_EXPANDLISTVIEW_H_INCLUDED

class ExpandListView;

class ExpandListViewModel
{
public:
    virtual void listWasScrolled() = 0;
};

class ExpandListViewItem
{
public:
    ExpandListViewItem();
    virtual ~ExpandListViewItem();

    void setOwnerView (ExpandListView*) noexcept;
    ExpandListView* getOwnerView() const noexcept;

    juce::Rectangle<int> getItemPosition (bool relativeToTreeViewTopLeft) const noexcept;
    void updatePositions (int rowNumber, int newY);
    void repaintItem() const;

    virtual juce::Component* createItemComponent(int rowNumber) { return nullptr; }
    virtual int getItemHeight(int rowNumber) = 0;
    virtual void paintItem (int rowNumber, juce::Graphics& g) {}
    virtual void itemClicked (const juce::MouseEvent& e) {}
    virtual void itemDoubleClicked (const juce::MouseEvent& e) {}

protected:
    int uid, y, itemHeight, itemWidth;
    ExpandListView* ownerView;
    friend class ExpandListView;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExpandListViewItem)
};

class ExpandListView
    : public juce::Component
{
public:
    ExpandListView(ExpandListViewModel* m = nullptr);
    ~ExpandListView();

    ExpandListViewModel* getModel();

    void addItem (ExpandListViewItem* newItem, int insertPosition = -1);
    int getNumItems() const noexcept;
    ExpandListViewItem* getItem (int index) const noexcept;
    void removeItem (int index, bool deleteItem = true);
    void clearItems();
    juce::Viewport* getViewport() const noexcept;
    void updatePosition();
    int getRowContainingPosition (int x, int y) const noexcept;
    void setHoverSelector(bool);
    int getHoverdItem() const;

    template <class ElementComparator> void addSubItemSorted (ElementComparator& comparator, ExpandListViewItem* newItem);
    template <class ElementComparator> void sortSubItems (ElementComparator& comparator);

private:
    //==============================================================================
    virtual void paint (juce::Graphics&) override;
    virtual void resized() override;
    virtual void enablementChanged() override;

    void recalculateIfNeeded();
    void itemsChanged() noexcept;
    bool removeItemFromList (int index, bool deleteItem);
    void removeAllItemsFromList();
    void paintItems (juce::Graphics&);
    void updateItemsPosition();

private:
    class ExpandListViewMouseHoverSelector;
    class ExpandListViewMouseHoverSelectorDelegate;
    class ListViewport;
    class ContentComponent;
    friend class ExpandListViewItem;
    friend class ContentComponent;
    friend class ExpandListViewMouseHoverSelector;
    friend class ExpandListViewMouseHoverSelectorDelegate;
    friend struct juce::ContainerDeletePolicy<ListViewport>;

    ExpandListViewModel* model;
    juce::ScopedPointer<ExpandListViewMouseHoverSelector> listviewHoverSelector;
    juce::ScopedPointer<ExpandListViewMouseHoverSelectorDelegate> listviewHoverSelectorDelegate;
    int totalHeight;
    bool needsRecalculating;
    juce::ScopedPointer<ListViewport> viewport;
    juce::OwnedArray<ExpandListViewItem> subItems;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExpandListView)
};

#endif // JUCE_EXPANDLISTVIEW_H_INCLUDED
