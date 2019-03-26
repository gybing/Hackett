#pragma once
#include "AltQInterfaceCore.h"
#include "AltQModuleIndexer.h"
#include "workerpool.h"

class AltQCore : public IAltQCore, 
	public IAltQCoreInternal, 
	public IQueueJobProcessor,
	public std::enable_shared_from_this<AltQCore>
{
public:
	AltQCore();
	virtual ~AltQCore();

	virtual bool Initialize(const std::string& storage_path, const IAltQCoreDelegatePtr& delegate, IAltQCoreInternalPtr& internal);
	virtual void Finalize();
	virtual bool Search(const std::string& statement, const std::string& context = std::string());
	virtual IAltQIndexerPtr CreateModuleIndexer(const std::string& name, const AltQModuleID& id);
	
	// IQueueJobProcessor
	virtual void PreJobProcess(QueueJob* pJob){}
	virtual void OnJobProcess(QueueJob* pJob);
	virtual void EndJobProcess(QueueJob* pJob);
	virtual void OnJobDiscard(QueueJob* pJob);

protected:
	bool m_isInited;
	std::string m_StoragePath;

	IAltQCoreDelegateWPtr m_DelegatePtr;
	std::map<AltQModuleID, AltQModuleIndexerPtr> m_Indexers;
};
typedef std::shared_ptr<AltQCore> AltQCorePtr;