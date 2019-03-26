#include "JuceHeader.h"
#include "ExpandListView.h"

//////////////////////////////////////////////////////////////////////////
// ContentComponent
class ExpandListView::ContentComponent
    : public juce::Component
    , public juce::AsyncUpdater
{
public:
    ContentComponent (ExpandListView& o)
        : owner (o),
        buttonUnderMouse (nullptr),
        isDragging (false),
        needSelectionOnMouseUp (false)
    {
    }

    void mouseDoubleClick (const juce::MouseEvent& e) override
    {
        if (e.getNumberOfClicks() != 3 && isEnabled())  // ignore triple clicks
        {
            juce::Rectangle<int> pos;
            if (ExpandListViewItem* const item = findItemAt (e.y, pos))
                if (e.x >= pos.getX())
                    item->itemDoubleClicked (e.withNewPosition (e.position - pos.getPosition().toFloat()));
        }
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        if (isEnabled())  // ignore triple clicks
        {
            juce::Rectangle<int> pos;
            if (ExpandListViewItem* const item = findItemAt (e.y, pos))
                if (e.x >= pos.getX())
                    item->itemClicked (e.withNewPosition (e.position - pos.getPosition().toFloat()));
        }
    }

    void paint (juce::Graphics& g) override
    {
        if (owner.subItems.size() > 0)
        {
            owner.recalculateIfNeeded();
            owner.paintItems(g);
        }
    }

    ExpandListViewItem* findItemAt (int targetY, juce::Rectangle<int>& itemPosition) const
    {
        if (owner.subItems.size() > 0)
        {
            owner.recalculateIfNeeded();

            if (juce::isPositiveAndBelow (targetY, owner.totalHeight))
            {
                for (int i = 0; i < owner.subItems.size(); ++i)
                {
                    ExpandListViewItem* const ti = owner.subItems.getUnchecked(i);

                    if (targetY < ti->itemHeight)
                    {
                        itemPosition = ti->getItemPosition (false);
                        return ti;
                    }
                    targetY -= ti->itemHeight;
                }
            }
        }

        return nullptr;
    }

    void updateComponents()
    {
        const int visibleTop = -getY();
        const int visibleBottom = visibleTop + getParentHeight();

        for (int i = items.size(); --i >= 0;)
            items.getUnchecked(i)->shouldKeep = false;

        {
            ExpandListViewItem* item = nullptr;
            int y = 0;
            for (unsigned int i=0; i<owner.subItems.size(); ++i)
            {
                if (ExpandListViewItem* item = owner.subItems.getUnchecked(i))
                {
                    if (y < visibleBottom)
                    {
                        y += item->itemHeight;

                        if (y >= visibleTop)
                        {
                            if (RowItem* const ri = findItem (item->uid))
                            {
                                ri->shouldKeep = true;
                            }
                            else if (juce::Component* const comp = item->createItemComponent(i))
                            {
                                items.add (new RowItem (item, comp, item->uid));
                                addAndMakeVisible (comp);
                            }
                        }
                    }
                }
            }
        }

        for (int i = items.size(); --i >= 0;)
        {
            RowItem* const ri = items.getUnchecked(i);
            bool keep = false;

            if (isParentOf (ri->component))
            {
                if (ri->shouldKeep)
                {
                    juce::Rectangle<int> pos (ri->item->getItemPosition (false));
                    pos.setSize (pos.getWidth(), ri->item->itemHeight);

                    if (pos.getBottom() >= visibleTop && pos.getY() < visibleBottom)
                    {
                        keep = true;
                        ri->component->setBounds (pos);
                    }
                }

                if ((! keep))
                {
                    keep = true;
                    ri->component->setSize (0, 0);
                }
            }

            if (! keep)
                items.remove (i);
        }
    }

    void resized() override
    {
        owner.itemsChanged();
    }

private:
    ExpandListView& owner;

    struct RowItem
    {
        RowItem (ExpandListViewItem* const it, juce::Component* const c, const int itemUID)
            : component (c), item (it), uid (itemUID), shouldKeep (true)
        {
        }

        ~RowItem()
        {
            delete component.get();
        }

        juce::WeakReference<juce::Component> component;
        ExpandListViewItem* item;
        int uid;
        bool shouldKeep;
    };

    juce::OwnedArray <RowItem> items;

    ExpandListViewItem* buttonUnderMouse;
    bool isDragging, needSelectionOnMouseUp;

    bool containsItem (ExpandListViewItem* const item) const noexcept
    {
        for (int i = items.size(); --i >= 0;)
            if (items.getUnchecked(i)->item == item)
                return true;

        return false;
    }

    RowItem* findItem (const int uid) const noexcept
    {
        for (int i = items.size(); --i >= 0;)
        {
            RowItem* const ri = items.getUnchecked(i);
            if (ri->uid == uid)
                return ri;
        }

        return nullptr;
    }

    void handleAsyncUpdate() override
    {
        owner.recalculateIfNeeded();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ContentComponent)
};

//////////////////////////////////////////////////////////////////////////
//
class ExpandListView::ListViewport  : public juce::Viewport
{
public:
    ListViewport(ExpandListView& o) noexcept : lastX (-1), owner(o)    {}

    void updateComponents (const bool triggerResize)
    {
        if (ContentComponent* const tvc = getContentComp())
        {
            if (triggerResize)
                tvc->resized();
            else
                tvc->updateComponents();
        }

        repaint();
    }

    void visibleAreaChanged (const juce::Rectangle<int>& newVisibleArea) override
    {
        const bool hasScrolledSideways = (newVisibleArea.getX() != lastX);
        lastX = newVisibleArea.getX();
        updateComponents (hasScrolledSideways);

        if (ExpandListViewModel* m = owner.getModel())
            m->listWasScrolled();
    }

    ContentComponent* getContentComp() const noexcept
    {
        return static_cast <ContentComponent*> (getViewedComponent());
    }

    bool keyPressed (const juce::KeyPress& key) override
    {
        juce::Component* const tree = getParentComponent();

        return (tree != nullptr && tree->keyPressed (key))
            || juce::Viewport::keyPressed (key);
    }

private:
    ExpandListView& owner;
    int lastX;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListViewport)
};

//////////////////////////////////////////////////////////////////////////
// ExpandListViewItem
ExpandListViewItem::ExpandListViewItem()
    : ownerView (nullptr),
    y (0),
    itemHeight (0),
    itemWidth (0)
{
    static int nextUID = 0;
    uid = nextUID++;
}

ExpandListViewItem::~ExpandListViewItem()
{
}

void ExpandListViewItem::setOwnerView (ExpandListView* newOwner) noexcept
{
    ownerView = newOwner;
}

ExpandListView* ExpandListViewItem::getOwnerView() const noexcept
{
    return ownerView;
}

void ExpandListViewItem::repaintItem() const
{
    if (ownerView != nullptr)
    {
        juce::Rectangle<int> r (getItemPosition (true));
        r.setLeft (0);
        ownerView->viewport->repaint (r);
    }
}

juce::Rectangle<int> ExpandListViewItem::getItemPosition(bool relativeToListViewTopLeft) const noexcept
{
    juce::Rectangle<int> r (0, y, itemWidth, itemHeight);
    if (relativeToListViewTopLeft && ownerView != nullptr)
        r -= ownerView->viewport->getViewPosition();

    return r;
}

void ExpandListViewItem::updatePositions(int rowNumber, int newY)
{
    y = newY;
    itemHeight = getItemHeight(rowNumber);
    itemWidth = ownerView->getWidth();
}

//////////////////////////////////////////////////////////////////////////
// ExpandListViewItem
ExpandListView::ExpandListView(ExpandListViewModel* m /*= nullptr*/)
    : needsRecalculating (true)
    , viewport(new ListViewport(*this))
    , model(m)
    , listviewHoverSelector(nullptr)
{
    addAndMakeVisible (viewport);
    viewport->setViewedComponent (new ContentComponent (*this));
    setWantsKeyboardFocus (true);
}

ExpandListView::~ExpandListView()
{
}

void ExpandListView::recalculateIfNeeded()
{
    if (needsRecalculating)
    {
        needsRecalculating = false;

        updateItemsPosition();
        viewport->updateComponents (false);

        const int itemCount = subItems.size();
        if (itemCount > 0)
        {
            viewport->getViewedComponent()->setSize (viewport->getMaximumVisibleWidth(), totalHeight);
        }
        else
        {
            viewport->getViewedComponent()->setSize (0, 0);
        }
    }
}

void ExpandListView::itemsChanged() noexcept
{
    needsRecalculating = true;
    repaint();
    viewport->getContentComp()->triggerAsyncUpdate();
}

void ExpandListView::addItem(ExpandListViewItem* newItem, int insertPosition /*= -1*/)
{
    if (newItem != nullptr)
    {
        newItem->setOwnerView (this);
        newItem->y = 0;
        newItem->itemHeight = 0;
        newItem->itemWidth = 0;
        subItems.insert (insertPosition, newItem);
        itemsChanged();
    }
}

int ExpandListView::getNumItems() const noexcept
{
    return subItems.size();
}

ExpandListViewItem* ExpandListView::getItem (int index) const noexcept
{
    return subItems [index];
}

void ExpandListView::clearItems()
{
    if (subItems.size() > 0)
    {
        removeAllItemsFromList();
        itemsChanged();
    }
}

void ExpandListView::removeAllItemsFromList()
{
    for (int i = subItems.size(); --i >= 0;)
        removeItemFromList (i, true);
}

bool ExpandListView::removeItemFromList(int index, bool deleteItem)
{
    if (ExpandListViewItem* child = subItems [index])
    {
        subItems.remove (index, deleteItem);
        return true;
    }

    return false;
}

void ExpandListView::removeItem (int index, bool deleteItem /*= true*/)
{
    if (removeItemFromList (index, deleteItem))
        itemsChanged();
}

template <class ElementComparator>
void ExpandListView::sortSubItems(ElementComparator& comparator)
{
    subItems.sort (comparator);
}

template <class ElementComparator>
void ExpandListView::addSubItemSorted(ElementComparator& comparator, ExpandListViewItem* newItem)
{
    addSubItem (newItem, findInsertIndexInSortedArray (comparator, subItems.begin(), newItem, 0, subItems.size()));
}

void ExpandListView::paintItems(juce::Graphics& g)
{
    const juce::Rectangle<int> clip (g.getClipBounds());
    const int itemW = getWidth();

    for (int i = 0; i < subItems.size(); ++i)
    {
        ExpandListViewItem* const ti = subItems.getUnchecked(i);

        const int relY = ti->y;

        if (relY >= clip.getBottom())
            break;

        if (relY + ti->itemHeight >= clip.getY())
        {
            juce::Graphics::ScopedSaveState ss (g);
            g.setOrigin (0, relY);
            if (g.reduceClipRegion (0, 0, itemW, ti->itemHeight))
                ti->paintItem (i, g);
        }
    }
}

void ExpandListView::updateItemsPosition()
{
    totalHeight = 0;
    for (int i = 0; i < subItems.size(); ++i)
    {
        ExpandListViewItem* const ti = subItems.getUnchecked(i);

        ti->updatePositions (i, totalHeight);
        totalHeight += ti->itemHeight;
    }
}

void ExpandListView::paint(juce::Graphics& g)
{
    g.fillAll (ProjectInfo::colorLightWhite);
}

void ExpandListView::resized()
{
    updatePosition();
}

void ExpandListView::enablementChanged()
{
    repaint();
}

juce::Viewport* ExpandListView::getViewport() const noexcept
{
    return viewport;
}

void ExpandListView::updatePosition()
{
    viewport->setBounds (getLocalBounds());

    itemsChanged();
    recalculateIfNeeded();
}

ExpandListViewModel* ExpandListView::getModel()
{
    return model;
}

int ExpandListView::getRowContainingPosition(int x, int y) const noexcept
{
    if (juce::isPositiveAndBelow (x, getWidth()))
    {
        const int totalItems = getNumItems();
        int totalHeight = 0;

        if (subItems.size() > 0)
        {
            for (int i = 0; i < subItems.size(); ++i)
            {
                ExpandListViewItem* const ti = subItems.getUnchecked(i);
                ti->updatePositions (i, totalHeight);
                totalHeight += ti->itemHeight;

                if (totalHeight >= y)
                {
                    if (juce::isPositiveAndBelow (i, totalItems))
                        return i;
                }
            }
        }
    }

    return -1;
}

//////////////////////////////////////////////////////////////////////////
//

class ExpandListView::ExpandListViewMouseHoverSelector : public juce::MouseListener
{
public:
    class Listener { public: 
    virtual void listViewRowHover() = 0;
    };

    ExpandListViewMouseHoverSelector (ExpandListView& lb, ExpandListView::ExpandListViewMouseHoverSelector::Listener* l = nullptr) 
        : listview (lb)
        , rowNumber(-1)
        , listener(l)
    {
        lb.addMouseListener (this, true);
    }

    void setListener(ExpandListView::ExpandListViewMouseHoverSelector::Listener* l) { listener = l; }
    void mouseExit (const juce::MouseEvent& e) override { mouseMove (e); }
    void mouseMove (const juce::MouseEvent& e) override
    {
        const juce::MouseEvent e2 (e.getEventRelativeTo (&listview));

        const int row = (e2.x < listview.getViewport()->getViewWidth()) ? listview.getRowContainingPosition (e2.x, e2.y) : -1;

        if (lastRowNumber != rowNumber ||
            rowNumber != row)
        {
            lastRowNumber = rowNumber;
            rowNumber = row;

            if (listener) listener->listViewRowHover();
        }
    }

    int rowNumber;
    int lastRowNumber;

private:
    ExpandListView& listview;
    ExpandListView::ExpandListViewMouseHoverSelector::Listener* listener;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExpandListViewMouseHoverSelector)
};

class ExpandListView::ExpandListViewMouseHoverSelectorDelegate
    : public ExpandListViewMouseHoverSelector::Listener
{
public:
    ExpandListViewMouseHoverSelectorDelegate::ExpandListViewMouseHoverSelectorDelegate(ExpandListView& lv, ExpandListView::ExpandListViewMouseHoverSelector& es)
        : listview(lv), listviewselector(es)
    {
    }

    virtual void listViewRowHover() override
    {
        if (listviewselector.rowNumber >=0 && listviewselector.rowNumber < listview.getNumItems())
            listview.getItem(listviewselector.rowNumber)->repaintItem();

        if (listviewselector.lastRowNumber >= 0 && listviewselector.lastRowNumber < listview.getNumItems())
            listview.getItem(listviewselector.lastRowNumber)->repaintItem();
    }

private:
    ExpandListView& listview;
    ExpandListView::ExpandListViewMouseHoverSelector& listviewselector;
};

void ExpandListView::setHoverSelector(bool e)
{
    if (e)
    {
        if (listviewHoverSelector == nullptr)
            listviewHoverSelector = new ExpandListViewMouseHoverSelector(*this, nullptr);

        if (listviewHoverSelectorDelegate == nullptr)
            listviewHoverSelectorDelegate = new ExpandListViewMouseHoverSelectorDelegate(*this, *listviewHoverSelector);

        listviewHoverSelector->setListener(listviewHoverSelectorDelegate.get());
    }
    else if (listviewHoverSelector != nullptr)
    {
        listviewHoverSelector = nullptr;
    }
}

int ExpandListView::getHoverdItem() const
{
    if (listviewHoverSelector)
        return listviewHoverSelector->rowNumber;

    return -1;
}
