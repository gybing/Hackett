#include "SQLStatement.h"
#include "SQLConnection.h"
#include <sqlext.h>

NS_ODBC_BEGIN

SQLStatement::SQLStatement(SQLConnection* con, LoggerInterface* log)
	: SQLErrorState(log)
	, m_conn(con)
	, m_state(false)
{
	SetLogTag("SQLStatement");

	if (!AllocStmt(m_conn->m_hdbc))
	{
		Log(lld_error, "Statement AllocStmt failed");
		return;
	}

	//关闭查询超时
	if (!SetStmtOption(SQL_ATTR_QUERY_TIMEOUT, 0))
	{
		CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
		Log(lld_error, "Statement SetStmtOption SQL_ATTR_QUERY_TIMEOUT failed");
	}
}

SQLStatement::~SQLStatement()
{
	if (m_hstmt)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
		m_hstmt = SQL_NULL_HSTMT;
	}
}

bool SQLStatement::ExecuteDirect(const std::string& sql, SQLInputData* in /*= nullptr*/, SQLOutputData* out /*= nullptr*/)
{
	m_inputs.reset(in);
	m_outputs.reset(out);

	if (!BindParam())
	{
		return false;
	}

	// 执行
	SQLRETURN ret = SQLExecDirect(m_hstmt, (SQLCHAR*)sql.c_str(), sql.length());
	if (!SQL_SUCCEEDED(ret))
	{
		Log(lld_error, "ExecuteDirect SQLExecDirect Failed ret: %d", ret);
		CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
		return false;
	}

	return BindColumn();
}

bool SQLStatement::Fetch()
{
	if (m_outputs)
	{
		for (auto& o : *m_outputs)
		{
			o.BeforeFetchData();
		}
	}

	SQLRETURN ret = SQLFetch(m_hstmt);
	if (!SQL_SUCCEEDED(ret))
	{
		if (ret == SQL_ERROR)
		{
			Log(lld_error, "Fetch SQLFetch Failed ret: %d", ret);
			CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
		}

		return false;
	}

	if (m_outputs)
	{
		for (unsigned short i = 0; i < m_outputs->size(); ++i)
		{
			if (!(*m_outputs)[i].AfterFetchData(m_hstmt, i + 1))
			{
				Log(lld_error, "Fetch ReviseData Failed col: %d", i);
				CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
				return false;
			}
		}
	}

	return true;
}

bool SQLStatement::Prepare(const std::string& sql, SQLInputData* in /*= nullptr*/, SQLOutputData* out /*= nullptr*/)
{
	SQLRETURN ret = SQLPrepare(m_hstmt, (SQLCHAR*)sql.c_str(), sql.length());
	bool result = SQL_SUCCEEDED(ret);
	if (!result)
	{
		Log(lld_error, "Prepare SQLPrepare Failed ret: %d", ret);
		CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
		return false;
	}

	m_inputs.reset(in);
	m_outputs.reset(out);

	return true;
}

bool SQLStatement::Execute()
{
	if (!BindParam())
	{
		return false;
	}

	// 执行
	SQLRETURN ret = SQLExecute(m_hstmt);
	if (!SQL_SUCCEEDED(ret))
	{
		Log(lld_error, "Execute SQLExecDirect Failed ret: %d", ret);
		CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
		return false;
	}

	return BindColumn();
}

bool SQLStatement::BindParam()
{
	if (m_state)
	{
		SQLCloseCursor(m_hstmt);
		m_state = false;
	}

	// 绑定参数
	if (m_inputs)
	{
		SQLRETURN ret;
		SQLUSMALLINT index = 0;
		SQLSMALLINT stype, ctype;
		SQLULEN size;
		SQLSMALLINT scale;
		SQLPOINTER data;
		SQLLEN len;

		for (auto& v : *m_inputs)
		{
			++index;

			data = v.PrepareInputData(ctype, stype, size, scale, len);

			ret = SQLBindParameter(m_hstmt,
				index,
				SQL_PARAM_INPUT,
				ctype,
				stype,
				size,
				scale,
				data,
				len,
				&v.status);

			if (!SQL_SUCCEEDED(ret))
			{
				Log(lld_error, "BindParameter Failed idx:%d ret: %d", index, ret);
				CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
				return false;
			}
		}
	}

	return true;
}

bool SQLStatement::BindColumn()
{
	// 绑定结果
	if (m_outputs)
	{
		SQLRETURN ret;
		SQLUSMALLINT index = 0;

		SQLSMALLINT ctype;
		SQLPOINTER data;
		SQLLEN len;

		for (auto& v : *m_outputs)
		{
			++index;

			if (data = v.PrepareOutputData(m_hstmt, index, ctype, len))
			{
				ret = SQLBindCol(m_hstmt,
					index,
					ctype,
					data,
					len,
					&v.status);
			}

			if (!SQL_SUCCEEDED(ret))
			{
				Log(lld_error, "BindParameter Failed idx:%d ret: %d", index, ret);
				CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
				return false;
			}
		}
	}

	m_state = true;
	return true;
}

bool SQLStatement::GetStmtOption(int opt, unsigned int& value)
{
	SQLINTEGER dummy;
	SQLRETURN ret = SQLGetStmtAttr(m_hstmt, opt, (SQLPOINTER)&value, sizeof(value), &dummy);
	if (!SQL_SUCCEEDED(ret))
	{
		Log(lld_error, "SetStmtOption SQLGetStmtAttr Failed ret: %d", ret);
		CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
		return false;
	}
	return true;
}

bool SQLStatement::GetStmtOption(int opt, std::string& value)
{
	value.resize(255, 0);
	SQLINTEGER optSize;
	SQLRETURN ret = SQLGetStmtAttr(m_hstmt, opt, (SQLPOINTER)value.data(), value.size(), &optSize);
	if (!SQL_SUCCEEDED(ret))
	{
		Log(lld_error, "SetStmtOption SQLGetStmtAttr Failed ret: %d", ret);
		CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
		return false;
	}

	if (optSize > 255) {
		value.resize(255 + optSize, 0);
		ret = SQLGetStmtAttr(m_hstmt, opt, (SQLPOINTER)(value.data() + 255), value.size() - 255, &optSize);
		if (!SQL_SUCCEEDED(ret))
		{
			Log(lld_error, "SetStmtOption SQLGetStmtAttr Failed ret: %d", ret);
			CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
			return false;
		}
	}
	return true;
}

bool SQLStatement::SetStmtOption(int opt, unsigned int value)
{
	SQLRETURN ret = SQLSetStmtAttr(m_hstmt, opt, (SQLPOINTER)value, SQL_IS_UINTEGER);
	if (!SQL_SUCCEEDED(ret))
	{
		Log(lld_error, "SetStmtOption SQLSetStmtAttr Failed ret: %d", ret);
		CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
		return false;
	}
	return true;
}

bool SQLStatement::SetStmtOption(int opt, const std::string& value)
{
	SQLRETURN ret = SQLSetStmtAttr(m_hstmt, opt, (SQLPOINTER)value.c_str(), value.length());
	if (!SQL_SUCCEEDED(ret))
	{
		Log(lld_error, "SetStmtOption SQLSetStmtAttr Failed ret: %d", ret);
		CheckErrorState(SQL_HANDLE_STMT, m_hstmt);
		return false;
	}
	return true;
}

bool SQLStatement::AllocStmt(void* hdbc)
{
	m_hstmt = SQL_NULL_HSTMT;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &m_hstmt);
	if (!SQL_SUCCEEDED(ret))
	{
		Log(lld_error, "AllocStmt SQLAllocHandle Failed ret: %d", ret);
		CheckErrorState(SQL_HANDLE_DBC, hdbc);
		return false;
	}

	return true;
}

NS_ODBC_END