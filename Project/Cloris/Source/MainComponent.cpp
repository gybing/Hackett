/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainContentComponent::MainContentComponent()
{
  // addAndMakeVisible(searchText = new TextEditor());
  // searchText->setColour(TextEditor::ColourIds::backgroundColourId, Colours::lightgrey);
  // addAndMakeVisible(searchTextExt = new TextEditor());
  // searchTextExt->setColour(TextEditor::ColourIds::backgroundColourId, Colours::lightgrey);
  // 
  // addAndMakeVisible(tableList = new TableListBox(String::empty, this));
  // tableList->setHeaderHeight(25);
  // tableList->setHeader(tableHeader = new TableHeaderComponent);
  // const int height = 80;
  // tableHeader->addColumn(L"文件名", 1, height);
  // tableHeader->addColumn(L"扩展名", 2, height);
  // tableHeader->addColumn(L"所在目录", 3, height);
  // tableHeader->addColumn(L"文件大小", 4, height);
  // tableHeader->addColumn(L"创建时间", 5, height);
  // tableHeader->addColumn(L"最近修改时间", 6, height);
  // tableHeader->addColumn(L"最近访问时间", 7, height);
  // tableHeader->addColumn(L"属性", 8, height);
  // tableHeader->addColumn(L"所在目录", 9, height);
    setSize (720, 400);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::resized()
{
    // searchText->setBounds(0, 0, getWidth() - 100, 25);
    // searchTextExt->setBounds(getWidth() - 100, 0, 100, 25);
    // tableList->setBounds(0, 25, getWidth(), getHeight() - 25);
}

int MainContentComponent::getNumRows()
{
    return 210;
}

void MainContentComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.setColour(Colours::lightgrey);
        g.fillRect(0, 0, width, height);
    }
}

void MainContentComponent::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{

}

void MainContentComponent::textEditorTextChanged(TextEditor& editor)
{
    textEditorReturnKeyPressed(editor);
}

void MainContentComponent::textEditorReturnKeyPressed(TextEditor& editor)
{
    String text = editor.getText();
    if (text.isNotEmpty())
    {
        if (search(text))
        {
            tableList->updateContent();
        }
    }
}

bool MainContentComponent::search(const String& text)
{
    return true;
}
