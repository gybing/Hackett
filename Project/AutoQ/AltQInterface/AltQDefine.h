#pragma once
#include <string>
#include <vector>
#include <memory>

#ifdef ALTQLIB
#define ALTQ__API __declspec(dllexport)
#else
#define ALTQ__API __declspec(dllimport)
#endif

// 用来标识模块的唯一ID，可用预生成的GUID代表，
// 确定后不要变更，否则版本更新后可能有问题。
typedef std::string AltQModuleID;		

// 模块名称/描述
typedef std::string AltQModuleName;
typedef std::string AltQModuleDescription;

// 索引文档ID， 用来唯一标识新增的或搜索结果的文档
typedef std::string AltQDocID;			

// 索引或搜索结果的内容，由具体业务模块负责生成与解析
typedef std::string AltQContent;

// 搜索结果评分
typedef float AltQScore;

// 索引文档
struct AltQDocument
{
	std::string m_ToIndex;			// 需要被索引的串
	AltQContent m_Content;		// 内容含义对AltQCore透明，由业务模块自行解析
	std::string m_UserData;			// 内核会对此字段单独索引，业务模块可以针对此字段对索引库进行操作。
};
typedef std::vector<AltQDocument> AltQDocuments;

// 检索结果
class AltQResult
{
public:
	AltQDocID m_DocID;				// 结果对应的索引文档ID
	AltQModuleID m_ModuleID;		// 结果对应的业务模块ID
	AltQContent m_Content;			// 结果内容，由具体业务模块进行解析
	AltQScore m_Score;					// 结果评分

	bool operator<(const AltQResult& result)
	{
		return m_Score < result.m_Score;
	}
};
typedef std::shared_ptr<AltQResult> AltQResultPtr;
typedef std::vector<AltQResultPtr> AltQResults;