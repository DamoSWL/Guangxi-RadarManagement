/* YWSocketClient.h */

#ifndef _HEADER_YWSOCKET_20080409
#define _HEADER_YWSOCKET_20080409

#ifdef WIN32
#include <Winsock2.h>
#include <mstcpip.h>

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

/*! \class YWSocketClient_TCP
	\brief YWSocketClient 远望网络通信�?

	YWSocketClient，远望网络通信类，基于 Windows Socket，采用TCP/IP协议。该类可用作服务端和客户端�?
	作为服务端时，调用函数CreateSer(...)；客户端时，调用函数CreateCli(...)�?
	\author lava_sdb
	\ingroup group_CDYW
	\date 2003 - 2006

	\b 开发历�?	
	\arg \c lava_sdb, 20080408, 创建

	\b 版权
	CopyRight ? 2003-2010 ChengDu YuanWang Science & Technology.,LTD.
*/
class YWSocketClient_TCP
{
	struct RecBuff{
		void *pThis; // 
		bool bLock;  // �����Ƿ��Ѿ�����
		char *pBuf;  // 
	};


#ifdef WIN32
	static void *CB_CleanReceiveThr( void *pSock );
#else
	static void CB_CleanReceiveThr( void *pSock );
#endif

	int m_socket;
	struct sockaddr_in m_sockaddr;

	int m_port;         //!< �Ƿ��Ƕ˿ں�

	bool m_bIsConnecting;	//! �Ƿ��������ӵ�������
	bool m_bIsConnected;	//! �Ƿ��Ѿ����ӵ���������

	string s_message;		//! Socket�����¼�

	bool m_bManul;	//! �Ƿ��ֶ�����

public:
	const char * GetError();

	//Construction
	YWSocketClient_TCP();
	virtual ~YWSocketClient_TCP();
	
	//! ��ȡ Socket ������
	int  GetSoket()			{ return m_socket; };

	//! ��ȡ���ӵĶ˿ں�
	int  GetPort()			{return m_port;};

protected:
	//Operations
	//! ����Socket
	bool Socket(int nAddressFormat = AF_INET,int nSocketType=SOCK_STREAM,int nProtocolType = 0);

	//! �� TCP Client ��ָ���� IP �� �˿�
	bool Bind( unsigned int nSocketPort, const char *szIP= NULL);

	/*! ���ӵ������
	\param szServerIP �����IP
	\param nHostPort ����˶˿ں�
	*/
	bool Connect(const char *szServerIP, unsigned int nHostPort);

	//! �ر�Socket ����
	bool ShutDown(int nHow = 2);

	/*! �ر� Socket ����
	\param bMan �Ƿ��ֶ��ر�
	- true ���ֶ��ر�
	- false �Զ��رգ������������쳣����
	*/
	bool Close( bool bMan );

public:
	//! ��������
	int SendSelect(void * lpBuf, int nBufLen );
	int Send(void * lpBuf, int nBufLen );

private:
	pthread_t m_thrRec;		//!< ���ݽ����߳�ID
	char *m_pBufRec;	//!< ���ݽ��ջ���
	void *RecData( );

	////////////////
	unsigned int m_iRecBuffLen;     //!< ���峤��
public:

	// Get and Set the unit length of SendData buffer
	inline int GetBufLen()	{return m_iRecBuffLen;};

	inline bool IsConnecting() { return m_bIsConnecting; }; // �Ƿ��������ӷ����
	inline bool IsConnected() { return m_bIsConnected; };   // �Ƿ��������˷����

	/*! ����TCP�ͻ��ˣ������ӵ�ָ���ķ�����
		\param szHostIP �����IP
		\param nHosPort ����˶˿ں�
		\param szBindIP �󶨱���IP
		\param tcpBufLen tcp/IP ���峤��

		- lava_sdb 2003-11-20 Create
		\return Success return true, fail return false
	*/
	bool CreateClient( const char* szServerIP, unsigned int nServerPort,  const char* szBindIP="",const unsigned int ReceBufLen=8192 ,const unsigned int SendBufLen=8192);

	//! ���ݽ��մ�����
	virtual int OnReceiveData( const char *pData, unsigned int len ) { return 0; };
	virtual void OnEvent(int EventType, void * pData=NULL, unsigned int len=0);

	void *m_pThis;	//ָ���Լ���ָ��
private:
	static void *thrReceive( void* pParam );


	///////
	pthread_t m_thrConn;

	char _szHostIP[256];	// Ҫ���ӵķ�������IP
	unsigned int _nHostPort;	// Ҫ���ӵķ������Ķ˿ں�

#ifdef WIN32
	static void *CB_CleanConnectThr( void *pSock );
#else
	static void CB_CleanConnectThr( void *pSock );
#endif

	//! �����߳�
	static void *thrConnect( void* pParam );

	void* Connectting();
};

#endif




