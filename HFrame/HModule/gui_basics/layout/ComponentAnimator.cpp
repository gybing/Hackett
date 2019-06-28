/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

class ComponentAnimator::AnimationTask
{
public:
    AnimationTask (Component* c) noexcept  : component (c) {}

    void reset (const Rectangle<int>& finalBounds,
                float finalAlpha,
                int millisecondsToSpendMoving,
                bool useProxyComponent,
                double startSpd, double endSpd)
    {
        msElapsed = 0;
        msTotal = jmax (1, millisecondsToSpendMoving);
        lastProgress = 0;
        destination = finalBounds;
        destAlpha = finalAlpha;

        isMoving = (finalBounds != component->getBounds());
        isChangingAlpha = (finalAlpha != component->getAlpha());

        left    = component->getX();
        top     = component->getY();
        right   = component->getRight();
        bottom  = component->getBottom();
        alpha   = component->getAlpha();

        const double invTotalDistance = 4.0 / (startSpd + endSpd + 2.0);
        startSpeed = jmax (0.0, startSpd * invTotalDistance);
        midSpeed = invTotalDistance;
        endSpeed = jmax (0.0, endSpd * invTotalDistance);

        if (useProxyComponent)
            proxy.reset (new ProxyComponent (*component));
        else
            proxy.reset();

        component->setVisible (! useProxyComponent);
    }

    bool useTimeslice (const int elapsed)
    {
        if (auto* c = proxy != nullptr ? proxy.get()
                                       : component.get())
        {
            msElapsed += elapsed;
            double newProgress = msElapsed / (double) msTotal;

            if (newProgress >= 0 && newProgress < 1.0)
            {
                const WeakReference<AnimationTask> weakRef (this);
                newProgress = timeToDistance (newProgress);
                const double delta = (newProgress - lastProgress) / (1.0 - lastProgress);
                HAssert (newProgress >= lastProgress);
                lastProgress = newProgress;

                if (delta < 1.0)
                {
                    bool stillBusy = false;

                    if (isMoving)
                    {
                        left   += (destination.getX()      - left)   * delta;
                        top    += (destination.getY()      - top)    * delta;
                        right  += (destination.getRight()  - right)  * delta;
                        bottom += (destination.getBottom() - bottom) * delta;

                        const Rectangle<int> newBounds (roundToInt (left),
                                                        roundToInt (top),
                                                        roundToInt (right - left),
                                                        roundToInt (bottom - top));

                        if (newBounds != destination)
                        {
                            c->setBounds (newBounds);
                            stillBusy = true;
                        }
                    }

                    // Check whether the animation was cancelled/deleted during
                    // a callback during the setBounds method
                    if (weakRef.wasObjectDeleted())
                        return false;

                    if (isChangingAlpha)
                    {
                        alpha += (destAlpha - alpha) * delta;
                        c->setAlpha ((float) alpha);
                        stillBusy = true;
                    }

                    if (stillBusy)
                        return true;
                }
            }
        }

        moveToFinalDestination();
        return false;
    }

    void moveToFinalDestination()
    {
        if (component != nullptr)
        {
            const WeakReference<AnimationTask> weakRef (this);
            component->setAlpha ((float) destAlpha);
            component->setBounds (destination);

            if (! weakRef.wasObjectDeleted())
                if (proxy != nullptr)
                    component->setVisible (destAlpha > 0);
        }
    }

    //==============================================================================
    struct ProxyComponent  : public Component
    {
        ProxyComponent (Component& c)
        {
            setWantsKeyboardFocus (false);
            setBounds (c.getBounds());
            setTransform (c.getTransform());
            setAlpha (c.getAlpha());
            setInterceptsMouseClicks (false, false);

            if (auto* parent = c.getParentComponent())
                parent->addAndMakeVisible (this);
            else if (c.isOnDesktop() && c.getPeer() != nullptr)
                addToDesktop (c.getPeer()->getStyleFlags() | ComponentPeer::windowIgnoresKeyPresses);
            else
                HAssertfalse; // seem to be trying to animate a component that's not visible..

            auto scale = (float) Desktop::getInstance().getDisplays().findDisplayForRect (getScreenBounds()).scale;

            image = c.createComponentSnapshot (c.getLocalBounds(), false, scale);

            setVisible (true);
            toBehind (&c);
        }

        void paint (Graphics& g) override
        {
            g.setOpacity (1.0f);
            g.drawImageTransformed (image, AffineTransform::scale (getWidth()  / (float) image.getWidth(),
                                                                   getHeight() / (float) image.getHeight()), false);
        }

    private:
        Image image;

        HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProxyComponent)
    };

    WeakReference<Component> component;
    std::unique_ptr<Component> proxy;

    Rectangle<int> destination;
    double destAlpha;

    int msElapsed, msTotal;
    double startSpeed, midSpeed, endSpeed, lastProgress;
    double left, top, right, bottom, alpha;
    bool isMoving, isChangingAlpha;

private:
    double timeToDistance (const double time) const noexcept
    {
        return (time < 0.5) ? time * (startSpeed + time * (midSpeed - startSpeed))
                            : 0.5 * (startSpeed + 0.5 * (midSpeed - startSpeed))
                                + (time - 0.5) * (midSpeed + (time - 0.5) * (endSpeed - midSpeed));
    }

    HDECLARE_WEAK_REFERENCEABLE (AnimationTask)
    HDECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimationTask)
};

//==============================================================================
ComponentAnimator::ComponentAnimator()  : lastTime (0) {}
ComponentAnimator::~ComponentAnimator() {}

//==============================================================================
ComponentAnimator::AnimationTask* ComponentAnimator::findTaskFor (Component* const component) const noexcept
{
    for (int i = tasks.size(); --i >= 0;)
        if (component == tasks.getUnchecked(i)->component.get())
            return tasks.getUnchecked(i);

    return nullptr;
}

void ComponentAnimator::animateComponent (Component* const component,
                                          const Rectangle<int>& finalBounds,
                                          const float finalAlpha,
                                          const int millisecondsToSpendMoving,
                                          const bool useProxyComponent,
                                          const double startSpeed,
                                          const double endSpeed)
{
    // the speeds must be 0 or greater!
    HAssert (startSpeed >= 0 && endSpeed >= 0);

    if (component != nullptr)
    {
        auto* at = findTaskFor (component);

        if (at == nullptr)
        {
            at = new AnimationTask (component);
            tasks.add (at);
            sendChangeMessage();
        }

        at->reset (finalBounds, finalAlpha, millisecondsToSpendMoving,
                   useProxyComponent, startSpeed, endSpeed);

        if (! isTimerRunning())
        {
            lastTime = Time::getMillisecondCounter();
            startTimerHz (50);
        }
    }
}

void ComponentAnimator::fadeOut (Component* component, int millisecondsToTake)
{
    if (component != nullptr)
    {
        if (component->isShowing() && millisecondsToTake > 0)
            animateComponent (component, component->getBounds(), 0.0f, millisecondsToTake, true, 1.0, 1.0);

        component->setVisible (false);
    }
}

void ComponentAnimator::fadeIn (Component* component, int millisecondsToTake)
{
    if (component != nullptr && ! (component->isVisible() && component->getAlpha() == 1.0f))
    {
        component->setAlpha (0.0f);
        component->setVisible (true);
        animateComponent (component, component->getBounds(), 1.0f, millisecondsToTake, false, 1.0, 1.0);
    }
}

void ComponentAnimator::cancelAllAnimations (const bool moveComponentsToTheirFinalPositions)
{
    if (tasks.size() > 0)
    {
        if (moveComponentsToTheirFinalPositions)
            for (int i = tasks.size(); --i >= 0;)
                tasks.getUnchecked(i)->moveToFinalDestination();

        tasks.clear();
        sendChangeMessage();
    }
}

void ComponentAnimator::cancelAnimation (Component* const component,
                                         const bool moveComponentToItsFinalPosition)
{
    if (auto* at = findTaskFor (component))
    {
        if (moveComponentToItsFinalPosition)
            at->moveToFinalDestination();

        tasks.removeObject (at);
        sendChangeMessage();
    }
}

Rectangle<int> ComponentAnimator::getComponentDestination (Component* const component)
{
    HAssert (component != nullptr);

    if (auto* at = findTaskFor (component))
        return at->destination;

    return component->getBounds();
}

bool ComponentAnimator::isAnimating (Component* component) const noexcept
{
    return findTaskFor (component) != nullptr;
}

bool ComponentAnimator::isAnimating() const noexcept
{
    return tasks.size() != 0;
}

void ComponentAnimator::timerCallback()
{
    auto timeNow = Time::getMillisecondCounter();

    if (lastTime == 0)
        lastTime = timeNow;

    auto elapsed = (int) (timeNow - lastTime);

    for (auto* task : Array<AnimationTask*> (tasks.begin(), tasks.size()))
    {
        if (tasks.contains (task) && ! task->useTimeslice (elapsed))
        {
            tasks.removeObject (task);
            sendChangeMessage();
        }
    }

    lastTime = timeNow;

    if (tasks.size() == 0)
        stopTimer();
}


