#pragma once
#include "AltQHeader.h"

// 动态库加载工具
class AltQDynamicLibrary : public DynamicLibrary, public juce::DeletedAtShutdown
{
public:
    AltQDynamicLibrary() {}
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AltQDynamicLibrary)
};

class AltQModuleCoreDelegate : public IAltQCoreDelegate
{
public:
    typedef std::shared_ptr<AltQModuleCoreDelegate> Ptr;

    AltQModuleCoreDelegate(){}
    virtual ~AltQModuleCoreDelegate(){}

    // 回调
    class Listener {
    public:
        virtual ~Listener()  {}
        virtual void OnSearchFinish(const AltQResults& results, const std::string& context) = 0;
    };
    ListenerList<Listener> listeners;
    void addListener(Listener* const l)      { listeners.add(l); }
    void removeListener(Listener* const l)   { listeners.remove(l); }

protected:
    virtual void OnSearchFinish(const AltQResults& results, const std::string& context) override
    {
        listeners.call(&Listener::OnSearchFinish, results, context);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AltQModuleCoreDelegate)
};

//==============================================================================
class AltQModuleCore
{
public:
    AltQModuleCore();
    ~AltQModuleCore();
    juce_DeclareSingleton(AltQModuleCore, false);

    bool Search(const String& statement, const String& context = String());

    AltQModuleCoreDelegate* getCoreDelegate();
    IAltQCoreInternalPtr& getCoreInternal();

protected:
    bool initAltQCore();

private:
    AltQModuleCoreDelegate::Ptr altQModuleCoreDelegatePtr;
    IAltQCoreInternalPtr altQModuleCoreInternalPtr;
    IAltQCorePtr altQCorePtr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AltQModuleCore)
};
