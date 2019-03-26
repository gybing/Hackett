#ifndef JUCE_READLISTVIEW_H_INCLUDED
#define JUCE_READLISTVIEW_H_INCLUDED


//==============================================================================
/**
    A subclass of this is used to drive a ReadListView.

    @see ReadListView
*/
class ReadListViewModel
{
public:
    //==============================================================================
    /** Destructor. */
    virtual ~ReadListViewModel()  {}

    //==============================================================================
    virtual int getNumRows() = 0;
    virtual juce::Component* refreshComponentForRow (int rowNumber, juce::Component* existingComponentToUpdate);
    virtual void listWasScrolled();
};


//==============================================================================
/**
    A list of items that can be scrolled vertically.

    To create a list, you'll need to create a subclass of ReadListViewModel. This can
    either paint each row of the list and respond to events via callbacks, or for
    more specialised tasks, it can supply a custom component to fill each row.

    @see ComboBox, TableListBox
*/
class ReadListView  : public juce::Component,
                           public juce::SettableTooltipClient
{
public:
    //==============================================================================
    /** Creates a ReadListView.

        The model pointer passed-in can be null, in which case you can set it later
        with setModel().
    */
    ReadListView (const juce::String& componentName = juce::String::empty,
             ReadListViewModel* model = nullptr, bool vertical = true);

    /** Destructor. */
    ~ReadListView();


    //==============================================================================
    /** Changes the current data model to display. */
    void setModel (ReadListViewModel* newModel);

    /** Returns the current list model. */
    ReadListViewModel* getModel() const noexcept                     { return model; }


    int getFirstWholeIndex() const;
    int getCurrentWholeIndex();

    //==============================================================================
    /** Causes the list to refresh its content.

        Call this when the number of rows in the list changes, or if you want it
        to call refreshComponentForRow() on all the row components.

        This must only be called from the main message thread.
    */
    void updateContent();

    void reloadData();

    //==============================================================================
    /** Scrolls the list to a particular position.

        The proportion is between 0 and 1.0, so 0 scrolls to the top of the list,
        1.0 scrolls to the bottom.

        If the total number of rows all fit onto the screen at once, then this
        method won't do anything.

        @see getVerticalPosition
    */
    void setScrollPosition (double newProportion);

    /** Returns the current vertical position as a proportion of the total.

        This can be used in conjunction with setVerticalPosition() to save and restore
        the list's position. It returns a value in the range 0 to 1.

        @see setVerticalPosition
    */
    double getScrollPosition() const;

    /** Scrolls if necessary to make sure that a particular row is visible. */
    void scrollToEnsureRowIsOnscreen (int row);

    /** Returns a pointer to the vertical scrollbar. */
    juce::ScrollBar* getVerticalScrollBar() const noexcept;

    /** Returns a pointer to the horizontal scrollbar. */
    juce::ScrollBar* getHorizontalScrollBar() const noexcept;

    /** Finds the row index that contains a given x,y position.
        The position is relative to the ReadListView's top-left.
        If no row exists at this position, the method will return -1.
        @see getComponentForRowNumber
    */
    int getRowContainingPosition (int x, int y) const noexcept;

    /** Finds a row index that would be the most suitable place to insert a new
        item for a given position.

        This is useful when the user is e.g. dragging and dropping onto the listbox,
        because it lets you easily choose the best position to insert the item that
        they drop, based on where they drop it.

        If the position is out of range, this will return -1. If the position is
        beyond the end of the list, it will return getNumRows() to indicate the end
        of the list.

        @see getComponentForRowNumber
    */
    int getInsertionIndexForPosition (int x, int y) const noexcept;

    /** Returns the position of one of the rows, relative to the top-left of
        the listbox.

        This may be off-screen, and the range of the row number that is passed-in is
        not checked to see if it's a valid row.
    */
    juce::Rectangle<int> getRowPosition (int rowNumber,
                                   bool relativeToComponentTopLeft) const noexcept;

    /** Finds the row component for a given row in the list.

        The component returned will have been created using createRowComponent().

        If the component for this row is off-screen or if the row is out-of-range,
        this will return nullptr.

        @see getRowContainingPosition
    */
    juce::Component* getComponentForRowNumber (int rowNumber) const noexcept;

    /** Returns the row number that the given component represents.
        If the component isn't one of the list's rows, this will return -1.
    */
    int getRowNumberOfComponent (juce::Component* rowComponent) const noexcept;

    //==============================================================================
    /** Sets the height of each row in the list.
        The default height is 22 pixels.
        @see getRowHeight
    */
    void setRowSize (int newSize);

    /** Returns the height of a row in the list.
        @see setRowHeight
    */
    int getRowSize() const noexcept                   { return rowSize; }

    /** Returns the number of rows actually visible.

        This is the number of whole rows which will fit on-screen, so the value might
        be more than the actual number of rows in the list.
    */
    int getNumRowsOnScreen() const noexcept;

    //==============================================================================
    /** A set of colour IDs to use to change the colour of various aspects of the label.

        These constants can be used either via the juce::Component::setColour(), or LookAndFeel::setColour()
        methods.

        @see juce::Component::setColour, juce::Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        backgroundColourId      = 0x1002800, /**< The background colour to fill the list with.
                                                  Make this transparent if you don't want the background to be filled. */
        outlineColourId         = 0x1002810, /**< An optional colour to use to draw a border around the list.
                                                  Make this transparent to not have an outline. */
        textColourId            = 0x1002820  /**< The preferred colour to use for drawing text in the listbox. */
    };

    /** Changes the width of the rows in the list.

        This can be used to make the list's row components wider than the list itself - the
        width of the rows will be either the width of the list or this value, whichever is
        greater, and if the rows become wider than the list, a horizontal scrollbar will
        appear.

        The default value for this is 0, which means that the rows will always
        be the same width as the list.
    */
    void setMinimumContentWidth (int newMinimumWidth);

    /** Repaints one of the rows.

        This does not invoke updateContent(), it just invokes a straightforward repaint
        for the area covered by this row.
    */
    void repaintRow (int rowNumber) noexcept;

    /** This fairly obscure method creates an image that shows the row components specified
        in rows (for example, these could be the currently selected row components).

        It's a handy method for doing drag-and-drop, as it can be passed to the
        DragAndDropContainer for use as the drag image.

        Note that it will make the row components temporarily invisible, so if you're
        using custom components this could affect them if they're sensitive to that
        sort of thing.

        @see juce::Component::createComponentSnapshot
    */
    virtual juce::Image createSnapshotOfRows (const juce::SparseSet<int>& rows, int& x, int& y);

    /** Returns the viewport that this ReadListView uses.

        You may need to use this to change parameters such as whether scrollbars
        are shown, etc.
    */
    juce::Viewport* getViewport() const noexcept;

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

private:
    //==============================================================================
    JUCE_PUBLIC_IN_DLL_BUILD (class ListViewport)
    JUCE_PUBLIC_IN_DLL_BUILD (class RowComponent)
    friend class ListViewport;
    friend class TableListBox;
    ReadListViewModel* model;
    juce::ScopedPointer<ListViewport> viewport;
    int totalItems, rowSize, minimumRowSize;
    bool vertical, hasDoneInitialUpdate;
    double lastPos;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReadListView)
};


#endif   // JUCE_READLISTVIEW_H_INCLUDED
