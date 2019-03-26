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

	// ��ʼ��
    virtual bool Initialize(const std::string& key, const std::string& storage_path, const IAltQCoreInternalPtr& core);

	// ж��
	// ������AltCore��Finalizeǰִ�д�Finalize.
	virtual void Finalize();

	// �����������
	// ����IAltQModuleResult�����������ָ�롣
	// �����������ɲ����UI�ͺ�̨ģ��������Э�̡�
	virtual IAltQModuleResultPtr ParseResult(const AltQResultPtr& result);

	// �����Ԫ�ر��û�ѡ��ʱ�����ô˽ӿ�.
	// ҵ��ģ����Ը�����Ҫ����indexer���������ֽӿ�
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
