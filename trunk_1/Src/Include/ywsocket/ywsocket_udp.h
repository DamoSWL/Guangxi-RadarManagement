/* YWSocket_UDP.h */

#ifndef _HEADER_YWSOCKET_CLIENT_UDP_20081026
#define _HEADER_YWSOCKET_CLIENT_UDP_20081026

#ifdef WIN32
	#include <Winsock2.h>
	#include <mstcpip.h>
	#include <ws2tcpip.h>

	#pragma comment(lib, "wsock32.lib")		// search for wsock32 lib at compile time
	#pragma comment(lib, "Ws2_32.lib")		// search for wsock32 lib at compile time
	#pragma comment(lib, "mpr.lib")			// search for mpr lib at compile time

	#define CloseSocket  closesocket
#else
	#include <Socket.h>
	#include <In.h>
	#include <ioLib.h>
	#include <sockLib.h>
	#include <inetLib.h>

	#define CloseSocket close

#endif

#include "ywsocketdefine.h"

#include <pthread.h>

#include <iostream>
#include <string>
#include <vector>
#include "math.h"
using namespace std;

/////////////////////////////////////////////////////////////////////////

/*! \class YWSocket_UDP
	\brief 远望网络通信类

	YWSocket_UDP，远望网络通信类，基于 Windows Socket，采用UDP/IP协议。该类可用作UDP服务/客户端。
	调用函数CreateCli(...)。

	\author lava_sdb
	\ingroup group_CDYW
	\date 2008 - 2010

	\b 开发历史:	
	\arg \c lava_sdb, 20080408, 创建
	\arg \c lava_sdb, 20081120, 将客户端和服务端合并为一个类;

	\b 版权
	CopyRight ? 2003-2010 ChengDu YuanWang Science & Technology.,LTD.
*/
class YWSocket_UDP
{
	int _recThrExitFlag;   // 接收线程退出标记
	timeval _recThrTimeout;  // 接收线程超时值

	unsigned char *m_pRecvBuf; // 数据接收缓冲

#ifdef WIN32
	static void *CB_CleanReceiveThr( void *pSock );
#else
	static void CB_CleanReceiveThr( void *pSock );
#endif

	int m_socket;
	struct sockaddr_in m_sockaddr;

	int m_port;         //!< 端口号

	bool m_bIsReceiving;	//! 是否处于数据接收状态
	
	bool m_bManul;	//! 是否用户主动终止连接

	void *m_pPar;

	bool m_bStartReceive; // 是否启动数据接收线程
	bool m_bCreateOK; // 是否Socket创建正常
	int m_NetType;//0点播，1组播，2广播
	struct ip_mreq mc_req;        /* multicast request structure */

public:
	string s_message;		//! Socket状态描述信息

public:
	const char * GetError();
	//Construction
	//! 构造函数
	YWSocket_UDP();
	virtual ~YWSocket_UDP();
	//Attributes
	//! 得到 Socket 的描述符
	int  GetSoket()			{ return m_socket; };

	//! 得到端口号，用于客户端，得到要连接的服务端的端口号
	int  GetPort()			{return m_port;};

	//Operations
	//! 创建Socket
	bool Socket(int nAddressFormat = AF_INET,int nSocketType=SOCK_STREAM,int nProtocolType = 0);

	//! 绑定到指定的IP
	bool Bind( unsigned int nSocketPort, const char *szIP= NULL);

	/*! 终止服务端（客户端）
	\param bMan 是否用户主动终止连接。
	- true 主动中断
	- false 被动终端，因网络异常等原因中断连接
	*/
	bool Close( bool bMan=true );

	
	/*! 发送数据
		\param lpBuf,
		\param nBufLen
		\param sendIP
		\param sendPort
	*/
	int Send(char * lpBuf, int nBufLen, const char *sendIP, unsigned short int sendPort );

private:
	pthread_t m_thrRec;		//!< 线程标识
	//! 选取空闲的接收缓冲

	void *RecData( );
	
	//! 用于测试接收到的数据流分包情况
	//void *RecDataTest();


	unsigned int m_iRecBuffLen;     //!< 缓冲区长度

public:
	

	inline bool IsWorking() { return m_bCreateOK; };  // 是否处于数据接收过程中
	inline bool IsReceiving() { return m_bIsReceiving; };

	/*! 创建UDP
	\param bStartRec true，启动数据接收线程；false，不启动数据接收线程
	\param szBindIP 绑定的IP
	\param nBindPort 绑定的端口号
	\param bufLen tcp/IP底层缓冲的宽度
	\param timeout select超时值，单位us;

	\b 修改：
	- lava_sdb 2003-11-20 Create
	\return Success return true, fail return false
	*/
	bool Create( bool bStartRec, const char* szBindIP, unsigned int nBindPort, unsigned int ReceBufLen=8192,unsigned int SendBufLen=8192, long timeout = 3000000);
	bool Create( bool bStartRec, const char* szLocalIP, unsigned int nLocalPort, const char* szMCIP, unsigned int nMCPort, unsigned int ReceBufLen=8192,unsigned int SendBufLen=8192, long timeout = 3000000);
	bool CreateEx( int NetType,bool bStartRec, const char* szLocalIP, unsigned int nLocalPort, const char* szDstIP, unsigned int nDstPort, unsigned int ReceBufLen=8192,unsigned int SendBufLen=8192, long timeout = 3000000);
	//! 启动数据接收线程
	void StartReceive();

	//! 停止数据接收线程
	void StopReceive();

	//! 当有完整命令接收到时，触发此
	virtual int OnReceiveData( const char *pData, unsigned int len ) { return 0; };
	virtual void OnEvent(int EventType, void * pData=NULL, unsigned int len=0) {};

	void *m_pThis;	//指向自己的指针
private:

	static void *thrReceive( void * pParam );
};

#endif




