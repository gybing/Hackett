#ifndef __SQL_ODBC_H_STATEMENT__
#define __SQL_ODBC_H_STATEMENT__
#include "SQLDefine.h"
#include "SQLErrorState.h"
#include "SQLMetaData.h"

NS_ODBC_BEGIN

class SQLConnection;

class SQLStatement :public SQLErrorState
{
public:
	SQLStatement(SQLConnection* con, LoggerInterface* log);
	~SQLStatement();

	// 执行准备的语句
	bool Execute();

	// 取结果
	bool Fetch();

protected:
	// 准备
	bool Prepare(const std::string& sql, SQLInputData* in = nullptr, SQLOutputData* out = nullptr);

	// 直接执行语句
	bool ExecuteDirect(const std::string& sql, SQLInputData* in = nullptr, SQLOutputData* out = nullptr);

	// 绑定输入输出参数
	bool BindParam();
	bool BindColumn();

	// 连接设置
	bool GetStmtOption(int opt, unsigned int& value);
	bool SetStmtOption(int opt, unsigned int value);
	bool GetStmtOption(int opt, std::string& value);
	bool SetStmtOption(int opt, const std::string& value);

	bool AllocStmt(void* hdbc);

private:
	SQLConnection* m_conn;
	void* m_hstmt;
	bool m_state;

	SQLInputData::Ptr m_inputs;
	SQLOutputData::Ptr m_outputs;

public:
	friend class SQLConnection;
	ODBC_DEFINE_PTR(SQLStatement);
};

NS_ODBC_END

#endif __SQL_ODBC_H_STATEMENT__