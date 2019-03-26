#include "AltQModuleFile.h"
#include "AltQFileUtil.h"
#include "iconv/CharEncode.h"
#include "str_util.h"
#include "AltQFileIndexerImpl.h"
#include "AltQFileDB.h"
#include <iostream>

static std::string g_module_id;
const char* g_module_name = "AltQModuleFile";

const int g_WholeIndexInterval = 3600;		// 暂定1小时一次完整性检查
const int g_FastIndexInterval = 300;				// 5分钟

AltQModuleFilePtr g_module;
ALTQ__API bool CreateAltQModule(IAltQModulePtr* module)
{
	if (g_module)
	{
		*module = g_module;
		return true;
	}

	g_module = std::make_shared<AltQModuleFile>();
	*module = g_module;
	return true;
}

AltQModuleFile::AltQModuleFile()
{
	m_Inited = false;
}

bool AltQModuleFile::Initialize(const std::string& key, const std::string& storage_path, const IAltQCoreInternalPtr& core)
{
    g_module_id = key;

	if (m_Inited)
	{
		return true;
	}

	m_StoragePath = storage_path;
	m_AltQCoreInternal = core;

    m_AltQFTSIndexer = m_AltQCoreInternal->CreateModuleIndexer(g_module_name, g_module_id);
    if (!SafeMkdir(m_StoragePath.c_str()))
        return false;

    if (!AltQFileDB::Instance()->Initialize((m_StoragePath + "/directories").c_str()))
        return 0;

	if (!Start())
		return false;

	m_Inited = true;
	return true;
}

void AltQModuleFile::Finalize()
{
	if (!m_Inited)
	{
		return;
	}

	m_ThreadEvent.Set();
	AltQFileIndexerFast::Instance()->Stop();
	AltQFileIndexerWhole::Instance()->Stop();
	WaitThreadExit();
	m_Inited = false;
}

IAltQModuleResultPtr AltQModuleFile::ParseResult(const AltQResultPtr& result)
{
	if (result->m_ModuleID != g_module_id)
	{
		return NULL;
	}

    AltQFileResultPtr file = std::make_shared<AltQFileResult>(result->m_DocID);
	if (!JsonToAltQFile(result->m_Content, file->m_File))
		return NULL;

	// 检查文件是否还可访问，如果不再存在，则返回空
	std::string filepath = file->m_File.m_FilePath;
	pathcat(filepath, file->m_File.m_FileName.c_str());
	filepath += ".";
	filepath += file->m_File.m_FileExt;

	if (!fexist(UTF8ToGBK(filepath.c_str(), filepath.length()).c_str()))
	{
		m_AltQFTSIndexer->DeleteDocByID(result->m_DocID);
		return NULL;
	}

	return file;
}

void AltQModuleFile::OnResultSelected(const AltQDocID& id)
{
	m_AltQFTSIndexer->IncreaseScore(id);
}

int AltQModuleFile::Run()
{
	int64_t time_last_whole = 0, time_last_fast = 0;
	do
	{
		// 快速索引
		if (time(NULL) - time_last_fast > g_FastIndexInterval)
		{
			AltQFileIndexerFast::Instance()->ExecUtilFinish(m_AltQFTSIndexer);
			time_last_fast = time(NULL);

			std::cout << "fast mode, file indexed:" << AltQFileIndexerFast::Instance()->FileIndexed() << std::endl;
			std::cout << "fast mode, dir indexed:" << AltQFileIndexerFast::Instance()->DirIndexed() << std::endl;
			AltQFileIndexerFast::Instance()->Destory();
		}

		// 完整索引
		if (time(NULL) - time_last_whole > g_WholeIndexInterval)
		{
			AltQFileIndexerWhole::Instance()->ExecUtilFinish(m_AltQFTSIndexer);
			time_last_whole = time(NULL);

			std::cout << "whole mode, file indexed:" << AltQFileIndexerWhole::Instance()->FileIndexed() << std::endl;
			std::cout << "whole mode, dir indexed:" << AltQFileIndexerWhole::Instance()->DirIndexed() << std::endl;
			AltQFileIndexerWhole::Instance()->Destory();
		}
	} while (!m_ThreadEvent.Wait(1000));

	AltQFileDB::Instance()->Finalize();
	AltQFileDB::Instance()->Destory();
	return 0;
}