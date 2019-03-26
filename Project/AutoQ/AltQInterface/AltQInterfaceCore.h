#pragma once
#include "AltQDefine.h"

// AltQ�ص��ӿ�
class IAltQCoreDelegate
{
public:
	virtual void OnSearchFinish(const AltQResults& results, const std::string& context) = 0;
};
typedef std::shared_ptr<IAltQCoreDelegate> IAltQCoreDelegatePtr;
typedef std::weak_ptr<IAltQCoreDelegate> IAltQCoreDelegateWPtr;


// �ṩ��ҵ��ģ��ʹ�õ��������ӿ�(ͬ���� ҵ��ģ����Ҫ���Լ����̳߳�)
class IAltQIndexer
{
public:
	// �������
	virtual bool AddDoc(const AltQDocument& doc) = 0;
	virtual bool AddDocBatch(const AltQDocuments& docs) = 0;

	// ���Ӵ��ĵ��ļ�������
	virtual bool IncreaseScore(const AltQDocID& id) = 0;

	// ɾ������
	// ����ֵ�����Ƿ�����ɹ�����ʹɾ����0����Ҳ����true
	virtual bool DeleteDocByID(const AltQDocID& id) = 0;

	// �˽ӿڿ���AltQDocument����������ɾ��
	// ����ֵ�����Ƿ�����ɹ�����ʹɾ����0����Ҳ����true
	virtual bool DeleteDocByUserData(const std::string& userdata) = 0;

	// �Ż�
	virtual bool Opti() = 0;
};
typedef std::shared_ptr<IAltQIndexer> IAltQIndexerPtr;

class IAltQCoreInternal
{
public:
	// Ϊҵ��ģ�鴴��������
	// ���ֱ���ϵͳ��Ψһ
	virtual IAltQIndexerPtr CreateModuleIndexer(const std::string& name, const AltQModuleID& id) = 0;
};
typedef std::shared_ptr<IAltQCoreInternal> IAltQCoreInternalPtr;

// AltQ�ں˽ӿ�
class IAltQCore
{
public:
	// internalΪ����������ṩ�����幦��ģ��ʹ�á�
	virtual bool Initialize(const std::string& storage_path, const IAltQCoreDelegatePtr& delegate, IAltQCoreInternalPtr& internal) = 0;
	virtual void Finalize() = 0;

	// ����
	virtual bool Search(const std::string& statement, const std::string& context = std::string()) = 0;
};
typedef std::shared_ptr<IAltQCore> IAltQCorePtr;

// UI���ʼ��ʱ���ڼ��ؾ���ҵ��ģ��ǰ��ɴ�ģ�鴴������ʼ��������
extern "C" ALTQ__API bool CreateAltQCore(IAltQCorePtr* altq);