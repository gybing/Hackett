#include "SQLTransaction.h"
#include "SQLConnection.h"
#include <sqlext.h>

NS_ODBC_BEGIN

SQLTransaction::SQLTransaction(SQLConnection& c, LoggerInterface* log /*= nullptr*/)
	: SQLErrorState(log)
	, m_conn(c)
	, m_iscommit(false)
{
	SetLogTag("SQLTransaction");

	m_conn.SetAutoCommit(false);
}

SQLTransaction::~SQLTransaction()
{
	if (!m_iscommit)
	{
		RollBack();
	}
	m_conn.SetAutoCommit(true);
}

void SQLTransaction::Commit()
{
	SQLRETURN ret = SQLEndTran(SQL_HANDLE_DBC, m_conn.m_hdbc, SQL_COMMIT);
	if (!SQL_SUCCEEDED(ret))
	{
		m_conn.Log(lld_error, "EndTrans SQLEndTran Failed ret: %d", ret);
		m_conn.CheckErrorState(SQL_HANDLE_DBC, m_conn.m_hdbc);
	}
	m_iscommit = true;
}

void SQLTransaction::RollBack()
{
	SQLRETURN ret = SQLEndTran(SQL_HANDLE_DBC, m_conn.m_hdbc, SQL_ROLLBACK);
	if (!SQL_SUCCEEDED(ret))
	{
		m_conn.Log(lld_error, "RollBack SQLEndTran Failed ret: %d", ret);
		m_conn.CheckErrorState(SQL_HANDLE_DBC, m_conn.m_hdbc);
	}
	m_iscommit = true;
}

NS_ODBC_END