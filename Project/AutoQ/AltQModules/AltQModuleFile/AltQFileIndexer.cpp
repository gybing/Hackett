#include "AltQFileIndexer.h"
#include "AltQFileUtil.h"
#include "AltQFileDB.h"
#include "str_util.h"
#include <iostream>
#include "iconv/CharEncode.h"

void AltQFileIndexer::ExecUtilFinish(const IAltQIndexerPtr& indexer)
{
	m_FileCount = 0;
	m_DirCount = 0;

	m_ValidExts = GetIndexExts();

	m_pDB = AltQFileDB::Instance();
	m_Indexer = indexer;

	Start();
	WaitThreadExit();
}

void AltQFileIndexer::Stop()
{
	m_EventExit.Set();
}

int64_t GetDirTime(const char* DirName)
{
	HANDLE hDir = CreateFileA(DirName, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS, NULL);

	FILETIME lpCreationTime; // �ļ��еĴ���ʱ�� 
	FILETIME lpLastAccessTime; // ���ļ��е��������ʱ�� 
	FILETIME lpLastWriteTime; // �ļ��е�����޸�ʱ�� 
	
	int64_t tm = 0;
	if (GetFileTime(hDir, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime))
	{
		tm = lpLastWriteTime.dwHighDateTime;
		tm = tm << 32;
		tm |= lpLastWriteTime.dwLowDateTime;
	}
	CloseHandle(hDir); // �رմ򿪹����ļ���
	return tm;
}

int AltQFileIndexer::Run()
{
	std::vector<std::pair<std::string, int>> dirs = GetIndexDirs();

	for (int i = 0; i < dirs.size(); i++)
	{
		int64_t tm = GetDirTime(dirs[i].first.c_str());
		if (!RecursionDirectories(dirs[i].first.c_str(), IsTempIndex() ? 0 : tm, 0, dirs[i].second))
			return false;
	}
	return 0;
}

struct DirTime 
{
	std::string m_Dir;
	int64_t m_Tm;
};

// ��ȵݹ����Ŀ¼
// ����false�������ټ���ִ��
bool AltQFileIndexer::RecursionDirectories(const char* path, int64_t tm, int deep_current/* = 0*/, int deep_max/* = 0*/)
{
	deep_current++;
	if (deep_max > 0 && deep_current > deep_max)
	{
		return true;
	}

	int interval = IndexInterval();
	if (interval > 0)
	{
		Sleep(interval);
	}

	m_DirCount++;

	WIN32_FIND_DATAA FindData;
	HANDLE hError;

	std::string file_path(path);			// ·����  
	std::string full_file_path;			// ȫ·����   

	file_path.append("/*.*");
	hError = FindFirstFileA(file_path.c_str(), &FindData);
	if (hError == INVALID_HANDLE_VALUE)
		return true;

	// �ж�Ŀ¼����״̬
	bool isNewDir = false, isModifyDir = false;

	int64_t tmDB;
	if (!m_pDB->GetDirTime(path, tmDB))
	{
		isNewDir = true;
	}
	else if (tm > tmDB)
	{
		isModifyDir = true;
	}

	if (isModifyDir)
		m_Indexer->DeleteDocByUserData(path);

	std::vector<DirTime>* dirs = new std::vector<DirTime>();
	while (FindNextFileA(hError, &FindData))
	{
		if (m_EventExit.Wait(0))
		{
			FindClose(hError);
			return false;
		}

		// ����".", ".."
		if (0 == strcmp(FindData.cFileName, ".") ||
			0 == strcmp(FindData.cFileName, ".."))
		{
			continue;
		}

		//����·��
		full_file_path = path;
		full_file_path.append("\\");
		full_file_path.append(FindData.cFileName);

		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			DirTime dt;
			dt.m_Dir = full_file_path;
			dt.m_Tm = FindData.ftLastWriteTime.dwHighDateTime;
			dt.m_Tm = dt.m_Tm << 32;
			dt.m_Tm |= FindData.ftLastAccessTime.dwLowDateTime;
			dirs->push_back(dt);
		}
		else
		{
			if (isNewDir || isModifyDir)
			{
				AltQDocument doc;
				AltQFile file;
				file.m_FilePath = path;

				SubstractFileName(FindData.cFileName, file.m_FileName, file.m_FileExt);
				if (!IsValidExt(file.m_FileExt))
					continue;

				doc.m_ToIndex = GBKToUTF8(FindData.cFileName, strlen(FindData.cFileName));
				doc.m_UserData = path;
				
				AltQFileToJson(file, doc.m_Content);
				m_Indexer->AddDoc(doc);
				m_FileCount++;
			}
		}
	}
	
	FindClose(hError);

	// ��¼���
	if (isNewDir)
	{
		m_pDB->InsertDir(path, tm);
	}
	else if (isModifyDir)
	{
		m_pDB->SetDirTime(path, tm);
	}

	// ������Ŀ¼
	for (int i = 0; i < dirs->size(); i++)
	{
		DirTime& dt = (*dirs)[i];
		if (!RecursionDirectories(dt.m_Dir.c_str(), IsTempIndex() ? 0 : dt.m_Tm, deep_current, deep_max))
		{
			delete dirs;
			return false;
		}
	}
	delete dirs;
	return true;
}

bool AltQFileIndexer::IsValidExt(const std::string& m_FileExt)
{
	std::set<std::string>::iterator iter = m_ValidExts.find(m_FileExt);
	return iter != m_ValidExts.end();
}