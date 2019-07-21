
/*********************************************************/
/*! \file PluginDef.h
	\brief 通用雷达系统数据结构定义文件

	Copyright (c) 2008,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.

	\version 1.0
	\author 舒定波
	\date 2008年03月25日

	\b 修改记录

	\ingroup LibGRSFrameworkInterface
*/
/*********************************************************/

#ifndef _PLUGIN_DEFINE_20080325
#define _PLUGIN_DEFINE_20080325

#include "Settings.h"
#include <string.h>
#include <stdlib.h>

//! 全局唯一标识符
typedef struct _YWGUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
}PL_GUID;


#ifdef YW_WINDOWS
	#define PATH_INV '\\'

	#define PF_ADDIN_CMD_MIN_MSG_ID				WM_USER+1000
	#define PF_ADDIN_CMD_MAX_MSG_ID				WM_USER+10000

	#define PF_MIN_MSG					0
	#define PF_MAX_MSG					0xFFFF
#else
	#define PATH_INV '\\'
#endif

#ifndef S_OK
    #define S_OK 0L
#endif

#ifndef S_FALSE
    #define S_FALSE 1L
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif



/*!
	\brief 比较两个GUID是否相等
*/
#define IsEqualID(rguid1, rguid2) (!memcmp(&rguid1, &rguid2, sizeof(PL_GUID)))

#define S_CANCEL 2L
#define S_LOAD_ERR 3L
#define S_LOAD_DEPEND_ERR 4L

#define ASK_LOAD_DEPEND 0L   // 加载依赖的插件？
#define ASK_SUSPEND_DEPEND 1L  // 挂起依赖的插件
#define ASK_TIP         2L   // 提示信息，不要求用户选择

#define PL_TRACE printf

typedef PL_GUID PL_PID;  //!< 插件 ID
typedef PL_GUID PL_IID;  //!< 功能接口 ID
typedef PL_GUID PL_TB_ID;   //!< 工具栏 ID
typedef PL_GUID PL_MENU_ID; //!< 菜单 ID
typedef PL_GUID PL_PSD_ID;   //!< 参数设置对话框 ID

typedef PL_GUID *LPPL_GUID;

#ifndef NULL
	#define NULL 0
#endif

#ifndef IN
	#define IN
#endif

#ifndef OUT
	#define OUT
#endif


/*! \brief 空 GUID {00000000-0000-0000-0000-000000000000}
*/
const PL_GUID guid_NULL =
{ 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };


typedef unsigned short PL_WORD;

typedef void * PL_HANDLE;  //!< 句柄
typedef PL_HANDLE PL_HWND;  //!< 句柄
typedef PL_HANDLE PL_PAGE;  //!< 参数设置对话框指针
typedef PL_HANDLE PL_HINSTANCE;  //!< DLL实例句柄（对应于 vxworks的 MODULE_ID）
typedef PL_HANDLE PL_HRESINSTANCE;  //!< 资源实例句柄
typedef PL_HANDLE PL_MODULE;    //!< 模块实力句柄

// this is the plugins SDK version number
// it will change when the SDK interface breaks
#define PLUGIN_SDK_VERSION_MAJOR 1 //!< SDK 主版本号
#define PLUGIN_SDK_VERSION_MINOR 0 //!< SDK 次版本号
#define PLUGIN_SDK_VERSION_RELEASE 6 //!< SDK 释放版本号

#ifndef _DEBUG
	#define YWTRACE()
#endif

extern "C" {

	/*! 转换字符串为GUID
		\param strGUID 传入的GUID字符串必须按下列形式 "{B91C4048-F814-45e8-9258-2F7CA002519B}"
	*/
	DLLIMPORT PL_GUID StringToGUID( const char *strGUID );
	DLLIMPORT char* GUIDToStringTo( PL_GUID guid);
	DLLIMPORT PL_GUID NewGUID();

#ifdef _DEBUG
	DLLIMPORT void YWTRACE( const char *format, ... );
#endif
	DLLIMPORT void YWTRACE0( const char *format, ... );
};




#endif

