#pragma once

enum AltQJobType
{
	AltQJobTypeIndex,
	AltQJobTypeIndexBatch,
	AltQJobTypeSearch,
	AltQJobTypeDelByID,
	AltQJobTypeDelByUserData,
	AltQJobTypeIncreaseScore,
	AltQJobTypeOpti
};

class AltQJob : public QueueJob
{
public:
	AltQJob(AltQJobType type, const std::string& context){ m_Type = type; m_Context = context; }
	AltQJobType m_Type;
	std::string m_Context;
};

class AltQJobIndex : public AltQJob
{
public:
	AltQJobIndex(const AltQDocument& doc, const std::string& context) : AltQJob(AltQJobTypeIndex, context)
	{
		m_Doc = doc;
	}
	AltQDocument m_Doc;
};

class AltQJobIndexBatch : public AltQJob
{
public:
	AltQJobIndexBatch(const AltQDocuments& doc, const std::string& context) : AltQJob(AltQJobTypeIndexBatch, context)
	{
		m_Doc = doc;
	}
	AltQDocuments m_Doc;
};

class AltQJobSearch : public AltQJob
{
public:
	AltQJobSearch(const std::string& statement, const std::string& context) : AltQJob(AltQJobTypeSearch, context)
	{
		m_Statement = statement;
	}

	std::string m_Statement;
};

class AltQJobDeleteByID : public AltQJob
{
public:
	AltQJobDeleteByID(const AltQDocID& id, const std::string& context) : AltQJob(AltQJobTypeDelByID, context)
	{
		m_ID = id;
	}

	AltQDocID m_ID;
};

class AltQJobIncreaseScore : public AltQJob
{
public:
	AltQJobIncreaseScore(const AltQDocID& id, const std::string& context) : AltQJob(AltQJobTypeOpti, context)
	{
		m_ID = id;
	}
	AltQDocID m_ID;
};

class AltQJobOpti : public AltQJob
{
public:
	AltQJobOpti() : AltQJob(AltQJobTypeIncreaseScore, "")
	{
	}
};

class AltQJobDeleteByUserData : public AltQJob
{
public:
	AltQJobDeleteByUserData(const std::string& userdata, const std::string& context) : AltQJob(AltQJobTypeDelByUserData, context)
	{
		m_UserData = userdata;
	}

	std::string m_UserData;
};