#include "AltQDispatcher.h"

AltQDispatcher::AltQDispatcher()
{
	m_Inited = false;
}

bool AltQDispatcher::Initialize()
{
	if (m_Inited)
	{
		return true;
	}
	
	if (!m_ThreadSearch.Initialize() ||
		!m_ThreadIndex.Initialize())
		return false;

	m_Inited = true;
	return true;
}

void AltQDispatcher::Finalize()
{
	if (!m_Inited)
	{
		return;
	}
	m_ThreadSearch.Finalize(0);
	m_ThreadIndex.Finalize(0);

	m_Inited = false;
}

bool AltQDispatcher::ProcessAsyn(AltQJob* job, IQueueJobProcessor* processor)
{
	switch (job->m_Type)
	{
	case AltQJobTypeIndex:
	case AltQJobTypeIndexBatch:
	case AltQJobTypeDelByID:
	case AltQJobTypeDelByUserData:
	case AltQJobTypeIncreaseScore:
	case AltQJobTypeOpti:
		return m_ThreadIndex.PutJob(job, processor);

	case AltQJobTypeSearch:
		return m_ThreadSearch.PutJob(job, processor);

	default:
		return false;
	}
}



