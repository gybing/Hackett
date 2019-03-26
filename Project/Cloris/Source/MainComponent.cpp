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
  // tableHeader->addColumn(L"�ļ���", 1, height);
  // tableHeader->addColumn(L"��չ��", 2, height);
  // tableHeader->addColumn(L"����Ŀ¼", 3, height);
  // tableHeader->addColumn(L"�ļ���С", 4, height);
  // tableHeader->addColumn(L"����ʱ��", 5, height);
  // tableHeader->addColumn(L"����޸�ʱ��", 6, height);
  // tableHeader->addColumn(L"�������ʱ��", 7, height);
  // tableHeader->addColumn(L"����", 8, height);
  // tableHeader->addColumn(L"����Ŀ¼", 9, height);
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
