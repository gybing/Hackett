#ifndef __SQL_ODBC_H_CONNECTION_POOL__
#define __SQL_ODBC_H_CONNECTION_POOL__
#include "SQLDefine.h"
#include "SQLConnection.h"

NS_ODBC_BEGIN

// 数据库连接池
class SQLConnectionPool : public SQLErrorState
{
public:
	bool Initialize(const std::string& con, int cnt, LoggerInterface* log = nullptr);
	void Finalize();

	SQLConnection* GetConnect();
	void PutConnect(SQLConnection* conn);

protected:
	SQLConnection* CreateConnect();

private:
	ThreadMutex m_mutex;
	std::string m_conn;
	int m_total;
	std::deque<SQLConnection*> m_queue;
};

// 数据库连接池自动连接对象
class AutoSQLConnection
{
public:
	AutoSQLConnection(SQLConnectionPool* pool);
	~AutoSQLConnection();

	SQLConnection* get();
	SQLConnection* operator->();
	SQLConnection* operator*();

	operator SQLConnection*();
	bool operator!();

private:
	SQLConnectionPool* m_pool;
	SQLConnection* m_conn;
};

NS_ODBC_END

#endif __SQL_ODBC_H_CONNECTION_POOL__