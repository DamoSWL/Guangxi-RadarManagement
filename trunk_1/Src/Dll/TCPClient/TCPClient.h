#ifndef CTCPClient_H
#define CTCPClient_H

#include "TCPClientPlugin.h"

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

#include <string>
#include <vector>
using namespace std;

#define ACTSENDTIME		(3*1000)//心跳包发送时间间隔（毫秒）
#define ACTTIMEOUT		(10*1000)//活动超时时间（毫秒）

#define TCPMAXPACKSIZE	(500*1000)	//最大网络包大小Byte,爱尔达风廓线最大网络包270164
#define TCPMINPACKSIZE	(64)	//最小网络包大小Byte
#define TCPMAXBUFFERSIZE	(200*1000*1024)	//最大缓冲区大小Byte
#define TCPMINBUFFERSIZE	(1000*1024)	//最小缓冲区大小Byte

#pragma pack(push,1)		// one byte alignment
namespace LibeventTCP
{
	#define LOG_TYPE_MESSAGE 0
	#define LOG_TYPE_ERROR  1
	#define LOG_TYPE_WARNING 2
	#define LOG_TYPE_DEBUG  3

	#define NET_SOI							0xa55a4321	
	#define NET_EOI							0x5aa51234
	#define NET_DATA_TYPE_ACTPACK			0x5551aaa1	//心跳包类型

	//网络包头20字节
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
	}tagNetPackHead;						//网络包头

	//网络包尾4字节
	typedef struct _NET_PACKEND_
	{
		unsigned int EOI;					//END OF INFORMATION，0x5aa51234
	}tagNetPackEnd;							//pt网络包头	

	//心跳包，大小64Bytes
	typedef struct _ACT_PACK
	{
		tagNetPackHead head;
		char buff[40];
		tagNetPackEnd end;
	}tagActPack;
}
#pragma pack(pop)		// end one byte alignment

class CTCPClient;
typedef struct _HAS_CONNECT_
{
	bool bConnecting;				//连接进行中
	bool bConnected;				//连接成功
	bool bNeedPack;					//true需要组包，false不需要组包
	bool bDispatchRunning;			//事件基运行标识
	bool bEnableReadWrite;			//读写使能标识
	unsigned short port;			//连接端口
	string LocalIP;					//本地IP
	string DstIP;						//服务地址
    struct bufferevent *bev;
	pthread_t tid;					//主线程ID
	struct event_base *base;		//主事件基
	evutil_socket_t socket;
	char *pReadBuff;
	unsigned long ActTime;				//活动时间,计算机启动后逝去的毫秒数
	CTCPClient *pMain;				//指向构造他的对象
	unsigned long StartConnectTime;		//启动连接时间,计算机启动后逝去的毫秒数
	bool bWriteFlag;	//true可写，false不可写
}tagClientMange;

typedef struct _CONNECT_MESSAGE_
{
	string LocalIP;
	string DstIP;				/*服务IP地址，为NULL或指向空字符串时不绑定指定IP*/
	unsigned short DstPort;		/*服务端口*/
	bool bNeedPack;				/*接收是否需要组包cdyw网络包格式,true 需要组包 */
	bool bConnectFlag;			//连接成功标识，用于标识连接结束是否回调断开事件
}tagConnectMess;


//包含数据发送线程和踢出用户线程

typedef struct _NETSENDDATA_
{
	int len;						//数据大小
	char *pData;					//数据存放的指针
}tagNetData;

typedef struct _NET_BUFF_
{				
	unsigned int wIndex,rIndex,PackNum;	//写索引,读索引,包计数
	unsigned int PackSize,BuffSize,MaxPackNum;	//包大小，网络总缓存大小，最大网络包数
	char *pBuff;			//总缓冲区指针
	tagNetData *pPack;	//网络包缓存	
}tagNetBuff;

class CTCPClient:
        public ITCPClientInstance
{
public:
	CTCPClient(const unsigned int RecePackSize,		/*接收包最大值*/
		const unsigned int SendPackSize,		/*发送包最大值*/
		const unsigned int ReceBuffSize,		/*接收缓冲区大小*/
		const unsigned int SendBuffSize			/*发送缓冲区大小*/);
    ~CTCPClient();

public: //接口函数
	bool ConnectServer(const char *LocalIP,const char *IP,		/*服务IP地址，为NULL或指向空字符串时不绑定指定IP*/
		const unsigned short Port,					/*绑定服务端口*/
		const bool bNeedPack = false);
	void DisconnectServer();
	bool IsConneted();
	bool SendData(
		const char* pData,			/*缓冲区地址*/
		const unsigned int len);		/*发送缓冲区长度*/
	void RegHandle(ITCPClientSocketNet *pHandle);	//注册ut命令处理过程
	
private:
	//wxCriticalSection  m_NetStartStopCS;	
private:
	int OutpuMessage(int LOGTYPE,const char *fmt, ...);
private:
	static void read_cb(bufferevent *bev, void *arg);
    static void write_cb(bufferevent *bev, void *arg);
    static void event_cb(bufferevent *bev, short event, void *arg);

	static void libeventfatal_cb(int err);    
private:
    ITCPClientSocketNet *m_pHandle;
private:
	pthread_mutex_t loglock;
private:
    tagClientMange m_Mange;//管理连接状态到客户端   
public:
	bool WriteDataToSocket(const char *buff,unsigned int len);
	void QuitAndClear();
	void SendQuitSignal();
public:
	bool m_bReadThreadRun,m_bReadSetEnd;
	static void *thread_readprocess(void *arg);
	void AddDataToReadBuff(const char *pData,const unsigned int len);
	bool GetDataFromReadBuff(tagNetData &NetData);
	pthread_mutex_t m_NetReadlock;
	tagNetBuff m_RBuff;			//接收缓存
	void ResetReadBuff();
private:
	void initReadThreadBuff(const unsigned int RecePackSize,const unsigned int ReceBuffSize);

public:
	bool m_bSendThreadRun,m_bSendSetEnd;
	static void *thread_sendprocess(void *arg);
	void AddDataToSendBuff(const char *pData,const unsigned int len);
	bool GetDataFromSendBuff(tagNetData &NetData);
	pthread_mutex_t m_NetSendlock;
	tagNetBuff m_SBuff;			//发送缓存
	void ResetSendBuff();
private:
	void initSendThreadBuff(const unsigned int SendPackSize,const unsigned int SendBuffSize);

public:
	tagConnectMess m_ConnectMessage;
	HANDLE m_ConnectThreadHandle;
	static void *thread_connect( void *arg );
	bool DoConnectServer(const string &LocalIP,const string &IP,		/*服务IP地址，为NULL或指向空字符串时不绑定指定IP*/
		const unsigned short Port,					/*绑定服务端口*/
		const bool bNeedPack = false				/*接收是否需要组包cdyw网络包格式,true 需要组包 */
		);
	bool GetIPFromAdd(const string &szAddress,string &IP);

	HANDLE m_ReadThreadHandle, m_SendThreadHandle;
};

#endif // CTCPClient_H
