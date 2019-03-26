#pragma once

class StoreTabbedComponent;

class StoreComponent : public juce::Component, public juce::VerticalToolMenu::VToolListener
{
public:
    StoreComponent();
    ~StoreComponent();

protected:
    virtual void paint (juce::Graphics&) override;
    virtual void resized() override;
    virtual void onVToolMenuItemClicked(juce::VerticalToolMenu*, juce::Button*) override;
    virtual void visibilityChanged();
private:
    juce::ScopedPointer<StoreTabbedComponent> tabbedComponent;
    juce::ScopedPointer<juce::VerticalToolMenu> toolMenu;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StoreComponent)
};

class StoreTabbedComponent : public juce::TabbedComponent
{
public:
    StoreTabbedComponent();
    ~StoreTabbedComponent();

    virtual void visibilityChanged();
protected:

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StoreTabbedComponent)
};
