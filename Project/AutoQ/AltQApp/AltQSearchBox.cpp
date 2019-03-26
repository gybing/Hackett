#include "AltQSearchBox.h"

//==============================================================================
AltQSearchBox::AltQSearchBox()
{
    setComponentID("altq_searchbox");

    // search edit
    addAndMakeVisible(searchInput = new AltQSearchEditor);
    searchInput->setSelectAllWhenFocused(true);
    searchInput->setComponentID("altq_searchbox_edit");
    searchInput->setFont(Font(AltQConstSearchBarHeight * 0.6f));
    searchInput->setTextToShowWhenEmpty(AltQConstDefaultTipText, Colour(137,137,137));
    searchInput->setColour(TextEditor::ColourIds::backgroundColourId, Colours::transparentBlack);
    searchInput->setColour(TextEditor::ColourIds::textColourId, Colour(58,58,58));
    searchInput->setColour(TextEditor::ColourIds::highlightColourId, Colour(51,143,255));
    searchInput->setColour(TextEditor::ColourIds::highlightedTextColourId, Colours::white);
    searchInput->setColour(TextEditor::ColourIds::outlineColourId, Colours::transparentBlack);
    searchInput->setColour(TextEditor::ColourIds::focusedOutlineColourId, Colours::transparentBlack);
    searchInput->setColour(TextEditor::ColourIds::shadowColourId, Colours::transparentBlack);
    searchInput->addListener(this);
    class SearchInputKeyListener : public KeyListener
    {
    public:
        AltQSearchBox& owner;
        SearchInputKeyListener(AltQSearchBox& o) : owner(o){}
        virtual bool keyPressed(const KeyPress& key,
            Component* originatingComponent)
        {
            if (key.isKeyCode(KeyPress::escapeKey))
            {
                if (DocumentWindow* d = owner.findParentComponentOfClass<DocumentWindow>())
                    d->setVisible(false);
                return true;
            }
            
            if (key.isKeyCode(KeyPress::downKey))
            {
                if (Component* comp = owner.getParentComponent())
                    if (comp = comp->findChildWithID("altq_result"))
                        if (comp = comp->findChildWithID("altq_result_list"))
                            if (comp = comp->findChildWithID("altq_result_list_obj"))
                            {
                                ListBox* list = (ListBox*)comp;
                                list->toFront(true);
                                list->selectRow(0);
                            }
            }
            return false;
        }
    };
    searchInput->addKeyListener(inputKeyListener = new SearchInputKeyListener(*this));

    class SearchInputMouseListener : public MouseListener
    {
    public:
        SearchInputMouseListener(AltQSearchBox& o) : owner(o){}
        AltQSearchBox& owner;
        virtual void mouseDown(const MouseEvent& event) override
        {
            ((Component*)owner.findParentComponentOfClass<DocumentWindow>())->mouseDown(event);
        }
        virtual void mouseDrag(const MouseEvent& event)
        {
            ((Component*)owner.findParentComponentOfClass<DocumentWindow>())->mouseDrag(event);
        }
        virtual void mouseUp(const MouseEvent& event)
        {
            ((Component*)owner.findParentComponentOfClass<DocumentWindow>())->mouseUp(event);
        }
    };

    // searchInput->addMouseListener(new SearchInputMouseListener(*this), true);
    // icon
    addAndMakeVisible(titleIcon = new ImageComponent);
    titleIcon->setInterceptsMouseClicks(false, false);

    setWantsKeyboardFocus(false);
    setInterceptsMouseClicks(false, true);
    setTitleIcon(AltQLoadImage(search_png));
}

AltQSearchBox::~AltQSearchBox()
{
}

void AltQSearchBox::paint (Graphics& g)
{
    
}

void AltQSearchBox::resized()
{
    titleIcon->setBounds(0.2f * AltQConstSearchBarHeight, 0.2f * AltQConstSearchBarHeight, 0.6f * AltQConstSearchBarHeight, 0.6f * AltQConstSearchBarHeight);
    searchInput->setBounds(AltQConstSearchBarHeight, 0.1f*AltQConstSearchBarHeight, AltQConstWidth - (2 * AltQConstSearchBarHeight), 0.8f*AltQConstSearchBarHeight);
}

void AltQSearchBox::setTitleIcon(Image icon)
{
    titleIcon->setImage(icon);
}

void AltQSearchBox::textEditorTextChanged(TextEditor& e)
{
    Component::BailOutChecker checker(this);
    listeners.callChecked(checker, &Listener::searchInputChanged, e);
}

void AltQSearchBox::textEditorReturnKeyPressed(TextEditor&)
{
    if (Component* comp = getParentComponent())
        if (comp = comp->findChildWithID("altq_result"))
            if (comp = comp->findChildWithID("altq_result_list"))
                if (comp = comp->findChildWithID("altq_result_list_obj"))
                {
                    ListBox* list = (ListBox*)comp;
                    list->toFront(true);
                    list->selectRow(0);
                    list->keyPressed(KeyPress(KeyPress::returnKey, 0, KeyPress::returnKey));
                }
}
void AltQSearchBox::textEditorEscapeKeyPressed(TextEditor&)
{

}
void AltQSearchBox::textEditorFocusLost(TextEditor&)
{

}

