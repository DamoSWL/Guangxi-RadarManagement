// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 TCPCLIENT_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// TCPCLIENT_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef TCPCLIENT_EXPORTS
#define TCPCLIENT_API __declspec(dllexport)
#else
#define TCPCLIENT_API __declspec(dllimport)
#endif
#pragma once;

#define WSMAXSIZE	32		//WebSock头最大字节数
enum _DISCONNECT_STATUS_
{
	TCPCLIENT_NOERROR = 0,	//无错误
	TCPCLIENT_DISCONNECT,		//本地主动断开连接
	TCPCLIENT_CONNECTTIMEOUT,	//连接超时，目的地址不可用或不存在
	TCPCLIENT_SERDISCONNECT,	//服务端主动断开连接
	TCPCLIENT_SERPORTERR,		//服务端IP地址存在，但未启动监听
	TCPCLIENT_ERROR,			//网络连接错误
	TCPCLIENT_BINDERROR,		//本地地址绑定错误
};
class ITCPClientInstance;
class  ITCPClientSocketNet
{
public:
	virtual void OnReceiveData(const char *pData, unsigned int len) = 0;//数据接收响应
	virtual void OnConnected() = 0;		//用户退出登录
	virtual void OnDisconnected(enum _DISCONNECT_STATUS_ type) = 0;	//用户退出登录	
																	//
	virtual void OnReceiveData(ITCPClientInstance *Instance, const char *pData, unsigned int len) {};//数据接收响应
	virtual void OnConnected(ITCPClientInstance *Instance) {};		//用户退出登录
	virtual void OnDisconnected(ITCPClientInstance *Instance, enum _DISCONNECT_STATUS_ type) {};	//用户退出登录	

	virtual void AddLog(int type, const char *format) = 0;
};
class  ITCPClientInstance
{
public:
	virtual bool ConnectServer(const char *LocalIP, const char *IP,		/*服务IP地址，为NULL或指向空字符串时不绑定指定IP*/
		const unsigned short Port,					/*绑定服务端口*/
		const bool bNeedPack = false				/*接收是否需要组包cdyw网络包格式,true 需要组包 ，同时自动发送心跳，自动检查心跳*/
	) = 0;
	virtual void DisconnectServer() = 0;
	virtual bool IsConneted() = 0;
	virtual bool SendData(
		const char* pData,			/*缓冲区地址*/
		const unsigned int len) = 0;		/*发送缓冲区长度*/
	virtual void RegHandle(ITCPClientSocketNet *pHandle) = 0;	//注册ut命令处理过程
};

// 此类是从 TCPClient.dll 导出的
class TCPCLIENT_API CTCPClientPlugin {
public:
	CTCPClientPlugin(void);
	~CTCPClientPlugin();
public:
	ITCPClientInstance *GetInstance(
		const unsigned int RecePackSize,		/*接收包最大值*/
		const unsigned int SendPackSize,		/*发送包最大值*/
		const unsigned int ReceBuffSize,		/*接收缓冲区大小*/
		const unsigned int SendBuffSize			/*发送缓冲区大小*/
	);					//获取TCP服务实例，可以获取多个
	void ReleaseInstance(ITCPClientInstance *pInstance);	//释放TCP服务实例

	int GetInstanceSize();
};

