#ifndef _INTERFACE_TCPClient_20120626
#define _INTERFACE_TCPClient_20120626

static const PL_GUID g_PL_TCPClient = { 0x2a45ae07, 0xe604, 0x444e, { 0x93, 0x30, 0x2, 0xc2, 0xbd, 0x9e, 0xe6, 0xe } };		//插件的ID

static const PL_GUID g_I_TCPClient = { 0x418a0f56, 0xa32a, 0x4c8c, { 0x9b, 0x7e, 0xed, 0x92, 0x19, 0xf6, 0xb6, 0xe5 } };	//插件的功能接口ID 
static const char *g_Name_TCPClient = "TCP客户端";	//TCPClient插件的中文名称

enum _DISCONNECT_STATUS_
{
	TCPCLIENT_NOERROR		=0,	//无错误
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
	virtual void OnReceiveData(const char *pData,unsigned int len)=0;//数据接收响应
	virtual void OnConnected()=0;		//用户退出登录
	virtual void OnDisconnected(enum _DISCONNECT_STATUS_ type)=0;	//用户退出登录	
	//
	virtual void OnReceiveData(ITCPClientInstance *Instance,const char *pData,unsigned int len) {};//数据接收响应
	virtual void OnConnected(ITCPClientInstance *Instance) {};		//用户退出登录
	virtual void OnDisconnected(ITCPClientInstance *Instance,enum _DISCONNECT_STATUS_ type) {};	//用户退出登录	
};
class  ITCPClientInstance
{
public:
	virtual bool ConnectServer(const char *LocalIP,const char *IP,		/*服务IP地址，为NULL或指向空字符串时不绑定指定IP*/
		const unsigned short Port,					/*绑定服务端口*/
		const bool bNeedPack = false				/*接收是否需要组包cdyw网络包格式,true 需要组包 ，同时自动发送心跳，自动检查心跳*/
		)=0;
	virtual void DisconnectServer()=0;
	virtual bool IsConneted()=0;
	virtual bool SendData(
		const char* pData,			/*缓冲区地址*/
		const unsigned int len)=0;		/*发送缓冲区长度*/
	virtual void RegHandle(ITCPClientSocketNet *pHandle)=0;	//注册ut命令处理过程
};

class ITCPClient
{
public:
	virtual ITCPClientInstance *GetInstance(
		const unsigned int RecePackSize,		/*接收包最大值*/
		const unsigned int SendPackSize,		/*发送包最大值*/
		const unsigned int ReceBuffSize,		/*接收缓冲区大小*/
		const unsigned int SendBuffSize			/*发送缓冲区大小*/
		) = 0;					//获取TCP服务实例，可以获取多个
	virtual void ReleaseInstance( ITCPClientInstance *pInstance) = 0;	//释放TCP服务实例
};

#endif

