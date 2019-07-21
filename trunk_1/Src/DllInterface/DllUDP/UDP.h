// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 UDP_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// UDP_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef UDP_EXPORTS
#define UDP_API __declspec(dllexport)
#else
#define UDP_API __declspec(dllimport)
#endif

#ifndef _UDP_20160803_
#define _UDP_20160803_

/*! 数据接收回调函数
	本回调函数，用于传递数据到上层程序
	\param pHandle 回调函数所属类的实例的指针
	\param pData 指向附加的数据缓冲的指针
	\param uiLen 附加数据的长度
*/
typedef int (*FunDataRecv)( void *pHandle, const char *pData, unsigned int uiLen );


/*! 事件接收回调函数
	\param pHandle 回调函数所属类的实例的指针
	\param uiOpt 事件类型
	\param pData 指向附加的数据缓冲的指针
	\param uiLen 附加数据的长度
*/
typedef void (*FunDataEvent)( void *pHandle, int uiOpt , char *pData, unsigned int uiLen );


/*! \class ISocketNet
	\brief 数据接收接口
*/
class ISocketNet
{
public:
	
	//************************************
	// Method:    StartReceive
	// FullName:  ISocketNet::StartReceive
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier: 启动UDP
	// Parameter: const int Type					0点对点、1组播、2广播
	// Parameter: const char * LocalIP				绑定IP
	// Parameter: const unsigned short LocalPort	本地端口
	// Parameter: const char * DstIP				组播IP
	// Parameter: const unsigned short DstPort		组播端口
	// Parameter: const unsigned int RevBuffLen		接收缓冲区大小
	// Parameter: const unsigned int SendBuffLen	发送缓冲区大小
	// Parameter: const unsigned int TimeOutMS		超时值（好嘛）
	//************************************
	virtual bool StartReceive(const int Type,const char *LocalIP,const unsigned short LocalPort,
		const char *DstIP,const unsigned short DstPort,
		const unsigned int RevBuffLen,const unsigned int SendBuffLen,const unsigned int TimeOutMS) = 0;

	//! 停止数据接收
	virtual bool StopReceive() = 0;

	/*! Socket 当前是否处于工作(数据接收)状态
	*/
	virtual bool IsWorking() = 0;

	/*! \brief 注册消息处理回调函数

		注册的插件在退出时必须调用 UnregHandle 函数，注销数据接收事件

		\param pHandle sigRecv和sigEvent回调函数所属类的实例的指针
		\param sigRecv 数据接收回调函数
		\param sigEvent 事件接收回调函数
		\see UnregHandle
	*/
	virtual void RegHandle( void* pHandle, FunDataRecv sigRecv, FunDataEvent sigEvent ) = 0;

	/*! 注销消息处理回调函数
		\param pHandle sigRecv和sigEvent回调函数所属类的实例的指针
		\param sigRecv 数据接收回调函数
		\param sigEvent 事件接收回调函数
		\see RegHandle
	*/
	virtual void UnregHandle( void* pHandle) = 0;

	/*! 发送数据到网络连接的另一端
		\param pData 指向发送数据缓冲的指针
		\param uiLen 发送数据的长度
	*/ 
	virtual bool SendData( const char *pData, unsigned int uiLen ,const char *DstIP=0,unsigned short DstPort=0) = 0;

	virtual const char *GetMessage() = 0;

};

// 此类是从 UDP.dll 导出的
class UDP_API CUDP 
{
public:
	CUDP(void);
	~CUDP();
public:
	
	/*! \brief 获取Socket实例
		\return 实例指针
	*/
	ISocketNet *GetInstance();


	/*! \brief 释放Socket实例
		\param pIns Socket的实例指针
	*/
	bool ReleaseInstance( ISocketNet *pIns);
};

#endif