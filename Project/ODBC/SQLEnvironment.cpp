#include "SQLEnvironment.h"
#include <sqlext.h>

NS_ODBC_BEGIN

void* SQLEnvironment::GetEnvironment() const
{
	return m_henv;
}

SQLEnvironment::SQLEnvironment()
{
	// 环境
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);
	if (!SQL_SUCCEEDED(ret))
	{
		m_henv = SQL_NULL_HENV;
		return;
	}

	// 检查SQL Server版本
	ret = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	if (!SQL_SUCCEEDED(ret))
	{
		SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
		m_henv = SQL_NULL_HENV;
		return;
	}
}

SQLEnvironment::~SQLEnvironment()
{
	if (m_henv != SQL_NULL_HENV)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
		m_henv = SQL_NULL_HENV;
	}
}

NS_ODBC_END
