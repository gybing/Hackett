#ifndef __SQL_ODBC_H_CONNECT__
#define __SQL_ODBC_H_CONNECT__
#include "SQLDefine.h"
#include "SQLErrorState.h"
#include "SQLEnvironment.h"
#include "SQLStatement.h"
#include "SQLTransaction.h"

NS_ODBC_BEGIN

// ���ݿ����Ӷ���
class SQLConnection : public SQLErrorState
{
public:
	SQLConnection(const std::string& con, LoggerInterface* log = nullptr);
	~SQLConnection();

	// ��������Դ
	bool Connect();

	// ��ʼ���ݿ�����
	SQLTransaction::Ptr BeginTrans();

	// ����Statement����
	SQLStatement::Ptr Prepare(const std::string& sql, SQLInputData* in = nullptr, SQLOutputData* out = nullptr);

	// ֱ��ִ��
	SQLStatement::Ptr Execute(const std::string& sql, SQLInputData* in = nullptr, SQLOutputData* out = nullptr);

	// �������״̬
	bool Available();

private:

	// �Զ��ύ
	bool GetAutoCommit();
	bool SetAutoCommit(bool isauto);

	// ��������
	bool GetConnOption(int opt, unsigned int& value);
	bool SetConnOption(int opt, unsigned int value);
	bool GetConnOption(int opt, std::string& value);
	bool SetConnOption(int opt, const std::string& value);

	// ����ODBC���
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