#include "JuceHeader.h"
#include "LXComponentAnimator.h"

//==============================================================================
LXComponentAnimator::LXComponentAnimator() : funcCallback(nullptr) {}
LXComponentAnimator::~LXComponentAnimator() {}

void LXComponentAnimator::animateComponent (Component* const component,
                                          const Rectangle<int>& finalBounds,
                                          const float finalAlpha,
                                          const int millisecondsToSpendMoving,
                                          const bool useProxyComponent,
                                          const double startSpeed,
                                          const double endSpeed,
                                          CallbackFunction callback)
{
    funcCallback = callback;
    ComponentAnimator::animateComponent(component, finalBounds, finalAlpha, millisecondsToSpendMoving, useProxyComponent, startSpeed, endSpeed);
}

void LXComponentAnimator::fadeOut(Component* component, int millisecondsToTake, CallbackFunction callback)
{
    funcCallback = callback;
    ComponentAnimator::fadeOut(component, millisecondsToTake);
}

void LXComponentAnimator::fadeIn(Component* component, int millisecondsToTake, CallbackFunction callback)
{
    funcCallback = callback;
    ComponentAnimator::fadeIn(component, millisecondsToTake);
}

void LXComponentAnimator::timerCallback()
{
    ComponentAnimator::timerCallback();
    if (funcCallback && !isAnimating())
    {
        funcCallback();
    }
}
