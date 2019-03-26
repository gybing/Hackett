#pragma once

class ShelfViewModel
{
public:
    virtual int getNumItems() = 0;
    virtual juce::Component* refreshItemComponentForIdx (int idx, bool isSelected, juce::Component* existingComponentToUpdate);
    virtual void shelfItemClicked (int idx, const juce::MouseEvent&);
    virtual void shelfItemDoubleClicked (int idx, const juce::MouseEvent&);
    virtual juce::var getDragSourceDescription (const juce::SparseSet<int>& rowsToDescribe);
    virtual juce::String getTooltipForItem (int idx);
    virtual void shelfWasScrolled();
    virtual void selectedItemsChanged(int lastSelectedItem);
    virtual void returnKeyPressed (int lastSelectedItem);
    virtual void deleteKeyPressed (int lastSelectedItem);
    virtual void backgroundClicked (const juce::MouseEvent&);
    virtual void paintCellItem (int idx, juce::Graphics& g, int width, int height, bool rowIsSelected);
    virtual void paintColPanel (int col, juce::Graphics& g, int x, int y, int width, int height);
    virtual juce::MouseCursor getMouseCursorForItem(int idx);
};

class ShelfViewComponent : public juce::Component
{
public:
    ShelfViewComponent(const juce::String& componentName = juce::String::empty, ShelfViewModel* m = nullptr);
    ~ShelfViewComponent();

    void setModel (ShelfViewModel* newModel);
    ShelfViewModel* getModel() const noexcept;

    void updateContent();
    int getSelectedItem (int index = 0) const;
    void setMultipleSelectionEnabled (bool shouldBeEnabled) noexcept;
    void setItemSelectedOnMouseDown (bool isSelectedOnMouseDown) noexcept;
    void setItemSelectionEnabled (bool b) noexcept;
    void selectItem (int idx, bool dontScrollToShowThisCol = false, bool clearLast = true);
    void selectRangeOfItems (int firstRow, int lastRow);
    int getNumSelectedItems() const;
    void deselectAllItems();
    void deselectItem (int idx);
    void flipItemSelection (int idx);
    juce::SparseSet<int> getSelectedItems() const;
    void setSelectedItems (const juce::SparseSet<int>& setOfItemsToBeSelected,
        juce::NotificationType sendNotificationEventToModel = juce::sendNotification);
    void setSelectedItemsNotUpdate (const juce::SparseSet<int>& setOfItemsToBeSelected);
    bool isItemSelected (int idx) const;
    int getLastItemSelected() const;
    void selectItemBasedOnModifierKeys (int itemClickedOn, juce::ModifierKeys modifiers, bool isMouseUpEvent);
    void setHorizontalPosition (double newProportion);
    double getHorizontalPosition() const;
    void scrollToEnsureItemIsOnscreen (int idx);
    juce::ScrollBar* getVerticalScrollBar() const noexcept;
    juce::ScrollBar* getHorizontalScrollBar() const noexcept;
    int getItemContainingPosition (int x, int y) const noexcept;
    void selectItemContainingArea (juce::Rectangle<int> area) noexcept;
    int getInsertionIndexForPosition (int x, int y) const noexcept;
    juce::Rectangle<int> getItemPosition (int idx, bool relativeToComponentTopLeft) const noexcept;
    juce::Component* getComponentForItemIdx (int idx) const noexcept;
    int getItemIdxOfComponent (Component* itemComponent) const noexcept;
    int getVisibleHeight() const noexcept;
    int getNumItemOnScreen() const noexcept;
    void setMinimumContentHeight (int newMinimumHeight);
    int getVisibleContentWidth() const noexcept;
    void repaintItem (int idx) noexcept;

    int getTotalCols() const noexcept;
    void setColWidth (int newWidth);
    int getColWidth() const noexcept;
    void scrollToEnsureColIsOnscreen (int col);
    juce::Rectangle<int> getColPosition (int colNumber,
        bool relativeToComponentTopLeft) const noexcept;
    int getNumColsOnScreen() const noexcept;
    void repaintCol (int colNumber) noexcept;
    juce::Viewport* getViewport() const noexcept;

    int getLastDragOverItem() const noexcept;
    void moveSelectedItemAnimate(const juce::DragAndDropTarget::SourceDetails& dragDetails);
    virtual juce::Image createSnapshotOfRows (const juce::SparseSet<int>& items, int& x, int& y);

    void setMouseDragSelectArea(juce::Rectangle<int>& area);
    enum ColourIds
    {
        backgroundColourId      = 0x1002800, /**< The background colour to fill the list with.
                                                  Make this transparent if you don't want the background to be filled. */
        outlineColourId         = 0x1002810, /**< An optional colour to use to draw a border around the list.
                                                  Make this transparent to not have an outline. */
        textColourId            = 0x1002820  /**< The preferred colour to use for drawing text in the listbox. */
    };

    //==============================================================================
    /** @internal */
    bool keyPressed (const juce::KeyPress&) override;
    /** @internal */
    bool keyStateChanged (bool isKeyDown) override;
    /** @internal */
    void paint (juce::Graphics&) override;
    /** @internal */
    void paintOverChildren (juce::Graphics&) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void visibilityChanged() override;
    /** @internal */
    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails&) override;
    /** @internal */
    void mouseUp (const juce::MouseEvent&) override;
    /** @internal */
    void colourChanged() override;
    /** @internal */
    void parentHierarchyChanged() override;
    /** @internal */
    void startDragAndDrop (const juce::MouseEvent&, const juce::SparseSet<int>& rowsToDrag,
        const juce::var& dragDescription, bool allowDraggingToOtherWindows);

protected:
    void selectItemInternal (int idx, bool dontScrollToShowThisRow, bool isMouseClick, bool clearLast = true);

public:
    class ShelfViewport;
    class ItemComponent;
    class ContentComponent;
    class ShelfViewMouseMoveSelector;

private:
    friend class ShelfViewport;
    friend class ItemComponent;
    friend class ContentComponent;
    friend class ShelfViewMouseMoveSelector;

    ShelfViewModel* model;
    bool hasDoneInitialUpdate, selectOnMouseDown, selectionEnabled;
    int totalItems, colWidth, minimumColHeigth;
    int lastItemSelected, lastDragOverItem;
    juce::ScopedPointer<juce::MouseListener> mouseMoveSelector;
    juce::SparseSet<int> selected;
    juce::ScopedPointer<ShelfViewport> viewport;
    juce::Rectangle<int> mouseDragArea;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShelfViewComponent)
};
