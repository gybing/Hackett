#include "JuceHeader.h"
#include "LXGridView.h"

class LXGridView::ItemComponent
    : public Component
    , public TooltipClient
{
public:
    ItemComponent(LXGridView& lb)
        : owner(lb), itemIndex(-1),
        selected(false), isDragging(false), selectIndexOnMouseUp(false)
    {
    }

    void paint(Graphics& g) override
    {
        if (LXGridViewModel* m = owner.getModel())
            m->paintGridViewItem(itemIndex, g, getWidth(), getHeight(), selected);
    }

    void update(const int newIndex, const bool nowSelected)
    {
        if (itemIndex != newIndex || selected != nowSelected)
        {
            repaint();
            itemIndex = newIndex;
            selected = nowSelected;
        }

        if (getAlpha() < 0.9f)
            setAlpha(1.0f);

        if (LXGridViewModel* m = owner.getModel())
        {
            setMouseCursor(m->getMouseCursorForIndex(itemIndex));

            customComponent = m->refreshComponentForIndex(newIndex, nowSelected, customComponent.release());

            if (customComponent != nullptr)
            {
                addAndMakeVisible(customComponent);
                customComponent->setBounds(getLocalBounds());
            }
        }
    } 

    void itemClicked(const MouseEvent& e)
    {
        LXComponentHelper::broughtComponentToFront(this);
        Rectangle<int> bounds = owner.getBounds();
        int offset = owner.getViewport()->getViewPositionY();
        bounds.setY(bounds.getY() + offset);
        bounds.expand(30, 30);
        owner.animator.animateComponent(this, bounds, 1.0f, 300, false, 0.0f, 0.0f, [=]() {
            if (LXGridViewModel* m = owner.getModel())
                m->gridViewItemClicked(itemIndex, e);
        });
    }

    void mouseDown(const MouseEvent& e) override
    {
        isDragging = false;
        selectIndexOnMouseUp = false;

        if (isEnabled())
        {
            if (owner.selectOnMouseDown && !selected)
            {
                owner.selectIndexsBasedOnModifierKeys(itemIndex, e.mods, false);

                itemClicked(e);
            }
            else
            {
                selectIndexOnMouseUp = true;
            }
        }
    }

    void mouseUp(const MouseEvent& e) override
    {
        if (isEnabled() && selectIndexOnMouseUp && !isDragging)
        {
            owner.selectIndexsBasedOnModifierKeys(itemIndex, e.mods, true);

            itemClicked(e);
        }
    }

    void mouseDoubleClick(const MouseEvent& e) override
    {
        if (LXGridViewModel* m = owner.getModel())
            if (isEnabled())
                m->gridViewItemDoubleClicked(itemIndex, e);
    }

    void mouseDrag(const MouseEvent& e) override
    {
        if (LXGridViewModel* m = owner.getModel())
        {
            if (isEnabled() && !(e.mouseWasClicked() || isDragging))
            {
                SparseSet<int> indexsToDrag;

                if (owner.selectOnMouseDown || owner.isIndexSelected(itemIndex))
                    indexsToDrag = owner.getSelectedIndexs();
                else
                    indexsToDrag.addRange(Range<int>::withStartAndLength(itemIndex, 1));

                if (indexsToDrag.size() > 0)
                {
                    const var dragDescription(m->getDragSourceDescription(indexsToDrag));

                    if (!(dragDescription.isVoid() || (dragDescription.isString() && dragDescription.toString().isEmpty())))
                    {
                        isDragging = true;
                        owner.startDragAndDrop(e, indexsToDrag, dragDescription, true);
                    }
                }
            }
        }
    }

    void resized() override
    {
        if (customComponent != nullptr)
            customComponent->setBounds(getLocalBounds());
    }

    String getTooltip() override
    {
        if (LXGridViewModel* m = owner.getModel())
            return m->getTooltipForIndex(itemIndex);

        return String::empty;
    }

    ScopedPointer<Component> customComponent;

private:
    LXGridView& owner;
    int itemIndex;
    bool selected, isDragging, selectIndexOnMouseUp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ItemComponent)
};


//==============================================================================
class LXGridView::ListViewport : public Viewport
{
public:
    ListViewport(LXGridView& lb)
        : owner(lb)
    {
        setWantsKeyboardFocus(false);

        Component* const content = new Component();
        setViewedComponent(content);
        content->setWantsKeyboardFocus(false);
    }

    ItemComponent* getComponentForIndex(const int index) const noexcept
    {
        return index < owner.totalItems ? items[index % jmax(1, items.size())] : nullptr;
    }

        ItemComponent* getComponentForIndexIfOnscreen(const int index) const noexcept
    {
        return (index >= firstIndex && index < firstIndex + items.size())
        ? getComponentForIndex(index) : nullptr;
    }

        int getItemIndexOfComponent(Component* const indexComponent) const noexcept
    {
        const int index = getViewedComponent()->getIndexOfChildComponent(indexComponent);
        const int num = items.size();

        for (int i = num; --i >= 0;)
            if (((firstIndex + i) % jmax(1, num)) == index)
                return firstIndex + i;

        return -1;
    }

        void visibleAreaChanged(const Rectangle<int>&) override
    {
        updateVisibleArea(true);

        if (LXGridViewModel* m = owner.getModel())
            m->gridViewWasScrolled();
    }

    void updateVisibleArea(const bool makeSureItUpdatesContent)
    {
        hasUpdated = false;

        Component& content = *getViewedComponent();
        const int newX = content.getX();
        int newY = content.getY();
        const int newW = jmax(owner.itemWidth, getMaximumVisibleWidth());
        const int newH = owner.totalLines * owner.getItemHeight();

        if (newY + newH < getMaximumVisibleHeight() && newH > getMaximumVisibleHeight())
            newY = getMaximumVisibleHeight() - newH;

        content.setBounds(newX, newY, newW, newH);

        if (makeSureItUpdatesContent && !hasUpdated)
            updateContents();
    }

    void updateContents()
    {
        hasUpdated = true;
        const int itemH = owner.getItemHeight();
        const int itemW = owner.getItemWidth();
        Component& content = *getViewedComponent();

        if (itemH > 0 && itemW > 0)
        {
            const int y = getViewPositionY();

            const int numNeeded = owner.horizonCount * (2 + getMaximumVisibleHeight() / itemH);
            items.removeRange(numNeeded, items.size());

            while (numNeeded > items.size())
            {
                ItemComponent* newItem = new ItemComponent(owner);
                items.add(newItem);
                content.addAndMakeVisible(newItem);
            }

            firstIndex = owner.horizonCount * (y / itemH);
            firstWholeIndex = owner.horizonCount * ((y + itemH - 1) / itemH);
            lastWholeIndex = owner.horizonCount * ((y + getMaximumVisibleHeight() - 1) / itemH);

            for (int i = 0; i < numNeeded; ++i)
            {
                const int index = i + firstIndex;

                if (ItemComponent* const itemComp = getComponentForIndex(index))
                {
                    itemComp->setBounds((index % owner.horizonCount) * itemW, (index / owner.horizonCount) * itemH, itemW, itemH);
                    itemComp->update(index, owner.isIndexSelected(index));
                }
            }
        }

        if (owner.headerComponent != nullptr)
            owner.headerComponent->setBounds(owner.outlineThickness + content.getX(),
            owner.outlineThickness,
            jmax(owner.getWidth() - owner.outlineThickness * 2,
            content.getWidth()),
            owner.headerComponent->getHeight());
    }

    void selectItem(const int index, const int itemH, const bool dontScroll,
        const int lastSelectedIndex, const int totalIndexs, const bool isMouseClick)
    {
        hasUpdated = false;

        if (index < firstWholeIndex && !dontScroll)
        {
            setViewPosition(getViewPositionX(), (index / itemH) * itemH);
        }
        else if (index >= lastWholeIndex && !dontScroll)
        {
            const int indexsOnScreen = lastWholeIndex - firstWholeIndex;

            if (index >= lastSelectedIndex + indexsOnScreen
                && indexsOnScreen < totalIndexs - 1
                && !isMouseClick)
            {
                setViewPosition(getViewPositionX(),
                    jlimit(0, jmax(0, totalIndexs - indexsOnScreen), (index / itemH)) * itemH);
            }
            else
            {
                setViewPosition(getViewPositionX(),
                    jmax(0, ((index / itemH) + 1) * itemH - getMaximumVisibleHeight()));
            }
        }

        if (!hasUpdated)
            updateContents();
    }

    void scrollToEnsureIndexIsOnscreen(const int index, const int indexH)
    {
        if (index < firstWholeIndex)
        {
            setViewPosition(getViewPositionX(), (index / owner.getItemHeight()) * indexH);
        }
        else if (index >= lastWholeIndex)
        {
            setViewPosition(getViewPositionX(),
                jmax(0, ((index / owner.getItemHeight()) + 1) * indexH - getMaximumVisibleHeight()));
        }
    }

    void paint(Graphics& g) override
    {
        if (isOpaque())
            g.fillAll(owner.findColour(LXGridView::backgroundColourId));
    }

    bool keyPressed(const KeyPress& key) override
    {
        if (Viewport::respondsToKey(key))
        {
            const int allowableMods = owner.multipleSelection ? ModifierKeys::shiftModifier : 0;

            if ((key.getModifiers().getRawFlags() & ~allowableMods) == 0)
            {
                // we want to avoid these keypresses going to the viewport, and instead allow
                // them to pass up to our listbox..
                return false;
            }
        }

        return Viewport::keyPressed(key);
    }

private:
    LXGridView& owner;
    OwnedArray<ItemComponent> items;
    int firstIndex, firstWholeIndex, lastWholeIndex;
    bool hasUpdated;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListViewport)
};

//==============================================================================
class LXGridViewMouseMoveSelector : public MouseListener
{
public:
    LXGridViewMouseMoveSelector(LXGridView& lb) : owner(lb)
    {
        owner.addMouseListener(this, true);
    }

    void mouseMove(const MouseEvent& e) override
    {
        const MouseEvent e2(e.getEventRelativeTo(&owner));
        owner.selectItem(owner.getIndexContainingPosition(e2.x, e2.y), true);
    }

    void mouseExit(const MouseEvent& e) override
    {
        mouseMove(e);
    }

private:
    LXGridView& owner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LXGridViewMouseMoveSelector)
};


//==============================================================================
LXGridView::LXGridView(const String& name, LXGridViewModel* const m)
    : Component(name),
    model(m),
    totalLines(0),
    totalItems(0),
    itemHeight(22),
    itemWidth(0),
    horizonCount(0),
    outlineThickness(0),
    lastIndexSelected(-1),
    multipleSelection(false),
    alwaysFlipSelection(false),
    hasDoneInitialUpdate(false),
    selectOnMouseDown(true)
{
    addAndMakeVisible(viewport = new ListViewport(*this));

    LXGridView::setWantsKeyboardFocus(true);
    LXGridView::colourChanged();
}

LXGridView::~LXGridView()
{
    headerComponent = nullptr;
    viewport = nullptr;
}

void LXGridView::setModel(LXGridViewModel* const newModel)
{
    if (model != newModel)
    {
        model = newModel;
        repaint();
        updateContent();
    }
}

void LXGridView::setMultipleSelectionEnabled(bool b) noexcept{ multipleSelection = b; }
void LXGridView::setClickingTogglesIndexSelection(bool b) noexcept{ alwaysFlipSelection = b; }
void LXGridView::setItemSelectedOnMouseDown(bool b) noexcept{ selectOnMouseDown = b; }

void LXGridView::setMouseMoveSelectsIndexs(bool b)
{
    if (b)
    {
        if (mouseMoveSelector == nullptr)
            mouseMoveSelector = new LXGridViewMouseMoveSelector(*this);
    }
    else
    {
        mouseMoveSelector = nullptr;
    }
}

//==============================================================================
void LXGridView::paint(Graphics& g)
{
    if (!hasDoneInitialUpdate)
        updateContent();

    g.fillAll(findColour(backgroundColourId));
}

void LXGridView::paintOverChildren(Graphics& g)
{
    if (outlineThickness > 0)
    {
        g.setColour(findColour(outlineColourId));
        g.drawRect(getLocalBounds(), outlineThickness);
    }
}

void LXGridView::resized()
{
    viewport->setBoundsInset(BorderSize<int>(outlineThickness + (headerComponent != nullptr ? headerComponent->getHeight() : 0),
        outlineThickness, outlineThickness, outlineThickness));

    viewport->setSingleStepSizes(20, getItemHeight());

    viewport->updateVisibleArea(false);
}

void LXGridView::visibilityChanged()
{
    viewport->updateVisibleArea(true);
}

Viewport* LXGridView::getViewport() const noexcept
{
    return viewport;
}

//==============================================================================
void LXGridView::updateContent()
{
    hasDoneInitialUpdate = true;
    totalItems = (model != nullptr) ? model->getNumItems() : 0;
    horizonCount = getVisibleContentWidth() / itemWidth;
    totalLines = totalItems / horizonCount + ((totalItems % horizonCount) != 0);

    bool selectionChanged = false;

    if (selected.size() > 0 && selected[selected.size() - 1] >= totalItems)
    {
        selected.removeRange(Range<int>(totalItems, std::numeric_limits<int>::max()));
        lastIndexSelected = getSelectedIndex(0);
        selectionChanged = true;
    }

    viewport->updateVisibleArea(isVisible());
    viewport->resized();

    if (selectionChanged && model != nullptr)
        model->selectedItemsChanged(lastIndexSelected);
}

//==============================================================================
void LXGridView::selectItem(int index, bool dontScroll, bool deselectOthersFirst)
{
    selectItemInternal(index, dontScroll, deselectOthersFirst, false);
}

void LXGridView::selectItemInternal(const int index,
    bool dontScroll,
    bool deselectOthersFirst,
    bool isMouseClick)
{
    if (!multipleSelection)
        deselectOthersFirst = true;

    if ((!isIndexSelected(index))
        || (deselectOthersFirst && getNumSelectedIndexs() > 1))
    {
        if (isPositiveAndBelow(index, totalItems))
        {
            if (deselectOthersFirst)
                selected.clear();

            selected.addRange(Range<int>(index, index + 1));

            if (getHeight() == 0 || getWidth() == 0)
                dontScroll = true;

            viewport->selectItem(index, getItemHeight(), dontScroll,
                lastIndexSelected, totalItems, isMouseClick);

            lastIndexSelected = index;
            model->selectedItemsChanged(index);
        }
        else
        {
            if (deselectOthersFirst)
                deselectAllIndexs();
        }
    }
}

void LXGridView::deselectIndex(const int index)
{
    if (selected.contains(index))
    {
        selected.removeRange(Range<int>(index, index + 1));

        if (index == lastIndexSelected)
            lastIndexSelected = getSelectedIndex(0);

        viewport->updateContents();
        model->selectedItemsChanged(lastIndexSelected);
    }
}

void LXGridView::setSelectedIndexs(const SparseSet<int>& setOfIndexsToBeSelected,
    const NotificationType sendNotificationEventToModel)
{
    selected = setOfIndexsToBeSelected;
    selected.removeRange(Range<int>(totalItems, std::numeric_limits<int>::max()));

    if (!isIndexSelected(lastIndexSelected))
        lastIndexSelected = getSelectedIndex(0);

    viewport->updateContents();

    if (model != nullptr && sendNotificationEventToModel == sendNotification)
        model->selectedItemsChanged(lastIndexSelected);
}

SparseSet<int> LXGridView::getSelectedIndexs() const
{
    return selected;
}

void LXGridView::selectRangeOfIndexs(int firstIndex, int lastIndex)
{
    if (multipleSelection && (firstIndex != lastIndex))
    {
        const int numIndexs = totalItems - 1;
        firstIndex = jlimit(0, jmax(0, numIndexs), firstIndex);
        lastIndex = jlimit(0, jmax(0, numIndexs), lastIndex);

        selected.addRange(Range<int>(jmin(firstIndex, lastIndex),
            jmax(firstIndex, lastIndex) + 1));

        selected.removeRange(Range<int>(lastIndex, lastIndex + 1));
    }

    selectItemInternal(lastIndex, false, false, true);
}

void LXGridView::flipIndexSelection(const int index)
{
    if (isIndexSelected(index))
        deselectIndex(index);
    else
        selectItemInternal(index, false, false, true);
}

void LXGridView::deselectAllIndexs()
{
    if (!selected.isEmpty())
    {
        selected.clear();
        lastIndexSelected = -1;

        viewport->updateContents();

        if (model != nullptr)
            model->selectedItemsChanged(lastIndexSelected);
    }
}

void LXGridView::selectIndexsBasedOnModifierKeys(const int index,
    ModifierKeys mods,
    const bool isMouseUpEvent)
{
    if (multipleSelection && (mods.isCommandDown() || alwaysFlipSelection))
    {
        flipIndexSelection(index);
    }
    else if (multipleSelection && mods.isShiftDown() && lastIndexSelected >= 0)
    {
        selectRangeOfIndexs(lastIndexSelected, index);
    }
    else if ((!mods.isPopupMenu()) || !isIndexSelected(index))
    {
        selectItemInternal(index, false, !(multipleSelection && (!isMouseUpEvent) && isIndexSelected(index)), true);
    }
}

int LXGridView::getNumSelectedIndexs() const
{
    return selected.size();
}

int LXGridView::getSelectedIndex(const int index) const
{
    return (isPositiveAndBelow(index, selected.size()))
        ? selected[index] : -1;
}

bool LXGridView::isIndexSelected(const int index) const
{
    return selected.contains(index);
}

int LXGridView::getLastIndexSelected() const
{
    return isIndexSelected(lastIndexSelected) ? lastIndexSelected : -1;
}

//==============================================================================
int LXGridView::getIndexContainingPosition(const int x, const int y) const noexcept
{
    if (isPositiveAndBelow(x, getWidth()))
    {
        const int index = (viewport->getViewPositionY() + y - viewport->getY()) / itemHeight;

        if (isPositiveAndBelow(index, totalItems))
            return index;
    }

    return -1;
}

int LXGridView::getInsertionIndexForPosition(const int x, const int y) const noexcept
{
    if (isPositiveAndBelow(x, getWidth()))
    {
        const int index = (viewport->getViewPositionY() + y + itemHeight / 2 - viewport->getY()) / itemHeight;
        return jlimit(0, totalItems, index);
    }

    return -1;
}

Component* LXGridView::getComponentForIndexNumber(const int index) const noexcept
{
    if (ItemComponent* const listIndexComp = viewport->getComponentForIndexIfOnscreen(index))
    return listIndexComp->customComponent ? static_cast<Component*> (listIndexComp->customComponent) : listIndexComp;

    return nullptr;
}

int LXGridView::getIndexNumberOfComponent(Component* const indexComponent) const noexcept
{
    return viewport->getItemIndexOfComponent(indexComponent);
}

Rectangle<int> LXGridView::getItemPosition(const int index,
const bool relativeToComponentTopLeft) const noexcept
{
    int y = viewport->getY() + itemHeight * index;

    if (relativeToComponentTopLeft)
        y -= viewport->getViewPositionY();

    return Rectangle<int>(viewport->getX(), y,
        viewport->getViewedComponent()->getWidth(), itemHeight);
}

void LXGridView::setVerticalPosition(const double proportion)
{
    const int offscreen = viewport->getViewedComponent()->getHeight() - viewport->getHeight();

    viewport->setViewPosition(viewport->getViewPositionX(),
        jmax(0, roundToInt(proportion * offscreen)));
}

double LXGridView::getVerticalPosition() const
{
    const int offscreen = viewport->getViewedComponent()->getHeight() - viewport->getHeight();

    return (offscreen > 0) ? viewport->getViewPositionY() / (double)offscreen
        : 0;
}

int LXGridView::getVisibleIndexWidth() const noexcept
{
    return viewport->getViewWidth();
}

void LXGridView::scrollToEnsureIndexIsOnscreen(const int index)
{
    viewport->scrollToEnsureIndexIsOnscreen(index, getItemHeight());
}

//==============================================================================
bool LXGridView::keyPressed(const KeyPress& key)
{
    const int numVisibleIndexs = viewport->getHeight() / getItemHeight();

    const bool multiple = multipleSelection
        && lastIndexSelected >= 0
        && key.getModifiers().isShiftDown();

    if (key.isKeyCode(KeyPress::upKey))
    {
        if (multiple)
            selectRangeOfIndexs(lastIndexSelected, lastIndexSelected - 1);
        else
            selectItem(jmax(0, lastIndexSelected - 1));
    }
    else if (key.isKeyCode(KeyPress::downKey))
    {
        if (multiple)
            selectRangeOfIndexs(lastIndexSelected, lastIndexSelected + 1);
        else
            selectItem(jmin(totalItems - 1, jmax(0, lastIndexSelected) + 1));
    }
    else if (key.isKeyCode(KeyPress::pageUpKey))
    {
        if (multiple)
            selectRangeOfIndexs(lastIndexSelected, lastIndexSelected - numVisibleIndexs);
        else
            selectItem(jmax(0, jmax(0, lastIndexSelected) - numVisibleIndexs));
    }
    else if (key.isKeyCode(KeyPress::pageDownKey))
    {
        if (multiple)
            selectRangeOfIndexs(lastIndexSelected, lastIndexSelected + numVisibleIndexs);
        else
            selectItem(jmin(totalItems - 1, jmax(0, lastIndexSelected) + numVisibleIndexs));
    }
    else if (key.isKeyCode(KeyPress::homeKey))
    {
        if (multiple)
            selectRangeOfIndexs(lastIndexSelected, 0);
        else
            selectItem(0);
    }
    else if (key.isKeyCode(KeyPress::endKey))
    {
        if (multiple)
            selectRangeOfIndexs(lastIndexSelected, totalItems - 1);
        else
            selectItem(totalItems - 1);
    }
    else if (key.isKeyCode(KeyPress::returnKey) && isIndexSelected(lastIndexSelected))
    {
        if (model != nullptr)
            model->returnKeyPressed(lastIndexSelected);
    }
    else if ((key.isKeyCode(KeyPress::deleteKey) || key.isKeyCode(KeyPress::backspaceKey))
        && isIndexSelected(lastIndexSelected))
    {
        if (model != nullptr)
            model->deleteKeyPressed(lastIndexSelected);
    }
    else if (multipleSelection && key == KeyPress('a', ModifierKeys::commandModifier, 0))
    {
        selectRangeOfIndexs(0, std::numeric_limits<int>::max());
    }
    else
    {
        return false;
    }

    return true;
}

bool LXGridView::keyStateChanged(const bool isKeyDown)
{
    return isKeyDown
        && (KeyPress::isKeyCurrentlyDown(KeyPress::upKey)
        || KeyPress::isKeyCurrentlyDown(KeyPress::pageUpKey)
        || KeyPress::isKeyCurrentlyDown(KeyPress::downKey)
        || KeyPress::isKeyCurrentlyDown(KeyPress::pageDownKey)
        || KeyPress::isKeyCurrentlyDown(KeyPress::homeKey)
        || KeyPress::isKeyCurrentlyDown(KeyPress::endKey)
        || KeyPress::isKeyCurrentlyDown(KeyPress::returnKey));
}

void LXGridView::mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel)
{
    bool eventWasUsed = false;

    if (wheel.deltaX != 0 && viewport->getHorizontalScrollBar()->isVisible())
    {
        eventWasUsed = true;
        viewport->getHorizontalScrollBar()->mouseWheelMove(e, wheel);
    }

    if (wheel.deltaY != 0 && viewport->getVerticalScrollBar()->isVisible())
    {
        eventWasUsed = true;
        viewport->getVerticalScrollBar()->mouseWheelMove(e, wheel);
    }

    if (!eventWasUsed)
        Component::mouseWheelMove(e, wheel);
}

void LXGridView::mouseUp(const MouseEvent& e)
{
    if (e.mouseWasClicked() && model != nullptr)
        model->backgroundClicked(e);
}

//==============================================================================
void LXGridView::setItemHeight(const int newHeight)
{
    itemHeight = jmax(1, newHeight);
    viewport->setSingleStepSizes(20, itemHeight);
    updateContent();
}

int LXGridView::getNumIndexsOnScreen() const noexcept
{
    return viewport->getMaximumVisibleHeight() / itemHeight;
}

void LXGridView::setItemWidth(const int newWidth)
{
    itemWidth = newWidth;
    updateContent();
}

int LXGridView::getVisibleContentWidth() const noexcept
{
    return viewport->getMaximumVisibleWidth();
}

ScrollBar* LXGridView::getVerticalScrollBar() const noexcept
{
    return viewport->getVerticalScrollBar();
}

ScrollBar* LXGridView::getHorizontalScrollBar() const noexcept
{
    return viewport->getHorizontalScrollBar();
}

void LXGridView::colourChanged()
{
    setOpaque(findColour(backgroundColourId).isOpaque());
    viewport->setOpaque(isOpaque());
    repaint();
}

void LXGridView::parentHierarchyChanged()
{
    colourChanged();
}

void LXGridView::setOutlineThickness(const int newThickness)
{
    outlineThickness = newThickness;
    resized();
}

void LXGridView::setHeaderComponent(Component* const newHeaderComponent)
{
    if (headerComponent != newHeaderComponent)
    {
        headerComponent = newHeaderComponent;

        addAndMakeVisible(newHeaderComponent);
        LXGridView::resized();
    }
}

void LXGridView::repaintItem(const int index) noexcept
{
    repaint(getItemPosition(index, true));
}

Image LXGridView::createSnapshotOfIndexs(const SparseSet<int>& indexs, int& imageX, int& imageY)
{
    Rectangle<int> imageArea;

    for (int i = 0; i < indexs.size(); ++i)
    {
        if (Component* indexComp = viewport->getComponentForIndexIfOnscreen(indexs[i]))
        {
            const Point<int> pos(getLocalPoint(indexComp, Point<int>()));
            const Rectangle<int> indexRect(pos.getX(), pos.getY(), indexComp->getWidth(), indexComp->getHeight());
            imageArea = imageArea.getUnion(indexRect);
        }
    }

    imageArea = imageArea.getIntersection(getLocalBounds());
    imageX = imageArea.getX();
    imageY = imageArea.getY();
    Image snapshot(Image::ARGB, imageArea.getWidth(), imageArea.getHeight(), true);

    for (int i = 0; i < indexs.size(); ++i)
    {
        if (Component* indexComp = viewport->getComponentForIndexIfOnscreen(indexs[i]))
        {
            Graphics g(snapshot);
            g.setOrigin(getLocalPoint(indexComp, Point<int>()) - imageArea.getPosition());

            if (g.reduceClipRegion(indexComp->getLocalBounds()))
            {
                g.beginTransparencyLayer(0.6f);
                indexComp->paintEntireComponent(g, false);
                g.endTransparencyLayer();
            }
        }
    }

    return snapshot;
}

void LXGridView::startDragAndDrop(const MouseEvent& e, const SparseSet<int>& indexsToDrag, const var& dragDescription, bool allowDraggingToOtherWindows)
{
    if (DragAndDropContainer* const dragContainer = DragAndDropContainer::findParentDragContainerFor(this))
    {
        int x, y;
        Image dragImage = createSnapshotOfIndexs(indexsToDrag, x, y);

        MouseEvent e2(e.getEventRelativeTo(this));
        const Point<int> p(x - e2.x, y - e2.y);
        dragContainer->startDragging(dragDescription, this, dragImage, allowDraggingToOtherWindows, &p);
    }
    else
    {
        // to be able to do a drag-and-drop operation, the listbox needs to
        // be inside a component which is also a DragAndDropContainer.
        jassertfalse;
    }
}

//==============================================================================
Component* LXGridViewModel::refreshComponentForIndex(int, bool, Component* existingComponentToUpdate)
{
    ignoreUnused(existingComponentToUpdate);
    jassert(existingComponentToUpdate == nullptr); // indicates a failure in the code that recycles the components
    return nullptr;
}

void LXGridViewModel::gridViewItemClicked(int, const MouseEvent&) {}
void LXGridViewModel::gridViewItemDoubleClicked(int, const MouseEvent&) {}
void LXGridViewModel::backgroundClicked(const MouseEvent&) {}
void LXGridViewModel::selectedItemsChanged(int) {}
void LXGridViewModel::deleteKeyPressed(int) {}
void LXGridViewModel::returnKeyPressed(int) {}
void LXGridViewModel::gridViewWasScrolled() {}
var LXGridViewModel::getDragSourceDescription(const SparseSet<int>&)      { return var(); }
String LXGridViewModel::getTooltipForIndex(int)                             { return String::empty; }
MouseCursor LXGridViewModel::getMouseCursorForIndex(int)                    { return MouseCursor::NormalCursor; }
