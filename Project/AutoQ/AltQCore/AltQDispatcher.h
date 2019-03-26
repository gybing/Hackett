#pragma once
#include "Singletons.h"
#include "workerpool.h"
#include "AltQDefine.h"
#include "AltQJob.h"

class AltQDispatcher : public Singletons<AltQDispatcher>
{
public:
	AltQDispatcher();

	bool Initialize();
	void Finalize();

	bool ProcessAsyn(AltQJob* job, IQueueJobProcessor* processor);

protected:
	WorkerPool m_ThreadIndex;
	WorkerPool m_ThreadSearch;

	bool m_Inited;
};