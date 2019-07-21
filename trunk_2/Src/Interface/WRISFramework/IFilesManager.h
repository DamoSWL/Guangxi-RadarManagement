

#ifndef __INTERFACE_IFilesManager_200931
#define __INTERFACE_IFilesManager_200931

typedef bool (*FunOpenFiles)( int nPaths, char **szPathNames );


class IFilesManager
{
public:

	/*! 注册文件打开函数
		\param pluginID 插件ID
		\param szFileType 文件类型
		\param pFunOpenFiles 函数指针
		\return true,注册成功；false,注册失败
	*/
	virtual bool RegOpenFiles( PL_PID pluginID, char *szFileType, FunOpenFiles pFunOpenFiles ) = 0;
	
	/*! 注销文件打开函数
		\param pluginID 插件ID
		\return true,注销成功；false,注销失败
	*/
	virtual bool UnRegOpenFiles( PL_PID pluginID, char *szFileType ) = 0;
};

#endif // __HEADER_IFilesManager_200931

