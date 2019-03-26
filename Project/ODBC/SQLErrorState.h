#ifndef __SQL_ODBC_H_ERRORSTATE__
#define __SQL_ODBC_H_ERRORSTATE__
#include "SQLDefine.h"

NS_ODBC_BEGIN

class SQLErrorState : public LoggerWraper
{
public:
	SQLErrorState(LoggerInterface* log = nullptr);
	virtual ~SQLErrorState();

	// ´íÎó¼ì²é
	bool CheckErrorState(short handleType, void* handle);
};

NS_ODBC_END

#endif __SQL_ODBC_H_ERRORSTATE__