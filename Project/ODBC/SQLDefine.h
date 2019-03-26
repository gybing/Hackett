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

// 字段最大长度
const __int64 ODBC_MAX_COL_SIZE = 8000;

// 大数据单次取大小
const __int64 ODBC_STREAM_FETCH_SIZE = 4 * 1024;

// GUID数据大小
const __int64 ODBC_GUID_COL_SIZE = 16;

// 大数据不限制长度
const __int64 ODBC_LENGTH_UNLIMITED = 0;

// 注意：如果数据库的时间日期字段为smalldatetime，写入__time64_t后再取出转换为__time64_t时存在误差
// 因为smalldatetime仅支持到分钟
// 将UTC时间戳转为TIMESTAMP_STRUCT的辅助函数，用于ODBC_DATATYPE_DATETIME数据类型
TIMESTAMP_STRUCT GetTimeStamp(__time64_t t);

// 将TIMESTAMP_STRUCT转为UTC时间戳的辅助函数，如果时间超出范围(1970.1.1-3000.12.31)时返回-1
__time64_t GetUTCTime(TIMESTAMP_STRUCT& ts);

// 将32位UTC时间戳转为TIMESTAMP_STRUCT的辅助函数，用于ODBC_DATATYPE_DATETIME数据类型
TIMESTAMP_STRUCT GetTimeStamp32(__time32_t t);

// 将TIMESTAMP_STRUCT转为32位UTC时间戳的辅助函数，如果时间超出范围(1970.1.1-2038.1.19)时返回-1
__time32_t GetUTCTime32(TIMESTAMP_STRUCT& ts);


// 定义Ptr指针
#define ODBC_DEFINE_PTR(__class__) typedef std::unique_ptr<__class__> Ptr;

enum SQLMetaDataType
{
	DataType_Unknown = 0,	// 未知
	DataType_Bool,			// 布尔值(BIT)
	DataType_Byte,			// 有符号单字节整数
	DataType_UByte,			// 无符号单字节整数
	DataType_Word,			// 有符号双字节整数
	DataType_UWord,			// 无符号双字节整数
	DataType_Int,			// 有符号四字节整数
	DataType_UInt,			// 无符号四字节整数
	DataType_Long,			// 有符号八字节整数
	DataType_ULong,			// 无符号八字节整数
	DataType_Float,			// 四字节浮点数
	DataType_Double,		// 八字节浮点数
	DataType_Datetime,		// 日期时间，对应C数据类型为SQL_TIMESTAMP_STRUCT							
	DataType_String,		// 字符串
	DataType_NString,		// 字符串(unicode)
	DataType_Binary,		// 二进制数据
	DataType_Guid			// 16字节GUID，对应C数据类型为长度16的无符号字节数组
};

// 二进制数据类型
struct SQLBLOB
{
	void* data;
	int size;

	// 构造方法1: 外部负责资源释放
	SQLBLOB(void* v, int s) : hold(false), data(v), size(s) {}

	// 构造方法2: 内部负责资源释放
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

/* GUID类型定义
typedef struct _GUID {
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];
} GUID; */

// 空数据
struct SQLNULL {};

//////////////////////////////////////////////////////////////////////////
// 数据标识映射
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

// 类型名称索引
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