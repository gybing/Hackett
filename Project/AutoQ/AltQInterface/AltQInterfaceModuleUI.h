#pragma once
#include "AltQDefine.h"
#include "AltQInterfaceModule.h"
#include "AltQBase.h"

typedef juce::Component AltQWindow;

class IAltQModuleUI
{
public:
    // 初始化
    virtual bool Initialize(const IAltQModulePtr& module) = 0;

    // 刷新搜索结果列表显示项, 可以返回空。
    // 主框架需要显示时调用，主框架负责内存管理
    // existingComponentToUpdate不为空表示需要刷新
    // 此时应该始终调用static_cast<YourAltQWindow*>(existingComponentToUpdate)转换
    // 如果转换失败应该创建新的 YourAltQWindow
    virtual AltQWindow* RefreshResultItemComponent(const IAltQModuleResultPtr& result, AltQWindow* existingWindowToUpdate) = 0;

    // 刷新详情预览窗口
    virtual AltQWindow* RefreshResultDetailComponent(const IAltQModuleResultPtr& result, AltQWindow* existingWindowToUpdate) = 0;

    // 用于显示列表项的右键菜单
    virtual bool ShowPopupMenu(const IAltQModuleResultPtr& result) = 0;

    // 用户双击了某一项或在某一项按下了回车键
    virtual bool OnResultItemExcute(const IAltQModuleResultPtr& result) = 0;

    // 卸载
    // 必须在AltCore的Finalize前执行此Finalize.
    virtual void Finalize() = 0;
};
typedef std::shared_ptr<IAltQModuleUI> IAltQModuleUIPtr;

// 每个AltQUI模块必须实现此接口
extern "C" ALTQ__API bool CreateAltQModuleUI(IAltQModuleUIPtr* module);

