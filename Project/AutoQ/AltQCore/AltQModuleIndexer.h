#pragma once
#include "AltQInterfaceCore.h"
#include "AltQFTS.h"
#include "workerpool.h"

class AltQModuleIndexer : public IAltQIndexer
{
public:
	bool Initialize(const char* pathname, const AltQModuleID& moduleid);
	void Finalize();

	bool Search(const std::string& statement, AltQResults& results);
	virtual bool AddDoc(const AltQDocument& doc);
	virtual bool AddDocBatch(const AltQDocuments& docs);

	virtual bool IncreaseScore(const AltQDocID& id);
	virtual bool DeleteDocByID(const AltQDocID& id);
	virtual bool DeleteDocByUserData(const std::string& userdata);
	
	bool Opti();

protected:
	AltQModuleID m_ModuleID;
	AltQFTS m_AltQFTS;
};

typedef std::shared_ptr<AltQModuleIndexer> AltQModuleIndexerPtr;