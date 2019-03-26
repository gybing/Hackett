#include "AltQResultListBox.h"
#include "native/AltQNativeHelper.h"
#include "AltQExtentions.h"
#include "AltQModuleCore.h"

//==============================================================================
AltQResultListBox::AltQResultListBox(AltQResultPreview* p)
    : preview(p)
{
    setComponentID("altq_result_list");

    addAndMakeVisible(listBox = new ResultListBox("reault_list", this));
    listBox->setComponentID("altq_result_list_obj");
    listBox->setRowHeight(AltQListItemHeight);
    listBox->setColour(ListBox::ColourIds::backgroundColourId, Colours::transparentBlack);
    listBox->updateContent();
    
    AltQModuleCore::getInstance()->getCoreDelegate()->addListener(this);
}

AltQResultListBox::~AltQResultListBox()
{
    AltQModuleCore::getInstance()->getCoreDelegate()->removeListener(this);
}

void AltQResultListBox::paint(Graphics&)
{

}

void AltQResultListBox::resized()
{
    listBox->setBoundsInset(BorderSize<int>(0));
}


int AltQResultListBox::getNumRows()
{
    return searchResults.size();
}
void AltQResultListBox::paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(Colours::lightgrey);
    }
}

void AltQResultListBox::listBoxItemClicked(int row, const MouseEvent& e)
{
    if (e.mods.isRightButtonDown())
    {
        if (const AltQResultPtr& result = getResultPtr(row))
        {
            if (const IAltQModuleResultPtr& parseResult = getParseResultPtr(result))
            {
                AltQExtentions::getInstance()->ShowPopupMenu(result, parseResult);
            }
        }
    }
    else
    {
        selectedRowsChanged(row);
    }
}

void AltQResultListBox::listBoxItemDoubleClicked(int row, const MouseEvent&)
{
    returnKeyPressed(row);
}

void AltQResultListBox::selectedRowsChanged(int lastRowSelected)
{
    if (!preview) return;

    if (const AltQResultPtr& result = getResultPtr(lastRowSelected))
    {
        if (const IAltQModuleResultPtr& parseResult = getParseResultPtr(result))
        {
            preview->setPreviewContent(result, parseResult);
        }
    }
}

void AltQResultListBox::deleteKeyPressed(int lastRowSelected)
{

}

void AltQResultListBox::returnKeyPressed(int lastRowSelected)
{
    if (const AltQResultPtr& result = getResultPtr(lastRowSelected))
    {
        if (const IAltQModuleResultPtr& parseResult = getParseResultPtr(result))
        {
            AltQExtentions::getInstance()->OnResultItemExcute(result, parseResult);
        }
    }
}

String AltQResultListBox::getTooltipForRow(int row)
{
    if (const AltQResultPtr& result = getResultPtr(row))
    {
        if (const IAltQModuleResultPtr& parseResult = getParseResultPtr(result))
        {
            return CharPointer_UTF8(parseResult->GetIcon().c_str());
        }
    }
    return "";
}

void AltQResultListBox::OnSearchFinish(const AltQResults& results, const std::string& context)
{
    juce::MessageManager::callAsync([=](){
        mapParseResult.clear();
        searchResults = results;
        searchContext = context;
        listBox->updateContent();
    });
}

AltQResultPtr AltQResultListBox::getResultPtr(int row)
{
    if (row < searchResults.size())
        return searchResults[row];

    return nullptr;
}

IAltQModuleResultPtr AltQResultListBox::getParseResultPtr(const AltQResultPtr& result)
{
    if (!result) return nullptr;

    auto it = mapParseResult.find(result->m_DocID);
    if (it != mapParseResult.end())
    {
        return it->second;
    }

    IAltQModuleResultPtr& ptr = mapParseResult[result->m_DocID];
    ptr = AltQExtentions::getInstance()->ParseResult(result);
    return ptr;
}

Component* AltQResultListBox::refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate)
{
    if (const AltQResultPtr& result = getResultPtr(rowNumber))
    {
        if (const IAltQModuleResultPtr& parseResult = getParseResultPtr(result))
        {
            Component* comp = AltQExtentions::getInstance()->RefreshResultItemComponent(result, parseResult, existingComponentToUpdate);
            if (comp)
            {
                comp->setInterceptsMouseClicks(false, false);
                comp->setWantsKeyboardFocus(false);
            }
            return comp;
        }
    }

    if (existingComponentToUpdate)
        deleteAndZero(existingComponentToUpdate);

    return nullptr;
}

bool ResultListBox::keyPressed(const KeyPress& key)
{
    if (key.isKeyCode(KeyPress::backspaceKey) ||
        key.isKeyCode(KeyPress::deleteKey) ||
        key.isKeyCode(KeyPress::escapeKey) ||
        !ListBox::keyPressed(key))
    {
        if (key.isKeyCode(KeyPress::returnKey))
        {
            return true;
        }
        TextEditor* text = (TextEditor*)getParentComponent()->getParentComponent()->getParentComponent()->findChildWithID("altq_searchbox")->findChildWithID("altq_searchbox_edit");
        text->toFront(true);
        text->keyPressed(key);
    }

    return true;
}