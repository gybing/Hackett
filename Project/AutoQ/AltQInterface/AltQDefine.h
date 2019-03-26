#pragma once
#include <string>
#include <vector>
#include <memory>

#ifdef ALTQLIB
#define ALTQ__API __declspec(dllexport)
#else
#define ALTQ__API __declspec(dllimport)
#endif

// ������ʶģ���ΨһID������Ԥ���ɵ�GUID����
// ȷ����Ҫ���������汾���º���������⡣
typedef std::string AltQModuleID;		

// ģ������/����
typedef std::string AltQModuleName;
typedef std::string AltQModuleDescription;

// �����ĵ�ID�� ����Ψһ��ʶ�����Ļ�����������ĵ�
typedef std::string AltQDocID;			

// ������������������ݣ��ɾ���ҵ��ģ�鸺�����������
typedef std::string AltQContent;

// �����������
typedef float AltQScore;

// �����ĵ�
struct AltQDocument
{
	std::string m_ToIndex;			// ��Ҫ�������Ĵ�
	AltQContent m_Content;		// ���ݺ����AltQCore͸������ҵ��ģ�����н���
	std::string m_UserData;			// �ں˻�Դ��ֶε���������ҵ��ģ�������Դ��ֶζ���������в�����
};
typedef std::vector<AltQDocument> AltQDocuments;

// �������
class AltQResult
{
public:
	AltQDocID m_DocID;				// �����Ӧ�������ĵ�ID
	AltQModuleID m_ModuleID;		// �����Ӧ��ҵ��ģ��ID
	AltQContent m_Content;			// ������ݣ��ɾ���ҵ��ģ����н���
	AltQScore m_Score;					// �������

	bool operator<(const AltQResult& result)
	{
		return m_Score < result.m_Score;
	}
};
typedef std::shared_ptr<AltQResult> AltQResultPtr;
typedef std::vector<AltQResultPtr> AltQResults;