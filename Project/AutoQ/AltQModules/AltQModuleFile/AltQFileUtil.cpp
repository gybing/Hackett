#include "altqfileutil.h"
#include "str_util.h"
#include "iconv/CharEncode.h"

void AltQFileToJson(const AltQFile& file, std::string& jsonstring)
{
	Json::FastWriter jw;
	Json::Value jv(Json::objectValue);

	jv["name"] = file.m_FileName;
	jv["ext"] = file.m_FileExt;
	jv["path"] = file.m_FilePath;
	jsonstring = jw.write(jv);
}

bool JsonToAltQFile(const std::string& jsonstring, AltQFile& file)
{
	if (jsonstring.empty())
		return false;

	Json::Reader jr;
	Json::Value jv;
	if (!jr.parse(jsonstring, jv) || !jv.isObject())
		return false;

	const char* name = jv["name"].asCString();
	const char* ext = jv["ext"].asCString();
	const char* path = jv["path"].asCString();

	file.m_FileName = GBKToUTF8(name, strlen(name));
	file.m_FileExt = GBKToUTF8(ext, strlen(ext));
	file.m_FilePath = GBKToUTF8(path, strlen(path));
	return true;
}

void SubstractFileName(const std::string& file, std::string& filename, std::string& fileext)
{
	std::string::size_type pos = file.find_last_of('.');
	if (pos == std::string::npos)
	{
		filename = file;
		fileext.clear();
	}
	else
	{
		filename = file.substr(0, pos);
		fileext = file.substr(pos + 1);
	}
}