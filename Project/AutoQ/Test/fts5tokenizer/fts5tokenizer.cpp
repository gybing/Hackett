// fts5tokenizer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdint.h>
#include <iostream>
#include "str_util.h"
#include "WinCounter.h"
#include "guid.h"
#include "WinConvert.h"
#include "pinyin.h"
#include "SQLite/SQLiteDB.h"
#include "SQLite/SQLiteStmt.h"
#include "SQLite/SQLiteTokenizer.h"

#include "consolecmd.h"

SQLiteDB db;

class CmdIndex : public ConsoleCmd
{
	virtual std::string GetCommName() const { return "index"; }
	virtual std::string GetInputDescription() const { return "text"; }
	virtual void Exec(const std::string& para) 
	{
		if (para.empty())
		{
			std::cout << "need parameter: text" << std::endl;
			return;
		}

		try
		{
			HighPrecisionCounter counter;
			AutoSQLiteStmt stmt_insert(&db, "INSERT INTO tableapp(name) VALUES(?)");
			stmt_insert->BindText(0, AnsiToUtf8(para).c_str());
			stmt_insert->Exec();
			std::cout << "index used:" << counter.Diff() << "微秒" << std::endl;
		}
		catch (...)
		{
			std::cout << "exception occur:" << db.ErrorDescription() << std::endl;
		}
	}
};

#include <conio.h>

class CmdSearch : public ConsoleCmd
{
	virtual std::string GetCommName() const { return "search"; }
	virtual std::string GetInputDescription() const { return "text"; }
	virtual void Exec(const std::string& para)
	{
		std::string para1;
		char c;
		while (c = _getch())
		{
			if (c == 8)
			{
				para1.pop_back();
			}
			else
			{
				para1 += c;
			}

			std::cout << "search:"<<para1 << std::endl;
			search(para1.c_str());
		}
	}

	void search(const char* statment)
	{
		try
		{
			HighPrecisionCounter counter;
			AutoSQLiteStmt stmt_search(&db, "SELECT rowid, name FROM tableapp WHERE tableapp MATCH ? ORDER BY rank ASC  LIMIT 10 OFFSET 0");
			stmt_search->BindText(0, AnsiToUtf8(statment).c_str());
			stmt_search->Exec();
			std::cout << "search used:" << counter.Diff() << "微秒" << std::endl;

			if (!stmt_search->hasResult())
			{
				std::cout << "no result" << std::endl;
				return;
			}
			else
			{
				do
				{
					int rowid = stmt_search->GetColumnInt(0);
					std::string name = stmt_search->GetColumnCString(1);
					std::cout << "\rrowid:" << rowid << " name:" << Utf8ToAnsi(name) << std::endl;
				} while (stmt_search->FetchRow());

				std::cout << std::endl;
			}
		}
		catch (...)
		{
			std::cout << "exception" << std::endl;
		}
	}
};

int tmain(int argc, _TCHAR* argv[])
{	
	try
	{
		// 创建全文检索数据库文件
		db.OpenDB("./test_fts.db", "");

		SQLiteTokenizer tokenizer;
		if (!tokenizer.CreateToDB(&db))
		{
			std::cout << "Create SQLiteTokenizer Failed" << std::endl;
			return 0;
		}

		AutoSQLiteStmt stmt_create(&db, strFormat("CREATE VIRTUAL TABLE IF NOT EXISTS tableapp USING fts5(name, prefix='2 4 6', tokenize='%s')", SQLiteTokenizer::Name()).c_str());
		stmt_create->Exec();
	}
	catch (...)
	{
		std::cout << "exception:" << db.ErrorDescription() << std::endl;
		return 0;
	}

	ConsoleCmdManager mgr;
	CmdIndex* cmdIndex = new CmdIndex();
	CmdSearch* cmdSearch = new CmdSearch();
	mgr.AddSubCmd(cmdIndex);
	mgr.AddSubCmd(cmdSearch);
	mgr.Run();

	system("pause");
	return 0;
}

