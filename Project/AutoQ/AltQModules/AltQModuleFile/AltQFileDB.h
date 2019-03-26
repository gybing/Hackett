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

	// �ж��Ƿ�Ϊ��Ŀ¼
	bool GetDirTime(const char* path, int64_t& modifyTime);

	// �����޸�ʱ��
	bool SetDirTime(const char* path, int64_t modifyTime);

	// �����¼�¼
	bool InsertDir(const char* path, int64_t modifyTime);

	// ��ȡ����޸�Ŀ¼
	std::vector<std::string> GetRecentModifyDirs(int count);

protected:
	friend class Singletons<AltQFileDB>;

public:
	SQLiteDB m_DB;
};