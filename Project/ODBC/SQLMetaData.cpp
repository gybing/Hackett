#include "SQLMetaData.h"
#include <windows.h>
#include <sqlext.h>

#pragma comment(lib, "odbc32.lib")

NS_ODBC_BEGIN

TIMESTAMP_STRUCT GetTimeStamp(__time64_t t)
{
	TIMESTAMP_STRUCT ts;

	struct tm _tm;
	_localtime64_s(&_tm, &t);
	ts.year = _tm.tm_year + 1900;
	ts.month = _tm.tm_mon + 1;
	ts.day = _tm.tm_mday;
	ts.hour = _tm.tm_hour;
	ts.minute = _tm.tm_min;
	ts.second = _tm.tm_sec;
	ts.fraction = 0;

	return ts;
}

__time64_t GetUTCTime(TIMESTAMP_STRUCT& ts)
{
	struct tm _tm;
	_tm.tm_year = ts.year - 1900;
	_tm.tm_mon = ts.month - 1;
	_tm.tm_mday = ts.day;
	_tm.tm_hour = ts.hour;
	_tm.tm_min = ts.minute;
	_tm.tm_sec = ts.second;

	return _mktime64(&_tm);
}

TIMESTAMP_STRUCT GetTimeStamp32(__time32_t t)
{
	TIMESTAMP_STRUCT ts;

	struct tm _tm;
	_localtime32_s(&_tm, &t);
	ts.year = _tm.tm_year + 1900;
	ts.month = _tm.tm_mon + 1;
	ts.day = _tm.tm_mday;
	ts.hour = _tm.tm_hour;
	ts.minute = _tm.tm_min;
	ts.second = _tm.tm_sec;
	ts.fraction = 0;

	return ts;
}

__time32_t GetUTCTime32(TIMESTAMP_STRUCT& ts)
{
	struct tm _tm;
	_tm.tm_year = ts.year - 1900;
	_tm.tm_mon = ts.month - 1;
	_tm.tm_mday = ts.day;
	_tm.tm_hour = ts.hour;
	_tm.tm_min = ts.minute;
	_tm.tm_sec = ts.second;

	return _mktime32(&_tm);
}

SQLMetaData::SQLMetaData(SQLMetaDataType t, void* v)
	: type(t)
	, value(v)
	, status(0)
	, columnsize(0)
{

}

void* SQLMetaData::PrepareInputData(short& ctype, short& stype, unsigned __int64& size, short& scale, __int64& len)
{
	ctype = SQL_UNKNOWN_TYPE;
	stype = SQL_C_DEFAULT;
	size = 0;
	void* data = value;
	len = 0;
	scale = 0;

	switch (type)
	{
	case DataType_Bool:
		ctype = SQL_C_BIT;
		stype = SQL_BIT;
		size = sizeof(bool);
		break;

	case DataType_Byte:
		ctype = SQL_C_TINYINT;
		stype = SQL_TINYINT;
		size = sizeof(char);
		break;

	case DataType_UByte:
		ctype = SQL_C_UTINYINT;
		stype = SQL_TINYINT;
		size = sizeof(unsigned char);
		break;

	case DataType_Word:
		ctype = SQL_C_SSHORT;
		stype = SQL_SMALLINT;
		size = sizeof(short);
		break;

	case DataType_UWord:
		ctype = SQL_C_SHORT;
		stype = SQL_SMALLINT;
		size = sizeof(unsigned short);
		break;

	case DataType_Int:
		ctype = SQL_C_SLONG;
		stype = SQL_INTEGER;
		size = sizeof(int);
		break;

	case DataType_UInt:
		ctype = SQL_C_ULONG;
		stype = SQL_INTEGER;
		size = sizeof(unsigned int);
		break;

	case DataType_Long:
		ctype = SQL_C_SBIGINT;
		stype = SQL_BIGINT;
		size = sizeof(__int64);
		break;

	case DataType_ULong:
		ctype = SQL_C_UBIGINT;
		stype = SQL_BIGINT;
		size = sizeof(unsigned __int64);
		break;

	case DataType_Float:
		ctype = SQL_C_FLOAT;
		stype = SQL_REAL;
		size = sizeof(float);
		break;

	case DataType_Double:
		ctype = SQL_C_DOUBLE;
		stype = SQL_DOUBLE;
		size = sizeof(double);
		break;

	case DataType_Guid:
		ctype = SQL_C_GUID;
		stype = SQL_GUID;
		size = ODBC_GUID_COL_SIZE;
		break;

	case DataType_Datetime:
		data = PrepareDataTime(ctype, stype, size, scale);
		break;

	case DataType_String:
		data = PrepareString(ctype, stype, size, len);
		break;

	case DataType_NString:
		data = PrepareWString(ctype, stype, size, len);
		break;

	case DataType_Binary:
		data = PrepareBinary(ctype, stype, size, len);
		break;

	case DataType_Unknown:
	default:
		break;
	}

	return data;
}


void* SQLMetaData::PrepareOutputData(void* stmt, int index, short& ctype, __int64& len)
{
	ctype = SQL_UNKNOWN_TYPE;
	void* data = value;
	len = 0;

	switch (type)
	{
	case DataType_Bool:
		ctype = SQL_C_BIT;
		break;

	case DataType_Byte:
		ctype = SQL_C_TINYINT;
		break;

	case DataType_UByte:
		ctype = SQL_C_UTINYINT;
		break;

	case DataType_Word:
		ctype = SQL_C_SSHORT;
		break;

	case DataType_UWord:
		ctype = SQL_C_SHORT;
		break;

	case DataType_Int:
		ctype = SQL_C_SLONG;
		break;

	case DataType_UInt:
		ctype = SQL_C_ULONG;
		break;

	case DataType_Long:
		ctype = SQL_C_SBIGINT;
		break;

	case DataType_ULong:
		ctype = SQL_C_UBIGINT;
		break;

	case DataType_Float:
		ctype = SQL_C_FLOAT;
		break;

	case DataType_Double:
		ctype = SQL_C_DOUBLE;
		break;

	case DataType_Guid:
		ctype = SQL_C_GUID;
		break;

	case DataType_Datetime:
		ctype = SQL_C_TIMESTAMP;
		break;

	case DataType_String:
	{
		columnsize = GetColumnSize(stmt, index);

		len = columnsize + 1;
		ctype = SQL_C_CHAR;

		if (!IsStream())
		{
			std::string* ss = static_cast<std::string*>(value);
			ss->reserve(columnsize);
			data = (void*)ss->data();
		}
		else
		{
			// StreamData
			// 清空缓冲区,后期调用SQLGetData取数据
			data = nullptr;
		}
	}
	break;

	case DataType_NString:
	{
		columnsize = GetColumnSize(stmt, index);

		len = (columnsize + 1) * 2;
		ctype = SQL_C_WCHAR;

		if (!IsStream())
		{
			std::wstring* ss = static_cast<std::wstring*>(value);
			ss->reserve(columnsize);
			data = (void*)ss->data();
		}
		else
		{
			// StreamData
			// 清空缓冲区,后期调用SQLGetData取数据
			data = nullptr;
		}
	}
	break;

	case DataType_Binary:
	{
		columnsize = GetColumnSize(stmt, index);

		len = columnsize;
		ctype = SQL_C_BINARY;

		if (!IsStream())
		{
			SQLBLOB* bb = static_cast<SQLBLOB*>(value);
			bb->alloc(columnsize);
			data = bb->data;
		}
		else
		{
			// StreamData
			// 清空缓冲区,后期调用SQLGetData取数据
			data = nullptr;
		}
	}
	break;

	case DataType_Unknown:
	default:
		break;
	}

	return data;
}

void ODBC::SQLMetaData::BeforeFetchData()
{
	if (IsStream())
	{
		return;
	}

	switch (type)
	{
	case DataType_String:
	{
		std::string* ss = static_cast<std::string*>(value);
		ss->resize(columnsize);
		break;
	}

	case DataType_NString:
	{
		std::wstring* ss = static_cast<std::wstring*>(value);
		ss->resize(columnsize);
		break;
	}

// 	case DataType_Binary:

	default:
		break;
	}
}

bool SQLMetaData::AfterFetchData(void* stmt, unsigned short index)
{
	switch (type)
	{
	case DataType_String:
	{
		std::string* ss = static_cast<std::string*>(value);

		if (IsStream())
		{
			return GetStreamData(stmt, index, *ss);
		}
		
		ss->resize(strnlen_s(ss->data(), ss->capacity()));
		break;
	}

	case DataType_NString:
	{
		std::wstring* ss = static_cast<std::wstring*>(value);

		if (IsStream())
		{
			return GetStreamData(stmt, index, *ss);
		}

		ss->resize(wcsnlen_s(ss->c_str(), ss->capacity()));
		break;
	}

	case DataType_Binary:
		return GetStreamData(stmt, index, *static_cast<SQLBLOB*>(value));

	default:
		break;
	}

	return true;
}

void* SQLMetaData::PrepareBinary(short& ctype, short& stype, unsigned __int64& size, __int64& len)
{
	ctype = SQL_C_BINARY;
	stype = SQL_VARBINARY;

	const SQLBLOB* ss = static_cast<const SQLBLOB*>(value);
	size = ss->size;
	len = size;
	status = size;

	if (size > ODBC_MAX_COL_SIZE)
	{
		stype = SQL_LONGVARBINARY;
	}
	else if (size == 0) size = 1;

	return (void*)ss->data;
}

void* SQLMetaData::PrepareString(short& ctype, short& stype, unsigned __int64& size, __int64& len)
{
	status = SQL_NTS;
	ctype = SQL_C_CHAR;
	stype = SQL_VARCHAR;

	const std::string* ss = static_cast<const std::string*>(value);
	size = ss->size();
	if (size > ODBC_MAX_COL_SIZE)
	{
		stype = SQL_LONGVARCHAR;
	}
	else if (size == 0) size = 1;

	return (void*)ss->c_str();
}

void* SQLMetaData::PrepareWString(short& ctype, short& stype, unsigned __int64& size, __int64& len)
{
	status = SQL_NTS;
	ctype = SQL_C_WCHAR;
	stype = SQL_WVARCHAR;

	const std::wstring* ss = static_cast<const std::wstring*>(value);
	size = ss->size();
	if (size > ODBC_MAX_COL_SIZE)
	{
		stype = SQL_WLONGVARCHAR;
	}
	else if (size == 0) size = 1;

	return (void*)ss->c_str();
}

__int64 SQLMetaData::GetColumnSize(void* stmt, unsigned short col)
{
	SQLULEN colSize;

	SQLRETURN ret = SQLDescribeCol(stmt, col, nullptr, 0, nullptr, nullptr, &colSize, nullptr, nullptr);
	if (!SQL_SUCCEEDED(ret))
	{
		return -1;
	}

	return colSize;
}

bool ODBC::SQLMetaData::IsStream() const
{
	return columnsize > ODBC_MAX_COL_SIZE;
}

bool SQLMetaData::GetStreamData(void* stmt, unsigned short index, std::string &s)
{
	s.clear();
	SQLRETURN rc;
	__int64 len = ODBC_STREAM_FETCH_SIZE;

	while (true)
	{
		status = 0;
		s.append(len, 0);

		rc = SQLGetData(stmt, index, (SQLSMALLINT)SQL_C_CHAR, (char*)s.data() + s.size() - len, (SQLLEN)len + 1, &status);
		if (rc == SQL_SUCCESS)
		{
			s.erase(s.size() - (len - status));
			return true;
		}
		else if (rc == SQL_SUCCESS_WITH_INFO && status > 0)
		{
			len = status - len;
			continue;
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool ODBC::SQLMetaData::GetStreamData(void* stmt, unsigned short index, SQLBLOB &s)
{
	s.clear();
	SQLRETURN rc;
	__int64 len = ODBC_STREAM_FETCH_SIZE;

	while (true)
	{
		status = 0;
		s.calloc(len);

		rc = SQLGetData(stmt, index, (SQLSMALLINT)SQL_C_BINARY, (char*)s.data + s.size - len, (SQLLEN)len, &status);
		if (rc == SQL_SUCCESS)
		{
			s.calloc(-(len - status));
			return true;
		}
		else if (rc == SQL_SUCCESS_WITH_INFO && status > 0)
		{
			len = status - len;
			continue;
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool ODBC::SQLMetaData::GetStreamData(void* stmt, unsigned short index, std::wstring &s)
{
	s.clear();
	SQLRETURN rc;
	__int64 len = ODBC_STREAM_FETCH_SIZE;

	while (true)
	{
		status = 0;
		s.append(len, 0);

		rc = SQLGetData(stmt, index, (SQLSMALLINT)SQL_C_WCHAR, (wchar_t*)s.data() + s.size() - len, (SQLLEN)((len + 1) * 2), &status);
		if (rc == SQL_SUCCESS)
		{
			s.erase(s.size() - (len - (status / 2)));
			return true;
		}
		else if (rc == SQL_SUCCESS_WITH_INFO && status > 0)
		{
			len = (status / 2) - len;
			continue;
		}
		else
		{
			return false;
		}
	}

	return true;
}

void* SQLMetaData::PrepareDataTime(short& ctype, short& stype, unsigned __int64& size, short& scale)
{
	ctype = SQL_C_TIMESTAMP;
	stype = SQL_TIMESTAMP;

	if (SQL_TIMESTAMP_STRUCT* ts = (SQL_TIMESTAMP_STRUCT*)value)
	{
		if (ts->fraction > 0)
		{
			ts->fraction /= 1000000;
			if (ts->fraction == 0)
			{
				scale = 19;
			}
			else if (ts->fraction >= 1 && ts->fraction <= 9)
			{
				size = 21;
				scale = 1;
			}
			else if (ts->fraction >= 10 && ts->fraction <= 99)
			{
				size = 22;
				scale = 2;
			}
			else if (ts->fraction >= 100 && ts->fraction <= 999)
			{
				size = 23;
				scale = 3;
			}

			ts->fraction *= 1000000;
		}
		else if (ts->second > 0)
		{
			size = 19;
		}
		else
		{
			size = 16;
		}
	}

	return value;
}

NS_ODBC_END