#include "SQLConnection.h"
#include <sqlext.h>

NS_ODBC_BEGIN

//登录超时
const unsigned int CONST_ODBC_TIMEOUT_LOGIN = 5;

//连接超时
const unsigned int CONST_ODBC_TIMEOUT_CONNECT = 5;

// ODBC环境定义
SQLEnvironment SQLConnection::m_env;


SQLConnection::SQLConnection(const std::string& con, LoggerInterface* log /*= nullptr*/)
	: SQLErrorState(log)
	, m_conn(con)
{
	SetLogTag("SQLConnection");

	if (!AllocConnect())
	{
		Log(lld_error, "Connection AllocConnect failed");
		return;
	}

	if (!Connect())
	{
		Log(lld_error, "Connection Connect failed conn:%s", m_conn.c_str());
		return;
	}
}

SQLConnection::~SQLConnection()
{
	if (m_hdbc)
	{
		//断开连接
		SQLDisconnect(m_hdbc);
		SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
		m_hdbc = SQL_NULL_HDBC;
	}
}

SQLStatement::Ptr SQLConnection::Prepare(const std::string& sql, SQLInputData* in /*= nullptr*/, SQLOutputData* out /*= nullptr*/)
{
	SQLStatement::Ptr stmt(new SQLStatement(this, GetLogger()));
	if (stmt->Prepare(sql, in, out))
	{
		return stmt;
	}
	return nullptr;
}

bool SQLConnection::Connect()
{
	SQLCHAR outconn[1024];
	SQLSMALLINT outconnlen;
	SQLRETURN ret;

	// 连接数据库, 尝试3次
	int retryCount = 3;
	while (retryCount--)
	{
		ret = SQLDriverConnect(m_hdbc, NULL, (SQLCHAR*)m_conn.c_str(), (SQLSMALLINT)m_conn.size(),
			outconn, (SQLSMALLINT)sizeof(outconn), &outconnlen, SQL_DRIVER_NOPROMPT);
		if (SQL_SUCCEEDED(ret))
		{
			// 设置自动提交模式
			if (!SetAutoCommit(true))
			{
				return false;
			}

			return true;
		}
	}

	CheckErrorState(SQL_HANDLE_DBC, m_hdbc);
	Log(lld_error, "Connect failed after %d retry: %d", retryCount, ret);
	return false;
}

SQLStatement::Ptr SQLConnection::Execute(const std::string& sql, SQLInputData* in /*= nullptr*/, SQLOutputData* out /*= nullptr*/)
{
	SQLStatement::Ptr stmt(new SQLStatement(this, GetLogger()));
	if (stmt->ExecuteDirect(sql, in, out))
	{
		return stmt;
	}

	return nullptr;
}

bool ODBC::SQLConnection::Available()
{
	try
	{
		int data = 0;
		SQLStatement stmt(this, nullptr);
		if (!stmt.ExecuteDirect("select 1", nullptr, INTO(&data)) || !stmt.Fetch())
		{
			return false;
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

SQLTransaction::Ptr SQLConnection::BeginTrans()
{
	return std::make_unique<SQLTransaction>(*this);
}

bool SQLConnection::SetAutoCommit(bool isauto)
{
	return SetConnOption(SQL_ATTR_AUTOCOMMIT, isauto ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF);
}

bool SQLConnection::GetAutoCommit()
{
	unsigned int value;
	return GetConnOption(SQL_ATTR_AUTOCOMMIT, value) && value == SQL_AUTOCOMMIT_ON;
}

bool SQLConnection::AllocConnect()
{
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_DBC, m_env.GetEnvironment(), &m_hdbc);
	if (!SQL_SUCCEEDED(ret))
	{
		Log(lld_error, "AllocConnect SQLAllocHandle Failed");
		CheckErrorState(SQL_HANDLE_ENV, m_env.GetEnvironment());
		return false;
	}

	//设置登录超时
	if (!SetConnOption(SQL_ATTR_LOGIN_TIMEOUT, CONST_ODBC_TIMEOUT_LOGIN))
	{
		Log(lld_error, "AllocConnect set SQL_ATTR_LOGIN_TIMEOUT Failed");
	}

	//设置连接超时
	if (!SetConnOption(SQL_ATTR_CONNECTION_TIMEOUT, CONST_ODBC_TIMEOUT_CONNECT))
	{
		Log(lld_error, "AllocConnect set SQL_ATTR_CONNECTION_TIMEOUT Failed");
	}

	return true;
}

bool SQLConnection::GetConnOption(int opt, unsigned int& value)
{
	SQLINTEGER dummy;
	SQLRETURN ret = SQLGetConnectAttr(m_hdbc, opt, (SQLPOINTER)&value, sizeof(value), &dummy);
	if (!SQL_SUCCEEDED(ret))
	{
		Log(lld_error, "GetSQLOption SQLGetConnectAttr Failed ret: %d", ret);
		CheckErrorState(SQL_HANDLE_DBC, m_hdbc);
		return false;
	}
	return true;
}

bool SQLConnection::GetConnOption(int opt, std::string& value)
{
	value.resize(255, 0);
	SQLINTEGER optSize;
	SQLRETURN ret = SQLGetConnectAttr(m_hdbc, opt, (SQLPOINTER)value.data(), value.size(), &optSize);
	if (!SQL_SUCCEEDED(ret))
	{
		Log(lld_error, "GetSQLOption SQLGetConnectAttr Failed ret: %d", ret);
		CheckErrorState(SQL_HANDLE_DBC, m_hdbc);
		return false;
	}

	if (optSize > 255) {
		value.resize(255 + optSize, 0);
		ret = SQLGetConnectAttr(m_hdbc, opt, (SQLPOINTER)(value.data() + 255), value.size() - 255, &optSize);
		if (!SQL_SUCCEEDED(ret))
		{
			Log(lld_error, "GetSQLOption SQLGetConnectAttr Failed ret: %d", ret);
			CheckErrorState(SQL_HANDLE_DBC, m_hdbc);
			return false;
		}
	}
	return true;
}

bool SQLConnection::SetConnOption(int opt, unsigned int value)
{
	SQLRETURN ret = SQLSetConnectAttr(m_hdbc, opt, (SQLPOINTER)value, SQL_IS_UINTEGER);
	if (!SQL_SUCCEEDED(ret))
	{
		Log(lld_error, "SetSQLOption SQLSetConnectAttr Failed ret: %d", ret);
		CheckErrorState(SQL_HANDLE_DBC, m_hdbc);
		return false;
	}
	return true;
}

bool SQLConnection::SetConnOption(int opt, const std::string& value)
{
	SQLRETURN ret = SQLSetConnectAttr(m_hdbc, opt, (SQLPOINTER)value.c_str(), value.length());
	if (!SQL_SUCCEEDED(ret))
	{
		Log(lld_error, "SetSQLOption SQLSetConnectAttr Failed ret: %d", ret);
		CheckErrorState(SQL_HANDLE_DBC, m_hdbc);
		return false;
	}
	return true;
}

NS_ODBC_END