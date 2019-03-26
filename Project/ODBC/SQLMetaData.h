#ifndef __SQL_ODBC_H_METADATA__
#define __SQL_ODBC_H_METADATA__
#include "SQLDefine.h"

NS_ODBC_BEGIN

struct SQLMetaData
{
	SQLMetaDataType type;
	void* value;
	__int64 status;

	// ���췽��
	SQLMetaData(SQLMetaDataType t, void* v);

	// ׼���������
	void* PrepareInputData(short& ctype, short& stype, unsigned __int64& size, short& scale, __int64& len);

	// ׼���������
	void* PrepareOutputData(void* stmt, int index, short& ctype, __int64& len);

	// ׼�����ؽ��
	void BeforeFetchData();
	bool AfterFetchData(void* stmt, unsigned short index);

private:
	__int64 columnsize;

	void* PrepareDataTime(short& ctype, short& stype, unsigned __int64& size, short& scale);
	void* PrepareBinary(short& ctype, short& stype, unsigned __int64& size, __int64& len);
	void* PrepareString(short& ctype, short& stype, unsigned __int64& size, __int64& len);
	void* PrepareWString(short& ctype, short& stype, unsigned __int64& size, __int64& len);

	__int64 GetColumnSize(void* stmt, unsigned short col);

	bool IsStream() const;
	bool GetStreamData(void* stmt, unsigned short index, std::string &s);
	bool GetStreamData(void* stmt, unsigned short index, std::wstring &s);
	bool GetStreamData(void* stmt, unsigned short index, SQLBLOB &s);
};

// �������
struct SQLInputData : public std::vector<SQLMetaData>
{
	typedef std::unique_ptr<SQLInputData> Ptr;

	template<typename T>
	void USE(T* val)
	{
		push_back(SQLMetaData(GetMetaType(typeid(T).name()), (void*)val));
	}

	template<typename T, typename... Args>
	void USE(T* val, Args... args)
	{
		USE(val);
		USE(args...);
	}

	static SQLInputData* Create() { return new SQLInputData; }

private:
	SQLInputData() {}
};

// �������
struct SQLOutputData : public std::vector<SQLMetaData>
{
	typedef std::unique_ptr<SQLOutputData> Ptr;

	template<typename T>
	void INTO(T* val)
	{
		push_back(SQLMetaData(GetMetaType(typeid(T).name()), (void*)val));
	}

	template<typename T, typename... Args>
	void INTO(T* val, Args... args)
	{
		INTO(val);
		INTO(args...);
	}

	static SQLOutputData* Create() { return new SQLOutputData; }

private:
	SQLOutputData() {}
};

// �������ģ��
// �÷�: USE(&v1, &v2, &v3...)

template<typename... Args>
SQLInputData* USE(Args... args)
{
	SQLInputData* in(SQLInputData::Create());
	in->USE(args...);
	return in;
}

// �������ģ�巽��
// �÷�: INTO(&v1, &v2, &v3...)

template<typename... Args>
SQLOutputData* INTO(Args... args)
{
	SQLOutputData* out(SQLOutputData::Create());
	out->INTO(args...);
	return out;
}

NS_ODBC_END

#endif __SQL_ODBC_H_METADATA__