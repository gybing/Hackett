#ifndef __SQL_ODBC_H_DEFINE__
#define __SQL_ODBC_H_DEFINE__
#include <memory>
#include <string>
#include <deque>
#include <atomic>
#include <logger.h>
#include <sqltypes.h>

#define NS_ODBC_BEGIN namespace ODBC {
#define NS_ODBC_END }
#define USING_NS_ODBC using namespace ODBC;

NS_ODBC_BEGIN

// �ֶ���󳤶�
const __int64 ODBC_MAX_COL_SIZE = 8000;

// �����ݵ���ȡ��С
const __int64 ODBC_STREAM_FETCH_SIZE = 4 * 1024;

// GUID���ݴ�С
const __int64 ODBC_GUID_COL_SIZE = 16;

// �����ݲ����Ƴ���
const __int64 ODBC_LENGTH_UNLIMITED = 0;

// ע�⣺������ݿ��ʱ�������ֶ�Ϊsmalldatetime��д��__time64_t����ȡ��ת��Ϊ__time64_tʱ�������
// ��Ϊsmalldatetime��֧�ֵ�����
// ��UTCʱ���תΪTIMESTAMP_STRUCT�ĸ�������������ODBC_DATATYPE_DATETIME��������
TIMESTAMP_STRUCT GetTimeStamp(__time64_t t);

// ��TIMESTAMP_STRUCTתΪUTCʱ����ĸ������������ʱ�䳬����Χ(1970.1.1-3000.12.31)ʱ����-1
__time64_t GetUTCTime(TIMESTAMP_STRUCT& ts);

// ��32λUTCʱ���תΪTIMESTAMP_STRUCT�ĸ�������������ODBC_DATATYPE_DATETIME��������
TIMESTAMP_STRUCT GetTimeStamp32(__time32_t t);

// ��TIMESTAMP_STRUCTתΪ32λUTCʱ����ĸ������������ʱ�䳬����Χ(1970.1.1-2038.1.19)ʱ����-1
__time32_t GetUTCTime32(TIMESTAMP_STRUCT& ts);


// ����Ptrָ��
#define ODBC_DEFINE_PTR(__class__) typedef std::unique_ptr<__class__> Ptr;

enum SQLMetaDataType
{
	DataType_Unknown = 0,	// δ֪
	DataType_Bool,			// ����ֵ(BIT)
	DataType_Byte,			// �з��ŵ��ֽ�����
	DataType_UByte,			// �޷��ŵ��ֽ�����
	DataType_Word,			// �з���˫�ֽ�����
	DataType_UWord,			// �޷���˫�ֽ�����
	DataType_Int,			// �з������ֽ�����
	DataType_UInt,			// �޷������ֽ�����
	DataType_Long,			// �з��Ű��ֽ�����
	DataType_ULong,			// �޷��Ű��ֽ�����
	DataType_Float,			// ���ֽڸ�����
	DataType_Double,		// ���ֽڸ�����
	DataType_Datetime,		// ����ʱ�䣬��ӦC��������ΪSQL_TIMESTAMP_STRUCT							
	DataType_String,		// �ַ���
	DataType_NString,		// �ַ���(unicode)
	DataType_Binary,		// ����������
	DataType_Guid			// 16�ֽ�GUID����ӦC��������Ϊ����16���޷����ֽ�����
};

// ��������������
struct SQLBLOB
{
	void* data;
	int size;

	// ���췽��1: �ⲿ������Դ�ͷ�
	SQLBLOB(void* v, int s) : hold(false), data(v), size(s) {}

	// ���췽��2: �ڲ�������Դ�ͷ�
	SQLBLOB() : hold(true), data(nullptr), size(0) {}

	void alloc(unsigned int s)
	{
		clear();
		data = malloc(s);
		size = s;
		hold = true;
	}

	bool calloc(unsigned int s)
	{
		if (hold && size > 0 && data)
		{
			size += s;
			data = realloc(data, size);
			return true;
		}
		return false;
	}

	void clear()
	{
		if (hold && data)
		{
			free(data);
		}
		data = nullptr;
		size = 0;
		hold = true;
	}

	bool empty() const
	{
		return 0 == size;
	}

	virtual ~SQLBLOB() { clear(); }

private:
	bool hold;
};

/* GUID���Ͷ���
typedef struct _GUID {
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];
} GUID; */

// ������
struct SQLNULL {};

//////////////////////////////////////////////////////////////////////////
// ���ݱ�ʶӳ��
static const char* SQLMetaTypeName[] =
{
	typeid(SQLNULL).name(),
	typeid(bool).name(),
	typeid(char).name(),
	typeid(unsigned char).name(),
	typeid(short).name(),
	typeid(unsigned short).name(),
	typeid(int).name(),
	typeid(unsigned int).name(),
	typeid(__int64).name(),
	typeid(unsigned __int64).name(),
	typeid(float).name(),
	typeid(double).name(),
	typeid(TIMESTAMP_STRUCT).name(),
	typeid(std::string).name(),
	typeid(std::wstring).name(),
	typeid(SQLBLOB).name(),
	typeid(GUID).name()
};

// ������������
static const std::map<const char*, SQLMetaDataType>::value_type SQLMetaTypeIndexData[] =
{
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_Unknown], DataType_Unknown),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_Bool], DataType_Bool),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_Byte], DataType_Byte),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_UByte], DataType_UByte),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_Word], DataType_Word),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_UWord], DataType_UWord),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_Int], DataType_Int),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_UInt], DataType_UInt),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_Long], DataType_Long),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_ULong], DataType_ULong),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_Float], DataType_Float),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_Double], DataType_Double),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_Datetime], DataType_Datetime),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_String], DataType_String),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_NString], DataType_NString),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_Binary], DataType_Binary),
	std::map<const char*, SQLMetaDataType>::value_type(SQLMetaTypeName[DataType_Guid], DataType_Guid),
};
static const std::map<const char*, SQLMetaDataType> SQLMetaTypeIndex(SQLMetaTypeIndexData, SQLMetaTypeIndexData + 17);

inline SQLMetaDataType GetMetaType(const char* s)
{
	const std::map<const char*, SQLMetaDataType>::const_iterator i = SQLMetaTypeIndex.find(s);
	return i != SQLMetaTypeIndex.end() ? i->second : DataType_Unknown;
}

NS_ODBC_END

#endif __SQL_ODBC_H_DEFINE__