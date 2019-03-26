#pragma once
#include "HWDocBase.h"
#include "HWDefine.h"

class IHWSubjectNotify
{
public:
    virtual void OnSystemNotify(HWEnum_SysNotifyType sysNotify, HWContext ctx) = 0;
    virtual void OnBusiNotify(HWEnum_BusiNotifyType busiNotify, const HWDict* dict) = 0;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IHWSubjectNotify)
};

class HWDocNotify : public HWSubject<IHWSubjectNotify>
{
public:
    juce_DeclareSingleton(HWDocNotify, false)
    HWDocNotify(void);
    ~HWDocNotify(void);

    void OnSystemNotify(HWEnum_SysNotifyType sysNotify, HWContext ctx);
    void OnBusiNotify(HWEnum_BusiNotifyType busiNotify, const HWDict* dict);
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWDocNotify)
};

