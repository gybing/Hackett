#include "AltQExtentions.h"
#include "native/AltQNativeHelper.h"

//==============================================================================
bool AltQModuleBase::initialize(const var& v, const File& path)
{
    moduleInfo.modulePath = path;

    if (const DynamicObject* objs = v.getDynamicObject())
    {
        moduleInfo.moduleID = objs->getProperty("key");
        moduleInfo.moduleName = objs->getProperty("name");
        moduleInfo.moduleDescription = objs->getProperty("description");

        const char* szkey = getType() == AltQModuleType_Theme ? "theme" : (getType() == AltQModuleType_Search ? "search" : "ui");
        return onInitialize(objs->getProperty(szkey));
    }

    return false;
}

const AltQModuleInfo& AltQModuleBase::getModuleInfo() const
{
    return moduleInfo;
}

bool AltQThemeModule::onInitialize(const var& v)
{
    return true;
}

AltQEnumModuleType AltQThemeModule::getType()
{
    return AltQModuleType_Theme;
}


bool AltQIndexModule::onInitialize(const var& v)
{
    String filename = v.getProperty("module", var()).toString();

    if (filename.isEmpty() || !open(moduleInfo.modulePath.getChildFile(filename).getFullPathName()))
        return false;

    typedef bool(*funCreateAltQModule)(IAltQModulePtr* module);
    if (funCreateAltQModule func = (funCreateAltQModule)(getFunction(ProjectInfo::moduleCreateFuncName)))
    {
        File storagePath(AltQNative::GetStoragePath());
        storagePath = storagePath.getChildFile(getModuleInfo().moduleID);
        storagePath.createDirectory();

        if (func(&modulePtr) && modulePtr->Initialize(moduleInfo.moduleID.toStdString(), storagePath.getFullPathName().toStdString(), AltQModuleCore::getInstance()->getCoreInternal()))
            return true;
    }

    return true;
}

AltQEnumModuleType AltQIndexModule::getType()
{
    return AltQModuleType_Search;
}

AltQIndexModule::~AltQIndexModule()
{
    if (modulePtr)
    {
        modulePtr->Finalize();
    }
}

IAltQModulePtr& AltQIndexModule::getModulePtr()
{
    return modulePtr;
}

bool AltQUIModule::onInitialize(const var& v)
{
    String filename = v.getProperty("module", var()).toString();

    if (filename.isEmpty() || !open(moduleInfo.modulePath.getChildFile(filename).getFullPathName()))
        return false;

    typedef bool(*funCreateAltQModuleUI)(IAltQModuleUIPtr* module);
    if (funCreateAltQModuleUI func = (funCreateAltQModuleUI)(getFunction(ProjectInfo::moduleUICreateFuncName)))
    {
        File storagePath(AltQNative::GetStoragePath());
        storagePath = storagePath.getChildFile(getModuleInfo().moduleID);
        storagePath.createDirectory();

        if (func(&moduleUIPtr) && moduleUIPtr->Initialize(AltQExtentions::getInstance()->getAltQModulePtr(moduleInfo.moduleID.toStdString())))
            return true;
    }

    return false;
}

AltQEnumModuleType AltQUIModule::getType()
{
    return AltQModule_UI;
}

IAltQModuleUIPtr& AltQUIModule::getModuleUIPtr()
{
    return moduleUIPtr;
}

//==============================================================================
juce_ImplementSingleton(AltQExtentions);

AltQExtentions::AltQExtentions()
{
}

AltQExtentions::~AltQExtentions()
{
}

void AltQExtentions::loadExtentions()
{
    mapIndexExtentions.clear();
    mapUIExtentions.clear();
    mapThemeExtentions.clear();

    File path(File::getCurrentWorkingDirectory().getChildFile(ProjectInfo::extentionPathName));
    if (path.isDirectory())
    {
        // 查找Plugin目录下的文件夹
        for (DirectoryIterator di(path, false, "*", File::findDirectories); di.next();)
        {
            const File& p = di.getFile();
            File extMain = p.getChildFile(ProjectInfo::extentionMainName);
            if (extMain.existsAsFile())
            {
                var v = JSON::parse(extMain);
                File path(extMain.getParentDirectory());

                createIndexModuleFromJson(v, path);
                createUIModuleFromJson(v, path);
                createThemeModuleFromJson(v, path);
            }
        }
    }
}

IAltQModuleResultPtr AltQExtentions::ParseResult(const AltQResultPtr& result)
{
    auto it = mapIndexExtentions.find(result->m_ModuleID);
    if (it != mapIndexExtentions.end())
    {
        return it->second->getModulePtr()->ParseResult(result);
    }
    return nullptr;
}

void AltQExtentions::resultSelected(const AltQResultPtr& result)
{
    auto it = mapIndexExtentions.find(result->m_ModuleID);
    if (it != mapIndexExtentions.end())
    {
        it->second->getModulePtr()->OnResultSelected(result->m_DocID);
    }
}

AltQIndexModule* AltQExtentions::createIndexModuleFromJson(const var& v, const File& path)
{
    AltQIndexModule* module = nullptr;
    if (DynamicObject* objs = v.getDynamicObject())
    {
        if (objs->hasProperty("search"))
        {
            module = new AltQIndexModule;

            if (!module->initialize(v, path))
            {
                String msg;
                msg << "[" << path.getFullPathName() << " ]";
                juce::AlertWindow::showMessageBox(AlertWindow::WarningIcon, "load search module error", msg);

                deleteAndZero(module);
            }
            else
            {
                mapIndexExtentions[module->getModuleInfo().moduleID.toStdString()] = module;
            }
        }
    }

    return module;
}

AltQThemeModule* AltQExtentions::createThemeModuleFromJson(const var& v, const File& path)
{
    AltQThemeModule* module = nullptr;
    if (DynamicObject* objs = v.getDynamicObject())
    {
        if (objs->hasProperty("theme"))
        {
            module = new AltQThemeModule;

            if (!module->initialize(v, path))
            {
                String msg;
                msg << "[" << path.getFullPathName() << " ]";
                juce::AlertWindow::showMessageBox(AlertWindow::WarningIcon, "load theme module error", msg);

                deleteAndZero(module);
            }
            else
            {
                mapThemeExtentions[module->getModuleInfo().moduleID.toStdString()] = module;
            }
        }
    }

    return module;
}

AltQUIModule* AltQExtentions::createUIModuleFromJson(const var& v, const File& path)
{
    AltQUIModule* module = nullptr;
    if (DynamicObject* objs = v.getDynamicObject())
    {
        if (objs->hasProperty("ui"))
        {
            module = new AltQUIModule;

            if (!module->initialize(v, path))
            {
                String msg;
                msg << "[" << path.getFullPathName() << " ]";
                juce::AlertWindow::showMessageBox(AlertWindow::WarningIcon, "load ui module error", msg);

                deleteAndZero(module);
            }
            else
            {
                mapUIExtentions[module->getModuleInfo().moduleID.toStdString()] = module;
            }
        }
    }

    return module;
}

IAltQModulePtr AltQExtentions::getAltQModulePtr(const AltQModuleID& moduleId)
{
    auto it = mapIndexExtentions.find(moduleId);
    if (it != mapIndexExtentions.end())
    {
        return it->second->getModulePtr();
    }
    return nullptr;
}

IAltQModuleUIPtr AltQExtentions::getAltQModuleUIPtr(const AltQModuleID& moduleId)
{
    auto it = mapUIExtentions.find(moduleId);
    if (it != mapUIExtentions.end())
    {
        return it->second->getModuleUIPtr();
    }
    return nullptr;
}

AltQWindow* AltQExtentions::RefreshResultItemComponent(const AltQResultPtr& result, const IAltQModuleResultPtr& parseResult, AltQWindow* existingWindowToUpdate)
{
    if (IAltQModuleUIPtr ui = getAltQModuleUIPtr(result->m_ModuleID))
    {
        return ui->RefreshResultItemComponent(parseResult, existingWindowToUpdate);
    }
    return nullptr;
}

AltQWindow* AltQExtentions::RefreshResultDetailComponent(const AltQResultPtr& result, const IAltQModuleResultPtr& parseResult, AltQWindow* existingWindowToUpdate)
{
    if (IAltQModuleUIPtr ui = getAltQModuleUIPtr(result->m_ModuleID))
    {
        return ui->RefreshResultDetailComponent(parseResult, existingWindowToUpdate);
    }
    return nullptr;
}

bool AltQExtentions::ShowPopupMenu(const AltQResultPtr& result, const IAltQModuleResultPtr& parseResult)
{
    if (IAltQModuleUIPtr ui = getAltQModuleUIPtr(result->m_ModuleID))
    {
        return ui->ShowPopupMenu(parseResult);
    }
    return false;
}

bool AltQExtentions::OnResultItemExcute(const AltQResultPtr& result, const IAltQModuleResultPtr& parseResult)
{
    if (IAltQModuleUIPtr ui = getAltQModuleUIPtr(result->m_ModuleID))
    {
        return ui->OnResultItemExcute(parseResult);
    }
    return false;
}

bool AltQExtentions::Initialize()
{
    loadExtentions();
    return true;
}

void AltQExtentions::Finalize()
{

}

