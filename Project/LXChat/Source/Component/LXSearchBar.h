#ifndef __LXSEARCHBAR_COMPONENT_HH__
#define __LXSEARCHBAR_COMPONENT_HH__

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================

class LXSearchBar : public Component, public TextEditor::Listener
{
public:
    //==============================================================================
    LXSearchBar();
    ~LXSearchBar();

protected:
    virtual void paint(Graphics&) override;
    virtual void resized() override;
private:
    ScopedPointer<TextEditor> searchEditor;
    ScopedPointer<ImageComponent> searchIcon;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LXSearchBar)
};

#endif // __LXUSERPANEL_COMPONENT_HH__