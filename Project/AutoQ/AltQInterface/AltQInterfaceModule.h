#pragma once
#include "AltQInterfaceCore.h"

// AltQ结果项显示接口
class IAltQModuleResult
{
public:
	// 结果项标题
	virtual std::string GetTitle() = 0;

	// 结果项说明
	virtual std::string GetDescription() = 0;

	// 结果项图标
	// 返回文件全路径名，UI根据后缀决定：
	// 1. 如果是.icon，则取其内容，
	// 2. 否则读取其图标
	virtual std::string GetIcon() = 0;

    // 取DocID
    virtual AltQDocID GetDocID() = 0;
};
typedef std::shared_ptr<IAltQModuleResult> IAltQModuleResultPtr;

// 业务模块必须继承此接口
class IAltQModule 
{
public:
	// 初始化
	// storage_path通常是存储目录下的模块子目录，由UI框架决定存储位置。
    virtual bool Initialize(const std::string& key, const std::string& storage_path, const IAltQCoreInternalPtr& core) = 0;

	// 卸载
	// 必须在AltCore的Finalize前执行此Finalize.
	virtual void Finalize() = 0;

	// 解析搜索结果
	virtual IAltQModuleResultPtr ParseResult(const AltQResultPtr& result) = 0;

	// 当结果元素被用户选择时，调用此接口.
	// 业务模块可以根据需要调用indexer的增加评分接口
	virtual void OnResultSelected(const AltQDocID& id) = 0;
};
typedef std::shared_ptr<IAltQModule> IAltQModulePtr;

// 每个AltQ业务模块必须实现此接口
extern "C" ALTQ__API bool CreateAltQModule(IAltQModulePtr* module);

