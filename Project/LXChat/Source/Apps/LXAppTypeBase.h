#ifndef __LXAPPTYPEBASE_COMPONENT_HH__
#define __LXAPPTYPEBASE_COMPONENT_HH__

//==============================================================================
class LXAppTypeBase
{
public:
    LXAppTypeBase(const String& appName);
    virtual ~LXAppTypeBase();

    virtual Component* createComponent() = 0;

    const String name;

    static Array<LXAppTypeBase*>& getAppTypeList();

private:
    JUCE_DECLARE_NON_COPYABLE(LXAppTypeBase)
};

//==============================================================================
// A templated subclass of LXAppTypeBase to make it easy for our apps to declare themselves.

template <class AppType>
class LXAppType : public LXAppTypeBase
{
public:
    LXAppType(const String& appName) : LXAppTypeBase(appName)
    {}

    Component* createComponent()    { return new AppType(); }

private:
    JUCE_DECLARE_NON_COPYABLE(LXAppType)
};

#endif // __LXAPPBASE_COMPONENT_HH__