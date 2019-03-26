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

	// ѭ����ȡ״̬��
	while (true)
	{
		ret = SQLGetDiagRec(handleType, handle, i++, (SQLCHAR*)stat, &err, (SQLCHAR*)msg, sizeof(msg), &msgLen);

		// û�м�¼������
		if (ret == SQL_NO_DATA)
		{
			return false;
		}

		//ֻ�����Ǿ����Ч
		if (!SQL_SUCCEEDED(ret))
		{
			Log(lld_error, "CheckErrorState Get diag record %d failed, ret: %d", i, ret);
			break;
		}

		// ����д����־,���ô��ݵ��ϲ�
		Log(lld_error, "CheckErrorState %d, State: %s, ErrorMsg: %s", err, stat, msg);
	}

	return true;
}

NS_ODBC_END