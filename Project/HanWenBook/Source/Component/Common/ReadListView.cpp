#include "JuceHeader.h"
#include "ReadListView.h"
#undef max

class ReadListView::RowComponent  : public juce::Component
{
public:
    RowComponent (ReadListView& lb)
        : owner (lb), row (-1), isNeedUpdateContent(true)
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour(juce::Colours::red);
    }

    void update (const int newRow, bool reload = false)
    {
        if (row != newRow || isNeedUpdateContent || reload)
        {
            repaint();
            row = newRow;

            if (ReadListViewModel* m = owner.getModel())
            {
                customComponent = m->refreshComponentForRow (newRow, customComponent.release());

                if (customComponent != nullptr)
                {
                    addAndMakeVisible (customComponent);
                    customComponent->setBounds (getLocalBounds());
                }
            }
            isNeedUpdateContent = false;
        }
    }

    void resized() override
    {
        if (customComponent != nullptr)
            customComponent->setBounds (getLocalBounds());

        const juce::Rectangle<int> b = getBounds();
        if (lastRowBounds != b)
        {
            isNeedUpdateContent = true;
            lastRowBounds = b;
        }
    }

    juce::ScopedPointer<juce::Component> customComponent;

private:
    ReadListView& owner;
    int row;
    bool isNeedUpdateContent;
    juce::Rectangle<int> lastRowBounds;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RowComponent)
};


//==============================================================================
class ReadListView::ListViewport  : public juce::Viewport
{
public:
    ListViewport (ReadListView& lb)
        : owner (lb), firstWholeIndex(0)
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

    void visibleAreaChanged (const juce::Rectangle<int>& r) override
    {
        updateVisibleArea (true);

        if (ReadListViewModel* m = owner.getModel())
            m->listWasScrolled();
    }

    void updateVisibleArea (const bool makeSureItUpdatesContent)
    {
        hasUpdated = false;

        juce::Component& content = *getViewedComponent();
        int newX = content.getX();
        int newY = content.getY();
        int newW, newH;

        if (owner.vertical)
        {
            newW = juce::jmax (owner.minimumRowSize, getMaximumVisibleWidth());
            newH = owner.totalItems * owner.getRowSize();

            if (newY + newH < getMaximumVisibleHeight() && newH > getMaximumVisibleHeight())
                newY = getMaximumVisibleHeight() - newH;
        }
        else
        {
            newW = owner.totalItems * owner.getRowSize();
            newH = juce::jmax (owner.minimumRowSize, getMaximumVisibleHeight());

            if (newX + newW < getMaximumVisibleWidth() && newW > getMaximumVisibleWidth())
                newX = getMaximumVisibleWidth() - newW;
        }

        content.setBounds (newX, newY, newW, newH);

        if (makeSureItUpdatesContent && ! hasUpdated)
            updateContents();
    }

    void updateContents(bool reload = false)
    {
        hasUpdated = true;
        const int rowSize = owner.getRowSize();
        juce::Component& content = *getViewedComponent();

        if (rowSize > 0)
        {
            if (owner.vertical)
            {
                const int y = getViewPositionY();
                const int w = content.getWidth();

                const int numNeeded = 2 + getMaximumVisibleHeight() / rowSize;
                rows.removeRange (numNeeded, rows.size());

                while (numNeeded > rows.size())
                {
                    RowComponent* newRow = new RowComponent (owner);
                    rows.add (newRow);
                    content.addAndMakeVisible (newRow);
                }

                firstIndex = y / rowSize;
                firstWholeIndex = (y + rowSize - 1) / rowSize;
                lastWholeIndex = (y + getMaximumVisibleHeight() - 1) / rowSize;

                for (int i = 0; i < numNeeded; ++i)
                {
                    const int row = i + firstIndex;

                    if (RowComponent* const rowComp = getComponentForRow (row))
                    {
                        rowComp->setBounds (0, row * rowSize, w, rowSize);
                        rowComp->update (row, reload);
                    }
                }
            }
            else
            {
                const int x = getViewPositionX();
                const int h = content.getHeight();

                const int numNeeded = 2 + getMaximumVisibleWidth() / rowSize;
                rows.removeRange (numNeeded, rows.size());

                while (numNeeded > rows.size())
                {
                    RowComponent* newRow = new RowComponent (owner);
                    rows.add (newRow);
                    content.addAndMakeVisible (newRow);
                }

                firstIndex = x / rowSize;
                firstWholeIndex = (x + rowSize - 1) / rowSize;
                lastWholeIndex = (x + getMaximumVisibleWidth() - 1) / rowSize;

                for (int i = 0; i < numNeeded; ++i)
                {
                    const int row = i + firstIndex;

                    if (RowComponent* const rowComp = getComponentForRow (row))
                    {
                        rowComp->setBounds (row * rowSize, 0, rowSize, h);
                        rowComp->update (row, reload);
                    }
                }
            }
        }
    }

    void scrollToEnsureRowIsOnscreen (const int row, const int s)
    {
        if (owner.vertical)
        {
            if (row < firstWholeIndex)
            {
                setViewPosition (getViewPositionX(), row * s);
            }
            else if (row >= lastWholeIndex)
            {
                setViewPosition (getViewPositionX(),
                    juce::jmax (0, (row  + 1) * s - getMaximumVisibleHeight()));
            }
        }
        else
        {
            if (row < firstWholeIndex)
            {
                setViewPosition (row * s, getViewPositionY());
            }
            else if (row >= lastWholeIndex)
            {
                setViewPosition (juce::jmax (0, (row  + 1) * s - getMaximumVisibleWidth()), getViewPositionY());
            }
        }
    }

    void paint (juce::Graphics& g) override
    {
        if (isOpaque())
            g.fillAll (owner.findColour (ReadListView::backgroundColourId));
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

    void mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override
    {
        if (owner.vertical)
        {
            juce::Viewport::mouseWheelMove(e, wheel);
        }
        else
        {
            if (! useMouseWheelMoveIfNeeded (e, wheel))
                Component::mouseWheelMove (e, wheel);
        }
    }

    bool useMouseWheelMoveIfNeeded (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
    {
        if (! (e.mods.isAltDown() || e.mods.isCtrlDown() || e.mods.isCommandDown()))
        {
            const int deltaX = wheel.deltaX == 0.0f ? 0 : (wheel.deltaX > 0.0f ? 2*owner.getRowSize() : -2*owner.getRowSize());
            const int deltaY = wheel.deltaY == 0.0f ? 0 : (wheel.deltaY > 0.0f ? 2*owner.getRowSize() : -2*owner.getRowSize());

            juce::Point<int> pos (getViewPosition());

            if (deltaX != 0 && deltaY != 0)
            {
                pos.x -= deltaX;
                pos.y -= deltaY;
            }
            else if (deltaX != 0)
            {
                pos.x -= deltaX != 0 ? deltaX : deltaY;
            }
            else if (deltaY != 0)
            {
                pos.x -= deltaX != 0 ? deltaX : deltaY;
            }

            if (pos != getViewPosition())
            {
                setViewPosition (pos);
                return true;
            }
        }

        return false;
    }

    int getFirstWholeIndex() const
    {
        return firstWholeIndex;
    }

    int getCurrentWholeIndex()
    {
        const int s = owner.getRowSize();
        if (s > 0)
        {
            const int pos = owner.vertical ? getViewPositionY() : getViewPositionX();
            return (pos+(s/2)) / s;
        }

        return firstWholeIndex;
    }

private:
    ReadListView& owner;
    juce::OwnedArray<RowComponent> rows;
    int firstIndex, firstWholeIndex, lastWholeIndex;
    bool hasUpdated;
    juce::Rectangle<int> lastBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListViewport)
};

//==============================================================================
ReadListView::ReadListView (const juce::String& name, ReadListViewModel* const m, bool v)
    : juce::Component (name),
      model (m),
      totalItems (0),
      rowSize (22),
      minimumRowSize (0),
      hasDoneInitialUpdate (false),
      vertical(v)
{
    addAndMakeVisible (viewport = new ListViewport (*this));

    ReadListView::setWantsKeyboardFocus (true);
    ReadListView::colourChanged();
}

ReadListView::~ReadListView()
{
    viewport = nullptr;
}

void ReadListView::setModel (ReadListViewModel* const newModel)
{
    if (model != newModel)
    {
        model = newModel;
        repaint();
        updateContent();
    }
}

//==============================================================================
void ReadListView::paint (juce::Graphics& g)
{
    if (! hasDoneInitialUpdate)
        updateContent();

    g.fillAll (findColour (backgroundColourId));
}

void ReadListView::paintOverChildren (juce::Graphics&)
{
}

void ReadListView::resized()
{
    viewport->setBoundsInset (juce::BorderSize<int> (0, 0, 0, 0));
    viewport->updateVisibleArea (false);
}

void ReadListView::visibilityChanged()
{
    viewport->updateVisibleArea (isVisible());
}

juce::Viewport* ReadListView::getViewport() const noexcept
{
    return viewport;
}

//==============================================================================
void ReadListView::updateContent()
{
    hasDoneInitialUpdate = true;
    totalItems = (model != nullptr) ? model->getNumRows() : 0;
    viewport->updateVisibleArea (isVisible());
    viewport->resized();
}

//==============================================================================
int ReadListView::getRowContainingPosition (const int x, const int y) const noexcept
{
    if (vertical)
    {
        if (juce::isPositiveAndBelow (x, getWidth()))
        {
            const int row = (viewport->getViewPositionY() + y - viewport->getY()) / rowSize;

            if (juce::isPositiveAndBelow (row, totalItems))
                return row;
        }
    }
    else
    {
        if (juce::isPositiveAndBelow (y, getHeight()))
        {
            const int row = (viewport->getViewPositionX() + x - viewport->getX()) / rowSize;

            if (juce::isPositiveAndBelow (row, totalItems))
                return row;
        }
    }

    return -1;
}

int ReadListView::getInsertionIndexForPosition (const int x, const int y) const noexcept
{
    if (vertical)
    {
        if (juce::isPositiveAndBelow (x, getWidth()))
        {
            const int row = (viewport->getViewPositionY() + y + rowSize / 2 - viewport->getY()) / rowSize;
            return juce::jlimit (0, totalItems, row);
        }
    }
    else
    {
        if (juce::isPositiveAndBelow (y, getHeight()))
        {
            const int row = (viewport->getViewPositionX() + x + rowSize / 2 - viewport->getX()) / rowSize;
            return juce::jlimit (0, totalItems, row);
        }
    }

    return -1;
}

juce::Component* ReadListView::getComponentForRowNumber (const int row) const noexcept
{
    if (RowComponent* const listRowComp = viewport->getComponentForRowIfOnscreen (row))
        return static_cast<juce::Component*> (listRowComp->customComponent);

    return nullptr;
}

int ReadListView::getRowNumberOfComponent (juce::Component* const rowComponent) const noexcept
{
    return viewport->getRowNumberOfComponent (rowComponent);
}

juce::Rectangle<int> ReadListView::getRowPosition (const int rowNumber,
                                        const bool relativeToComponentTopLeft) const noexcept
{
    if (vertical)
    {
        int y = viewport->getY() + rowSize * rowNumber;

        if (relativeToComponentTopLeft)
            y -= viewport->getViewPositionY();

        return juce::Rectangle<int> (viewport->getX(), y,
            viewport->getViewedComponent()->getWidth(), rowSize);
    }
    else
    {
        int x = viewport->getX() + rowSize * rowNumber;

        if (relativeToComponentTopLeft)
            x -= viewport->getViewPositionX();

        return juce::Rectangle<int> (x, viewport->getY(),
            rowSize, viewport->getViewedComponent()->getHeight());
    }

    return juce::Rectangle<int>();
}

void ReadListView::setScrollPosition (const double proportion)
{
    if (vertical)
    {
        const int offscreen = viewport->getViewedComponent()->getHeight() - viewport->getHeight();
        viewport->setViewPosition (viewport->getViewPositionX(),
            juce::jmax (0, juce::roundToInt (proportion * offscreen)));
    }
    else
    {
        const int offscreen = viewport->getViewedComponent()->getWidth() - viewport->getWidth();
        viewport->setViewPosition (juce::jmax (0, juce::roundToInt (proportion * offscreen)),
            viewport->getViewPositionY());
    }
}

double ReadListView::getScrollPosition() const
{
    if (vertical)
    {
        const int offscreen = viewport->getViewedComponent()->getHeight() - viewport->getHeight();
        return (offscreen > 0) ? viewport->getViewPositionY() / (double) offscreen : 0;
    }
    else
    {
        const int offscreen = viewport->getViewedComponent()->getWidth() - viewport->getWidth();
        return (offscreen > 0) ? viewport->getViewPositionX() / (double) offscreen : 0;
    }
}

void ReadListView::scrollToEnsureRowIsOnscreen (const int row)
{
    viewport->scrollToEnsureRowIsOnscreen (row, getRowSize());
}

//==============================================================================
bool ReadListView::keyPressed (const juce::KeyPress& key)
{
    return false;
}

bool ReadListView::keyStateChanged (const bool isKeyDown)
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

void ReadListView::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
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
        juce::Component::mouseWheelMove (e, wheel);
}

void ReadListView::mouseUp (const juce::MouseEvent& e)
{

}

//==============================================================================
void ReadListView::setRowSize (const int newSize)
{
    if (rowSize != newSize)
    {
        const double pos = getScrollPosition();
        rowSize = juce::jmax (1, newSize);
        updateContent();
        setScrollPosition(pos);
    }
}

int ReadListView::getNumRowsOnScreen() const noexcept
{
    return (vertical ? viewport->getMaximumVisibleHeight() : viewport->getMaximumVisibleWidth()) / rowSize;
}

void ReadListView::setMinimumContentWidth (const int newMinimumWidth)
{
    minimumRowSize = newMinimumWidth;
    updateContent();
}

juce::ScrollBar* ReadListView::getVerticalScrollBar() const noexcept
{
    return viewport->getVerticalScrollBar();
}

juce::ScrollBar* ReadListView::getHorizontalScrollBar() const noexcept
{
    return viewport->getHorizontalScrollBar();
}

void ReadListView::colourChanged()
{
    setOpaque (findColour (backgroundColourId).isOpaque());
    viewport->setOpaque (isOpaque());
    repaint();
}

void ReadListView::parentHierarchyChanged()
{
    colourChanged();
}

void ReadListView::repaintRow (const int rowNumber) noexcept
{
    repaint (getRowPosition (rowNumber, true));
}

juce::Image ReadListView::createSnapshotOfRows (const juce::SparseSet<int>& rows, int& imageX, int& imageY)
{
    juce::Rectangle<int> imageArea;
    const int firstRow = vertical ? getRowContainingPosition (0, viewport->getY()) : getRowContainingPosition (viewport->getX(), 0);

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

int ReadListView::getFirstWholeIndex() const
{
    return viewport->getFirstWholeIndex();
}

void ReadListView::reloadData()
{
    viewport->updateContents(true);
}

int ReadListView::getCurrentWholeIndex()
{
    return viewport->getCurrentWholeIndex();
}

//==============================================================================
juce::Component* ReadListViewModel::refreshComponentForRow (int, juce::Component* existingComponentToUpdate)
{
    (void) existingComponentToUpdate;
    jassert (existingComponentToUpdate == nullptr); // indicates a failure in the code that recycles the components
    return nullptr;
}

void ReadListViewModel::listWasScrolled() {}
