#include "JuceHeader.h"
#include "ShelfViewComponent.h"

#undef max
#define ITEMCOUNT 16

class ShelfViewComponent::ItemComponent : public juce::Component,
    public juce::TooltipClient
{
public:
    ItemComponent (ShelfViewComponent& sv) : owner (sv), index (-1),
        bSelect (false), bDragging (false), selectOnMouseUp (false)
    {
    }

    void paint (juce::Graphics& g) override
    {
        if (ShelfViewModel* m = owner.getModel())
            m->paintCellItem (index, g, getWidth(), getHeight(), bSelect);
    }

    void update (const int newIdx, const bool nowSelected)
    {
        // disable 不用的ItemComponent
        setEnabled(index <= owner.totalItems);

        if (index != newIdx || bSelect != nowSelected)
        {
            repaint();
            index = newIdx;
            bSelect = nowSelected;
        }

        if (ShelfViewModel* m = owner.getModel())
        {
            setMouseCursor (m->getMouseCursorForItem (index));
            customComponent = m->refreshItemComponentForIdx (newIdx, nowSelected, customComponent.release());

            if (customComponent != nullptr)
            {
                addAndMakeVisible (customComponent);
                customComponent->setBounds (getLocalBounds());
            }
        }
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        bDragging = false;
        selectOnMouseUp = false;

        if (isEnabled())
        {
            if (owner.selectOnMouseDown && ! bSelect)
            {
                owner.selectItemBasedOnModifierKeys (index, e.mods, false);
            }
            else
            {
                selectOnMouseUp = true;
            }
        }
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        if (isEnabled() && !bDragging)
        {
            if (selectOnMouseUp)
                owner.selectItemBasedOnModifierKeys (index, e.mods, true);

            if (ShelfViewModel* m = owner.getModel())
                m->shelfItemClicked (index, e);
        }
    }

    void mouseDoubleClick (const juce::MouseEvent& e) override
    {
        if (ShelfViewModel* m = owner.getModel())
            m->shelfItemDoubleClicked (index, e);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        // 鼠标拖动框选状态直接返回
        if (!owner.mouseDragArea.isEmpty())
            return;

        if (ShelfViewModel* m = owner.getModel())
        {
            if (isEnabled() && ! (e.mouseWasClicked() || bDragging))
            {
                juce::SparseSet<int> itemsToDrag;

                if (owner.selectOnMouseDown || owner.isItemSelected (index))
                    itemsToDrag = owner.getSelectedItems();
                else
                    itemsToDrag.addRange (juce::Range<int>::withStartAndLength (index, 1));

                if (itemsToDrag.size() > 0)
                {
                    const juce::var dragDescription (m->getDragSourceDescription (itemsToDrag));

                    if (! (dragDescription.isVoid() || (dragDescription.isString() && dragDescription.toString().isEmpty())))
                    {
                        bDragging = true;
                        owner.startDragAndDrop (e, itemsToDrag, dragDescription, true);
                    }
                }
            }
        }
    }

    void resized() override
    {
        if (customComponent != nullptr)
            customComponent->setBounds (getLocalBounds());
    }

    juce::String getTooltip() override
    {
        if (ShelfViewModel* m = owner.getModel())
            return m->getTooltipForItem (index);
        return juce::String::empty;
    }

    int getIndex() const
    {
        return index;
    }

    void setIndex(int idx)
    {
        index = idx;
    }
    juce::ScopedPointer<juce::Component> customComponent;

private:
    ShelfViewComponent& owner;
    int index;
    bool bSelect, bDragging, selectOnMouseUp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ItemComponent)
};

class ShelfViewComponent::ShelfViewport  : public juce::Viewport
{
public:
    ShelfViewport (ShelfViewComponent& sv) : owner (sv)
    {
        setWantsKeyboardFocus (false);

        juce::Component* const content = new juce::Component();
        setViewedComponent (content);
        content->setWantsKeyboardFocus (false);
    }

    ItemComponent* getComponentForIdx (const int idx) const noexcept
    {
        return items [idx % juce::jmax (1, items.size())];
    }

    ItemComponent* getComponentForIdxIfOnscreen (const int idx) const noexcept
    {
        return (idx >= firstIndex && idx < firstIndex + items.size())
            ? getComponentForIdx (idx) : nullptr;
    }

    int getIdxOfComponent (juce::Component* const idxComponent) const noexcept
    {
        const int index = getViewedComponent()->getIndexOfChildComponent (idxComponent);
        const int num = items.size();

        for (int i = num; --i >= 0;)
            if (((firstIndex + i) % juce::jmax (1, num)) == index)
                return firstIndex + i;

        return -1;
    }

    void visibleAreaChanged (const juce::Rectangle<int>&) override
    {
        updateVisibleArea (true);
        if (ShelfViewModel* m = owner.getModel())
            m->shelfWasScrolled();
    }

    void updateVisibleArea (const bool makeSureItUpdatesContent)
    {
        hasUpdated = false;

        juce::Component& content = *getViewedComponent();
        int newX = content.getX();
        const int newY = content.getY();
        const int newW = owner.getTotalCols() * owner.getColWidth();
        const int newH = juce::jmax (owner.minimumColHeigth, getMaximumVisibleHeight());

        if (newX + newW < getMaximumVisibleWidth() && newW > getMaximumVisibleWidth())
            newX = getMaximumVisibleWidth() - newW;

        content.setBounds (newX, newY, newW, newH);

        if (makeSureItUpdatesContent && ! hasUpdated)
            updateContents();
    }

    void updateContents()
    {
        hasUpdated = true;
        const int colW = owner.getColWidth();
        juce::Component& content = *getViewedComponent();

        if (colW > 0)
        {
            const int x = getViewPositionX();
            const int h = content.getHeight();
            const int maxVisibleWidth = getMaximumVisibleWidth();
            const int numNeeded = ITEMCOUNT * (2 + maxVisibleWidth / colW);
            items.removeRange (numNeeded, items.size());

            while (numNeeded > items.size())
            {
                ItemComponent* newItem = new ItemComponent (owner);
                items.add (newItem);
                content.addAndMakeVisible (newItem);
            }

            firstIndex = ITEMCOUNT * (x / colW);
            firstWholeIndex = ITEMCOUNT * ((x + colW - 1) / colW);
            lastWholeIndex = ITEMCOUNT * ((x + maxVisibleWidth - 1) / colW);

            for (int i = 0; i < numNeeded; ++i)
            {
                const int idx = i + firstIndex;

                if (ItemComponent* const itemComp = getComponentForIdx (idx))
                {
                    const int cell = idx % ITEMCOUNT;
                    const int col = idx / ITEMCOUNT;
                    juce::Rectangle<int> bounds = CXOption::getInstance()->GetBookBounds(cell);
                    bounds += juce::Point<int>(colW * col, 0);
                    itemComp->setBounds (bounds);
                    itemComp->update (idx, owner.isItemSelected (idx));
                }
            }
        }
    }

    void moveItemsAnimate(juce::SparseSet<int>& sel, int dest)
    {
        if (!sel.isEmpty() && !sel.contains(dest))
        {
            const int selcount = sel.size();
            int nLow = 0;
            int nLar = 0;

            if (sel[selcount-1] < dest)
            {
                nLow = selcount - 1;
                nLar = 1;
                for (int i=selcount-1; i>=0; --i)
                    items.move(sel[i], dest);
            }
            else if (sel[0] > dest)
            {
                nLar = selcount;
                for (int i=0; i<selcount; ++i)
                    items.move(sel[i], dest);
            }
            else
            {
                juce::SparseSet<int> LarSel;
                juce::SparseSet<int> lowSel;

                for (unsigned int i=0; i<sel.getNumRanges(); ++i)
                {
                    const juce::Range<int> rage = sel.getRange(i);
                    if (rage.getStart() > dest)
                        LarSel.addRange(rage);
                    else
                        lowSel.addRange(rage);
                }

                for (int i=lowSel.size() - 1; i>=0; --i)
                    items.move(lowSel[i], dest);
                for (int i=0; i<LarSel.size(); ++i)
                    items.move(LarSel[i], dest);

                nLow = LarSel.size() -1;
                nLar = lowSel.size() + 1;
            }

            sel.clear();
            sel.addRange(juce::Range<int>(dest-nLow, dest+nLar));
            updateContentsPosition(true);
        }
    }

    void updateContentsPosition(bool bAnimate)
    {
        juce::ComponentAnimator& animator = juce::Desktop::getInstance().getAnimator();
        const int colW = owner.getColWidth();

        juce::Rectangle<int> bounds;
        ItemComponent* comp;

        for (int i = 0; i < items.size() && i < owner.totalItems; ++i)
        {
            comp = items[i];
            if (comp && comp->getIndex() < owner.totalItems && comp->getIndex() != i)
            {
                const int cell = i % ITEMCOUNT;
                const int col = i / ITEMCOUNT;
                bounds = CXOption::getInstance()->GetBookBounds(cell);
                bounds.setX(bounds.getX() + (colW * col));
                if (bAnimate && !owner.isItemSelected(i))
                    animator.animateComponent(comp, bounds, 1.0f, 300, true, 0.0f, 0.0f);
                else
                    comp->setBounds (bounds);

                comp->setIndex(i);
            }
        }
    }

    void selectItem (const int idx, const int colW, const bool dontScroll,
        const int lastSelectedIdx, const int totalItems, const bool isMouseClick)
    {
        hasUpdated = false;
        const int col = idx/ITEMCOUNT;

        if (idx < firstWholeIndex && ! dontScroll)
        {
            setViewPosition (col * colW, getViewPositionY());
        }
        else if (idx >= lastWholeIndex && ! dontScroll)
        {
            const int itemsOnScreen = lastWholeIndex - firstWholeIndex;

            if (idx >= lastSelectedIdx + itemsOnScreen
                && itemsOnScreen < totalItems - 1
                && ! isMouseClick)
            {
                setViewPosition (juce::jlimit (0, juce::jmax (0, ((totalItems - itemsOnScreen)/ITEMCOUNT)), col) * colW, getViewPositionY());
            }
            else
            {
                setViewPosition (juce::jmax (0, (col  + 1) * colW - getMaximumVisibleWidth()), getViewPositionY());
            }
        }

        if (! hasUpdated)
            updateContents();
    }

    void scrollToEnsureColIsOnscreen (const int col, const int colW)
    {
        if (col < firstWholeIndex/ITEMCOUNT)
        {
            setViewPosition (col * colW, getViewPositionY());
        }
        else if (col >= lastWholeIndex/ITEMCOUNT)
        {
            setViewPosition (juce::jmax (0, (col  + 1) * colW - getMaximumVisibleWidth()), getViewPositionY());
        }
    }

    void scrollToEnsureItemIsOnscreen (const int idx, const int colW)
    {
        const int col = idx/ITEMCOUNT;
        if (idx < firstWholeIndex)
        {
            setViewPosition (col * colW, getViewPositionY());
        }
        else if (idx >= lastWholeIndex)
        {
            setViewPosition (juce::jmax (0, (col  + 1) * colW - getMaximumVisibleWidth()), getViewPositionY());
        }
    }

    void paint (juce::Graphics& g) override
    {
        if (isOpaque())
            g.fillAll (owner.findColour (HorListBox::backgroundColourId));

        if (ShelfViewModel* m = owner.getModel())
        {
            const int viewX = getViewPositionX();
            const int viewY = getViewPositionY();
            const int colWidth = owner.getColWidth();
            const int colHeight = getViewHeight();
            const int viewRight = getRight();

            if (viewRight > 0 && colWidth > 0 && colHeight > 0)
            {
                int col;
                int posX = -(viewX % colWidth);
                for (; posX < viewRight;)
                {
                    col = (viewX / colWidth) + (posX != 0);
                    m->paintColPanel(col, g, posX, viewY, colWidth, colHeight);
                    posX += colWidth;
                }
            }
        }
    }

    bool keyPressed (const juce::KeyPress& key) override
    {
        if (juce::Viewport::respondsToKey (key))
        {
            const int allowableMods = juce::ModifierKeys::shiftModifier;

            if ((key.getModifiers().getRawFlags() & ~allowableMods) == 0)
            {
                // we want to avoid these keypresses going to the viewport, and instead allow
                // them to pass up to our listbox..
                return false;
            }
        }

        return juce::Viewport::keyPressed (key);
    }

private:
    ShelfViewComponent& owner;
    juce::OwnedArray<ItemComponent> items;
    int firstIndex, firstWholeIndex, lastWholeIndex;
    bool hasUpdated;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShelfViewport)
};

class ShelfViewComponent::ShelfViewMouseMoveSelector  : public juce::MouseListener
{
public:
    ShelfViewMouseMoveSelector (ShelfViewComponent& sv) 
        : owner (sv), isMouseDown(false)
    {
        owner.addMouseListener (this, true);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (isMouseDown)
        {
            const juce::MouseEvent e2 (e.getEventRelativeTo (&owner));
            juce::Rectangle<int> rcArea(pointMouseDown, e2.getPosition());
            owner.setMouseDragSelectArea(rcArea);
        }
    }

    virtual void mouseDown (const juce::MouseEvent& e) 
    {
        const juce::MouseEvent e2 (e.getEventRelativeTo (&owner));
        pointMouseDown = e2.getMouseDownPosition();
        int sel = owner.getItemContainingPosition (pointMouseDown.x, pointMouseDown.y);
        if (sel == -1 || sel >= owner.totalItems)
        {
            owner.deselectAllItems();
            isMouseDown = true;
        }
    }

    virtual void mouseUp (const juce::MouseEvent& e) 
    {
        isMouseDown = false;
        juce::Rectangle<int> bounds(0,0,0,0);
        owner.setMouseDragSelectArea(bounds);
    }

private:
    ShelfViewComponent& owner;
    bool isMouseDown;
    juce::Point<int> pointMouseDown;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShelfViewMouseMoveSelector)
};

//////////////////////////////////////////////////////////////////////////
//
ShelfViewComponent::ShelfViewComponent(const juce::String& name, ShelfViewModel* m)
    : juce::Component (name),
    model (m),
    totalItems (0),
    colWidth (22),
    minimumColHeigth (0),
    lastItemSelected (-1),
    hasDoneInitialUpdate (false),
    selectOnMouseDown (true),
    selectionEnabled(true),
    lastDragOverItem(-1)
{
    addAndMakeVisible (viewport = new ShelfViewport (*this));
    viewport->setSingleStepSizes (500, 500);

    ShelfViewComponent::setWantsKeyboardFocus (true);
    ShelfViewComponent::colourChanged();
}

ShelfViewComponent::~ShelfViewComponent()
{
    viewport = nullptr;
}

juce::Viewport* ShelfViewComponent::getViewport() const noexcept
{
    return viewport;
}

void ShelfViewComponent::paint(juce::Graphics& g)
{
    if (!hasDoneInitialUpdate)
        updateContent();

    g.fillAll (findColour (backgroundColourId));
}

void ShelfViewComponent::paintOverChildren(juce::Graphics& g)
{
    if (!mouseDragArea.isEmpty())
    {
        g.setColour(juce::Colour(0x5F000000));
        g.drawRect(mouseDragArea);
    }
}

void ShelfViewComponent::updateContent()
{
    hasDoneInitialUpdate = true;
    totalItems = (model != nullptr) ? model->getNumItems() : 0;

    bool selectionChanged = false;

    if (selected.size() > 0 && selected [selected.size() - 1] >= totalItems)
    {
        selected.removeRange (juce::Range<int> (totalItems, std::numeric_limits<int>::max()));
        lastItemSelected = getSelectedItem (0);
        selectionChanged = true;
    }

    viewport->updateVisibleArea (isVisible());
    viewport->resized();

    if (selectionChanged && model != nullptr)
        model->selectedItemsChanged (lastItemSelected);
}

int ShelfViewComponent::getNumColsOnScreen() const noexcept
{
    return viewport->getMaximumVisibleWidth() / colWidth;
}

bool ShelfViewComponent::keyPressed(const juce::KeyPress& key)
{
    const int numVisibleItems = ITEMCOUNT * viewport->getWidth() / getColWidth();

    const bool multiple = lastItemSelected >= 0 && key.getModifiers().isShiftDown();

    if (key.isKeyCode (juce::KeyPress::upKey))
    {
        if (multiple)
            selectRangeOfItems (lastItemSelected, lastItemSelected - 1);
        else
            selectItem (juce::jmax (0, lastItemSelected - 1));
    }
    else if (key.isKeyCode (juce::KeyPress::downKey))
    {
        if (multiple)
            selectRangeOfItems (lastItemSelected, lastItemSelected + 1);
        else
            selectItem (juce::jmin (totalItems - 1, juce::jmax (0, lastItemSelected) + 1));
    }
    else if (key.isKeyCode (juce::KeyPress::pageUpKey))
    {
        if (multiple)
            selectRangeOfItems (lastItemSelected, lastItemSelected - numVisibleItems);
        else
            selectItem (juce::jmax (0, juce::jmax (0, lastItemSelected) - numVisibleItems));
    }
    else if (key.isKeyCode (juce::KeyPress::pageDownKey))
    {
        if (multiple)
            selectRangeOfItems (lastItemSelected, lastItemSelected + numVisibleItems);
        else
            selectItem (juce::jmin (totalItems - 1, juce::jmax (0, lastItemSelected) + numVisibleItems));
    }
    else if (key.isKeyCode (juce::KeyPress::homeKey))
    {
        if (multiple)
            selectRangeOfItems (lastItemSelected, 0);
        else
            selectItem (0);
    }
    else if (key.isKeyCode (juce::KeyPress::endKey))
    {
        if (multiple)
            selectRangeOfItems (lastItemSelected, totalItems - 1);
        else
            selectItem (totalItems - 1);
    }
    else if (key.isKeyCode (juce::KeyPress::returnKey) && isItemSelected (lastItemSelected))
    {
        if (model != nullptr)
            model->returnKeyPressed (lastItemSelected);
    }
    else if ((key.isKeyCode (juce::KeyPress::deleteKey) || key.isKeyCode (juce::KeyPress::backspaceKey))
        && isItemSelected (lastItemSelected))
    {
        if (model != nullptr)
            model->deleteKeyPressed (lastItemSelected);
    }
    else if (key == juce::KeyPress ('a', juce::ModifierKeys::commandModifier, 0))
    {
        selectRangeOfItems (0, std::numeric_limits<int>::max());
    }
    else
    {
        return false;
    }

    return true;
}

bool ShelfViewComponent::keyStateChanged(bool isKeyDown)
{
    return isKeyDown
        && (juce::KeyPress::isKeyCurrentlyDown (juce::KeyPress::upKey)
        || juce::KeyPress::isKeyCurrentlyDown (juce::KeyPress::pageUpKey)
        || juce::KeyPress::isKeyCurrentlyDown (juce::KeyPress::downKey)
        || juce::KeyPress::isKeyCurrentlyDown (juce::KeyPress::pageDownKey)
        || juce::KeyPress::isKeyCurrentlyDown (juce::KeyPress::homeKey)
        || juce::KeyPress::isKeyCurrentlyDown (juce::KeyPress::endKey)
        || juce::KeyPress::isKeyCurrentlyDown (juce::KeyPress::returnKey));
}

void ShelfViewComponent::resized()
{
    viewport->setBoundsInset (juce::BorderSize<int> (0,0, 0, 0));
}

void ShelfViewComponent::visibilityChanged()
{
    viewport->updateVisibleArea (true);
}

void ShelfViewComponent::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    bool eventWasUsed = false;

    if (wheel.deltaX != 0 && viewport->getHorizontalScrollBar()->isVisible())
    {
        eventWasUsed = true;
        viewport->getHorizontalScrollBar()->mouseWheelMove (e, wheel);
    }

    if (wheel.deltaY != 0 && viewport->getVerticalScrollBar()->isVisible())
    {
        eventWasUsed = true;
        viewport->getVerticalScrollBar()->mouseWheelMove (e, wheel);
    }

    if (! eventWasUsed)
        Component::mouseWheelMove (e, wheel);
}

void ShelfViewComponent::mouseUp(const juce::MouseEvent& e)
{
    if (e.mouseWasClicked() && model != nullptr)
        model->backgroundClicked (e);
}

void ShelfViewComponent::colourChanged()
{
    setOpaque (findColour (backgroundColourId).isOpaque());
    viewport->setOpaque (isOpaque());
    repaint();
}

void ShelfViewComponent::parentHierarchyChanged()
{
    colourChanged();
}

void ShelfViewComponent::startDragAndDrop(const juce::MouseEvent& e, const juce::SparseSet<int>& rowsToDrag, const juce::var& dragDescription, bool allowDraggingToOtherWindows)
{
    if (juce::DragAndDropContainer* const dragContainer = juce::DragAndDropContainer::findParentDragContainerFor (this))
    {
        int x, y;
        juce::Image dragImage = createSnapshotOfRows (rowsToDrag, x, y);

        juce::MouseEvent e2 (e.getEventRelativeTo (this));
        const juce::Point<int> p (x - e2.x, y - e2.y);
        dragContainer->startDragging (dragDescription, this, dragImage, allowDraggingToOtherWindows, &p);
    }
    else
    {
        // to be able to do a drag-and-drop operation, the listbox needs to
        // be inside a component which is also a DragAndDropContainer.
        jassertfalse;
    }
}

bool ShelfViewComponent::isItemSelected(int idx) const
{
    return selected.contains (idx);
}

juce::SparseSet<int> ShelfViewComponent::getSelectedItems() const
{
    return selected;
}

int ShelfViewComponent::getTotalCols() const noexcept
{
    return (totalItems/ITEMCOUNT) + ((totalItems%ITEMCOUNT) != 0);
}

int ShelfViewComponent::getSelectedItem(int index /*= 0*/) const
{
    return (juce::isPositiveAndBelow (index, selected.size()))
        ? selected [index] : -1;
}

void ShelfViewComponent::setMultipleSelectionEnabled(bool shouldBeEnabled) noexcept
{
    if (shouldBeEnabled)
    {
        if (mouseMoveSelector == nullptr)
            mouseMoveSelector = new ShelfViewMouseMoveSelector (*this);
    }
    else
    {
        mouseMoveSelector = nullptr;
    }
}

void ShelfViewComponent::setItemSelectedOnMouseDown(bool isSelectedOnMouseDown) noexcept
{
    selectOnMouseDown = isSelectedOnMouseDown;
}

void ShelfViewComponent::setItemSelectionEnabled(bool b) noexcept
{
    selectionEnabled = b;
}

void ShelfViewComponent::selectItem(int idx, bool dontScroll /*= false*/, bool clearLast/* = true*/)
{
    selectItemInternal (idx, dontScroll, false, clearLast);
}

void ShelfViewComponent::selectItemInternal(int idx, bool dontScroll, bool isMouseClick, bool clearLast/* = true*/)
{
    if ((! isItemSelected (idx)) || (getNumSelectedItems() > 1))
    {
        if (juce::isPositiveAndBelow (idx, totalItems))
        {
            if (clearLast)
                selected.clear();

            selected.addRange (juce::Range<int> (idx, idx + 1));

            if (getHeight() == 0 || getWidth() == 0)
                dontScroll = true;

            viewport->selectItem (idx, getColWidth(), dontScroll,
                lastItemSelected, totalItems, isMouseClick);

            lastItemSelected = idx;
            if (model != nullptr) 
                model->selectedItemsChanged(idx);
        }
        else
        {
            deselectAllItems();
        }
    }
}

int ShelfViewComponent::getNumSelectedItems() const
{
    return selected.size();
}

void ShelfViewComponent::deselectAllItems()
{
    if (! selected.isEmpty())
    {
        selected.clear();
        lastItemSelected = -1;
        viewport->updateContents();
        if (model != nullptr) 
            model->selectedItemsChanged (lastItemSelected);
    }
}

void ShelfViewComponent::selectRangeOfItems(int firstRow, int lastRow)
{
    if (firstRow != lastRow)
    {
        const int numItems = totalItems - 1;
        firstRow = juce::jlimit (0, juce::jmax (0, numItems), firstRow);
        lastRow  = juce::jlimit (0, juce::jmax (0, numItems), lastRow);

        selected.addRange (juce::Range<int> (juce::jmin (firstRow, lastRow),
            juce::jmax (firstRow, lastRow) + 1));

        selected.removeRange (juce::Range<int> (lastRow, lastRow + 1));
    }

    selectItemInternal (lastRow, false, true, false);
}

void ShelfViewComponent::deselectItem(int idx)
{
    if (selected.contains (idx))
    {
        selected.removeRange (juce::Range<int> (idx, idx + 1));

        if (idx == lastItemSelected)
            lastItemSelected = getSelectedItem (0);

        viewport->updateContents();
        if (model != nullptr) 
            model->selectedItemsChanged (lastItemSelected);
    }
}

void ShelfViewComponent::setSelectedItems(const juce::SparseSet<int>& setOfItemsToBeSelected, 
                                          juce::NotificationType sendNotificationEventToModel /*= juce::sendNotification*/)
{
    selected = setOfItemsToBeSelected;
    selected.removeRange (juce::Range<int> (totalItems, std::numeric_limits<int>::max()));

    if (! isItemSelected (lastItemSelected))
        lastItemSelected = getSelectedItem (0);

    viewport->updateContents();

    if (model!=nullptr && sendNotificationEventToModel == juce::sendNotification)
        model->selectedItemsChanged (lastItemSelected);
}

void ShelfViewComponent::setSelectedItemsNotUpdate(const juce::SparseSet<int>& setOfItemsToBeSelected)
{
    selected = setOfItemsToBeSelected;
}

int ShelfViewComponent::getLastItemSelected() const
{
    return isItemSelected (lastItemSelected) ? lastItemSelected : -1;
}

void ShelfViewComponent::flipItemSelection(int idx)
{
    if (isItemSelected (idx))
        deselectItem (idx);
    else
        selectItemInternal (idx, false, true);
}

void ShelfViewComponent::selectItemBasedOnModifierKeys(int idx, juce::ModifierKeys mods, bool isMouseUpEvent)
{
    if (selectionEnabled)
    {
        if (mods.isCommandDown())
        {
            flipItemSelection (idx);
        }
        else if (mods.isShiftDown() && lastItemSelected >= 0)
        {
            selectRangeOfItems (lastItemSelected, idx);
        }
        else if ((! mods.isPopupMenu()) || ! isItemSelected (idx))
        {
            selectItemInternal (idx, true, true);
        }
    }
}

void ShelfViewComponent::setHorizontalPosition(double proportion)
{
    const int offscreen = viewport->getViewedComponent()->getWidth() - viewport->getWidth();

    viewport->setViewPosition (juce::jmax (0, juce::roundToInt (proportion * offscreen)),
        viewport->getViewPositionY());
}

double ShelfViewComponent::getHorizontalPosition() const
{
    const int offscreen = viewport->getViewedComponent()->getWidth() - viewport->getWidth();
    return (offscreen > 0) ? viewport->getViewPositionX() / (double) offscreen : 0;
}

void ShelfViewComponent::scrollToEnsureItemIsOnscreen(int idx)
{
    viewport->scrollToEnsureItemIsOnscreen(idx, getColWidth());
}

juce::ScrollBar* ShelfViewComponent::getVerticalScrollBar() const noexcept
{
    return viewport->getVerticalScrollBar();
}

juce::ScrollBar* ShelfViewComponent::getHorizontalScrollBar() const noexcept
{
    return viewport->getHorizontalScrollBar();
}

int ShelfViewComponent::getItemContainingPosition(int x, int y) const noexcept
{
    if (juce::isPositiveAndBelow (x, getWidth()))
    {
        const int col = (viewport->getViewPositionX() + x - viewport->getX()) / colWidth;
        if (juce::isPositiveAndBelow (col, totalItems))
        {
            for (unsigned int i=0; i<ITEMCOUNT; ++i)
            {
                juce::Rectangle<int> bounds = CXOption::getInstance()->GetBookBounds(i);
                bounds.setX(bounds.getX() + (col*colWidth) - viewport->getViewPositionX());
                if (bounds.contains(x, y))
                {
                    return (col*ITEMCOUNT) + i;
                }
            }
        }
    }

    return -1;
}

void ShelfViewComponent::selectItemContainingArea(juce::Rectangle<int> area) noexcept
{
    deselectAllItems();

    juce::Rectangle<int> rect = getBounds();
    if (!area.getIntersection(rect).isEmpty())
    {
        const int bCol = (viewport->getViewPositionX() + area.getX() - viewport->getX()) / colWidth;
        const int eCol = (viewport->getViewPositionX() + area.getRight() - viewport->getX()) / colWidth;
        for (int col=bCol; col<=eCol; ++col)
        {
            if (juce::isPositiveAndBelow (col, totalItems))
            {
                for (unsigned int i=0; i<ITEMCOUNT; ++i)
                {
                    juce::Rectangle<int> bounds = CXOption::getInstance()->GetBookBounds(i);
                    bounds.setX(bounds.getX() + (col*colWidth) - viewport->getViewPositionX());
                    if (!area.getIntersection(bounds).isEmpty())
                    {
                        const int idx = (col*ITEMCOUNT) + i;
                        if ( idx < totalItems)
                        {
                            selectItem(idx, true, false);
                        }
                    }
                }
            }
        }
    }
}

int ShelfViewComponent::getInsertionIndexForPosition(int x, int y) const noexcept
{
    if (juce::isPositiveAndBelow (getWidth(), y))
    {
        int col = (viewport->getViewPositionX() + x + colWidth / 2 - viewport->getX()) / colWidth;
        col = juce::jlimit (0, totalItems, col);

        for (unsigned int i=0; i<ITEMCOUNT; ++i)
        {
            juce::Rectangle<int> bounds = CXOption::getInstance()->GetBookBounds(i);
            bounds += juce::Point<int>(col*getColWidth(), 0);
            if (bounds.contains(x, y))
            {
                return (col*ITEMCOUNT) + i;
            }
        }
    }

    return -1;
}

juce::Rectangle<int> ShelfViewComponent::getItemPosition(int idx, bool relativeToComponentTopLeft) const noexcept
{
    const int col = idx / ITEMCOUNT;
    const int i = idx % ITEMCOUNT;

    int x = viewport->getX() + colWidth * col;

    if (relativeToComponentTopLeft)
        x -= viewport->getViewPositionX();

    return CXOption::getInstance()->GetBookBounds(i) + juce::Point<int>(x, 0);
}

juce::Component* ShelfViewComponent::getComponentForItemIdx(int idx) const noexcept
{
    if (ItemComponent* const itemComp = viewport->getComponentForIdxIfOnscreen (idx))
        return static_cast<juce::Component*> (itemComp->customComponent);

    return nullptr;
}

int ShelfViewComponent::getItemIdxOfComponent(Component* itemComponent) const noexcept
{
    return viewport->getIdxOfComponent (itemComponent);
}

int ShelfViewComponent::getVisibleHeight() const noexcept
{
    return viewport->getViewHeight();
}

void ShelfViewComponent::setColWidth (int newWidth)
{
    colWidth = juce::jmax (1, newWidth);
    viewport->setSingleStepSizes (colWidth, colWidth);
    updateContent();
}

int ShelfViewComponent::getColWidth() const noexcept
{
    return colWidth;
}

int ShelfViewComponent::getNumItemOnScreen() const noexcept
{
    return ITEMCOUNT * viewport->getMaximumVisibleWidth() / colWidth;
}

void ShelfViewComponent::setMinimumContentHeight(int newMinimumHeight)
{
    minimumColHeigth = newMinimumHeight;
    updateContent();
}

int ShelfViewComponent::getVisibleContentWidth() const noexcept
{
    return viewport->getMaximumVisibleWidth();
}

void ShelfViewComponent::repaintItem(int idx) noexcept
{
    repaint (getItemPosition (idx, true));
}

void ShelfViewComponent::scrollToEnsureColIsOnscreen(int col)
{
    viewport->scrollToEnsureColIsOnscreen (col, getColWidth());
}

juce::Rectangle<int> ShelfViewComponent::getColPosition(int colNumber, bool relativeToComponentTopLeft) const noexcept
{
    int x = viewport->getX() + colWidth * colNumber;

    if (relativeToComponentTopLeft)
        x -= viewport->getViewPositionX();

    return juce::Rectangle<int> (x, viewport->getY(),
        colWidth, viewport->getViewedComponent()->getHeight());
}

void ShelfViewComponent::repaintCol(int colNumber) noexcept
{
    repaint (getColPosition (colNumber, true));
}

juce::Image ShelfViewComponent::createSnapshotOfRows(const juce::SparseSet<int>& items, int& imageX, int& imageY)
{
    juce::Rectangle<int> imageArea;
    juce::Array<juce::Component*> arrComps;

    for (unsigned int i=0; i<items.size(); ++i)
    {
        juce::Component* itemComp = viewport->getComponentForIdx(items[i]);
        if (itemComp != nullptr)
        {
            const juce::Point<int> pos (getLocalPoint (itemComp, juce::Point<int>()));
            const juce::Rectangle<int> rowRect (pos.getX(), pos.getY(), itemComp->getWidth(), itemComp->getHeight());
            imageArea = imageArea.getUnion (rowRect);
            arrComps.add(itemComp);
        }
    }

    imageArea = imageArea.getIntersection (getLocalBounds());
    imageX = imageArea.getX();
    imageY = imageArea.getY();
    juce::Image snapshot (juce::Image::ARGB, imageArea.getWidth(), imageArea.getHeight(), true);

    for (unsigned int i=0; i<arrComps.size(); ++i)
    {
        juce::Component* itemComp = arrComps[i];
        if (itemComp != nullptr)
        {
            juce::Graphics g (snapshot);
            g.setOrigin (getLocalPoint (itemComp, juce::Point<int>()) - imageArea.getPosition());

            if (g.reduceClipRegion (itemComp->getLocalBounds()))
            {
                itemComp->paintEntireComponent (g, false);
            }
        }
    }

    return snapshot;
}

ShelfViewModel* ShelfViewComponent::getModel() const noexcept
{
    return model;
}

void ShelfViewComponent::setModel(ShelfViewModel* newModel)
{
    if (model != newModel)
    {
        model = newModel;
        repaint();
        updateContent();
    }
}

void ShelfViewComponent::moveSelectedItemAnimate(const juce::DragAndDropTarget::SourceDetails& dragDetails)
{
    int dragOver = getItemContainingPosition (
        dragDetails.localPosition.getX(), 
        dragDetails.localPosition.getY());

    if (dragOver >= 0)
    {
        if (dragOver >= totalItems) dragOver = totalItems-1;

        viewport->moveItemsAnimate(selected, dragOver);
        lastDragOverItem = dragOver;
    }
}

int ShelfViewComponent::getLastDragOverItem() const noexcept
{
    return lastDragOverItem;
}

void ShelfViewComponent::setMouseDragSelectArea(juce::Rectangle<int>& area)
{
    mouseDragArea = area;

    if (!mouseDragArea.isEmpty())
        selectItemContainingArea(mouseDragArea);

    repaint();
}

void ShelfViewModel::shelfItemClicked (int idx, const juce::MouseEvent&) {}
void ShelfViewModel::shelfItemDoubleClicked (int idx, const juce::MouseEvent&) {}
void ShelfViewModel::shelfWasScrolled() {}
void ShelfViewModel::selectedItemsChanged(int lastSelectedItem) {}
void ShelfViewModel::returnKeyPressed (int lastSelectedItem) {}
void ShelfViewModel::deleteKeyPressed (int lastSelectedItem) {}
void ShelfViewModel::backgroundClicked (const juce::MouseEvent&) {}
void ShelfViewModel::paintCellItem (int idx, juce::Graphics& g, int width, int height, bool rowIsSelected) {}
juce::Component* ShelfViewModel::refreshItemComponentForIdx (int idx, bool isSelected, juce::Component* existingComponentToUpdate)
{
    (void) existingComponentToUpdate;
    jassert (existingComponentToUpdate == nullptr); // indicates a failure in the code that recycles the components
    return nullptr;
}

juce::var ShelfViewModel::getDragSourceDescription (const juce::SparseSet<int>& rowsToDescribe)
{
    return juce::var();
}

juce::String ShelfViewModel::getTooltipForItem (int idx) 
{
    return juce::String::empty;
}

juce::MouseCursor ShelfViewModel::getMouseCursorForItem(int idx)
{
    return juce::MouseCursor::NormalCursor;
}

void ShelfViewModel::paintColPanel(int col, juce::Graphics& g, int x, int y, int width, int height)
{

}
