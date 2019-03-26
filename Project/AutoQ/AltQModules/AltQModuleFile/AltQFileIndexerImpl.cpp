#include "AltQFileIndexerImpl.h"
#include "str_util.h"
#include <shlobj.h>
#include <shlwapi.h>

int system_dirs[] =
{
	CSIDL_ADMINTOOLS, //'���û���\��ʼ�˵�\����\ϵͳ������
	5, CSIDL_ALTSTARTUP, //'δ���ػ�������
	5, CSIDL_APPDATA, //'���û���\Ӧ�ó��������
	5, CSIDL_BITBUCKET, //'�����棩\����վ
	5, CSIDL_CONTROLS, //'�ҵĵ���\�������
	5, CSIDL_COOKIES, //
	5, CSIDL_DESKTOP, // '����
	5, CSIDL_DESKTOPDIRECTORY, // '���û���\����
	5, CSIDL_FAVORITES, // '���û���\���Ի�����
	5, CSIDL_FONTS, //'windows\����
	5, CSIDL_HISTORY,
	5, CSIDL_INTERNET, //'IE�������ϵ�ͼ��
	5, CSIDL_INTERNET_CACHE, //'�����������ļ���
	5, CSIDL_LOCAL_APPDATA, //'���û���\��������\Ӧ�ó�������
	5, CSIDL_DRIVES,//'�ҵĵ���
	5, CSIDL_MYPICTURES,//'C:\Program Files\My Pictures
	5, CSIDL_NETHOOD,//'���û���\�����ھ��е�Ԫ��
	5, CSIDL_NETWORK,//'�����ھ�
	5, CSIDL_PRINTERS,//'�ҵĵ���\��ӡ��
	5, CSIDL_PRINTHOOD,//'���û���\��ӡ������
	5, CSIDL_PERSONAL,//'�ҵ��ĵ�
	5, CSIDL_PROGRAM_FILES,//'C:\Program Files
	5, CSIDL_PROGRAM_FILESX86,//'x86 apps (Alpha)�ĳ����ļ�Ŀ¼
	5, CSIDL_PROGRAMS,//'��ʼ�˵�\����
	5, CSIDL_PROGRAM_FILES_COMMON,//'Program Files\Common
	5, CSIDL_PROGRAM_FILES_COMMONX86,//'RISC�ϵ�x86 \Program Files\Common
	5, CSIDL_RECENT,//'(�û�)\�����¼Ŀ¼
	5, CSIDL_SENDTO,//'(�û�)\���͵�Ŀ¼
	5, CSIDL_STARTMENU,//'(�û�)\��ʼ�˵�
	5, CSIDL_STARTUP,//'��ʼ�˵�\����\����
	5, CSIDL_SYSTEM,//'system�ļ���
	5, CSIDL_SYSTEMX86,//'x86 apps (Alpha)��system�ļ���
	5, CSIDL_TEMPLATES,//
	5, CSIDL_PROFILE,//'�û���ò�ļ���
	5, CSIDL_WINDOWS,//'WindowsĿ¼��SYSROOT()
	5, CSIDL_COMMON_ADMINTOOLS,//'�������û���\��ʼ�˵�\����\ϵͳ������
	5, CSIDL_COMMON_ALTSTARTUP,//'δ���ػ���ͨ������
	5, CSIDL_COMMON_APPDATA,//'�������û���\Ӧ�ó�������
	5, CSIDL_COMMON_DESKTOPDIRECTORY,//'�������û���\����
	5, CSIDL_COMMON_DOCUMENTS,//'�������û���\�ĵ�
	5, CSIDL_COMMON_FAVORITES,//'(�����û�)\����
	5, CSIDL_COMMON_PROGRAMS,//'(�����û�)\����
	5, CSIDL_COMMON_STARTMENU,//'(�����û�)\��ʼ�˵�
	5, CSIDL_COMMON_STARTUP,//'(�����û�)\����
	5, CSIDL_COMMON_TEMPLATES, //'(�����û�)\��ʱ
	5
};

std::vector<std::pair<std::string, int>> AltQFileIndexerFast::GetIndexDirs()
{
	std::vector<std::pair<std::string, int>> sys_dirs, registry_dir;
	sys_dirs = GetSystemDefineDirs();
	registry_dir = GetRegistryDirs();
	sys_dirs.insert(sys_dirs.end(), registry_dir.begin(), registry_dir.end());
	return sys_dirs;
}

std::set<std::string> AltQFileIndexerFast::GetIndexExts()
{
	std::set<std::string> ValidExts;
	ValidExts.insert("exe");
	ValidExts.insert("lnk");
	return ValidExts;
}

std::vector<std::pair<std::string, int>> AltQFileIndexerFast::GetSystemDefineDirs()
{
	std::vector<std::pair<std::string, int>> dirs;
	
	int count = sizeof(system_dirs) / sizeof(int) / 2;
	for (int i = 0; i < count; i++)
	{
		std::string dir1 = GetSysDir(CSIDL_CONTROLS);
		std::string dir2 = GetSysDir(CSIDL_PROGRAM_FILESX86);
		std::string dir = GetSysDir(system_dirs[i*2]);
		if (!dir.empty())
		{
			std::pair<std::string, int> pair;
			pair.first = dir;
			pair.second = system_dirs[i * 2 + 1];
			dirs.push_back(pair);
		}
	}
	return dirs;
}

std::vector<std::pair<std::string, int>> AltQFileIndexerFast::GetRegistryDirs()
{
	return std::vector<std::pair<std::string, int>>();
}

std::string AltQFileIndexerFast::GetSysDir(int id)
{
	char DataDir[MAX_PATH];
	memset(DataDir, 0, sizeof(DataDir));
	if (SUCCEEDED(SHGetFolderPath(NULL, id, NULL, 0, DataDir)))
		return DataDir;
	else
		return "";
}

std::vector<std::pair<std::string, int>> AltQFileIndexerWhole::GetIndexDirs()
{
	std::vector<std::pair<std::string, int>> diskpaths;
	DWORD disks = GetLogicalDrives();

	int n = 1;
	for (int i = 1; i <= 26; i++)		// ����ҵ���26���̷��ɣ�����Ĳ�֪����ɶ��ĸ�ˡ�
	{
		if (disks & n)
		{
			std::string path = strFormat("%c:/", 64 + i);
			if (GetDriveTypeA(path.c_str()) == DRIVE_FIXED)
			{
				std::pair<std::string, int> pair;
				pair.first = path.substr(0, path.length() - 1);
				pair.second = 0;
				diskpaths.push_back(pair);
			}
		}

		n = n * 2;
	}

	return diskpaths;
}

std::set<std::string> AltQFileIndexerWhole::GetIndexExts()
{
	std::set<std::string> ValidExts;
	ValidExts.insert("exe");
	ValidExts.insert("lnk");
	ValidExts.insert("iso");
	ValidExts.insert("rar");
	ValidExts.insert("zip");
	ValidExts.insert("avi");
	ValidExts.insert("rm");
	ValidExts.insert("mdf");
	ValidExts.insert("txt");
	ValidExts.insert("bat");
	ValidExts.insert("mid");

	ValidExts.insert("pdf");

	ValidExts.insert("doc");
	ValidExts.insert("docx");
	ValidExts.insert("cvs");
	ValidExts.insert("xls");
	ValidExts.insert("xlsx");
	ValidExts.insert("ppt");
	ValidExts.insert("pptx");

	ValidExts.insert("bmp");
	ValidExts.insert("png");
	ValidExts.insert("jpg");
	ValidExts.insert("jpeg");
	ValidExts.insert("gif");

	ValidExts.insert("cpp");
	ValidExts.insert("c");
	ValidExts.insert("vbs");
	ValidExts.insert("java");
	ValidExts.insert("cs");
	ValidExts.insert("swift");
	ValidExts.insert("mm");
	ValidExts.insert("m");
	ValidExts.insert("h");

	return ValidExts;
}