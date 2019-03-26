#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "JuceHeader.h"

//==============================================================================
class MainContentComponent
    : public Component
    , public TableListBoxModel
    , public TextEditor::Listener
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

protected:
    void resized();

    // TextEditor
    virtual void textEditorTextChanged(TextEditor&) override;
    virtual void textEditorReturnKeyPressed(TextEditor&) override;

    // TableListBoxModel
    virtual int getNumRows() override;
    virtual void paintRowBackground(Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;
    virtual void paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    // virtual Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate);
    // virtual void cellClicked(int rowNumber, int columnId, const MouseEvent&);
    // virtual void cellDoubleClicked(int rowNumber, int columnId, const MouseEvent&);
    // virtual void backgroundClicked(const MouseEvent&);
    // virtual void sortOrderChanged(int newSortColumnId, bool isForwards);
    // virtual int getColumnAutoSizeWidth(int columnId);
    // virtual String getCellTooltip(int rowNumber, int columnId);

protected:
    bool search(const String& text);

private:
    ScopedPointer<TextEditor> searchText;
    ScopedPointer<TextEditor> searchTextExt;
    ScopedPointer<TableListBox> tableList;
    ScopedPointer<TableHeaderComponent> tableHeader;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
