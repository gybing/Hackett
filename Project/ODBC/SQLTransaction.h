#ifndef __LX_ODBC_H_TRANSACTION__
#define __LX_ODBC_H_TRANSACTION__
#include "SQLDefine.h"
#include "SQLErrorState.h"

NS_ODBC_BEGIN

class SQLConnection;

// 数据库事务
class SQLTransaction : public SQLErrorState
{
public:
	SQLTransaction(SQLConnection& c, LoggerInterface* log = nullptr);
	~SQLTransaction();

	void Commit();
	void RollBack();

private:
	SQLConnection& m_conn;
	bool m_iscommit;

public:
	ODBC_DEFINE_PTR(SQLTransaction);
};

NS_ODBC_END

#endif __LX_ODBC_H_TRANSACTION__