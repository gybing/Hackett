#include "AltQFileIndexerImpl.h"
#include "str_util.h"
#include <shlobj.h>
#include <shlwapi.h>

int system_dirs[] =
{
	CSIDL_ADMINTOOLS, //'（用户）\开始菜单\程序\系统管理工具
	5, CSIDL_ALTSTARTUP, //'未本地化的启动
	5, CSIDL_APPDATA, //'（用户）\应用程序的数据
	5, CSIDL_BITBUCKET, //'（桌面）\回收站
	5, CSIDL_CONTROLS, //'我的电脑\控制面板
	5, CSIDL_COOKIES, //
	5, CSIDL_DESKTOP, // '桌面
	5, CSIDL_DESKTOPDIRECTORY, // '（用户）\桌面
	5, CSIDL_FAVORITES, // '（用户）\个性化设置
	5, CSIDL_FONTS, //'windows\字体
	5, CSIDL_HISTORY,
	5, CSIDL_INTERNET, //'IE（桌面上的图标
	5, CSIDL_INTERNET_CACHE, //'因特网缓存文件夹
	5, CSIDL_LOCAL_APPDATA, //'（用户）\本地设置\应用程序数据
	5, CSIDL_DRIVES,//'我的电脑
	5, CSIDL_MYPICTURES,//'C:\Program Files\My Pictures
	5, CSIDL_NETHOOD,//'（用户）\网上邻居中的元素
	5, CSIDL_NETWORK,//'网上邻居
	5, CSIDL_PRINTERS,//'我的电脑\打印机
	5, CSIDL_PRINTHOOD,//'（用户）\打印机连接
	5, CSIDL_PERSONAL,//'我的文档
	5, CSIDL_PROGRAM_FILES,//'C:\Program Files
	5, CSIDL_PROGRAM_FILESX86,//'x86 apps (Alpha)的程序文件目录
	5, CSIDL_PROGRAMS,//'开始菜单\程序
	5, CSIDL_PROGRAM_FILES_COMMON,//'Program Files\Common
	5, CSIDL_PROGRAM_FILES_COMMONX86,//'RISC上的x86 \Program Files\Common
	5, CSIDL_RECENT,//'(用户)\最近记录目录
	5, CSIDL_SENDTO,//'(用户)\发送到目录
	5, CSIDL_STARTMENU,//'(用户)\开始菜单
	5, CSIDL_STARTUP,//'开始菜单\程序\启动
	5, CSIDL_SYSTEM,//'system文件夹
	5, CSIDL_SYSTEMX86,//'x86 apps (Alpha)的system文件夹
	5, CSIDL_TEMPLATES,//
	5, CSIDL_PROFILE,//'用户概貌文件夹
	5, CSIDL_WINDOWS,//'Windows目录或SYSROOT()
	5, CSIDL_COMMON_ADMINTOOLS,//'（所有用户）\开始菜单\程序\系统管理工具
	5, CSIDL_COMMON_ALTSTARTUP,//'未本地化的通用启动
	5, CSIDL_COMMON_APPDATA,//'（所有用户）\应用程序数据
	5, CSIDL_COMMON_DESKTOPDIRECTORY,//'（所有用户）\桌面
	5, CSIDL_COMMON_DOCUMENTS,//'（所有用户）\文档
	5, CSIDL_COMMON_FAVORITES,//'(所有用户)\设置
	5, CSIDL_COMMON_PROGRAMS,//'(所有用户)\程序
	5, CSIDL_COMMON_STARTMENU,//'(所有用户)\开始菜单
	5, CSIDL_COMMON_STARTUP,//'(所有用户)\启动
	5, CSIDL_COMMON_TEMPLATES, //'(所有用户)\临时
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
	for (int i = 1; i <= 26; i++)		// 最多找到第26个盘符吧，后面的不知道叫啥字母了。
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