#pragma once
#include "SQLite/SQLiteDB.h"
#include "SQLite/SQLiteStmt.h"
#include "SQLite/SQLiteTokenizer.h"
#include "Singletons.h"
#include <string>
#include <memory>
#include <map>
#include "AltQDefine.h"

class AltQFTS
{
public:
	bool create(const char* pathfile);

	// �������
	bool add(const AltQDocument& doc, AltQDocID& id = AltQDocID());

	// ɾ������
	bool del(const AltQDocID& id);
	bool delbyuserdata(const std::string& userdata);

	// ����
	bool search(const char* statment, AltQResults& docs);

	bool increasescore(const AltQDocID& id);

	// �Ż�(����ʱ����)
	bool opti();

	// �ر�
	void close();

protected:
	bool opendb(const char* name, SQLiteDB& db);

protected:
	std::string m_Name;

	SQLiteDB m_DBSearcher;
	SQLiteDB m_DBIndexer;

	SQLiteTokenizer m_Tokenizer;
};
typedef std::shared_ptr<AltQFTS> AltQFTSPtr;

