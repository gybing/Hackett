#pragma once
#include "AltQInterfaceCore.h"
#include "base_thread.h"
#include "AltQModuleFileDefine.h"
#include <set>
#include "Singletons.h"

// 完成所有目录内的常见后缀文件
class AltQFileDB;
class AltQFileIndexer : public BaseThread
{
public:
	// 开始执行任务直到完成
	void ExecUtilFinish(const IAltQIndexerPtr& indexer);

	// 停止直到
	void Stop();

	int FileIndexed(){ return m_FileCount;  }
	int DirIndexed(){ return m_DirCount; }

protected:
	virtual std::vector<std::pair<std::string,int>> GetIndexDirs() = 0;
	virtual std::set<std::string> GetIndexExts() = 0;
	virtual bool IsTempIndex(){ return false; }
	virtual int IndexInterval(){ return 0; }

private:
	int Run();

	// 如果deep_max <= 0 代表无限深度
	bool RecursionDirectories(const char* path, int64_t tmModify, int deep_current = 0, int deep_max = 0);
	bool IsValidExt(const std::string& m_FileExt);
	
private:
	AltQFileDB* m_pDB;
	ThreadEvent m_EventExit;
	IAltQIndexerPtr m_Indexer;

	friend class Singletons<AltQFileIndexer>;
	std::set<std::string> m_ValidExts;

	int m_FileCount;
	int m_DirCount;
};
typedef std::shared_ptr<AltQFileIndexer> AltQFileIndexerWholePtr;