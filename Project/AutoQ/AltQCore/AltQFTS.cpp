#include "AltQFTS.h"
#include "str_util.h"
#include "guid.h"
#include "iconv/CharEncode.h"
#include "WinConvert.h"
#include "AltQDefine.h"
#include "FLException.h"

bool AltQFTS::create(const char* pathfile)
{
	m_Name = pathfile;

	try
	{
		if (!opendb(m_Name.c_str(), m_DBSearcher) ||
			!opendb(m_Name.c_str(), m_DBIndexer))
			return false;

// 		AltQDocument doc;
// 		doc.m_Content = "123";
// 		doc.m_UserData = CGUID::Create().ToString();
// 		doc.m_ToIndex = AnsiToUtf8("启动安装器");
// 		add(doc);
// 
// 		AltQResults results;
// 		search(AnsiToUtf8("启动").c_str(), results);
		return true;
	}
    catch (FLException& e)
	{
		return false;
	}
}

bool AltQFTS::add(const AltQDocument& doc, AltQDocID& id)
{
	AutoSQLiteStmt stmt(&m_DBIndexer, "INSERT INTO altq(altqid, altqidx, altqlen, altquserdata, altqcontent, altqweight) VALUES(?,?,?,?,?,?)");

	try
	{
		std::string value = "\"" + CGUID::Create().ToString() + "\"";
		stmt->BindText(0, value.c_str());
		stmt->BindText(1, tolower(doc.m_ToIndex).c_str());
		stmt->BindText(2, i32tostr(doc.m_ToIndex.length()).c_str());

		value = "\"" + doc.m_UserData + "\"";
		stmt->BindText(3, value.c_str());
		stmt->BindText(4, doc.m_Content.c_str());
		stmt->BindText(5, "0");

		stmt->Exec();

		id = value;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool AltQFTS::del(const AltQDocID& id)
{
	AutoSQLiteStmt stmt(&m_DBIndexer, "DELETE FROM altq WHERE altq MATCH ?");

	try
	{
		std::string value = "\"" + id + "\"";
		stmt->BindText(0, strFormat("altqid:%s", value.c_str()).c_str());
		stmt->Exec();
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool AltQFTS::delbyuserdata(const std::string& userdata)
{
	AutoSQLiteStmt stmt(&m_DBIndexer, "DELETE FROM altq WHERE altq MATCH ?");

	try
	{
		std::string value = "\"" + userdata + "\"";
		stmt->BindText(0, strFormat("altquserdata:%s", value.c_str()).c_str());
		stmt->Exec();
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool AltQFTS::search(const char* statment, AltQResults& docs)
{
	std::string stats = tolower(statment);
	AutoSQLiteStmt stmt(&m_DBSearcher, "SELECT altqid, altqcontent, altqlen, altqidx FROM altq WHERE altq MATCH ? ORDER BY CAST(altqlen as INTEGER)  ASC LIMIT 200 OFFSET 0");
	
	try
	{
		stmt->BindText(0, strFormat("altqidx:%s", stats.c_str()).c_str());
		stmt->Exec();

		std::vector<std::string> vecidxs;
		if (stmt->hasResult())
		{
			do
			{
				AltQResultPtr doc = std::make_shared<AltQResult>();
				doc->m_DocID = stmt->GetColumnCString(0);
				doc->m_DocID = doc->m_DocID.substr(1, doc->m_DocID.length() - 2);
				doc->m_Content = stmt->GetColumnCString(1);
				const char* str_score = stmt->GetColumnCString(2);
				doc->m_Score = (str_score == NULL ? 0 : str2int(str_score));
				docs.push_back(doc);
				vecidxs.push_back(stmt->GetColumnCString(3));
			} while (stmt->FetchRow());
		}

		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool AltQFTS::increasescore(const AltQDocID& id)
{
	AutoSQLiteStmt stmtselect(&m_DBIndexer, "SELECT altqweight FROM altq WHERE altq MATCH ?");
	AutoSQLiteStmt stmt(&m_DBIndexer, "UPDATE altq set altqweight = ? WHERE altq MATCH ?");

	try
	{
		stmtselect->BindText(0, strFormat("altqid:%s", id.c_str()).c_str());
		stmtselect->Exec();

		if (stmtselect->hasResult())
		{
			std::string score = stmtselect->GetColumnCString(0);
			int nscore = str2int(score.c_str());
			nscore++;
			score = i32tostr(nscore);

			stmt->BindText(0, score.c_str());
			stmt->BindText(1, strFormat("altqid:%s", id.c_str()).c_str());
			stmt->Exec();
		}

		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool AltQFTS::opti()
{
	try
	{
		AutoSQLiteStmt stmt(&m_DBIndexer, "INSERT INTO altq(altq) VALUES('optimize');");
		stmt->Exec();
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool AltQFTS::opendb(const char* name, SQLiteDB& db)
{
	// 创建全文检索数据库文件
	db.OpenDB(name, "");

	SQLiteTokenizer tokenizer;
	if (!tokenizer.CreateToDB(&db))
		return false;

	AutoSQLiteStmt stmt_create(&db, strFormat("CREATE VIRTUAL TABLE IF NOT EXISTS altq USING fts5(altqid, altqidx, altqlen, altquserdata, altqcontent UNINDEXED, altqweight UNINDEXED, prefix='1 2 3 4', tokenize='%s')", SQLiteTokenizer::Name()).c_str());
	stmt_create->Exec();
	return true;
}

void AltQFTS::close()
{
	m_DBSearcher.CloseDB();
	m_DBIndexer.CloseDB();
}
