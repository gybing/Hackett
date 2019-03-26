#include "SQLConnectionPool.h"

NS_ODBC_BEGIN

bool SQLConnectionPool::Initialize(const std::string& con, int cnt, LoggerInterface* log /*= nullptr*/)
{
	SetLogger(log);
	SetLogTag("SQLConnectionPool");

	AutoThreadMutex lock(m_mutex);

	m_conn = con;
	m_total = cnt;

	while (m_queue.size() < m_total)
	{
		SQLConnection* c = CreateConnect();
		if (c == nullptr)
		{
			return false;
		}

		m_queue.push_front(c);
	}

	return true;
}

void SQLConnectionPool::Finalize()
{
	AutoThreadMutex lock(m_mutex);

	while (!m_queue.empty())
	{
		delete m_queue.back();
		m_queue.pop_back();
	}
}

SQLConnection* SQLConnectionPool::GetConnect()
{
	AutoThreadMutex lock(m_mutex);

	if (m_queue.empty())
	{
		return CreateConnect();
	}
	
	SQLConnection* con = m_queue.back();
	m_queue.pop_back();

	// ¼ì²éÁ¬½Ó×´Ì¬
	if (!con->Available())
	{
		delete con;
		con = CreateConnect();
	}

	return con;
}

void SQLConnectionPool::PutConnect(SQLConnection* conn)
{
	AutoThreadMutex lock(m_mutex);

	if (m_queue.size() >= m_total)
	{
		delete conn;
	}
	else
	{
		m_queue.push_back(conn);
	}
}

SQLConnection* SQLConnectionPool::CreateConnect()
{
	return new SQLConnection(m_conn, GetLogger());
}

AutoSQLConnection::AutoSQLConnection(SQLConnectionPool* pool)
	: m_pool(pool)
	, m_conn(m_pool->GetConnect())
{

}

AutoSQLConnection::~AutoSQLConnection()
{
	m_pool->PutConnect(m_conn);
}

SQLConnection* AutoSQLConnection::get()
{
	return m_conn;
}

SQLConnection* AutoSQLConnection::operator->()
{
	return m_conn;
}

SQLConnection* AutoSQLConnection::operator*()
{
	return m_conn;
}

AutoSQLConnection::operator SQLConnection*()
{
	return m_conn;
}

bool AutoSQLConnection::operator!()
{
	return !m_conn;
}

NS_ODBC_END