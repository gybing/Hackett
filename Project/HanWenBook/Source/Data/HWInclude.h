#pragma once

// juce::String not support directly constract a utf8 string
#undef HWDictElemCString
#define HWDictElemString(TypeBasePtr, key) juce::CharPointer_UTF8((const char*)((HWTypeBase*)(*(HWDict*)TypeBasePtr)[key]->Value()))

// 后台
#include "HWDefine.h"
#include "../Source/Tools/CXOption.h"
#include "HWDocBase.h"
#include "HWLog.h"
#define CXLOG log_ui

#include "../Source/Data/HWDocAuth.h"
#include "../Source/Data/HWDocUserData.h"
#include "../Source/Data/HWDocBookData.h"
#include "../Source/Data/HWDocBookType.h"
#include "../Source/Data/HWDocHelper.h"
#include "../Source/Data/HWDocRead.h"
#include "../Source/Data/HWDocDraw.h"
#include "../Source/Data/HWDocSchool.h"
#include "../Source/Data/HWDocRecommend.h"
#include "../Source/Data/HWDocTag.h"
#include "../Source/Data/HWDocBookMark.h"
#include "../Source/Data/HWDocBookSign.h"
#include "../Source/Data/HWDocBookPostil.h"
#include "../Source/Data/HWDocComment.h"
#include "../Source/Data/HWDocDynamic.h"
#include "../Source/Data/HWDocSpace.h"
#include "../Source/Data/HWDocNotify.h"

#include "KnlBlock.h"

// 退出参数
enum CXEnumSysExitType{ CXSET_HIDE = 0x02, CXSET_EXIT = 0x04, CXSET_ASK = 0x08 };

// 阅读桌面
enum CXPDFPageNoType
{
    PAGE_FRONT     = -1,//封面
    PAGE_BACK      = -2,//封底
    PAGE_INVALID   = -5,//无效的PDF页码
};

// 用户信息相关
const int CSHWUserLevelHold[]=
{
    0,
    10,
    30,
    60,
    110,
    200,
    400,
    800,
    1600,
    3200,
    6400
};

const juce::String CSHWUserLevel[] =
{
    L"白丁",	
    L"书童",	
    L"先生",	
    L"秀才",	
    L"举人",	
    L"进士",	
    L"探花",	
    L"榜眼",	
    L"状元",
    L"大学士",	
    L"圣贤",

    L"丫头",
    L"小家碧玉",
    L"大家闺秀",
    L"佳人",	
    L"才女",	
    L"巾帼",	
    L"小郡主",	
    L"小公主",	
    L"长公主",	
    L"书香博士",
    L"圣仙子"
};

// 检测，调试功能
#define SAFE_DELETE(p)  {if(p!=NULL){delete p; p = NULL;}}

// 功能宏定义
#define WRECT(rc)   ((rc).right - (rc).left)
#define HRECT(rc)   ((rc).bottom - (rc).top)

#define FOR_EACH_LIST_NP(__TYPE__, __NAME__) \
    std::list<__TYPE__>::iterator it = __NAME__.begin(); \
    std::list<__TYPE__>::iterator itEnd = __NAME__.end(); \
    for (; it != itEnd;)

#define FOR_EACH_LIST(__TYPE__, __NAME__) \
    std::list<__TYPE__>::iterator it = __NAME__.begin(); \
    std::list<__TYPE__>::iterator itEnd = __NAME__.end(); \
    for (; it != itEnd; ++it)

#define FOR_EACH_LIST_PTR(__TYPE__, __NAME__) \
    std::list<__TYPE__>::iterator it = __NAME__->begin(); \
    std::list<__TYPE__>::iterator itEnd = __NAME__->end(); \
    for (; it != itEnd; ++it)

#define FOR_EACH_VECTOR(__ITER__, __NAME__) \
    unsigned int __ITER__ = 0; unsigned int size_type_vec_all_count = __NAME__.size(); \
    for(; __ITER__ < size_type_vec_all_count; ++__ITER__)

#define FOR_EACH_VECTOR_NP(__TYPE__, __NAME__) \
    std::vector<__TYPE__>::iterator it = __NAME__.begin(); \
    std::vector<__TYPE__>::iterator itEnd = __NAME__.end(); \
    for (; it != itEnd; )

#define FOR_EACH_MAP(__TYPE__, __VAL__, __NAME__) \
    std::map<__TYPE__, __VAL__>::iterator it = __NAME__.begin(); \
    std::map<__TYPE__, __VAL__>::iterator itEnd = __NAME__.end(); \
    for (; it != itEnd; ++it)

#define FOR_EACH_MAP_NP(__TYPE__, __VAL__, __NAME__) \
    std::map<__TYPE__, __VAL__>::iterator it = __NAME__.begin(); \
    std::map<__TYPE__, __VAL__>::iterator itEnd = __NAME__.end(); \
    for (; it != itEnd; )

#define FIND_MAP(__TYPE__, __VAL__, __NAME__, __KEY__) \
    std::map<__TYPE__, __VAL__>::const_iterator it = __NAME__.find(__KEY__); \
    if ( it != __NAME__.end() )

#define FIND_NOT_MAP(__TYPE__, __VAL__, __NAME__, __KEY__) \
    std::map<__TYPE__, __VAL__>::const_iterator it = __NAME__.find(__KEY__); \
    if ( it == __NAME__.end() )

#define SAFE_DELETE_VECTOR_PTR(__NAME__) \
    if ( __NAME__ ) \
{ \
    for (unsigned int _idx=0; _idx<__NAME__->size(); ++_idx) \
{ \
    if ((*__NAME__)[_idx]) delete (*__NAME__)[_idx]; \
    } \
    __NAME__->clear(); \
    delete __NAME__; \
    __NAME__ = NULL; \
    }

#define SAFE_DELETE_VECTOR(__NAME__) \
    for (unsigned int _idx=0; _idx<__NAME__.size(); ++_idx) \
{ \
    if (__NAME__[_idx]) delete __NAME__[_idx]; \
    } \
    __NAME__.clear(); \

#define SAFE_DELETE_MAP_PTR(__TYPE__, __VAL__, __NAME__) \
    if (__NAME__) \
{ \
    std::map<__TYPE__, __VAL__>::iterator it = __NAME__->begin(); \
    std::map<__TYPE__, __VAL__>::iterator itEnd = __NAME__->end(); \
    for (; it != itEnd; ++it) \
{ \
    delete it->second; \
    } \
    __NAME__->clear(); \
    delete __NAME__; \
    __NAME__ = NULL; \
    }