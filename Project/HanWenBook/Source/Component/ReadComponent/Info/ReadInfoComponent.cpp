#include "JuceHeader.h"
#include "ReadInfoComponent.h"
#include "ReadInfoCatalog.h"
#include "ReadInfoBrief.h"
#include "ReadInfoMark.h"
#include "ReadInfoSign.h"
#include "ReadInfoHistory.h"
#include "ReadInfoSearch.h"
#include "ReadInfoPostil.h"

ReadInfoComponent::ReadInfoComponent()
    : TabbedComponent (juce::TabbedButtonBar::TabsAtTop)
{
    setOutline(0);
    setTabBarDepth(0);

    addTab (L"目录", juce::Colours::transparentBlack, new ReadInfoCatalog, true);
    addTab (L"简介", juce::Colours::transparentBlack, new ReadInfoBrief, true);
    addTab (L"书签", juce::Colours::transparentBlack, new ReadInfoMark, true);
    addTab (L"批注", juce::Colours::transparentBlack, new ReadInfoPostil, true);
    addTab (L"书评", juce::Colours::transparentBlack, new juce::Component, true);
    addTab (L"好词妙句", juce::Colours::transparentBlack, new ReadInfoSign, true);
    addTab (L"阅读历史", juce::Colours::transparentBlack, new ReadInfoHistory, true);
    addTab (L"全文搜索", juce::Colours::transparentBlack, new ReadInfoSearch, true);
}

ReadInfoComponent::~ReadInfoComponent()
{

}

void ReadInfoComponent::paint(juce::Graphics& g)
{
    g.fillAll(ProjectInfo::colorLightWhite);
}


void ReadInfoComponent::visibilityChanged()
{
    if (Component* comp = getCurrentContentComponent())
        comp->visibilityChanged();
}
