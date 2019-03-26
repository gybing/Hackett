#pragma once
#include <string>
#include <stdint.h>
#include "SQLite/SQLiteDB.h"
#include "Singletons.h"
#include <vector>

class AltQFileDB : public Singletons<AltQFileDB>
{
public:
	bool Initialize(const char* filepath);
	void Finalize();

	// 判定是否为新目录
	bool GetDirTime(const char* path, int64_t& modifyTime);

	// 设置修改时间
	bool SetDirTime(const char* path, int64_t modifyTime);

	// 插入新记录
	bool InsertDir(const char* path, int64_t modifyTime);

	// 获取最近修改目录
	std::vector<std::string> GetRecentModifyDirs(int count);

protected:
	friend class Singletons<AltQFileDB>;

public:
	SQLiteDB m_DB;
};