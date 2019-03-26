#pragma once
#include "AltQDefine.h"
#include "AltQInterfaceModule.h"
#include "base_thread.h"
#include "thread_util.h"

class AltQModuleFile : public IAltQModule, public BaseThread
{
public:
	AltQModuleFile();

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
	virtual int Run();

protected:
	bool m_Inited;
	std::string m_StoragePath;
	IAltQIndexerPtr m_AltQFTSIndexer;
	IAltQCoreInternalPtr m_AltQCoreInternal;

	ThreadEvent m_ThreadEvent;
};
typedef std::shared_ptr<AltQModuleFile> AltQModuleFilePtr;

