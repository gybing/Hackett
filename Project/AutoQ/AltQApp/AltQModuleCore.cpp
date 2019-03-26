#include "AltQModuleCore.h"
#include "native/AltQNativeHelper.h"

//==============================================================================
juce_ImplementSingleton(AltQModuleCore);

AltQModuleCore::AltQModuleCore()
{
    initAltQCore();
}

AltQModuleCore::~AltQModuleCore()
{
    if (altQCorePtr)
        altQCorePtr->Finalize();
}

bool AltQModuleCore::initAltQCore()
{
    if (!altQModuleCoreDelegatePtr)
    {
        altQModuleCoreDelegatePtr = std::make_shared<AltQModuleCoreDelegate>();
    }

    if (!altQCorePtr)
    {
        AltQDynamicLibrary* core = new AltQDynamicLibrary;

        if (!core->open(ProjectInfo::coreModuleName))
            return false;

        typedef bool(*funCreateAltQCore)(IAltQCorePtr* altq);

        if (funCreateAltQCore func = (funCreateAltQCore)(core->getFunction(ProjectInfo::coreCreateFuncName)))
        {
            if (func(&altQCorePtr))
            {
                if (altQCorePtr->Initialize(AltQNative::GetStoragePath().toStdString(), std::dynamic_pointer_cast<IAltQCoreDelegate>(altQModuleCoreDelegatePtr), altQModuleCoreInternalPtr))
                    return true;
            }
        }
    }
    
    return false;
}

bool AltQModuleCore::Search(const String& statement, const String& context /*= String()*/)
{
    if (altQCorePtr && statement.isNotEmpty())
        return altQCorePtr->Search(statement.toStdString(), context.toStdString());

    return false;
}

AltQModuleCoreDelegate* AltQModuleCore::getCoreDelegate()
{
    return altQModuleCoreDelegatePtr.get();
}

IAltQCoreInternalPtr& AltQModuleCore::getCoreInternal()
{
    return altQModuleCoreInternalPtr;
}
