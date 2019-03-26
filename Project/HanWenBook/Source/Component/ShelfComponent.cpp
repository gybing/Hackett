#include "JuceHeader.h"
#include "ShelfComponent.h"
#include "ShelfComponent/LeftShelfComponent.h"
#include "ShelfComponent/RightShelfComponent.h"

ShelfComponent::ShelfComponent()
{
    rightShelfComponent = new RightShelfComponent;
    addAndMakeVisible(rightShelfComponent);
    leftShelfComponent = new LeftShelfComponent(rightShelfComponent->getRightContentNotify());
    addAndMakeVisible(leftShelfComponent);

}

ShelfComponent::~ShelfComponent()
{
}

void ShelfComponent::resized()
{
    leftShelfComponent->setBounds(0, 0, 220, getHeight());
    rightShelfComponent->setBounds(220, 0, getWidth()-220, getHeight());
}
