// test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include "AltQInterfaceCore.h"
#include "AltQInterfaceModule.h"
#include "str_util.h"
#include "WinCounter.h"
#include "WinConvert.h"

#include <iostream>  
#include <string>  
#include <vector>  
#include <memory>  
#include <cstring>  
#include <list>
#include <windows.h>  
// 
// int dirs = 0;
// int files = 0;
// 
// bool RecursionDirectories(const char* path)
// {
// 	WIN32_FIND_DATAA FindData;
// 	HANDLE hError;
// 
// 	std::string file_path(path);			// ·����  
// 	std::string full_file_path;			// ȫ·����   
// 
// 	file_path.append("/*.*");
// 	hError = FindFirstFileA(file_path.c_str(), &FindData);
// 	if (hError == INVALID_HANDLE_VALUE)
// 		return true;
// 
// 	// �ж�Ŀ¼����״̬
// 	while (FindNextFileA(hError, &FindData))
// 	{
// 		// ����".", ".."
// 		if (0 == strcmp(FindData.cFileName, ".") ||
// 			0 == strcmp(FindData.cFileName, ".."))
// 		{
// 			continue;
// 		}
// 
// 		//����·��
// 		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
// 
// 			dirs++;
// 			full_file_path = path;
// 			full_file_path.append("/");
// 			full_file_path.append(FindData.cFileName);
// 			if (!RecursionDirectories(full_file_path.c_str()))
// 				return false;
// 		}
// 		else
// 		{
// 			files++;
// 		}
// 	}
// 
// 	return true;
// }
// 
// #include <queue>
// void TravalDir(const char* path)
// {
// 	std::queue<std::string> q;
// 	q.push(path);
// 
// 	std::string current_path;
// 	while (!q.empty()) 
// 	{
// 		current_path = q.front();
// 
// 		WIN32_FIND_DATAA FindData;
// 		HANDLE hError;
// 
// 		std::string file_path = current_path;
// 		std::string full_file_path;			// ȫ·����   
// 
// 		file_path.append("/*.*");
// 		hError = FindFirstFileA(file_path.c_str(), &FindData);
// 		if (hError == INVALID_HANDLE_VALUE)
// 		{
// 			q.pop();
// 			continue;
// 		}
// 
// 		// �ж�Ŀ¼����״̬
// 		while (FindNextFileA(hError, &FindData))
// 		{
// 			// ����".", ".."
// 			if (0 == strcmp(FindData.cFileName, ".") ||
// 				0 == strcmp(FindData.cFileName, ".."))
// 			{
// 				continue;
// 			}
// 
// 			//����·��
// 			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
// 			{
// 				dirs++;
// 				full_file_path = current_path;
// 				full_file_path.append("/");
// 				full_file_path.append(FindData.cFileName);
// 				q.push(full_file_path);
// 			}
// 			else
// 			{
// 				files++;
// 			}
// 		}
// 
// 		q.pop();
// 	}
// }
// 
// int64_t GetDriverTime(const char* driver)
// {
// 	std::string search = driver;
//  //	search += "*";
// 
// 	WIN32_FIND_DATAA data;
// 	HANDLE hError = FindFirstFileA(search.c_str(), &data);
// 	if (hError == INVALID_HANDLE_VALUE)
// 	{
// 		return 0;
// 	}
// 
// }
// 
// BOOL GetDirTime(const char* DirName){
// 	// ���ļ���
// 	SYSTEMTIME stime;
// 
// 	HANDLE hDir = CreateFileA(DirName, GENERIC_READ,
// 		FILE_SHARE_READ | FILE_SHARE_DELETE,
// 		NULL, OPEN_EXISTING,
// 		FILE_FLAG_BACKUP_SEMANTICS, NULL);
// 	FILETIME lpCreationTime; // �ļ��еĴ���ʱ�� 
// 	FILETIME lpLastAccessTime; // ���ļ��е��������ʱ�� 
// 	FILETIME lpLastWriteTime; // �ļ��е�����޸�ʱ�� 
// 	// ��ȡ�ļ���ʱ��������Ϣ
// 	if (GetFileTime(hDir, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime)){
// 		FILETIME ftime;
// 		FileTimeToLocalFileTime(&lpLastWriteTime, &ftime); // ת���ɱ���ʱ�� 
// 		FileTimeToSystemTime(&ftime, &stime); // ת����ϵͳʱ���ʽ
// 	}
// 	CloseHandle(hDir); // �رմ򿪹����ļ���
// 	return true;
// }
// 
// void main()
// {
// 	int64_t tm = GetDirTime("c:/");
// 
// 	HighPrecisionCounter couter;
// 	TravalDir("e:");
// 	std::cout << dirs << " " << files << " " <<couter.Diff() << std::endl;
// 
// 	dirs = 0;
// 	files = 0;
// 	couter.Clear();
// 	RecursionDirectories("e:");
// 	std::cout <<dirs<<" "<<files<<" "<<couter.Diff() << std::endl;
// 
// 
// 	system("pause");
// }

class altqDelegate : public IAltQCoreDelegate
{
	virtual void OnSearchFinish(const AltQResults& results, const std::string& context)
	{
		if (!results.empty())
		{
			AltQResultPtr relt = results[0];
			std::cout << relt->m_Content << std::endl;
		}
	}
};
typedef std::shared_ptr<altqDelegate> altqDelegatePtr;


void main()
{
	IAltQCorePtr corePtr;
	if (!CreateAltQCore(&corePtr))
	{
		return;
	}

	altqDelegatePtr delegate = std::make_shared<altqDelegate>();
	IAltQCoreInternalPtr coreInternal;
	if (!corePtr->Initialize("d:/altq_storage", delegate, coreInternal))
	{
		return;
	}

	IAltQModulePtr modulePtr;
	if (!CreateAltQModule(&modulePtr))
	{
		return;
	}

	if (!modulePtr->Initialize("d:/altq_storage/altqfile", coreInternal))
	{
		return;
	}

// 	while (true)
// 	{
// 		Sleep(1000);
// 		if (!corePtr->Search(AnsiToUtf8("2014")))
// 			return;
// 	}

//	Sleep(5000);
	system("pause");

	modulePtr->Finalize();
	corePtr->Finalize();
	
}
