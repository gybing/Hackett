#include "JuceHeader.h"
#include "StoreComponent.h"

using namespace juce;
const wchar_t* options[] = { L"免费", L"分类", L"排行",nullptr };
const wchar_t* names[] = { L"menu_free", L"menu_type", L"menu_rank", nullptr };

StoreComponent::StoreComponent()
{
    addAndMakeVisible(tabbedComponent = new StoreTabbedComponent);
    addAndMakeVisible(toolMenu = new juce::VerticalToolMenu(this));
    toolMenu->setColour(juce::VerticalToolMenu::backgroundId, ProjectInfo::colorBlueBlack);
    toolMenu->setItemInset(20);

    const wchar_t* images[] = {
        L"store_tab_recommend_normal.png", L"store_tab_recommend_over.png",
        L"store_tab_category_normal.png", L"store_tab_category_over.png",
        L"store_tab_rank_normal.png", L"store_tab_rank_over.png"};
    
    for (unsigned int i=0; i<3; ++i)
    {
        if (juce::ToolButton* b = static_cast<juce::ToolButton*>(
            toolMenu->appand(true, 0x01 + i, options[i],
            CXIMAGE_CACHE_LOAD(images[(i*2)+0]),
            CXIMAGE_CACHE_LOAD(images[(i*2)+1]),
            CXIMAGE_CACHE_LOAD(images[(i*2)+1]))))
        {
            b->setRadioGroupId(0x1012);
            b->setColour(juce::ToolButton::ColourIds::textColourId, juce::Colour(0xFF503a3a));
            b->setColour(juce::ToolButton::ColourIds::textColourOnId, juce::Colours::white);
        }
    }

    if (juce::ToolButton* button = static_cast<juce::ToolButton*>(toolMenu->getItemByID(0x01)))
    {
        button->setToggleState(true, juce::NotificationType::dontSendNotification);
    }
}

StoreComponent::~StoreComponent()
{
}

void StoreComponent::paint(Graphics& g)
{
    Component::paint(g);
    g.setColour(ProjectInfo::colorBlueBlack);
    g.fillRect(0,0,56,getHeight());
}

void StoreComponent::onVToolMenuItemClicked(juce::VerticalToolMenu*, juce::Button* btn)
{
    btn->setToggleState(true, juce::NotificationType::dontSendNotification);

    const juce::String strID = btn->getComponentID();
    if (strID.isEmpty())
        return;

    switch (strID.getIntValue())
    {
    case 0x01:
        {
            tabbedComponent->setCurrentTabIndex(0, true);
            break;
        }
    case 0x02:
        {
            tabbedComponent->setCurrentTabIndex(1, true);
            break;
        }
    case 0x03:
        {
            tabbedComponent->setCurrentTabIndex(2, true);
            break;
        }
    default:
        break;
    }
}

void StoreComponent::visibilityChanged()
{
    tabbedComponent->visibilityChanged();
}

void StoreComponent::resized()
{
    toolMenu->setBounds(8, 20, 35, getHeight() - 20);
    tabbedComponent->setBounds(getLocalBounds().withLeft(56));
}

//////////////////////////////////////////////////////////////////////////

StoreTabbedComponent::StoreTabbedComponent()
: TabbedComponent (TabbedButtonBar::TabsAtLeft)	
{
    setOutline(0);
    setTabBarDepth(0);
    
#ifdef JUCE_WINDOWS
    class StoreFreeComponent : public CEFBrowserControl
    {
        virtual const wchar_t* getDefaultUrl() { return L"http://c8test.hanwenbook.com/?f=recom"; }
        JUCE_LEAK_DETECTOR (StoreFreeComponent)
    };
    class StoreTypeComponent : public CEFBrowserControl
    {
        virtual const wchar_t* getDefaultUrl() { return L"http://c8test.hanwenbook.com/?f=library"; }
        JUCE_LEAK_DETECTOR (StoreTypeComponent)
    };
    class StoreRankComponent : public CEFBrowserControl
    {
        virtual const wchar_t* getDefaultUrl() { return L"http://c8test.hanwenbook.com/?f=bookrank"; }
        JUCE_LEAK_DETECTOR (StoreRankComponent)
    };
#else if JUCE_MAC
    class StoreFreeComponent : public juce::WebBrowserComponent
    {
    public:
        StoreFreeComponent()
        {
            goToURL(L"http://c8test.hanwenbook.com/?f=recom");
        }
        JUCE_LEAK_DETECTOR (StoreFreeComponent)
    };
    class StoreTypeComponent : public juce::WebBrowserComponent
    {
    public:
        StoreTypeComponent()
        {
            goToURL(L"http://c8test.hanwenbook.com/?f=library");
        }
        JUCE_LEAK_DETECTOR (StoreTypeComponent)
    };
    class StoreRankComponent : public juce::WebBrowserComponent
    {
    public:
        StoreRankComponent()
        {
            goToURL(L"http://c8test.hanwenbook.com/?f=bookrank");
        }
        JUCE_LEAK_DETECTOR (StoreRankComponent)
    };
#endif
    
    addTab (L"免费", Colours::transparentBlack, new StoreFreeComponent, true);
    addTab (L"分类", Colours::transparentBlack, new StoreTypeComponent, true);
    addTab (L"排行", Colours::transparentBlack, new StoreRankComponent, true);
}

StoreTabbedComponent::~StoreTabbedComponent()
{
}

void StoreTabbedComponent::visibilityChanged()
{
    if (Component* comp = getCurrentContentComponent())
        comp->visibilityChanged();
}
