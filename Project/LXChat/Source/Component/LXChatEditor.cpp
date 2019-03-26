#include "JuceHeader.h"
#include "LXChatEditor.h"

//==============================================================================
LXChatEditor::LXChatEditor()
{
    setFont(juce::Font(18));
    setColour(juce::TextEditor::ColourIds::backgroundColourId, Colour(0xFFF2F2F2));
    setColour(juce::TextEditor::ColourIds::outlineColourId, Colours::transparentBlack);
    setColour(juce::TextEditor::ColourIds::focusedOutlineColourId, Colours::transparentBlack);
    setColour(juce::TextEditor::ColourIds::textColourId, ProjectInfo::colorLightBlack);
    setColour(juce::TextEditor::ColourIds::shadowColourId, Colours::white);
    addListener(this);
}

LXChatEditor::~LXChatEditor()
{
}

void LXChatEditor::paint (Graphics& g)
{
    TextEditor::paint(g);
}

void LXChatEditor::resized()
{
    TextEditor::resized();
}

void LXChatEditor::textEditorFocusLost(TextEditor&)
{
    setColour(juce::TextEditor::ColourIds::backgroundColourId, Colour(0xFFF2F2F2));
}

void LXChatEditor::focusGained(FocusChangeType tp)
{
    TextEditor::focusGained(tp);
    setColour(juce::TextEditor::ColourIds::backgroundColourId, Colours::white);
}

void LXChatEditor::mouseDrag(const MouseEvent& e)
{
    const juce::MouseEvent e2(e.getEventRelativeTo(this));
    Point<int> pos = e2.getPosition();
    
    if (pos.getY() > 0 && pos.getY() < 5)
    {
        setMouseCursor(MouseCursor::UpDownResizeCursor);
        setTopLeftPosition(Point<int>(getX(), getY() + e2.getOffsetFromDragStart().getY()));
    }
}

void LXChatEditor::mouseMove(const MouseEvent& e)
{
    const juce::MouseEvent e2(e.getEventRelativeTo(this));
    Point<int> pos = e2.getPosition();
    if (pos.getY() > 0 && pos.getY() < 5)
    {
        setMouseCursor(MouseCursor::UpDownResizeCursor);
    }
    else
    {
        setMouseCursor(MouseCursor::ParentCursor);
    }
}
