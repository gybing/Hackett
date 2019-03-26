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

	// 添加索引
	bool add(const AltQDocument& doc, AltQDocID& id = AltQDocID());

	// 删除索引
	bool del(const AltQDocID& id);
	bool delbyuserdata(const std::string& userdata);

	// 检索
	bool search(const char* statment, AltQResults& docs);

	bool increasescore(const AltQDocID& id);

	// 优化(空闲时调用)
	bool opti();

	// 关闭
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

