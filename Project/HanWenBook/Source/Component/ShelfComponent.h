#pragma once

class LeftShelfComponent;
class RightShelfComponent;

class ShelfComponent : public juce::Component
{
public:
    ShelfComponent();
    ~ShelfComponent();

protected:
    void resized() override;

private:
    juce::ScopedPointer<LeftShelfComponent> leftShelfComponent;
    juce::ScopedPointer<RightShelfComponent> rightShelfComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShelfComponent)
};

