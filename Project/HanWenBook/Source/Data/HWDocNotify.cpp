#include "JuceHeader.h"
#include "HWDocNotify.h"


juce_ImplementSingleton(HWDocNotify)

HWDocNotify::HWDocNotify( void )
{
}

HWDocNotify::~HWDocNotify( void )
{
}

void HWDocNotify::OnSystemNotify(HWEnum_SysNotifyType sysNotify, HWContext ctx)
{
    HWNOTIFY(OnSystemNotify(sysNotify, ctx))
}

void HWDocNotify::OnBusiNotify(HWEnum_BusiNotifyType busiNotify, const HWDict* dict)
{
    HWNOTIFY(OnBusiNotify(busiNotify, dict))
}

