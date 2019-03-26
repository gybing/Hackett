#ifndef JUCE_LISTVIEW_H_INCLUDED
#define JUCE_LISTVIEW_H_INCLUDED

class ListView;

class ListViewModel
{
public:
    virtual void listWasScrolled() = 0;
};

class ListViewItem
{
public:
    ListViewItem();
    virtual ~ListViewItem();

    void setOwnerView (ListView*) noexcept;
    ListView* getOwnerView() const noexcept;

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
    ListView* ownerView;
    friend class ListView;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListViewItem)
};

class ListView : public juce::Component
{
public:
    ListView(ListViewModel* m = nullptr);
    ~ListView();

    ListViewModel* getModel();

    void addItem (ListViewItem* newItem, int insertPosition = -1);
    int getNumItems() const noexcept;
    ListViewItem* getItem (int index) const noexcept;
    void removeItem (int index, bool deleteItem = true);
    void clearItems();
    juce::Viewport* getViewport() const noexcept;
    void updatePosition();

    template <class ElementComparator> void addSubItemSorted (ElementComparator& comparator, ListViewItem* newItem);
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
    class ListViewport;
    class ContentComponent;
    friend class ListViewItem;
    friend class ContentComponent;
    friend struct juce::ContainerDeletePolicy<ListViewport>;

    ListViewModel* model;
    int totalHeight;
    bool needsRecalculating;
    juce::ScopedPointer<ListViewport> viewport;
    juce::OwnedArray<ListViewItem> subItems;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListView)
};

#endif // JUCE_LISTVIEW_H_INCLUDED
