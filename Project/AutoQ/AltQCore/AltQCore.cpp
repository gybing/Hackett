#include "AltQCore.h"
#include "str_util.h"
#include "AltQDispatcher.h"
#include <algorithm>

IAltQCorePtr instance;
ALTQ__API bool CreateAltQCore(IAltQCorePtr* altq)
{
	if (instance)
	{
		*altq = instance;
		return true;
	}
	
	instance = std::dynamic_pointer_cast<IAltQCore>(std::make_shared<AltQCore>());
	if (!instance)
	{
		return false;
	}

	*altq = instance;
	return true;
}


AltQCore::AltQCore() : m_isInited(false)
{
	
}

AltQCore::~AltQCore()
{
	Finalize();
}

bool AltQCore::Initialize(const std::string& storage_path, const IAltQCoreDelegatePtr& delegate, IAltQCoreInternalPtr& internal)
{
	m_DelegatePtr = delegate;
	m_StoragePath = storage_path;
	if (!SafeMkdir(m_StoragePath.c_str()))
		return false;

	if (!AltQDispatcher::Instance()->Initialize())
		return false;

	internal = shared_from_this();
	m_isInited = true;
	return true;
}

void AltQCore::Finalize()
{
	if (!m_isInited)
		return;

	std::map<AltQModuleID, AltQModuleIndexerPtr>::iterator iter = m_Indexers.begin();
	while (iter != m_Indexers.end())
	{
		iter->second->Finalize();
		iter++;
	}

	AltQDispatcher::Instance()->Finalize();

	m_isInited = false;
}

bool AltQCore::Search(const std::string& statement, const std::string& context)
{
	return AltQDispatcher::Instance()->ProcessAsyn(new AltQJobSearch(statement, context), this);
}

IAltQIndexerPtr AltQCore::CreateModuleIndexer(const std::string& name, const AltQModuleID& id)
{
	std::string idxname = id + "/" + name;

	std::map<AltQModuleID, AltQModuleIndexerPtr>::iterator iter = m_Indexers.find(idxname);
	if (iter != m_Indexers.end())
	{
		return iter->second;
	}

	std::string pathname = m_StoragePath;
	if (!pathcat(pathname, idxname.c_str()))
		return NULL;

	AltQModuleIndexerPtr indexer = std::make_shared<AltQModuleIndexer>();
	if (!indexer->Initialize(pathname.c_str(), id))
		return false;

	m_Indexers[idxname] = indexer;
	return indexer;
}

void AltQCore::OnJobProcess(QueueJob* pJob)
{
	AltQJobSearch* searchJob = (AltQJobSearch*)pJob;

	std::string statment = /*searchJob->m_Statement + " AND " +*/ searchJob->m_Statement + "*";
	// 遍历搜索，合并结果
	AltQResults results, all;
	std::map<AltQModuleID, AltQModuleIndexerPtr>::iterator iter = m_Indexers.begin();
	while (iter != m_Indexers.end())
	{
		if (iter->second->Search(statment, results))
		{
			if (!results.empty())
				all.insert(all.end(), results.begin(), results.end());
		}
		iter++;
	}

	//std::sort(all.begin(), all.end());
	IAltQCoreDelegatePtr delegate = m_DelegatePtr.lock();
	if (delegate)
	{
		delegate->OnSearchFinish(all, searchJob->m_Context);
	}
}

void AltQCore::EndJobProcess(QueueJob* pJob)
{
	delete pJob;
}

void AltQCore::OnJobDiscard(QueueJob* pJob)
{
	delete pJob;
}
