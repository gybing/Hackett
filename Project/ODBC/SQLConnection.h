#ifndef __SQL_ODBC_H_CONNECT__
#define __SQL_ODBC_H_CONNECT__
#include "SQLDefine.h"
#include "SQLErrorState.h"
#include "SQLEnvironment.h"
#include "SQLStatement.h"
#include "SQLTransaction.h"

NS_ODBC_BEGIN

// 数据库连接对象
class SQLConnection : public SQLErrorState
{
public:
	SQLConnection(const std::string& con, LoggerInterface* log = nullptr);
	~SQLConnection();

	// 连接数据源
	bool Connect();

	// 开始数据库事务
	SQLTransaction::Ptr BeginTrans();

	// 创建Statement对象
	SQLStatement::Ptr Prepare(const std::string& sql, SQLInputData* in = nullptr, SQLOutputData* out = nullptr);

	// 直接执行
	SQLStatement::Ptr Execute(const std::string& sql, SQLInputData* in = nullptr, SQLOutputData* out = nullptr);

	// 检查连接状态
	bool Available();

private:

	// 自动提交
	bool GetAutoCommit();
	bool SetAutoCommit(bool isauto);

	// 连接设置
	bool GetConnOption(int opt, unsigned int& value);
	bool SetConnOption(int opt, unsigned int value);
	bool GetConnOption(int opt, std::string& value);
	bool SetConnOption(int opt, const std::string& value);

	// 创建ODBC句柄
	bool AllocConnect();

private:
	static SQLEnvironment m_env;
	std::string m_conn;
	void* m_hdbc;

public:
	friend class SQLStatement;
	friend class SQLTransaction;
	ODBC_DEFINE_PTR(SQLConnection);
};

NS_ODBC_END

#endif __SQL_ODBC_H_CONNECT__