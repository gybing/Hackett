#include "AltQModuleIndexer.h"
#include "AltQFTS.h"
#include "str_util.h"
#include "AltQDispatcher.h"

bool AltQModuleIndexer::Initialize(const char* pathname, const AltQModuleID& moduleid)
{
	m_ModuleID = moduleid;
	return m_AltQFTS.create(pathname);
}

void AltQModuleIndexer::Finalize()
{
	m_AltQFTS.close();
}

bool AltQModuleIndexer::Search(const std::string& statement, AltQResults& results)
{
	results.clear();	
	if (!m_AltQFTS.search(statement.c_str(), results))
		return false;

	for (int i = 0; i < results.size(); i++)
	{
		results[i]->m_ModuleID = m_ModuleID;
	}

	return true;
}

bool AltQModuleIndexer::AddDoc(const AltQDocument& doc)
{
	return m_AltQFTS.add(doc);
}

bool AltQModuleIndexer::IncreaseScore(const AltQDocID& id)
{
	return m_AltQFTS.increasescore(id);
}

bool AltQModuleIndexer::DeleteDocByID(const AltQDocID& id)
{
	return m_AltQFTS.del(id);
}

bool AltQModuleIndexer::DeleteDocByUserData(const std::string& userdata)
{
	return m_AltQFTS.delbyuserdata(userdata);
}

bool AltQModuleIndexer::Opti()
{
	return m_AltQFTS.opti();;
}

bool AltQModuleIndexer::AddDocBatch(const AltQDocuments& docs)
{
	for (int i = 0; i < docs.size(); i++)
	{
		if (!AddDoc(docs[i]))
			return false;
	}
	return true;
}