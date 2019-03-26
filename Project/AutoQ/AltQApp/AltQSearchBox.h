#pragma once
#include "AltQHeader.h"

class AltQSearchEditor : public TextEditor
{
public:
    AltQSearchEditor(){}
    ~AltQSearchEditor(){}

private:
    virtual void mouseDown(const MouseEvent& e) override
    {
        if (e.getMouseDownX() > getTextWidth() + 2)
        {
            TextEditor::mouseDown(e);
            if (MouseListener* d = findParentComponentOfClass<DocumentWindow>())
                d->mouseDown(e);
        }
    }

    void mouseDrag(const MouseEvent& e) override
    {
        TextEditor::mouseDrag(e);
        if (MouseListener* d = findParentComponentOfClass<DocumentWindow>())
            d->mouseDrag(e);
    }
    virtual void mouseUp(const MouseEvent& e) override
    {
        TextEditor::mouseUp(e);
        if (MouseListener* d = findParentComponentOfClass<DocumentWindow>())
            d->mouseUp(e);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AltQSearchEditor)
};

//==============================================================================
class AltQSearchBox : public Component, public TextEditor::Listener
{
public:
    //==============================================================================
    AltQSearchBox();
    ~AltQSearchBox();

    void setTitleIcon(Image icon);

    // »Øµ÷
    class Listener
    {
    public: 
        virtual ~Listener()  {}
        virtual void searchInputChanged(TextEditor&) = 0;
    };
    ListenerList<Listener> listeners;
    void addListener(Listener* const l)      { listeners.add(l); }
    void removeListener(Listener* const l)   { listeners.remove(l); }

protected:
    virtual void paint(Graphics&) override;
    virtual void resized() override;

    // TestEditor
    virtual void textEditorTextChanged(TextEditor&) override;
    virtual void textEditorReturnKeyPressed(TextEditor&) override;
    virtual void textEditorEscapeKeyPressed(TextEditor&) override;
    virtual void textEditorFocusLost(TextEditor&) override;

private:
    ScopedPointer<KeyListener> inputKeyListener;
    ScopedPointer<ImageComponent> titleIcon;

public:
    ScopedPointer<AltQSearchEditor> searchInput;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AltQSearchBox)
};
