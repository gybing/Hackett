#include "JuceHeader.h"
#include "ListView.h"

//////////////////////////////////////////////////////////////////////////
// ContentComponent
class ListView::ContentComponent
    : public juce::Component
    , public juce::AsyncUpdater
{
public:
    ContentComponent (ListView& o)
        : owner (o),
        buttonUnderMouse (nullptr),
        isDragging (false),
        needSelectionOnMouseUp (false)
    {
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        if (isEnabled())  // ignore triple clicks
        {
            juce::Rectangle<int> pos;
            if (ListViewItem* const item = findItemAt (e.y, pos))
                if (e.x >= pos.getX())
                    item->itemClicked (e.withNewPosition (e.position - pos.getPosition().toFloat()));
        }
    }

    void mouseDoubleClick (const juce::MouseEvent& e) override
    {
        if (e.getNumberOfClicks() != 3 && isEnabled())  // ignore triple clicks
        {
            juce::Rectangle<int> pos;
            if (ListViewItem* const item = findItemAt (e.y, pos))
                if (e.x >= pos.getX())
                    item->itemDoubleClicked (e.withNewPosition (e.position - pos.getPosition().toFloat()));
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

    ListViewItem* findItemAt (int targetY, juce::Rectangle<int>& itemPosition) const
    {
        if (owner.subItems.size() > 0)
        {
            owner.recalculateIfNeeded();

            if (juce::isPositiveAndBelow (targetY, owner.totalHeight))
            {
                for (int i = 0; i < owner.subItems.size(); ++i)
                {
                    ListViewItem* const ti = owner.subItems.getUnchecked(i);

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
            ListViewItem* item = nullptr;
            int y = 0;
            for (unsigned int i=0; i<owner.subItems.size(); ++i)
            {
                if (ListViewItem* item = owner.subItems.getUnchecked(i))
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
    ListView& owner;

    struct RowItem
    {
        RowItem (ListViewItem* const it, juce::Component* const c, const int itemUID)
            : component (c), item (it), uid (itemUID), shouldKeep (true)
        {
        }

        ~RowItem()
        {
            delete component.get();
        }

        juce::WeakReference<juce::Component> component;
        ListViewItem* item;
        int uid;
        bool shouldKeep;
    };

    juce::OwnedArray <RowItem> items;

    ListViewItem* buttonUnderMouse;
    bool isDragging, needSelectionOnMouseUp;

    bool containsItem (ListViewItem* const item) const noexcept
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
class ListView::ListViewport  : public juce::Viewport
{
public:
    ListViewport(ListView& o) noexcept : lastX (-1), owner(o)    {}

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

        if (ListViewModel* m = owner.getModel())
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
    ListView& owner;
    int lastX;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListViewport)
};

//////////////////////////////////////////////////////////////////////////
// ListViewItem
ListViewItem::ListViewItem()
    : ownerView (nullptr),
    y (0),
    itemHeight (0),
    itemWidth (0)
{
    static int nextUID = 0;
    uid = nextUID++;
}

ListViewItem::~ListViewItem()
{
}

void ListViewItem::setOwnerView (ListView* newOwner) noexcept
{
    ownerView = newOwner;
}

ListView* ListViewItem::getOwnerView() const noexcept
{
    return ownerView;
}

void ListViewItem::repaintItem() const
{
    if (ownerView != nullptr)
    {
        juce::Rectangle<int> r (getItemPosition (true));
        r.setLeft (0);
        ownerView->viewport->repaint (r);
    }
}

juce::Rectangle<int> ListViewItem::getItemPosition(bool relativeToTreeViewTopLeft) const noexcept
{
    juce::Rectangle<int> r (0, y, itemWidth, itemHeight);
    if (relativeToTreeViewTopLeft && ownerView != nullptr)
        r -= ownerView->viewport->getViewPosition();

    return r;
}

void ListViewItem::updatePositions(int rowNumber, int newY)
{
    y = newY;
    itemHeight = getItemHeight(rowNumber);
    itemWidth = ownerView->getWidth();
}

//////////////////////////////////////////////////////////////////////////
// ListViewItem
ListView::ListView(ListViewModel* m /*= nullptr*/)
    : needsRecalculating (true)
    , viewport(new ListViewport(*this))
    , model(m)
{
    addAndMakeVisible (viewport);
    viewport->setViewedComponent (new ContentComponent (*this));
    setWantsKeyboardFocus (true);
}

ListView::~ListView()
{
}

void ListView::recalculateIfNeeded()
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

void ListView::itemsChanged() noexcept
{
    needsRecalculating = true;
    repaint();
    viewport->getContentComp()->triggerAsyncUpdate();
}

void ListView::addItem(ListViewItem* newItem, int insertPosition /*= -1*/)
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

int ListView::getNumItems() const noexcept
{
    return subItems.size();
}

ListViewItem* ListView::getItem (int index) const noexcept
{
    return subItems [index];
}

void ListView::clearItems()
{
    if (subItems.size() > 0)
    {
        removeAllItemsFromList();
        itemsChanged();
    }
}

void ListView::removeAllItemsFromList()
{
    for (int i = subItems.size(); --i >= 0;)
        removeItemFromList (i, true);
}

bool ListView::removeItemFromList(int index, bool deleteItem)
{
    if (ListViewItem* child = subItems [index])
    {
        subItems.remove (index, deleteItem);
        return true;
    }

    return false;
}

void ListView::removeItem (int index, bool deleteItem /*= true*/)
{
    if (removeItemFromList (index, deleteItem))
        itemsChanged();
}

template <class ElementComparator>
void ListView::sortSubItems(ElementComparator& comparator)
{
    subItems.sort (comparator);
}

template <class ElementComparator>
void ListView::addSubItemSorted(ElementComparator& comparator, ListViewItem* newItem)
{
    addSubItem (newItem, findInsertIndexInSortedArray (comparator, subItems.begin(), newItem, 0, subItems.size()));
}

void ListView::paintItems(juce::Graphics& g)
{
    const juce::Rectangle<int> clip (g.getClipBounds());
    const int itemW = getWidth();

    for (int i = 0; i < subItems.size(); ++i)
    {
        ListViewItem* const ti = subItems.getUnchecked(i);

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

void ListView::updateItemsPosition()
{
    totalHeight = 0;
    for (int i = 0; i < subItems.size(); ++i)
    {
        ListViewItem* const ti = subItems.getUnchecked(i);

        ti->updatePositions (i, totalHeight);
        totalHeight += ti->itemHeight;
    }
}

void ListView::paint(juce::Graphics& g)
{
    g.fillAll (ProjectInfo::colorLightWhite);
}

void ListView::resized()
{
    updatePosition();
}

void ListView::enablementChanged()
{
    repaint();
}

juce::Viewport* ListView::getViewport() const noexcept
{
    return viewport;
}

void ListView::updatePosition()
{
    viewport->setBounds (getLocalBounds());

    itemsChanged();
    recalculateIfNeeded();
}

ListViewModel* ListView::getModel()
{
    return model;
}
