#pragma once
#include "AltQDefine.h"

// AltQ回调接口
class IAltQCoreDelegate
{
public:
	virtual void OnSearchFinish(const AltQResults& results, const std::string& context) = 0;
};
typedef std::shared_ptr<IAltQCoreDelegate> IAltQCoreDelegatePtr;
typedef std::weak_ptr<IAltQCoreDelegate> IAltQCoreDelegateWPtr;


// 提供给业务模块使用的索引器接口(同步， 业务模块需要有自己的线程池)
class IAltQIndexer
{
public:
	// 添加索引
	virtual bool AddDoc(const AltQDocument& doc) = 0;
	virtual bool AddDocBatch(const AltQDocuments& docs) = 0;

	// 增加此文档的检索评分
	virtual bool IncreaseScore(const AltQDocID& id) = 0;

	// 删除索引
	// 返回值代表是否操作成功，即使删除了0条，也返回true
	virtual bool DeleteDocByID(const AltQDocID& id) = 0;

	// 此接口可与AltQDocument配合完成批量删除
	// 返回值代表是否操作成功，即使删除了0条，也返回true
	virtual bool DeleteDocByUserData(const std::string& userdata) = 0;

	// 优化
	virtual bool Opti() = 0;
};
typedef std::shared_ptr<IAltQIndexer> IAltQIndexerPtr;

class IAltQCoreInternal
{
public:
	// 为业务模块创建索引器
	// 名字必须系统内唯一
	virtual IAltQIndexerPtr CreateModuleIndexer(const std::string& name, const AltQModuleID& id) = 0;
};
typedef std::shared_ptr<IAltQCoreInternal> IAltQCoreInternalPtr;

// AltQ内核接口
class IAltQCore
{
public:
	// internal为输出参数，提供给具体功能模块使用。
	virtual bool Initialize(const std::string& storage_path, const IAltQCoreDelegatePtr& delegate, IAltQCoreInternalPtr& internal) = 0;
	virtual void Finalize() = 0;

	// 检索
	virtual bool Search(const std::string& statement, const std::string& context = std::string()) = 0;
};
typedef std::shared_ptr<IAltQCore> IAltQCorePtr;

// UI层初始化时，在加载具体业务模块前完成此模块创建及初始化工作。
extern "C" ALTQ__API bool CreateAltQCore(IAltQCorePtr* altq);