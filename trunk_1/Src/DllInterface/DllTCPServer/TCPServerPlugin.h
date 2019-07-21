// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 TCPSERVER_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// TCPSERVER_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef TCPSERVER_EXPORTS
#define TCPSERVER_API __declspec(dllexport)
#else
#define TCPSERVER_API __declspec(dllimport)
#endif
#pragma once

#define WSMAXSIZE	32		//WebSock头最大字节数
enum UserLogoinStatus
{
	NOLOGOIN = -1,		//没有登录
	LOGOINCLOSE = 0,		//已关闭
	LOGOINFALSE = 1,		//登录错误，可能用户名密码
	LOGOINREPEAT = 2,		//该用户已经存在
	LOGOINTRUE = 3		//登录正确	
};

#pragma pack(push, 1)
typedef struct _SENDDATATYPE_
{
	union
	{
		unsigned char cTypeBuf;
		struct
		{
			unsigned char all : 1;			//向所有的客户端发送
			unsigned char Socket : 1;		//发送指定的数据到socket
			unsigned char Web : 1;			//向所有WEB的客户端发送
			unsigned char iPad : 1;		//向所有iPad的客户端发送
			unsigned char UT : 1;			//向所有UT的客户端发送
			unsigned char cTmp : 3;
		};
	};
}tagSendDataType;

typedef struct _UserLogoinStatus_
{
	union
	{
		int STBuf;
		struct
		{
			char LogoinStatus;
			char WebSocket;
			tagSendDataType SendUserType;
			char tmp1;
		};
	};
}tagUserLogoinStatus;
#pragma pack( pop )

class  ITCPServerSocketNet
{
public:
	virtual void OnReceiveData(const char *pData, unsigned int len, int Socket, const char *IP) = 0;//数据接收响应
	virtual void OnConnected(int Socket, const char *IP) = 0;		//用户退出登录
	virtual void OnDisconnected(int Socket) = 0;	//用户退出登录	
	virtual void AddLog(int type, const char *format) = 0;	//Log输出
};
class  ITCPInstance
{
public:
	virtual bool StartServer(const char *IP,		/*服务IP地址，为NULL或指向空字符串时不绑定指定IP*/
		const unsigned short Port,					/*绑定服务端口*/
		const unsigned int iMaxNumConnections = 1,	/*允许最大连接数*/
		const bool bNeedPack = false,				/*接收是否需要组包cdyw网络包格式,true 需要组包 */
		const bool bNeedLogoin = false				/*客户端是否需要登录*/
	) = 0;
	virtual void StopServer() = 0;
	virtual bool IsWork() = 0;
	virtual bool SendData(
		const char* pData,					/*缓冲区地址*/
		const unsigned int len,				/*发送缓冲区长度*/
		int Socket,							/*发送的Socket，非指定的客户端，可以填写0*/
		unsigned int iSendUserType = 1) = 0;	/*发送数据到指定的客服端，默认发送到指定socket中*/
	virtual void RegHandle(ITCPServerSocketNet *pHandle) = 0;	//注册ut命令处理过程
	virtual void DisconnectUser(int Socket, bool bAll = false) = 0;				//主动断开指定Socket连接
	virtual void SetLoginSuccess(int Socket, int status) = 0;	//设置登录状态，status见enum UserLogoinStatus
};

// 此类是从 TCPServer.dll 导出的
class TCPSERVER_API CTCPServerPlugin
{
public:
	CTCPServerPlugin(void);
	~CTCPServerPlugin();

public:
	ITCPInstance *GetInstance(const unsigned int RecePackSize,		/*接收包最大值*/
		const unsigned int SendPackSize,		/*发送包最大值*/
		const unsigned int ReceBuffSize,		/*接收缓冲区大小*/
		const unsigned int SendBuffSize);
	bool ReleaseInstance(ITCPInstance *pIns);
};


