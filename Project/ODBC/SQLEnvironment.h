#ifndef __SQL_ODBC_H_ENVIRONMENT__
#define __SQL_ODBC_H_ENVIRONMENT__
#include "SQLDefine.h"

NS_ODBC_BEGIN

class SQLEnvironment
{
public:
	void* GetEnvironment() const;

public:
	SQLEnvironment();
	~SQLEnvironment();

private:
	void* m_henv;
};

NS_ODBC_END

#endif __SQL_ODBC_H_ENVIRONMENT__