#include "JuceHeader.h"
#include "HorListBox.h"

class HorListBox::RowComponent  : public juce::Component,
                               public juce::TooltipClient
{
public:
    RowComponent (HorListBox& lb)
        : owner (lb), row (-1),
          selected (false), isDragging (false), selectRowOnMouseUp (false)
    {
    }

    void paint (juce::Graphics& g) override
    {
        if (juce::ListBoxModel* m = owner.getModel())
            m->paintListBoxItem (row, g, getWidth(), getHeight(), selected);
    }

    void update (const int newRow, const bool nowSelected)
    {
        if (row != newRow || selected != nowSelected)
        {
            repaint();
            row = newRow;
            selected = nowSelected;
        }

        if (juce::ListBoxModel* m = owner.getModel())
        {
            setMouseCursor (m->getMouseCursorForRow (row));

            customComponent = m->refreshComponentForRow (newRow, nowSelected, customComponent.release());

            if (customComponent != nullptr)
            {
                addAndMakeVisible (customComponent);
                customComponent->setBounds (getLocalBounds());
            }
        }
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        isDragging = false;
        selectRowOnMouseUp = false;

        if (isEnabled())
        {
            if (owner.selectOnMouseDown && ! selected)
            {
                owner.selectRowsBasedOnModifierKeys (row, e.mods, false);

                if (juce::ListBoxModel* m = owner.getModel())
                    m->listBoxItemClicked (row, e);
            }
            else
            {
                selectRowOnMouseUp = true;
            }
        }
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        if (isEnabled() && selectRowOnMouseUp && ! isDragging)
        {
            owner.selectRowsBasedOnModifierKeys (row, e.mods, true);

            if (juce::ListBoxModel* m = owner.getModel())
                m->listBoxItemClicked (row, e);
        }
    }

    void mouseDoubleClick (const juce::MouseEvent& e) override
    {
        if (juce::ListBoxModel* m = owner.getModel())
            if (isEnabled())
                m->listBoxItemDoubleClicked (row, e);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (juce::ListBoxModel* m = owner.getModel())
        {
            if (isEnabled() && ! (e.mouseWasClicked() || isDragging))
            {
                juce::SparseSet<int> rowsToDrag;

                if (owner.selectOnMouseDown || owner.isRowSelected (row))
                    rowsToDrag = owner.getSelectedRows();
                else
                    rowsToDrag.addRange (juce::Range<int>::withStartAndLength (row, 1));

                if (rowsToDrag.size() > 0)
                {
                    const juce::var dragDescription (m->getDragSourceDescription (rowsToDrag));

                    if (! (dragDescription.isVoid() || (dragDescription.isString() && dragDescription.toString().isEmpty())))
                    {
                        isDragging = true;
                        owner.startDragAndDrop (e, rowsToDrag, dragDescription, true);
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
        if (juce::ListBoxModel* m = owner.getModel())
            return m->getTooltipForRow (row);

        return juce::String::empty;
    }

    juce::ScopedPointer<juce::Component> customComponent;

private:
    HorListBox& owner;
    int row;
    bool selected, isDragging, selectRowOnMouseUp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RowComponent)
};


//==============================================================================
class HorListBox::ListViewport  : public juce::Viewport
{
public:
    ListViewport (HorListBox& lb)
        : owner (lb)
    {
        setWantsKeyboardFocus (false);

        juce::Component* const content = new juce::Component();
        setViewedComponent (content);
        content->setWantsKeyboardFocus (false);
    }

    RowComponent* getComponentForRow (const int row) const noexcept
    {
        return rows [row % juce::jmax (1, rows.size())];
    }

    RowComponent* getComponentForRowIfOnscreen (const int row) const noexcept
    {
        return (row >= firstIndex && row < firstIndex + rows.size())
                 ? getComponentForRow (row) : nullptr;
    }

    int getRowNumberOfComponent (juce::Component* const rowComponent) const noexcept
    {
        const int index = getViewedComponent()->getIndexOfChildComponent (rowComponent);
        const int num = rows.size();

        for (int i = num; --i >= 0;)
            if (((firstIndex + i) % juce::jmax (1, num)) == index)
                return firstIndex + i;

        return -1;
    }

    void visibleAreaChanged (const juce::Rectangle<int>&) override
    {
        updateVisibleArea (true);

        if (juce::ListBoxModel* m = owner.getModel())
            m->listWasScrolled();
    }

    void updateVisibleArea (const bool makeSureItUpdatesContent)
    {
        hasUpdated = false;

        juce::Component& content = *getViewedComponent();
        int newX = content.getX();
        const int newY = content.getY();
        const int newW = owner.totalItems * owner.getRowWidth();
        const int newH = juce::jmax (owner.minimumRowHeigth, getMaximumVisibleHeight());

        if (newX + newW < getMaximumVisibleWidth() && newW > getMaximumVisibleWidth())
            newX = getMaximumVisibleWidth() - newW;

        content.setBounds (newX, newY, newW, newH);

        if (makeSureItUpdatesContent && ! hasUpdated)
            updateContents();
    }

    void updateContents()
    {
        hasUpdated = true;
        const int rowW = owner.getRowWidth();
        juce::Component& content = *getViewedComponent();

        if (rowW > 0)
        {
            const int x = getViewPositionX();
            const int h = content.getHeight();

            const int numNeeded = 2 + getMaximumVisibleWidth() / rowW;
            rows.removeRange (numNeeded, rows.size());

            while (numNeeded > rows.size())
            {
                RowComponent* newRow = new RowComponent (owner);
                rows.add (newRow);
                content.addAndMakeVisible (newRow);
            }

            firstIndex = x / rowW;
            firstWholeIndex = (x + rowW - 1) / rowW;
            lastWholeIndex = (x + getMaximumVisibleWidth() - 1) / rowW;

            for (int i = 0; i < numNeeded; ++i)
            {
                const int row = i + firstIndex;

                if (RowComponent* const rowComp = getComponentForRow (row))
                {
                    rowComp->setBounds (row * rowW, 0, rowW, h);
                    rowComp->update (row, owner.isRowSelected (row));
                }
            }
        }
    }

    void selectRow (const int row, const int rowW, const bool dontScroll,
                    const int lastSelectedRow, const int totalRows, const bool isMouseClick)
    {
        hasUpdated = false;

        if (row < firstWholeIndex && ! dontScroll)
        {
            setViewPosition (row * rowW, getViewPositionY());
        }
        else if (row >= lastWholeIndex && ! dontScroll)
        {
            const int rowsOnScreen = lastWholeIndex - firstWholeIndex;

            if (row >= lastSelectedRow + rowsOnScreen
                 && rowsOnScreen < totalRows - 1
                 && ! isMouseClick)
            {
                setViewPosition (juce::jlimit (0, juce::jmax (0, totalRows - rowsOnScreen), row) * rowW, getViewPositionY());
            }
            else
            {
                setViewPosition (juce::jmax (0, (row  + 1) * rowW - getMaximumVisibleWidth()), getViewPositionY());
            }
        }

        if (! hasUpdated)
            updateContents();
    }

    void scrollToEnsureRowIsOnscreen (const int row, const int rowW)
    {
        if (row < firstWholeIndex)
        {
            setViewPosition (row * rowW, getViewPositionY());
        }
        else if (row >= lastWholeIndex)
        {
            setViewPosition (juce::jmax (0, (row  + 1) * rowW - getMaximumVisibleWidth()), getViewPositionY());
        }
    }

    void paint (juce::Graphics& g) override
    {
        if (isOpaque())
            g.fillAll (owner.findColour (HorListBox::backgroundColourId));
    }

    bool keyPressed (const juce::KeyPress& key) override
    {
        if (juce::Viewport::respondsToKey (key))
        {
            const int allowableMods = owner.multipleSelection ? juce::ModifierKeys::shiftModifier : 0;

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
    HorListBox& owner;
    juce::OwnedArray<RowComponent> rows;
    int firstIndex, firstWholeIndex, lastWholeIndex;
    bool hasUpdated;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListViewport)
};

//==============================================================================
class ListBoxMouseMoveSelector  : public juce::MouseListener
{
public:
    ListBoxMouseMoveSelector (HorListBox& lb) : owner (lb)
    {
        owner.addMouseListener (this, true);
    }

    void mouseMove (const juce::MouseEvent& e) override
    {
        const juce::MouseEvent e2 (e.getEventRelativeTo (&owner));
        owner.selectRow (owner.getRowContainingPosition (e2.x, e2.y), true);
    }

    void mouseExit (const juce::MouseEvent& e) override
    {
        mouseMove (e);
    }

private:
    HorListBox& owner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListBoxMouseMoveSelector)
};


//==============================================================================
HorListBox::HorListBox (const juce::String& name, juce::ListBoxModel* const m)
    : juce::Component (name),
      model (m),
      totalItems (0),
      rowWidth (22),
      minimumRowHeigth (0),
      outlineThickness (0),
      lastRowSelected (-1),
      multipleSelection (false),
      alwaysFlipSelection (false),
      hasDoneInitialUpdate (false),
      selectOnMouseDown (true),
      selectionEnabled(true)
{
    addAndMakeVisible (viewport = new ListViewport (*this));

    HorListBox::setWantsKeyboardFocus (true);
    HorListBox::colourChanged();
}

HorListBox::~HorListBox()
{
    viewport = nullptr;
}

void HorListBox::setModel (juce::ListBoxModel* const newModel)
{
    if (model != newModel)
    {
        model = newModel;
        repaint();
        updateContent();
    }
}

void HorListBox::setMultipleSelectionEnabled (bool b) noexcept         { multipleSelection = b; }
void HorListBox::setClickingTogglesRowSelection (bool b) noexcept      { alwaysFlipSelection = b; }
void HorListBox::setRowSelectedOnMouseDown (bool b) noexcept           { selectOnMouseDown = b; }
void HorListBox::setRowSelectionEnabled (bool b) noexcept           { selectionEnabled = b; }

void HorListBox::setMouseMoveSelectsRows (bool b)
{
    if (b)
    {
        if (mouseMoveSelector == nullptr)
            mouseMoveSelector = new ListBoxMouseMoveSelector (*this);
    }
    else
    {
        mouseMoveSelector = nullptr;
    }
}

//==============================================================================
void HorListBox::paint (juce::Graphics& g)
{
    if (! hasDoneInitialUpdate)
        updateContent();

    g.fillAll (findColour (backgroundColourId));
}

void HorListBox::paintOverChildren (juce::Graphics& g)
{
    if (outlineThickness > 0)
    {
        g.setColour (findColour (outlineColourId));
        g.drawRect (getLocalBounds(), outlineThickness);
    }
}

void HorListBox::resized()
{
    viewport->setBoundsInset (juce::BorderSize<int> (outlineThickness,
                                               outlineThickness, outlineThickness, outlineThickness));

    viewport->setSingleStepSizes (getRowWidth(), 20);

    viewport->updateVisibleArea (false);
}

void HorListBox::visibilityChanged()
{
    viewport->updateVisibleArea (true);
}

juce::Viewport* HorListBox::getViewport() const noexcept
{
    return viewport;
}

#undef max
//==============================================================================
void HorListBox::updateContent()
{
    hasDoneInitialUpdate = true;
    totalItems = (model != nullptr) ? model->getNumRows() : 0;

    bool selectionChanged = false;

    if (selected.size() > 0 && selected [selected.size() - 1] >= totalItems)
    {
        selected.removeRange (juce::Range<int> (totalItems, std::numeric_limits<int>::max()));
        lastRowSelected = getSelectedRow (0);
        selectionChanged = true;
    }

    viewport->updateVisibleArea (isVisible());
    viewport->resized();

    if (selectionChanged && model != nullptr)
        model->selectedRowsChanged (lastRowSelected);
}

//==============================================================================
void HorListBox::selectRow (int row, bool dontScroll, bool deselectOthersFirst)
{
    selectRowInternal (row, dontScroll, deselectOthersFirst, false);
}

void HorListBox::selectRowInternal (const int row,
                                 bool dontScroll,
                                 bool deselectOthersFirst,
                                 bool isMouseClick)
{
    if (! multipleSelection)
        deselectOthersFirst = true;

    if ((! isRowSelected (row))
         || (deselectOthersFirst && getNumSelectedRows() > 1))
    {
        if (juce::isPositiveAndBelow (row, totalItems))
        {
            if (deselectOthersFirst)
                selected.clear();

            selected.addRange (juce::Range<int> (row, row + 1));

            if (getHeight() == 0 || getWidth() == 0)
                dontScroll = true;

            viewport->selectRow (row, getRowWidth(), dontScroll,
                                 lastRowSelected, totalItems, isMouseClick);

            lastRowSelected = row;
            model->selectedRowsChanged (row);
        }
        else
        {
            if (deselectOthersFirst)
                deselectAllRows();
        }
    }
}

void HorListBox::deselectRow (const int row)
{
    if (selected.contains (row))
    {
        selected.removeRange (juce::Range<int> (row, row + 1));

        if (row == lastRowSelected)
            lastRowSelected = getSelectedRow (0);

        viewport->updateContents();
        model->selectedRowsChanged (lastRowSelected);
    }
}

void HorListBox::setSelectedRows (const juce::SparseSet<int>& setOfRowsToBeSelected,
                               const juce::NotificationType sendNotificationEventToModel)
{
    selected = setOfRowsToBeSelected;
    selected.removeRange (juce::Range<int> (totalItems, std::numeric_limits<int>::max()));

    if (! isRowSelected (lastRowSelected))
        lastRowSelected = getSelectedRow (0);

    viewport->updateContents();

    if (model != nullptr && sendNotificationEventToModel == juce::sendNotification)
        model->selectedRowsChanged (lastRowSelected);
}

juce::SparseSet<int> HorListBox::getSelectedRows() const
{
    return selected;
}

void HorListBox::selectRangeOfRows (int firstRow, int lastRow)
{
    if (multipleSelection && (firstRow != lastRow))
    {
        const int numRows = totalItems - 1;
        firstRow = juce::jlimit (0, juce::jmax (0, numRows), firstRow);
        lastRow  = juce::jlimit (0, juce::jmax (0, numRows), lastRow);

        selected.addRange (juce::Range<int> (juce::jmin (firstRow, lastRow),
                                       juce::jmax (firstRow, lastRow) + 1));

        selected.removeRange (juce::Range<int> (lastRow, lastRow + 1));
    }

    selectRowInternal (lastRow, false, false, true);
}

void HorListBox::flipRowSelection (const int row)
{
    if (isRowSelected (row))
        deselectRow (row);
    else
        selectRowInternal (row, false, false, true);
}

void HorListBox::deselectAllRows()
{
    if (! selected.isEmpty())
    {
        selected.clear();
        lastRowSelected = -1;

        viewport->updateContents();

        if (model != nullptr)
            model->selectedRowsChanged (lastRowSelected);
    }
}

void HorListBox::selectRowsBasedOnModifierKeys (const int row,
                                             juce::ModifierKeys mods,
                                             const bool isMouseUpEvent)
{
    if (selectionEnabled)
    {
        if (multipleSelection && (mods.isCommandDown() || alwaysFlipSelection))
        {
            flipRowSelection (row);
        }
        else if (multipleSelection && mods.isShiftDown() && lastRowSelected >= 0)
        {
            selectRangeOfRows (lastRowSelected, row);
        }
        else if ((! mods.isPopupMenu()) || ! isRowSelected (row))
        {
            selectRowInternal (row, false, ! (multipleSelection && (! isMouseUpEvent) && isRowSelected (row)), true);
        }
    }
}

int HorListBox::getNumSelectedRows() const
{
    return selected.size();
}

int HorListBox::getSelectedRow (const int index) const
{
    return (juce::isPositiveAndBelow (index, selected.size()))
                ? selected [index] : -1;
}

bool HorListBox::isRowSelected (const int row) const
{
    return selected.contains (row);
}

int HorListBox::getLastRowSelected() const
{
    return isRowSelected (lastRowSelected) ? lastRowSelected : -1;
}

//==============================================================================
int HorListBox::getRowContainingPosition (const int x, const int y) const noexcept
{
    if (juce::isPositiveAndBelow (getWidth(), y))
    {
        const int row = (viewport->getViewPositionX() + x - viewport->getX()) / rowWidth;

        if (juce::isPositiveAndBelow (row, totalItems))
            return row;
    }

    return -1;
}

int HorListBox::getInsertionIndexForPosition (const int x, const int y) const noexcept
{
    if (juce::isPositiveAndBelow (getWidth(), y))
    {
        const int row = (viewport->getViewPositionX() + x + rowWidth / 2 - viewport->getX()) / rowWidth;
        return juce::jlimit (0, totalItems, row);
    }

    return -1;
}

juce::Component* HorListBox::getComponentForRowNumber (const int row) const noexcept
{
    if (RowComponent* const listRowComp = viewport->getComponentForRowIfOnscreen (row))
        return static_cast<juce::Component*> (listRowComp->customComponent);

    return nullptr;
}

int HorListBox::getRowNumberOfComponent (juce::Component* const rowComponent) const noexcept
{
    return viewport->getRowNumberOfComponent (rowComponent);
}

juce::Rectangle<int> HorListBox::getRowPosition (const int rowNumber,
                                        const bool relativeToComponentTopLeft) const noexcept
{
    int x = viewport->getX() + rowWidth * rowNumber;

    if (relativeToComponentTopLeft)
        x -= viewport->getViewPositionX();

    return juce::Rectangle<int> (x, viewport->getY(),
                           rowWidth, viewport->getViewedComponent()->getHeight());
}

void HorListBox::setHorizontalPosition (const double proportion)
{
    const int offscreen = viewport->getViewedComponent()->getWidth() - viewport->getWidth();

    viewport->setViewPosition (juce::jmax (0, juce::roundToInt (proportion * offscreen)),
                               viewport->getViewPositionY());
}

double HorListBox::getHorizontalPosition() const
{
    const int offscreen = viewport->getViewedComponent()->getWidth() - viewport->getWidth();

    return (offscreen > 0) ? viewport->getViewPositionX() / (double) offscreen : 0;
}

int HorListBox::getVisibleRowHeight() const noexcept
{
    return viewport->getViewHeight();
}

void HorListBox::scrollToEnsureRowIsOnscreen (const int row)
{
    viewport->scrollToEnsureRowIsOnscreen (row, getRowWidth());
}

//==============================================================================
bool HorListBox::keyPressed (const juce::KeyPress& key)
{
    const int numVisibleRows = viewport->getWidth() / getRowWidth();

    const bool multiple = multipleSelection
                            && lastRowSelected >= 0
                            && key.getModifiers().isShiftDown();

    if (key.isKeyCode (juce::KeyPress::upKey))
    {
        if (multiple)
            selectRangeOfRows (lastRowSelected, lastRowSelected - 1);
        else
            selectRow (juce::jmax (0, lastRowSelected - 1));
    }
    else if (key.isKeyCode (juce::KeyPress::downKey))
    {
        if (multiple)
            selectRangeOfRows (lastRowSelected, lastRowSelected + 1);
        else
            selectRow (juce::jmin (totalItems - 1, juce::jmax (0, lastRowSelected) + 1));
    }
    else if (key.isKeyCode (juce::KeyPress::pageUpKey))
    {
        if (multiple)
            selectRangeOfRows (lastRowSelected, lastRowSelected - numVisibleRows);
        else
            selectRow (juce::jmax (0, juce::jmax (0, lastRowSelected) - numVisibleRows));
    }
    else if (key.isKeyCode (juce::KeyPress::pageDownKey))
    {
        if (multiple)
            selectRangeOfRows (lastRowSelected, lastRowSelected + numVisibleRows);
        else
            selectRow (juce::jmin (totalItems - 1, juce::jmax (0, lastRowSelected) + numVisibleRows));
    }
    else if (key.isKeyCode (juce::KeyPress::homeKey))
    {
        if (multiple)
            selectRangeOfRows (lastRowSelected, 0);
        else
            selectRow (0);
    }
    else if (key.isKeyCode (juce::KeyPress::endKey))
    {
        if (multiple)
            selectRangeOfRows (lastRowSelected, totalItems - 1);
        else
            selectRow (totalItems - 1);
    }
    else if (key.isKeyCode (juce::KeyPress::returnKey) && isRowSelected (lastRowSelected))
    {
        if (model != nullptr)
            model->returnKeyPressed (lastRowSelected);
    }
    else if ((key.isKeyCode (juce::KeyPress::deleteKey) || key.isKeyCode (juce::KeyPress::backspaceKey))
               && isRowSelected (lastRowSelected))
    {
        if (model != nullptr)
            model->deleteKeyPressed (lastRowSelected);
    }
    else if (multipleSelection && key == juce::KeyPress ('a', juce::ModifierKeys::commandModifier, 0))
    {
        selectRangeOfRows (0, std::numeric_limits<int>::max());
    }
    else
    {
        return false;
    }

    return true;
}

bool HorListBox::keyStateChanged (const bool isKeyDown)
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

void HorListBox::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
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

void HorListBox::mouseUp (const juce::MouseEvent& e)
{
    if (e.mouseWasClicked() && model != nullptr)
        model->backgroundClicked (e);
}

//==============================================================================
void HorListBox::setRowWidth (const int newWidth)
{
    rowWidth = juce::jmax (1, newWidth);
    viewport->setSingleStepSizes (rowWidth, 20);
    updateContent();
}

int HorListBox::getNumRowsOnScreen() const noexcept
{
    return viewport->getMaximumVisibleWidth() / rowWidth;
}

void HorListBox::setMinimumContentHeight (const int newMinimumHeight)
{
    minimumRowHeigth = newMinimumHeight;
    updateContent();
}

int HorListBox::getVisibleContentWidth() const noexcept
{
    return viewport->getMaximumVisibleWidth();
}

juce::ScrollBar* HorListBox::getVerticalScrollBar() const noexcept
{
    return viewport->getVerticalScrollBar();
}

juce::ScrollBar* HorListBox::getHorizontalScrollBar() const noexcept
{
    return viewport->getHorizontalScrollBar();
}

void HorListBox::colourChanged()
{
    setOpaque (findColour (backgroundColourId).isOpaque());
    viewport->setOpaque (isOpaque());
    repaint();
}

void HorListBox::parentHierarchyChanged()
{
    colourChanged();
}

void HorListBox::setOutlineThickness (const int newThickness)
{
    outlineThickness = newThickness;
    resized();
}

void HorListBox::repaintRow (const int rowNumber) noexcept
{
    repaint (getRowPosition (rowNumber, true));
}

juce::Image HorListBox::createSnapshotOfRows (const juce::SparseSet<int>& rows, int& imageX, int& imageY)
{
    juce::Rectangle<int> imageArea;
    const int firstRow = getRowContainingPosition (viewport->getX(), 0);

    for (int i = getNumRowsOnScreen() + 2; --i >= 0;)
    {
        juce::Component* rowComp = viewport->getComponentForRowIfOnscreen (firstRow + i);

        if (rowComp != nullptr && rows.contains (firstRow + i))
        {
            const juce::Point<int> pos (getLocalPoint (rowComp, juce::Point<int>()));
            const juce::Rectangle<int> rowRect (pos.getX(), pos.getY(), rowComp->getWidth(), rowComp->getHeight());
            imageArea = imageArea.getUnion (rowRect);
        }
    }

    imageArea = imageArea.getIntersection (getLocalBounds());
    imageX = imageArea.getX();
    imageY = imageArea.getY();
    juce::Image snapshot (juce::Image::ARGB, imageArea.getWidth(), imageArea.getHeight(), true);

    for (int i = getNumRowsOnScreen() + 2; --i >= 0;)
    {
        juce::Component* rowComp = viewport->getComponentForRowIfOnscreen (firstRow + i);

        if (rowComp != nullptr && rows.contains (firstRow + i))
        {
            juce::Graphics g (snapshot);
            g.setOrigin (getLocalPoint (rowComp, juce::Point<int>()) - imageArea.getPosition());

            if (g.reduceClipRegion (rowComp->getLocalBounds()))
            {
                g.beginTransparencyLayer (0.6f);
                rowComp->paintEntireComponent (g, false);
                g.endTransparencyLayer();
            }
        }
    }

    return snapshot;
}

void HorListBox::startDragAndDrop (const juce::MouseEvent& e, const juce::SparseSet<int>& rowsToDrag, const juce::var& dragDescription, bool allowDraggingToOtherWindows)
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
