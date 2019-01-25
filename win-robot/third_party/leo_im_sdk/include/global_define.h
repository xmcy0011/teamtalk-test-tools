/*******************************************************************************
 *  @file      macros.h 2012\9\11 17:09:12 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @summary   定义一些常用宏
 ******************************************************************************/

#ifndef _GLOBAL_DEFINE_F4FBBB0E_0884_473A_9319_2E2DDB83ED6F_H_
#define _GLOBAL_DEFINE_F4FBBB0E_0884_473A_9319_2E2DDB83ED6F_H_

#include <assert.h>
#include <string>
#include <tchar.h>
#include <vector>
/******************************************************************************/
#pragma warning(disable : 4267)

typedef signed char						Int8;
typedef unsigned char					UInt8;
typedef signed short					Int16;
typedef unsigned short					UInt16;
typedef signed int						Int32;
typedef unsigned int					UInt32;
typedef signed __int64					Int64;
typedef unsigned __int64				UInt64;
typedef std::basic_string<char>         ByteBuffer;
typedef std::string                     utf8String;
typedef char                            utf8char;
typedef unsigned char                   byte;

#define PTR_NULL(p)				{ assert(p); if (0 == p) { return 0; } }
#define PTR_VOID(p)				{ assert(p); if (0 == p) { return; } }
#define STR_VOID(p)				{ assert(p); if (0 == p || 0 == _tcslen(p)) { return; } }
#define PTR_FALSE(p)			{ assert(p); if (0 == p) { return FALSE; } }


#define NAMESPACE_BEGIN(n)		namespace n {
#define NAMESPACE_END(n)		}	

#define WARNING_STRING2(x) #x
#define WARNING_STRING(x) WARNING_STRING2(x)
#define WARNING_TARGET(x) "["#x"] "
#define WARNING_TODO(target, desc)  message(__FILE__ "(" WARNING_STRING(__LINE__) ") : warning : @TODO " WARNING_TARGET(target)##desc)
#define WARNING_FIX(target, desc)   message(__FILE__ "(" WARNING_STRING(__LINE__) ") : warning : @FIXME " WARNING_TARGET(target)##desc)
#define WARNING_WARN(target, desc)  message(__FILE__ "(" WARNING_STRING(__LINE__) ") : warning : @WARN " WARNING_TARGET(target)##desc)

#if defined(IN)
#undef IN
#endif // IN

#define  IN

#if defined(OUT)
#undef OUT
#endif // OUT

#define  OUT

// 计算数组的长度
#define DIMEN_OF(array)			(sizeof((array)) / sizeof((array)[0]))

// html 中标识的本机文件前缀
#define WIN_LOCAL_FILE_PREFIX	_T("file:///")
/******************************************************************************/
#endif// _GLOBAL_DEFINE_F4FBBB0E_0884_473A_9319_2E2DDB83ED6F_H_