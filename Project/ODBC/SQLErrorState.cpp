#include "SQLErrorState.h"
#include <sqlext.h>

NS_ODBC_BEGIN

SQLErrorState::SQLErrorState(LoggerInterface* log)
{
	SetLogger(log);
	SetLogTag("SQLErrorState");
}

SQLErrorState::~SQLErrorState()
{

}

bool SQLErrorState::CheckErrorState(short handleType, void* handle)
{
	SQLRETURN ret;
	char stat[6], msg[SQL_MAX_MESSAGE_LENGTH];
	SQLINTEGER err;
	SQLSMALLINT msgLen;
	SQLSMALLINT i = 1;

	// 循环获取状态串
	while (true)
	{
		ret = SQLGetDiagRec(handleType, handle, i++, (SQLCHAR*)stat, &err, (SQLCHAR*)msg, sizeof(msg), &msgLen);

		// 没有记录就跳出
		if (ret == SQL_NO_DATA)
		{
			return false;
		}

		//只可能是句柄无效
		if (!SQL_SUCCEEDED(ret))
		{
			Log(lld_error, "CheckErrorState Get diag record %d failed, ret: %d", i, ret);
			break;
		}

		// 错误写入日志,不用传递到上层
		Log(lld_error, "CheckErrorState %d, State: %s, ErrorMsg: %s", err, stat, msg);
	}

	return true;
}

NS_ODBC_END