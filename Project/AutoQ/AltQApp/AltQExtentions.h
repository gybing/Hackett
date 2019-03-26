#pragma once
#include "AltQHeader.h"
#include "AltQModuleCore.h"
#include <map>

enum AltQEnumModuleType { AltQModuleType_Theme, AltQModuleType_Search, AltQModule_UI };

struct AltQModuleInfo
{
    String moduleID;
    String moduleName;
    String moduleDescription;
    String moduleVersion;
    File modulePath;
};


class AltQExtentions;

// 模块基类
class AltQModuleBase
{
public:
    virtual ~AltQModuleBase(){}

    bool initialize(const var& v, const File& path);
    const AltQModuleInfo& getModuleInfo() const;

    virtual AltQEnumModuleType getType() = 0;

protected:
    virtual bool onInitialize(const var& v) = 0;

    AltQModuleInfo moduleInfo;
    var value;
};

// 主题模块
class AltQThemeModule : public AltQModuleBase
{
public:
    AltQThemeModule(){}
    ~AltQThemeModule(){}

protected:
    virtual bool onInitialize(const var& v) override;
    virtual AltQEnumModuleType getType() override;

private:
    friend class AltQExtentions;
};

// 索引模块
class AltQIndexModule : public AltQModuleBase, public DynamicLibrary
{
public:
    AltQIndexModule(){}
    ~AltQIndexModule();

    IAltQModulePtr& getModulePtr();

protected:
    virtual bool onInitialize(const var& v) override;
    virtual AltQEnumModuleType getType() override;

private:
    friend class AltQExtentions;
    IAltQModulePtr modulePtr;
};

// UI模块
class AltQUIModule : public AltQModuleBase, public DynamicLibrary
{
public:
    AltQUIModule(){}
    ~AltQUIModule(){}

    IAltQModuleUIPtr& getModuleUIPtr();

protected:
    virtual bool onInitialize(const var& v) override;
    virtual AltQEnumModuleType getType() override;

private:
    friend class AltQExtentions;
    IAltQModuleUIPtr moduleUIPtr;
};

// 插件管理
class AltQExtentions
{
public:
    AltQExtentions();
    ~AltQExtentions();
    juce_DeclareSingleton(AltQExtentions, false);

    bool Initialize();
    void Finalize();

    // module
    IAltQModuleResultPtr ParseResult(const AltQResultPtr& result);
    void resultSelected(const AltQResultPtr& result);

    // ui
    AltQWindow* RefreshResultItemComponent(const AltQResultPtr& result, const IAltQModuleResultPtr& parseResult, AltQWindow* existingWindowToUpdate);
    AltQWindow* RefreshResultDetailComponent(const AltQResultPtr& result, const IAltQModuleResultPtr& parseResult, AltQWindow* existingWindowToUpdate);
    bool ShowPopupMenu(const AltQResultPtr& result, const IAltQModuleResultPtr& parseResult);
    bool OnResultItemExcute(const AltQResultPtr& result, const IAltQModuleResultPtr& parseResult);

    IAltQModulePtr getAltQModulePtr(const AltQModuleID& moduleId);
    IAltQModuleUIPtr getAltQModuleUIPtr(const AltQModuleID& moduleId);

protected:
    void loadExtentions();

    AltQIndexModule* createIndexModuleFromJson(const var& v, const File& path);
    AltQUIModule* createUIModuleFromJson(const var& v, const File& path);
    AltQThemeModule* createThemeModuleFromJson(const var& v, const File& path);

private:
    std::map<AltQModuleID, ScopedPointer<AltQIndexModule>> mapIndexExtentions;
    std::map<AltQModuleID, ScopedPointer<AltQUIModule>> mapUIExtentions;
    std::map<AltQModuleID, ScopedPointer<AltQThemeModule>> mapThemeExtentions;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AltQExtentions)
};
