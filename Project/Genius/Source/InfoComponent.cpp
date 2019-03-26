#include "InfoComponent.h"
#include "WiFiHelper.h"

const wchar_t* DefaultWiFiName = L"WiFi-Free";
const wchar_t* DefaultWiFiSSID = L"12345678";

//==============================================================================
InfoComponent::InfoComponent()
{
    labelConnentInfo = new AttributedString;
    labelConnentInfo->setJustification(Justification::centred);
    labelConnentInfo->append(L"WIFIÐÅºÅÎÈ¶¨£¬×î¸ßÍøËÙ512KB/s", 18, ProjectInfo::colorLightCyan);

    addAndMakeVisible(textEditName = new TextEditor);
    textEditName->setColour(TextEditor::ColourIds::backgroundColourId, Colour(0xFFF7F8F7));
    textEditName->setColour(TextEditor::ColourIds::outlineColourId, Colour(0xFFF7F8F7));
    textEditName->setColour(TextEditor::ColourIds::focusedOutlineColourId, ProjectInfo::colorLightGreen);
    textEditName->setColour(TextEditor::ColourIds::textColourId, ProjectInfo::colorLightRed);
    textEditName->setColour(TextEditor::ColourIds::highlightColourId, ProjectInfo::colorLightBlue);
    textEditName->setColour(TextEditor::ColourIds::highlightedTextColourId, Colours::white);
    textEditName->setFont(18);
    textEditName->setBorder(BorderSize<int>(1));
    textEditName->addListener(this);
    textEditName->setText(DefaultWiFiName);
    textEditName->setTextToShowWhenEmpty(L"ÇëÊäÈëÄúÏ²»¶µÄWiFiÃû³Æ", Colours::grey);

    addAndMakeVisible(textEditPwd = new TextEditor);
    textEditPwd->setColour(TextEditor::ColourIds::backgroundColourId, Colour(0xFFF7F8F7));
    textEditPwd->setColour(TextEditor::ColourIds::outlineColourId, Colour(0xFFF7F8F7));
    textEditPwd->setColour(TextEditor::ColourIds::focusedOutlineColourId, ProjectInfo::colorLightGreen);
    textEditPwd->setColour(TextEditor::ColourIds::textColourId, ProjectInfo::colorLightRed);
    textEditPwd->setColour(TextEditor::ColourIds::highlightColourId, ProjectInfo::colorLightBlue);
    textEditPwd->setColour(TextEditor::ColourIds::highlightedTextColourId, Colours::white);
    textEditPwd->setFont(18);
    textEditPwd->setBorder(BorderSize<int>(1));
    textEditPwd->addListener(this);
    textEditPwd->setText(DefaultWiFiSSID);
    textEditPwd->setTextToShowWhenEmpty(L"ÇëÊäÈë8-12Î»Êý×Ö»ò×ÖÄ¸", Colours::grey);

    addAndMakeVisible(labelWifiName = new Label(String::empty, L"WiFiÃû³Æ:"));
    labelWifiName->setColour(Label::ColourIds::textColourId, Colours::grey);
    labelWifiName->setFont(18);
    labelWifiName->attachToComponent(textEditName, true);

    addAndMakeVisible(labelWifiPwd = new Label(String::empty, L"WiFiÃÜÂë:"));
    labelWifiPwd->setColour(Label::ColourIds::textColourId, Colours::grey);
    labelWifiPwd->setFont(18);
    labelWifiPwd->attachToComponent(textEditPwd, true);

    textEditPwd->addAndMakeVisible(buttonHide = new TextButton);
    buttonHide->setColour(TextButton::ColourIds::buttonColourId, Colours::transparentBlack);
    buttonHide->setColour(TextButton::ColourIds::buttonOnColourId, Colours::transparentBlack);
    buttonHide->setColour(TextButton::ColourIds::textColourOnId, Colours::grey.contrasting(0.2f));
    buttonHide->setColour(TextButton::ColourIds::textColourOffId, Colours::grey);
    buttonHide->setButtonText(L"Òþ²ØÃÜÂë");

    addChildComponent(buttonSave = new TextButton);
    buttonSave->setColour(TextButton::ColourIds::buttonColourId, ProjectInfo::colorLightCyan);
    buttonSave->setColour(TextButton::ColourIds::buttonOnColourId, ProjectInfo::colorLightCyan.contrasting(0.2f));
    buttonSave->setColour(TextButton::ColourIds::textColourOnId, Colours::white);
    buttonSave->setColour(TextButton::ColourIds::textColourOffId, Colours::white);
    buttonSave->setButtonText(L"±£´æ");
    buttonSave->addListener(this);

    addAndMakeVisible(buttonEdit = new TextButton);
    buttonEdit->setColour(TextButton::ColourIds::buttonColourId, Colours::transparentBlack);
    buttonEdit->setColour(TextButton::ColourIds::buttonOnColourId, Colours::transparentBlack);
    buttonEdit->setColour(TextButton::ColourIds::textColourOnId, ProjectInfo::colorLightCyan);
    buttonEdit->setColour(TextButton::ColourIds::textColourOffId, Colours::grey);
    buttonEdit->setButtonText(L"ÐÞ¸Ä");
    buttonEdit->addListener(this);

    wifiHelper = new WiFiHelper;
}

InfoComponent::~InfoComponent()
{
}

void InfoComponent::paint (Graphics& g)
{
    labelConnentInfo->draw(g, Rectangle<float>(0.0f, 220.0f, getWidth()*1.0f, 30.0f));
}

void InfoComponent::resized()
{
    const int w = getWidth();
    const int h = getHeight();

    textEditName->setBounds(115, 290, 180, 25);
    textEditPwd->setBounds(115, 290 + 45, 180, 25);
    buttonHide->setBounds(textEditPwd->getWidth() - 52, 2, 50, 19);
    buttonHide->addListener(this);

    buttonSave->setBounds(170, 290 + 45 + 40, 60, 23);
    buttonEdit->setBounds(240, 290 + 45 + 40, 60, 23);
}

void InfoComponent::buttonClicked(Button* button)
{
    if (button == buttonHide)
    {
        if (buttonHide->getButtonText() == L"Òþ²ØÃÜÂë")
        {
            buttonHide->setButtonText(L"ÏÔÊ¾ÃÜÂë");
            textEditPwd->setPasswordCharacter(L'¡ñ');
        }
        else
        {
            buttonHide->setButtonText(L"Òþ²ØÃÜÂë");
            textEditPwd->setPasswordCharacter(0);
        }
    }

    else if (button == buttonEdit)
    {
        if (buttonEdit->getButtonText() == L"ÐÞ¸Ä")
        {
            textEditName->toFront(true);
            buttonEdit->setButtonText(L"È¡Ïû");
            buttonSave->setVisible(true);
        }
        else
        {
            buttonEdit->toFront(true);
            buttonEdit->setButtonText(L"ÐÞ¸Ä");
            buttonSave->setVisible(false);
        }
    }
    
    else if (button == buttonSave)
    {
        String strSSID = textEditName->getText();
        if (strSSID.isEmpty())
        {
            textEditName->toFront(true);
            return;
        }

        String strKey = textEditPwd->getText();
        if (strKey.isEmpty())
        {
            textEditPwd->toFront(true);
            return;
        }

        if (wifiHelper->WirelessNetworkInvokeAPIStart(strSSID, strKey))
        {
            buttonEdit->setButtonText(L"ÐÞ¸Ä");
            buttonSave->setVisible(false);
            buttonEdit->toFront(true);
        }
    }
}

void InfoComponent::textEditorTextChanged(TextEditor&)
{
}

void InfoComponent::textEditorReturnKeyPressed(TextEditor&)
{
    buttonClicked(buttonSave);
}

void InfoComponent::textEditorEscapeKeyPressed(TextEditor&)
{
    buttonClicked(buttonEdit);
}

void InfoComponent::textEditorFocusLost(TextEditor&)
{
}