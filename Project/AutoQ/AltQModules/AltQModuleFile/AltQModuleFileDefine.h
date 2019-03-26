#pragma once
#include "AltQInterfaceModule.h"

struct AltQFileTime
{
	std::string m_dir;
	int64_t m_time;
};


struct AltQFile
{
	std::string m_FileName;
	std::string m_FilePath;
	std::string m_FileExt;		// 扩展名, 文件类型
};

class AltQFileResult : public IAltQModuleResult
{
public:
    AltQFileResult(const AltQDocID& doc) : m_DocID(doc) {}

	virtual std::string GetTitle()
	{
		return m_File.m_FileName + "." + m_File.m_FileExt;
	}

	virtual std::string GetDescription()
	{
		return m_File.m_FilePath;
	}

	virtual std::string GetIcon()
	{
		return m_File.m_FilePath + "/" + m_File.m_FileName + "." + m_File.m_FileExt;
	}

    virtual AltQDocID GetDocID()
    {
        return m_DocID;
    }

    AltQDocID m_DocID;
	AltQFile m_File;
};
typedef std::shared_ptr<AltQFileResult> AltQFileResultPtr;