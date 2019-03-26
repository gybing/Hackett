#ifndef LX_COMPONENTANIMATOR_H_INCLUDED
#define LX_COMPONENTANIMATOR_H_INCLUDED

//=================================================================================
// 继承自 ComponentAnimator 组件动画类
// 增加动画完成回调

class LXComponentAnimator  : public ComponentAnimator
{
public:
    typedef std::function<void()> CallbackFunction;

    //==============================================================================
    LXComponentAnimator();
    ~LXComponentAnimator();

    void animateComponent (Component* component,
                           const Rectangle<int>& finalBounds,
                           float finalAlpha,
                           int animationDurationMilliseconds,
                           bool useProxyComponent,
                           double startSpeed,
                           double endSpeed,
                           CallbackFunction callback = nullptr);

    void fadeOut(Component* component, int millisecondsToTake, CallbackFunction callback = nullptr);
    void fadeIn(Component* component, int millisecondsToTake, CallbackFunction callback = nullptr);

private:
    void timerCallback() override;
    CallbackFunction funcCallback;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LXComponentAnimator)
};


#endif   // LX_COMPONENTANIMATOR_H_INCLUDED
