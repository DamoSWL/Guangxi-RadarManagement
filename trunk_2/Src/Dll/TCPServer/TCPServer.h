#ifndef CTCPSERVER_H
#define CTCPSERVER_H

#include "TCPServerPlugin.h"

#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <inet.h>
#endif

#include <pthread.h>
#include <event.h>
#include <event2/listener.h>
#include <event2/thread.h>

#include <vector>
using namespace std;
#include "Websocket.h"

#define CHECKCLIENTACTTIME	(1*1000)//检测客户端并删除时间（毫秒）
#define CLIENTTIMEOUT		(10*1000)//客户端活动超时时间（毫秒）
#define SERVERMONITORTIME	(15*1000)//服务端异常监控时间（毫秒）

#define TCPMAXPACKSIZE	(500*1000)	//最大网络包大小Byte,爱尔达风廓线最大网络包270164
#define TCPMINPACKSIZE	(64)	//最小网络包大小Byte
#define TCPMAXBUFFERSIZE	(200*1000*1024)	//最大缓冲区大小Byte
#define TCPMINBUFFERSIZE	(1000*1024)	//最小缓冲区大小Byte

class CTCPServer;

#pragma pack(push,1)		// one byte alignment
namespace LibeventTCP
{
#define LOG_TYPE_MESSAGE 0
#define LOG_TYPE_ERROR  1
#define LOG_TYPE_WARNING 2
#define LOG_TYPE_DEBUG  3
#define LOG_TYPE_NETTEST  9999		//网络测试

#define NET_SOI							0xa55a4321	
#define NET_EOI							0x5aa51234
#define NET_DATA_TYPE_ACTPACK			0x5551aaa1	//心跳包类型

	typedef struct _NET_PACKHEAD_
	{
		unsigned int SOI;					//START OF INFORMATION，0xa55a4321
		unsigned int DataPackSize;			//整个网路包的大小

		unsigned int VER:4;			//通信协议版本号
		unsigned int FrameSize:11;	//当前包DATA部分长度
		unsigned int FrameIndex:8;		//分包当前包序号
		unsigned int TotalFrame:8;	//分包总包数
		unsigned int spa2:1;		//保留

		unsigned int DataType;				//数据类型
		unsigned int PackID;				//网络包序号
	}tagNetPackHead;						

	typedef struct _NET_PACKEND_
	{
		unsigned int EOI;					//END OF INFORMATION，0x5aa51234
	}tagNetPackEnd;							//pt网络包头

	//心跳包，大小64Bytes
	typedef struct _ACT_PACK
	{
		tagNetPackHead head;
		char buff[512];
		tagNetPackEnd end;
	}tagActPack;
}
#pragma pack(pop)		// end one byte alignment

typedef struct _ITME_
{
	evutil_socket_t   fd;    //
	unsigned short port;
	char ip[32];
}tagItem;

typedef struct _AddFd_
{
	evutil_socket_t   fd;    //
	struct sockaddr_in address;
}tagAddFd;

//线程连接管理器
typedef struct _THREAD_CONNECT_MANAGE_
{
	pthread_mutex_t lock;
	vector<tagItem> ConnectVec;
}tagThreadConnectMange;

typedef struct {
	bool bRunning;		//线程运行状态
	//pthread_t tid;
	HANDLE pThreadHandle;
	struct event_base *base;
	struct event event;
	int read_fd;
	int write_fd;
	tagThreadConnectMange Mange;
	CTCPServer *pMain;
	char *pBuff;
}LIBEVENT_THREAD;

typedef struct _CLIENT_
{
	tagItem item;
	unsigned long ActiveTime;	//记录最后一次活动时间
	int	iLogoin;		//已经登陆上
	bool bSendClose;	//执行过关闭socket命令
	bool bWebSocket;	//是否是WEB使用的WebSocket
	tagSendDataType SendUserType;
	unsigned int ThreadID;			//处理该客户端的线程
	struct bufferevent *bev;
}tagClient;

typedef struct _HAS_CONNECT_
{
	bool bRunning;					//服务状态
	bool bNeedPack;						//true需要组包，false不需要组包
	bool bNeedLogoin;				//需要登录
	unsigned short port;		//
	string LocalIP;				//服务地址
	unsigned int AllowMaxClientNum;	//最大允许连接数
	evutil_socket_t listener_fd;		//监听socket
	struct evconnlistener *plistener;
	struct bufferevent *bev;
	//pthread_t tid;					//主线程ID
	HANDLE pThreadHandle;
	struct event_base *base;		//主事件基

	CTCPServer *pMain;					//指向构造对象
	pthread_mutex_t lock;			//客户端列表容器操作锁
	vector<tagClient> ClientVec;			//客户端列表
}tagClientMange;

//包含数据发送线程和踢出用户线程

typedef struct _NETSENDDATA_
{
	int socket;						//全发送socket设置为无效即可
	size_t len;						//数据大小,在发送数据中包含WebSocket头
	size_t baselen;					//数据大小
	tagSendDataType sendUserType;
	char *pData;					//数据存放的指针，在发送数据中包含WebSocket的头
	char *pBaseData;				//除了WebSocket头的数据地址，这里不需要删除，指针指向的是pData中
}tagNetData;

#define MAXTHREADNUM	4		//连接响应线程数（不要轻易修改该值）

typedef struct _START_MESSAGE_
{
	string LocalIP;
	unsigned short LocalPort;		/*服务端口*/
	unsigned int iMaxNumConnections;
	bool bNeedPack;
	bool bNeedLogoin;				/*接收是否需要组包cdyw网络包格式,true 需要组包 */
}tagStartMess;

typedef struct _NET_BUFF_
{				
	unsigned int wIndex,rIndex,PackNum;	//写索引,读索引,包计数
	unsigned int PackSize,BuffSize,MaxPackNum;	//包大小，网络总缓存大小，最大网络包数
	char *pBuff;			//总缓冲区指针
	tagNetData *pPack;	//网络包缓存	
}tagNetBuff;
/*
m_SBuff.wIndex
m_SBuff.rIndex
m_SBuff.PackNum
m_SBuff.PackSize
m_SBuff.BuffSize
m_SBuff.MaxPackNum
m_SBuff.pBuff
m_SBuff.pPack
*/

class CTCPServer:
	public ITCPInstance
{
public:
	CTCPServer(
		const unsigned int RecePackSize,		/*接收包最大值*/
		const unsigned int SendPackSize,		/*发送包最大值*/
		const unsigned int ReceBuffSize,		/*接收缓冲区大小*/
		const unsigned int SendBuffSize
		);
	~CTCPServer();

public: //接口函数
	bool StartServer(const char *IP,		/*服务IP地址，为NULL或指向空字符串时不绑定指定IP*/
		const unsigned short Port,					/*绑定服务端口*/
		const unsigned int iMaxNumConnections = 1,	/*允许最大连接数*/
		const bool bNeedPack = false,				/*接收是否需要组包cdyw网络包格式,true 需要组包 */
		const bool bNeedLogoin = false				/*客户端是否需要登录*/
		);
	void StopServer();
	bool IsWork();
	bool SendData(
		const char* pData,					/*缓冲区地址*/ 
		const unsigned int len,				/*发送缓冲区长度*/ 
		int Socket,							/*发送的Socket，非指定的客户端，可以填写0*/ 
		unsigned int iSendUserType=1);	/*发送数据到指定的客服端，默认发送到指定socket中*/
	void RegHandle(ITCPServerSocketNet *pHandle);	//注册ut命令处理过程
	void DisconnectUser(int Socket,bool bAll=false);				//主动断开指定Socket连接
	void SetLoginSuccess(int Socket,int status);	//设置登录状态
private:
	void ClearStartErrorStatus();
private:
	int OutpuMessage(int LOGTYPE,const char *fmt, ...);
private:
	//监听连接回调函数
	static void accept_conn_cb(struct evconnlistener *listener,
		evutil_socket_t fd, struct sockaddr_in *address, int socklen,
		void *ctx);
	//监听连接错误回调函数
	static void	accept_error_cb(struct evconnlistener *listener, void *ctx);

	static void event_read(bufferevent *bev, void *arg);
	static void event_cb(bufferevent *bev, short event, void *arg);

	static void libevent_Connect_process(int fd, short which, void *arg);
	static void *thread_main( void *arg );
	static void *thread_worker(void *arg);
	static void libeventfatal_cb(int err);
	void PutItem(const tagItem&item,LIBEVENT_THREAD *thread);
	bool GetItem(tagItem&item,LIBEVENT_THREAD *thread);

	bool CreateWorkThreadAndStart();
public:
	ITCPServerSocketNet *m_pHandle;
private:
	int fd[2];//主线程与子线程工作通信fd
	pthread_mutex_t loglock;
private:
	tagClientMange m_Mange;//管理连接状态到客户端

	inline void InserClient(const LIBEVENT_THREAD *me,const tagItem &item);
	inline void CloseSocketAndRemove(int fd,bool bAll=false);
	inline bool FindClientAndSetAct(int fd,tagClient &client);

	inline void FreebevAndNotiyDisconnect(tagClient&Client,bool bOnDisEvent);
	inline bool CheckAndCloseNotActiveClient(bool check);	

	inline bool CheckIsError();//检查是否存在错误

public:
	inline bool WriteDataToSocket(const char *buff,unsigned int len,tagClient &client);
	int m_last_thread;	//线程调度序号
	LIBEVENT_THREAD m_threads[MAXTHREADNUM];

public:
	bool m_bReadThreadRunning,m_bReadSetEnd;
	static void *thread_readprocess(void *arg);
	void AddDataToReadBuff(const char *pData,const unsigned int len,const int socket);
	bool GetDataFromReadBuff(tagNetData &NetData);
	pthread_mutex_t m_NetReadlock;
	tagNetBuff m_RBuff;			//接收缓存
	void ResetReadBuff();
private:
	void initReadThreadBuff(const unsigned int RecePackSize,const unsigned int ReceBuffSize);

public:
	bool m_bSendThreadRunning,m_bSendSetEnd;
	static void *thread_SendAndMainProcess(void *arg);
	void AddDataToSendBuff(const char *pData,const unsigned int len,const int socket, const unsigned char sendusertype);
	bool GetDataFromSendBuff(tagNetData &NetData);
	pthread_mutex_t m_NetSendlock;
	tagNetBuff m_SBuff;			//发送缓存
	void ResetSendBuff();
private:
	void initSendThreadBuff(const unsigned int SendPackSize,const unsigned int SendBuffSize);


public:
	vector<tagClient> m_SendVec;
	void SysUpdataSendVec();

public:
	tagStartMess m_StartMessage;
	static void *thread_startAndMonitor( void *arg );
	bool DoStartServer(const string &IP,		/*服务IP地址，为NULL或指向空字符串时不绑定指定IP*/
		const unsigned short Port,					/*绑定服务端口*/
		const unsigned int iMaxNumConnections = 1,	/*允许最大连接数*/
		const bool bNeedPack = false,				/*接收是否需要组包cdyw网络包格式,true 需要组包 */
		const bool bNeedLogoin = false				/*客户端是否需要登录*/
		);
	void DoStopServer();
public:
	void EndStartThread();
	bool m_StartThreadRunning;				//启动线程运行状态
	bool m_bStartThreadEndSignal;		//启动线程结束信号
	bool m_bReceiveStartFlag;			//接收到启动标识

	bool FindClient(int fd);
public:
	unsigned long m_PreConnectTick;

	void ProcessAddClient(int fd,struct sockaddr_in add);

public:
	bool m_bThreadProcessOnEventRunning,m_bThreadProcessOnEventEnd;
	pthread_mutex_t m_OnDisLock;
	vector<int> m_VecOnDisconnect;
	static void *thread_ProcessOnEvent( void *arg );
	inline void AddFdToOnDisconnectVect(int fd);

public:
	size_t m_CurConnectNum;
	bool m_bReadError,m_bWriteError;//读写异常标识
	unsigned long m_PreTime;//上一次输出提示信息时间戳

	HANDLE m_ProcessOnEventThreadHandle;
	HANDLE m_startAndMonitorThreadHandle;
	HANDLE m_ReadThreadHandle, m_SendThreadHandle;
};

#endif // CTCPSERVER_H
