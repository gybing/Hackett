#pragma once
#include "AltQDefine.h"
#include "AltQInterfaceModule.h"
#include "base_thread.h"
#include "thread_util.h"

class AltQModuleFile : public IAltQModule, public BaseThread
{
public:
	AltQModuleFile();

	// 初始化
    virtual bool Initialize(const std::string& key, const std::string& storage_path, const IAltQCoreInternalPtr& core);

	// 卸载
	// 必须在AltCore的Finalize前执行此Finalize.
	virtual void Finalize();

	// 解析搜索结果
	// 返回IAltQModuleResult派生类的智能指针。
	// 派生类内容由插件的UI和后台模块类自行协商。
	virtual IAltQModuleResultPtr ParseResult(const AltQResultPtr& result);

	// 当结果元素被用户选择时，调用此接口.
	// 业务模块可以根据需要调用indexer的增加评分接口
	virtual void OnResultSelected(const AltQDocID& id);

protected:
	virtual int Run();

protected:
	bool m_Inited;
	std::string m_StoragePath;
	IAltQIndexerPtr m_AltQFTSIndexer;
	IAltQCoreInternalPtr m_AltQCoreInternal;

	ThreadEvent m_ThreadEvent;
};
typedef std::shared_ptr<AltQModuleFile> AltQModuleFilePtr;

