#include "JuceHeader.h"
#include "OptionComponent.h"

//////////////////////////////////////////////////////////////////////////
//
class DrawableColorRect : public juce::Component
{
public:
    DrawableColorRect(juce::Colour c) : color(c) { }
    ~DrawableColorRect() {}

private:
    virtual void paint(juce::Graphics& g) override
    {
        g.fillAll(color);
        g.setColour(ProjectInfo::colorDarkGray);
        g.drawRect(getLocalBounds());
    }

    juce::Colour color;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrawableColorRect)
};

//////////////////////////////////////////////////////////////////////////
//
class OptionReadComp
    : public juce::Component
{
public:
    OptionReadComp::OptionReadComp()
    {
        juce::Label* label;
        addAndMakeVisible(label = new juce::Label(juce::String::empty, L"图书背景颜色"));
        label->setFont(juce::Font(18));
        label->setBounds(35, 35, 150, 18);
        label->setMinimumHorizontalScale(1.0f);
        label->setInterceptsMouseClicks(false, false);
        label->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
        addAndMakeVisible(label = new juce::Label(juce::String::empty, L"鼠标在图书上框选时"));
        label->setFont(juce::Font(18));
        label->setBounds(35, 35 + 75, 150, 18);
        label->setMinimumHorizontalScale(1.0f);
        label->setInterceptsMouseClicks(false, false);
        label->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);
        addAndMakeVisible(label = new juce::Label(juce::String::empty, L"字体显示"));
        label->setFont(juce::Font(18));
        label->setBounds(35, 35 + 75 + 75, 150, 18);
        label->setMinimumHorizontalScale(1.0f);
        label->setInterceptsMouseClicks(false, false);
        label->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);

        //
        addAndMakeVisible(toggleColor[0] = new juce::ToggleButton(juce::String::empty));
        toggleColor[0]->setBounds(50, 70, 75, 25);
        toggleColor[0]->setRadioGroupId(0x010205);
        toggleColor[0]->addAndMakeVisible(colorRect[0] = new DrawableColorRect(ProjectInfo::colorLightWhite));
        colorRect[0]->setBounds(35, 2, 20, 20);
        colorRect[0]->setInterceptsMouseClicks(false, false);

        addAndMakeVisible(toggleColor[1] = new juce::ToggleButton(juce::String::empty));
        toggleColor[1]->setBounds(150, 70, 75, 25);
        toggleColor[1]->setRadioGroupId(0x010205);
        toggleColor[1]->addAndMakeVisible(colorRect[1] = new DrawableColorRect(ProjectInfo::colorEyeGreen));
        colorRect[1]->setBounds(35, 2, 20, 20);
        colorRect[1]->setInterceptsMouseClicks(false, false);

        //
        addAndMakeVisible(toggleMouse[0] = new juce::ToggleButton(L"弹出右键菜单"));
        toggleMouse[0]->setBounds(50, 70 + 75, 150, 25);
        toggleMouse[0]->setRadioGroupId(0x010206);
        addAndMakeVisible(toggleMouse[1] = new juce::ToggleButton(L"不弹出右键菜单"));
        toggleMouse[1]->setBounds(225, 70 + 75, 150, 25);
        toggleMouse[1]->setRadioGroupId(0x010206);

        //
        addAndMakeVisible(toggleFont[0] = new juce::ToggleButton(L"平滑字体显示"));
        toggleFont[0]->setBounds(50, 70 + 75 + 75, 150, 25);
        toggleFont[0]->setRadioGroupId(0x010207);
        addAndMakeVisible(toggleFont[1] = new juce::ToggleButton(L"锐化字体显示"));
        toggleFont[1]->setBounds(225, 70 + 75 + 75, 150, 25);
        toggleFont[1]->setRadioGroupId(0x010207);
    }

    OptionReadComp::~OptionReadComp() {}

    juce::ScopedPointer<juce::ToggleButton> toggleColor[2];
    juce::ScopedPointer<juce::ToggleButton> toggleMouse[2];
    juce::ScopedPointer<juce::ToggleButton> toggleFont[2];
    juce::ScopedPointer<DrawableColorRect> colorRect[2];

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionReadComp)
};

//////////////////////////////////////////////////////////////////////////
//
class OptionSystemComp
    : public juce::Component
{
public:
    OptionSystemComp::OptionSystemComp()
    {
        juce::Label* label;
        addAndMakeVisible(label = new juce::Label(juce::String::empty, L"关闭按钮功能"));
        label->setFont(juce::Font(18));
        label->setBounds(35, 35, 150, 18);
        label->setMinimumHorizontalScale(1.0f);
        label->setInterceptsMouseClicks(false, false);
        label->setColour(juce::Label::ColourIds::textColourId, ProjectInfo::colorLightBlack);

        //
        addAndMakeVisible(toggleExit[0] = new juce::ToggleButton(L"隐藏到任务通知区域不退出程序"));
        toggleExit[0]->setBounds(50, 70, 300, 25);
        toggleExit[0]->setRadioGroupId(0x010205);
        addAndMakeVisible(toggleExit[1] = new juce::ToggleButton(L"退出程序"));
        toggleExit[1]->setBounds(50, 70 + 35, 300, 25);
        toggleExit[1]->setRadioGroupId(0x010205);

        //
        addAndMakeVisible(toggleAsk = new juce::ToggleButton(L"每次询问我"));
        toggleAsk->setBounds(50, 70 + 35 + 35, 300, 25);
    }

    OptionSystemComp::~OptionSystemComp()
    {
    }

    juce::ScopedPointer<juce::ToggleButton> toggleExit[2];
    juce::ScopedPointer<juce::ToggleButton> toggleAsk;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionSystemComp)
};
//////////////////////////////////////////////////////////////////////////
//
OptionComponent::OptionComponent(int style)
    : defaultStyle(style)
{
    setBounds(0, 0, 646, 460);
    initDefaultComponent();
    loadOptions();
}

OptionComponent::~OptionComponent()
{
}

void OptionComponent::showOptionWindow(juce::Component* parent, int style)
{
    juce::CXDialogWindow::showDialog(L"设置", new OptionComponent(style), parent, true, juce::Justification::centred);
}

void OptionComponent::paint(juce::Graphics& g)
{
    g.fillAll(ProjectInfo::colorLightWhite);
}


void OptionComponent::OnResponse(HWEnum_ReqID reqID, HWEnum_ErrorID err, HWEnum_BusiID busi, HWContext ctx, HWParaUI* para)
{
    
}

void OptionComponent::initDefaultComponent()
{
    buttonReadOption = new juce::TextButton;
    buttonReadOption->setButtonText(L"阅读设置");
    buttonReadOption->setColour(juce::TextButton::ColourIds::textColourOnId, ProjectInfo::colorGoldYello);
    buttonReadOption->setColour(juce::TextButton::ColourIds::textColourOffId, ProjectInfo::colorLightBlack);
    buttonReadOption->setRadioGroupId(0x01015);
    buttonReadOption->setClickingTogglesState (false);
    buttonReadOption->addListener(this);
    addAndMakeVisible(buttonReadOption);

    buttonSystemOption = new juce::TextButton;
    buttonSystemOption->setButtonText(L"系统设置");
    buttonSystemOption->setColour(juce::TextButton::ColourIds::textColourOnId, ProjectInfo::colorGoldYello);
    buttonSystemOption->setColour(juce::TextButton::ColourIds::textColourOffId, ProjectInfo::colorLightBlack);
    buttonSystemOption->setRadioGroupId(0x01015);
    buttonSystemOption->setClickingTogglesState (false);
    buttonSystemOption->addListener(this);
    addAndMakeVisible(buttonSystemOption);

    tabbedComponent = new juce::TabbedComponent(juce::TabbedButtonBar::TabsAtLeft);
    tabbedComponent->setOutline(0);
    tabbedComponent->setTabBarDepth(0);
    tabbedComponent->addTab(L" ", ProjectInfo::colorLightWhite, new OptionReadComp, true);
    tabbedComponent->addTab(L" ", ProjectInfo::colorLightWhite, new OptionSystemComp, true);
    addAndMakeVisible(tabbedComponent);

    buttonOK = new juce::ColorButton;
    buttonOK->setColour(juce::ColorButton::ColourIds::buttonColourNormalId, ProjectInfo::colorLightBlue);
    buttonOK->setColour(juce::ColorButton::ColourIds::buttonColourHoverId, ProjectInfo::colorLightBlue.contrasting(0.1f));
    buttonOK->setColour(juce::ColorButton::ColourIds::buttonColourDownId, ProjectInfo::colorLightBlue.contrasting(0.2f));
    buttonOK->setColour(juce::ColorButton::ColourIds::textColourNormalId, ProjectInfo::colorLightBlue);
    buttonOK->setColour(juce::ColorButton::ColourIds::textColourHoverId, ProjectInfo::colorLightBlue.contrasting(0.1f));
    buttonOK->setColour(juce::ColorButton::ColourIds::textColourDownId, ProjectInfo::colorLightBlue.contrasting(0.2f));
    buttonOK->setButtonBorder(1);
    buttonOK->setButtonText(L"确定");
    buttonOK->addListener(this);
    addAndMakeVisible(buttonOK); 
    buttonCancel = new juce::ColorButton;
    buttonCancel->setColour(juce::ColorButton::ColourIds::buttonColourNormalId, juce::Colours::grey);
    buttonCancel->setColour(juce::ColorButton::ColourIds::buttonColourHoverId, ProjectInfo::colorLightBlue.contrasting(0.1f));
    buttonCancel->setColour(juce::ColorButton::ColourIds::buttonColourDownId, ProjectInfo::colorLightBlue.contrasting(0.2f));
    buttonCancel->setColour(juce::ColorButton::ColourIds::textColourNormalId, juce::Colours::grey);
    buttonCancel->setColour(juce::ColorButton::ColourIds::textColourHoverId, ProjectInfo::colorLightBlue.contrasting(0.1f));
    buttonCancel->setColour(juce::ColorButton::ColourIds::textColourDownId, ProjectInfo::colorLightBlue.contrasting(0.2f));
    buttonCancel->setButtonBorder(1);
    buttonCancel->setButtonText(L"取消");
    buttonCancel->addListener(this);
    addAndMakeVisible(buttonCancel); 
    buttonAplay = new juce::ColorButton;
    buttonAplay->setColour(juce::ColorButton::ColourIds::buttonColourNormalId, ProjectInfo::colorLightBlue);
    buttonAplay->setColour(juce::ColorButton::ColourIds::buttonColourHoverId, ProjectInfo::colorLightBlue.contrasting(0.1f));
    buttonAplay->setColour(juce::ColorButton::ColourIds::buttonColourDownId, ProjectInfo::colorLightBlue.contrasting(0.2f));
    buttonAplay->setColour(juce::ColorButton::ColourIds::textColourNormalId, ProjectInfo::colorLightBlue);
    buttonAplay->setColour(juce::ColorButton::ColourIds::textColourHoverId, ProjectInfo::colorLightBlue.contrasting(0.1f));
    buttonAplay->setColour(juce::ColorButton::ColourIds::textColourDownId, ProjectInfo::colorLightBlue.contrasting(0.2f));
    buttonAplay->setButtonBorder(1);
    buttonAplay->setButtonText(L"应用");
    buttonAplay->addListener(this);
    addAndMakeVisible(buttonAplay); 

    if (defaultStyle == 0)
    {
        tabbedComponent->setCurrentTabIndex(defaultStyle, false);
        buttonReadOption->setToggleState(true, juce::dontSendNotification);
    }
    else if (defaultStyle == 1)
    {
        tabbedComponent->setCurrentTabIndex(defaultStyle, false);
        buttonSystemOption->setToggleState(true, juce::dontSendNotification);
    }

    resized();
}

void OptionComponent::resized()
{
    if (tabbedComponent)
    {
        tabbedComponent->setBounds(160, 0, getWidth()-160, getHeight() - 60);

        buttonReadOption->setBounds(10, 35, 120, 21);
        buttonSystemOption->setBounds(10, 70, 120, 21);

        int pos = 300;
        buttonOK->setBounds(pos, getHeight() - 45, 95, 30); 
        pos += 110;

        buttonCancel->setBounds(pos, getHeight() - 45, 95, 30); 
        pos += 110;

        buttonAplay->setBounds(pos, getHeight() - 45, 95, 30);
    }
}

void OptionComponent::buttonClicked(juce::Button* b)
{
    if (b == buttonReadOption)
    {
        b->setToggleState(true, juce::dontSendNotification);
        tabbedComponent->setCurrentTabIndex(0);
    }
    else if (b == buttonSystemOption)
    {
        b->setToggleState(true, juce::dontSendNotification);
        tabbedComponent->setCurrentTabIndex(1);
    }
    else if (b == buttonOK)
    {
        saveOptions();
        if (juce::Component* parent = getParentComponent())
            parent->exitModalState (1);
    }
    else if (b == buttonCancel)
    {
        if (juce::Component* parent = getParentComponent())
            parent->exitModalState (0);
    }
    else if (b == buttonAplay)
    {
        saveOptions();
    }
}

void OptionComponent::saveOptions()
{
    if (OptionSystemComp* sysComp = static_cast<OptionSystemComp*>(tabbedComponent->getTabContentComponent(1)))
    {
        int dwExitType = 0;
        if (sysComp->toggleExit[0]->getToggleState()) dwExitType |= CXSET_HIDE;
        if (sysComp->toggleExit[1]->getToggleState()) dwExitType |= CXSET_EXIT;
        if (sysComp->toggleAsk->getToggleState()) dwExitType |= CXSET_ASK;
        CXOption::getInstance()->SetSystemExitType(dwExitType);
    }

    if (OptionReadComp* readComp = static_cast<OptionReadComp*>(tabbedComponent->getTabContentComponent(0)))
    {
        // 阅读背景色
        CXOption::getInstance()->SetBookPageBackColor(readComp->toggleColor[1]->getToggleState());

        // 阅读桌面右键菜单
        CXOption::getInstance()->SetReadDeskRightMenuAutoPop(readComp->toggleMouse[0]->getToggleState());

        // 阅读桌面字体平滑
        CXOption::getInstance()->SetReadFontSmooth(readComp->toggleFont[0]->getToggleState());
    }

    CXOption::getInstance()->SaveUserLoginConfig();
    CXOption::getInstance()->SaveSystemConfig();
}

void OptionComponent::loadOptions()
{
    if (OptionSystemComp* sysComp = static_cast<OptionSystemComp*>(tabbedComponent->getTabContentComponent(1)))
    {
        int dwType = CXOption::getInstance()->GetSystemExitType();
        if (dwType & CXSET_HIDE) sysComp->toggleExit[0]->setToggleState(true, juce::dontSendNotification);
        if (dwType & CXSET_EXIT) sysComp->toggleExit[1]->setToggleState(true, juce::dontSendNotification);
        if (dwType & CXSET_ASK) sysComp->toggleAsk->setToggleState(true, juce::dontSendNotification);
    }

    if (OptionReadComp* readComp = static_cast<OptionReadComp*>(tabbedComponent->getTabContentComponent(0)))
    {
        // 阅读背景色
        if (CXOption::getInstance()->GetBookPageBackColor())
            readComp->toggleColor[1]->setToggleState(true, juce::dontSendNotification);
        else
            readComp->toggleColor[0]->setToggleState(true, juce::dontSendNotification);

        // 阅读桌面右键菜单
        if (CXOption::getInstance()->GetReadDeskRightMenuAutoPop())
            readComp->toggleMouse[0]->setToggleState(true, juce::dontSendNotification);
        else
            readComp->toggleMouse[1]->setToggleState(true, juce::dontSendNotification);

        // 阅读桌面字体平滑
        if (CXOption::getInstance()->GetReadFontSmooth())
            readComp->toggleFont[0]->setToggleState(true, juce::dontSendNotification);
        else 
            readComp->toggleFont[1]->setToggleState(true, juce::dontSendNotification);
    }
}