#include "AltQFileDB.h"
#include "SQLite/SQLiteStmt.h"

bool AltQFileDB::Initialize(const char* filepath)
{
	try
	{
		m_DB.OpenDB(filepath, "");

		AutoSQLiteStmt stmt(&m_DB, "CREATE TABLE IF NOT EXISTS altqdirs(dir TEXT, modtime INTEGER, PRIMARY KEY(dir))");
		stmt->Exec();

		AutoSQLiteStmt stmtIdx(&m_DB, "CREATE INDEX IF NOT EXISTS idxTime on altqdirs(modtime);");
		stmtIdx->Exec();
		return true;
	}
	catch (...)
	{
		return false;
	}
}

void AltQFileDB::Finalize()
{
	m_DB.CloseDB();
}

bool AltQFileDB::GetDirTime(const char* path, int64_t& modifyTime)
{
	try
	{
		AutoSQLiteStmt stmt(&m_DB, "SELECT modtime FROM altqdirs WHERE dir = ?");
		stmt->BindText(0, path);
		stmt->Exec();

		if (!stmt->hasResult())
		{
			return false;
		}

		modifyTime = stmt->GetColumnInt64(0);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool AltQFileDB::SetDirTime(const char* path, int64_t modifyTime)
{
	try
	{
		AutoSQLiteStmt stmt(&m_DB, "UPDATE altqdirs SET modtime = ? WHERE dir = ?");
		stmt->BindInt64(0, modifyTime);
		stmt->BindText(1, path);
		stmt->Exec();
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool AltQFileDB::InsertDir(const char* path, int64_t modifyTime)
{
	try
	{
		AutoSQLiteStmt stmt(&m_DB, "INSERT INTO altqdirs (dir, modtime) VALUES(?,?);");
		stmt->BindText(0, path);
		stmt->BindInt64(1, modifyTime);
		stmt->Exec();
		return true;
	}
	catch (...)
	{
		return false;
	}
}

std::vector<std::string> AltQFileDB::GetRecentModifyDirs(int count)
{
	std::vector<std::string> dirs;
	try
	{
		AutoSQLiteStmt stmt(&m_DB, "SELECT dir FROM altqdirs ORDER BY modtime desc  LIMIT ? OFFSET 0");
		stmt->BindInt(0, count);
		stmt->Exec();

		if (!stmt->hasResult())
		{
			return dirs;
		}

		do 
		{
			dirs.push_back(stmt->GetColumnCString(0));
		} while (stmt->FetchRow());
		
		return dirs;
	}
	catch (...)
	{
		return dirs;
	}
}

