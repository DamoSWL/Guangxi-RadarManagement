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
	\brief YWSocketClient è¿œæœ›ç½‘ç»œé€šä¿¡ç±?

	YWSocketClientï¼Œè¿œæœ›ç½‘ç»œé€šä¿¡ç±»ï¼ŒåŸºäºŽ Windows Socketï¼Œé‡‡ç”¨TCP/IPåè®®ã€‚è¯¥ç±»å¯ç”¨ä½œæœåŠ¡ç«¯å’Œå®¢æˆ·ç«¯ã€?
	ä½œä¸ºæœåŠ¡ç«¯æ—¶ï¼Œè°ƒç”¨å‡½æ•°CreateSer(...)ï¼›å®¢æˆ·ç«¯æ—¶ï¼Œè°ƒç”¨å‡½æ•°CreateCli(...)ã€?
	\author lava_sdb
	\ingroup group_CDYW
	\date 2003 - 2006

	\b å¼€å‘åŽ†å?	
	\arg \c lava_sdb, 20080408, åˆ›å»º

	\b ç‰ˆæƒ
	CopyRight ? 2003-2010 ChengDu YuanWang Science & Technology.,LTD.
*/
class YWSocketClient_TCP
{
	struct RecBuff{
		void *pThis; // 
		bool bLock;  // »º³åÊÇ·ñÒÑ¾­±»Ëø
		char *pBuf;  // 
	};


#ifdef WIN32
	static void *CB_CleanReceiveThr( void *pSock );
#else
	static void CB_CleanReceiveThr( void *pSock );
#endif

	int m_socket;
	struct sockaddr_in m_sockaddr;

	int m_port;         //!< ÊÇ·ñÊÇ¶Ë¿ÚºÅ

	bool m_bIsConnecting;	//! ÊÇ·ñÕýÔÚÁ¬½Óµ½·þÎñÆ÷
	bool m_bIsConnected;	//! ÊÇ·ñÒÑ¾­Á¬½Óµ½·þÎñÆ÷ÉÏ

	string s_message;		//! SocketÁ¬½ÓÊÂ¼þ

	bool m_bManul;	//! ÊÇ·ñÊÖ¶¯Á¬½Ó

public:
	const char * GetError();

	//Construction
	YWSocketClient_TCP();
	virtual ~YWSocketClient_TCP();
	
	//! »ñÈ¡ Socket ÃèÊö·û
	int  GetSoket()			{ return m_socket; };

	//! »ñÈ¡Á¬½ÓµÄ¶Ë¿ÚºÅ
	int  GetPort()			{return m_port;};

protected:
	//Operations
	//! ´´½¨Socket
	bool Socket(int nAddressFormat = AF_INET,int nSocketType=SOCK_STREAM,int nProtocolType = 0);

	//! °ó¶¨ TCP Client µ½Ö¸¶¨µÄ IP ºÍ ¶Ë¿Ú
	bool Bind( unsigned int nSocketPort, const char *szIP= NULL);

	/*! Á¬½Óµ½·þÎñ¶Ë
	\param szServerIP ·þÎñ¶ËIP
	\param nHostPort ·þÎñ¶Ë¶Ë¿ÚºÅ
	*/
	bool Connect(const char *szServerIP, unsigned int nHostPort);

	//! ¹Ø±ÕSocket Á¬½Ó
	bool ShutDown(int nHow = 2);

	/*! ¹Ø±Õ Socket Á¬½Ó
	\param bMan ÊÇ·ñÊÖ¶¯¹Ø±Õ
	- true ÊÇÊÖ¶¯¹Ø±Õ
	- false ×Ô¶¯¹Ø±Õ£¬¿ÉÄÜÊÇÍøÂçÒì³£µ¼ÖÂ
	*/
	bool Close( bool bMan );

public:
	//! ·¢ËÍÊý¾Ý
	int SendSelect(void * lpBuf, int nBufLen );
	int Send(void * lpBuf, int nBufLen );

private:
	pthread_t m_thrRec;		//!< Êý¾Ý½ÓÊÕÏß³ÌID
	char *m_pBufRec;	//!< Êý¾Ý½ÓÊÕ»º³å
	void *RecData( );

	////////////////
	unsigned int m_iRecBuffLen;     //!< »º³å³¤¶È
public:

	// Get and Set the unit length of SendData buffer
	inline int GetBufLen()	{return m_iRecBuffLen;};

	inline bool IsConnecting() { return m_bIsConnecting; }; // ÊÇ·ñÕýÔÚÁ¬½Ó·þÎñ¶Ë
	inline bool IsConnected() { return m_bIsConnected; };   // ÊÇ·ñÁ¬½ÓÉÏÁË·þÎñ¶Ë

	/*! ´´½¨TCP¿Í»§¶Ë£¬²¢Á¬½Óµ½Ö¸¶¨µÄ·þÎñÆ÷
		\param szHostIP ·þÎñ¶ËIP
		\param nHosPort ·þÎñ¶Ë¶Ë¿ÚºÅ
		\param szBindIP °ó¶¨±¾µØIP
		\param tcpBufLen tcp/IP »º³å³¤¶È

		- lava_sdb 2003-11-20 Create
		\return Success return true, fail return false
	*/
	bool CreateClient( const char* szServerIP, unsigned int nServerPort,  const char* szBindIP="",const unsigned int ReceBufLen=8192 ,const unsigned int SendBufLen=8192);

	//! Êý¾Ý½ÓÊÕ´¦Àíº¯Êý
	virtual int OnReceiveData( const char *pData, unsigned int len ) { return 0; };
	virtual void OnEvent(int EventType, void * pData=NULL, unsigned int len=0);

	void *m_pThis;	//Ö¸Ïò×Ô¼ºµÄÖ¸Õë
private:
	static void *thrReceive( void* pParam );


	///////
	pthread_t m_thrConn;

	char _szHostIP[256];	// ÒªÁ¬½ÓµÄ·þÎñÆ÷µÄIP
	unsigned int _nHostPort;	// ÒªÁ¬½ÓµÄ·þÎñÆ÷µÄ¶Ë¿ÚºÅ

#ifdef WIN32
	static void *CB_CleanConnectThr( void *pSock );
#else
	static void CB_CleanConnectThr( void *pSock );
#endif

	//! Á¬½ÓÏß³Ì
	static void *thrConnect( void* pParam );

	void* Connectting();
};

#endif




