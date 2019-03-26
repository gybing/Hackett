#ifndef __LXCHATEDITOR_COMPONENT_HH__
#define __LXCHATEDITOR_COMPONENT_HH__

//==============================================================================

class LXChatEditor : public TextEditor::Listener, public TextEditor
{
public:
    //==============================================================================
    LXChatEditor();
    ~LXChatEditor();

protected:
    virtual void paint (Graphics&) override;
    virtual void resized() override;
    virtual void mouseDrag(const MouseEvent&) override;
    virtual void mouseMove(const MouseEvent&) override;
    virtual void focusGained(FocusChangeType) override;

    virtual void textEditorTextChanged(TextEditor&) override {}
    virtual void textEditorReturnKeyPressed(TextEditor&) override {}
    virtual void textEditorEscapeKeyPressed(TextEditor&) override {}
    virtual void textEditorFocusLost(TextEditor&) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LXChatEditor)
};

#endif // __LXCHATEDITOR_COMPONENT_HH__