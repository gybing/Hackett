#pragma once
#include "AltQHeader.h"
#include "AltQResultPreview.h"
#include "AltQModuleCore.h"
#include <map>

class ResultListBox : public ListBox
{
public:
    ResultListBox(const String& componentName = String(),
        ListBoxModel* model = nullptr) : ListBox(componentName, model){}

private:
    bool keyPressed(const KeyPress& key) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResultListBox)
};

//==============================================================================
class AltQResultListBox
    : public Component
    , public ListBoxModel
    , public AltQModuleCoreDelegate::Listener
{
public:
    //==============================================================================
    AltQResultListBox(AltQResultPreview* p = nullptr);
    ~AltQResultListBox();

protected:
    virtual void paint(Graphics&) override;
    virtual void resized() override;

    // 
    virtual void OnSearchFinish(const AltQResults& results, const std::string& context) override;

    //ListBoxModel
    virtual int getNumRows() override;
    virtual void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;
    virtual Component* refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override;
    virtual void listBoxItemClicked(int row, const MouseEvent&) override;
    virtual void listBoxItemDoubleClicked(int row, const MouseEvent&) override;
    virtual void selectedRowsChanged(int lastRowSelected) override;
    virtual void deleteKeyPressed(int lastRowSelected) override;
    virtual void returnKeyPressed(int lastRowSelected) override;
    virtual String getTooltipForRow(int row) override;

    AltQResultPtr getResultPtr(int row);
    IAltQModuleResultPtr getParseResultPtr(const AltQResultPtr& result);

private:
    std::map<AltQDocID, IAltQModuleResultPtr> mapParseResult;
    AltQResults searchResults;
    std::string searchContext;

    AltQResultPreview* preview;

    ScopedPointer<ResultListBox> listBox;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AltQResultListBox)
};
