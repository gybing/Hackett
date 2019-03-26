#pragma once
#include "AltQInterfaceCore.h"

// AltQ�������ʾ�ӿ�
class IAltQModuleResult
{
public:
	// ��������
	virtual std::string GetTitle() = 0;

	// �����˵��
	virtual std::string GetDescription() = 0;

	// �����ͼ��
	// �����ļ�ȫ·������UI���ݺ�׺������
	// 1. �����.icon����ȡ�����ݣ�
	// 2. �����ȡ��ͼ��
	virtual std::string GetIcon() = 0;

    // ȡDocID
    virtual AltQDocID GetDocID() = 0;
};
typedef std::shared_ptr<IAltQModuleResult> IAltQModuleResultPtr;

// ҵ��ģ�����̳д˽ӿ�
class IAltQModule 
{
public:
	// ��ʼ��
	// storage_pathͨ���Ǵ洢Ŀ¼�µ�ģ����Ŀ¼����UI��ܾ����洢λ�á�
    virtual bool Initialize(const std::string& key, const std::string& storage_path, const IAltQCoreInternalPtr& core) = 0;

	// ж��
	// ������AltCore��Finalizeǰִ�д�Finalize.
	virtual void Finalize() = 0;

	// �����������
	virtual IAltQModuleResultPtr ParseResult(const AltQResultPtr& result) = 0;

	// �����Ԫ�ر��û�ѡ��ʱ�����ô˽ӿ�.
	// ҵ��ģ����Ը�����Ҫ����indexer���������ֽӿ�
	virtual void OnResultSelected(const AltQDocID& id) = 0;
};
typedef std::shared_ptr<IAltQModule> IAltQModulePtr;

// ÿ��AltQҵ��ģ�����ʵ�ִ˽ӿ�
extern "C" ALTQ__API bool CreateAltQModule(IAltQModulePtr* module);

