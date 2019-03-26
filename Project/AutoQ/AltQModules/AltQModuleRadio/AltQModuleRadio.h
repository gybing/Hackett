#pragma once
#include "AltQDefine.h"
#include "AltQInterfaceModule.h"
#include "AltQBase.h"
#include "AltQModuleRadioDefine.h"
#include "AltQRadioDouban.h"
using namespace juce;

class AltQChannelIndexer;
class AltQModuleRadio : public IAltQModule
{
public:
	AltQModuleRadio();

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

protected:
    AltQRadioDouban radio;
    String m_StoragePath;
	IAltQIndexerPtr m_AltQRadioIndexer;
	IAltQCoreInternalPtr m_AltQCoreInternal;
    ScopedPointer<AltQChannelIndexer> indexer;
};
typedef std::shared_ptr<AltQModuleRadio> AltQModuleRadioPtr;

class AltQChannelIndexer : public Thread
{
public:
    AltQChannelIndexer(IAltQIndexerPtr idx, const std::vector<AltQDataChannel::Ptr>& chs)
        : indexer(idx), channels(chs), Thread("AltQModuleRadio_Thread")
    {}
    ~AltQChannelIndexer(){}

private:
    IAltQIndexerPtr indexer;
    const std::vector<AltQDataChannel::Ptr> channels;

    virtual void run() override;
};
