#ifndef INFOCOMPONENT_H_INCLUDED
#define INFOCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class WiFiHelper;

//==============================================================================
class InfoComponent
    : public Component
    , public Button::Listener
    , public TextEditor::Listener
{
public:
    //==============================================================================
    InfoComponent();
    ~InfoComponent();

protected:
    virtual void paint(Graphics& g) override;
    virtual void resized() override;
    virtual void buttonClicked(Button*) override;
    virtual void textEditorTextChanged(TextEditor&) override;
    virtual void textEditorReturnKeyPressed(TextEditor&) override;
    virtual void textEditorEscapeKeyPressed(TextEditor&) override;
    virtual void textEditorFocusLost(TextEditor&) override;
private:
    ScopedPointer<AttributedString> labelConnentInfo;
    ScopedPointer<Label> labelWifiName;
    ScopedPointer<Label> labelWifiPwd;
    ScopedPointer<TextEditor> textEditName;
    ScopedPointer<TextEditor> textEditPwd;
    ScopedPointer<TextButton> buttonHide;
    ScopedPointer<TextButton> buttonSave;
    ScopedPointer<TextButton> buttonEdit;
    ScopedPointer<WiFiHelper> wifiHelper;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoComponent)
};


#endif  // INFOCOMPONENT_H_INCLUDED
