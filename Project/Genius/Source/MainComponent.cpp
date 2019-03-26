#include "MainComponent.h"
#include "InfoComponent.h"
#include "DeviceComponent.h"

//==============================================================================
MainContentComponent::MainContentComponent()
{
    addAndMakeVisible(tabbedComponent = new TabbedComponent(TabbedButtonBar::TabsAtBottom));
    tabbedComponent->setTabBarDepth(0);
    tabbedComponent->setOutline(0);

    tabbedComponent->addTab(L"WIFI信息", Colours::white, new InfoComponent, true);
    tabbedComponent->addTab(L"已连接设备", Colours::white, new DeviceComponent, true);

    for (unsigned int i = 0; i<tabbedComponent->getNumTabs(); ++i)
    {
        TextButton* m = new TextButton;
        m->setButtonText(tabbedComponent->getTabbedButtonBar().getTabNames()[i]);
        m->addListener(this);
        if (i == tabbedComponent->getCurrentTabIndex())
            m->setToggleState(true, dontSendNotification);
        addAndMakeVisible(m);
        menus.add(m);
    }

    setSize(340, 460);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (Graphics& g)
{
}

void MainContentComponent::resized()
{
    const int menuHeight = 30;
    const int menuWidth = getWidth() / menus.size();

    tabbedComponent->setBoundsInset(BorderSize<int>(0, 0, menuHeight, 0));
    for (unsigned int i = 0; i < menus.size(); ++i)
    {
        if (TextButton* m = menus.getUnchecked(i))
            m->setBounds(i * menuWidth, getHeight() - menuHeight, menuWidth, menuHeight);
    }
}

void MainContentComponent::buttonClicked(Button* button)
{
    for (unsigned int i = 0; i < menus.size(); ++i)
    {
        if (TextButton* m = menus.getUnchecked(i))
        {
            if (m == button)
            {
                m->setToggleState(true, dontSendNotification);
                tabbedComponent->setCurrentTabIndex(i);
            }
            else if (m->getToggleState())
            {
                m->setToggleState(false, dontSendNotification);
            }
        }
    }
}
