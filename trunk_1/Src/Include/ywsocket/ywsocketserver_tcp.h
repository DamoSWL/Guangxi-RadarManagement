/* YWSocket.h */

#ifndef _YWSOCKET_H
#define _YWSOCKET_H

#include <winsock.h>
#pragma comment(lib, "wsock32.lib")		// search for wsock32 lib at compile time
#pragma comment(lib, "mpr.lib")			// search for mpr lib at compile time

#include <iostream>
#include <string>
#include <vector>
#include "math.h"
#include <TCHAR.h>
	using namespace std;
//////////////////////////////////////////////////////////////////////////
#ifndef SOCKET_DROPDOWN
const int SOCKET_DROPDOWN	= -1;
#endif
//////////////////////////////////////////////////////////////////////////
const LPCTSTR lpEvCliVec	= _TEXT("Client Vec IO");
const LPCTSTR lpSendBuff	= _TEXT("SendData Event");

const LPCSTR lpLostData		= "Net is slow,Data has Lost";
const LPCSTR CONNECT_INTERRAPT	="Connect has disconnected";


#ifndef SHUT_RECEIVE
	#define SHUT_RECEIVE    0
#endif

#ifndef SHUT_SEND
	#define SHUT_SEND		1
#endif

#ifndef SHUT_BOTH
	#define SHUT_BOTH		2
#endif

#ifndef SIZE_IP 
	#define SIZE_IP      20
#endif

//////////////////////////////////////////////////////////////////////////

extern int g_SockQuit;



//! YWSocket 远望网络通信类
/**
YWSocket，远望网络通信类，基于 Windows Socket，采用TCP/IP协议。该类可用作服务端和客户端。
作为服务端时，调用函数CreateSer(...)；客户端时，调用函数CreateCli(...)。
\author lava_sdb
\ingroup group_CDYW
\date 2003 - 2006

\b 开发历史:	
\arg \c lava_sdb, 20030804, 创建
\arg \c lava_sdb, 20031118, add the MulitThread and Event
\arg \c lava_sdb, 20040102, modify the method which send data to client
\arg \c lava_sdb, 20040103, Add a socket Server using send the error message
\arg \c lava_sdb, 20041018, fix the bug: Server can't distinguish the data come from which client
\arg \c lava_sdb, 20041030, Add the Data Auto Receive Administa,can Set client's auto receive status. Can set the number of auto receive data.
\arg \c lava_sdb, 20050419, Fix the bug, when it as Server, client drop ,the W_Recv thread is not suspend, so CPU usege reach 100%. If call the Close the MAccept thread don't over.
\arg \c lava_sdb, 20050520, 增加了对多网卡的支持0
\arg \c lava_sdb, 20050805, 修改了作为客户端时，不能正常检测与服务端连接状态的 BUG
\arg \c lava_sdb, 20050822, 修改了bug,当作为客户程序同网卡绑定时，只能连接一次服务器的问题。 客户端在绑定时，不能为其指定端口号！！
\arg \c lava_sdb, 20060428, 修改了线程关闭函数,减少了使用TerminateThread(),导致线程关闭发生异常;
\arg \c lava_sdb, 20060428, 增加函数CloseNoReceiveThr(),当Socket在接收线程中发生异常,要求关闭socket时,调用此函数
\arg \c lava_sdb, 20070329, 修复了内存泄露Bug,在关掉Socket时,没有正确的释放掉 m_buffer1 请求的内存
\arg \c lava_sdb, 20070329, 修改了发送函数为非阻塞模式(使用select模拟非阻塞,防止socket在异常时阻塞),增加了SendBlock()函数，以阻塞模式发送数据
\arg \c lava_sdb, 20070402, 修改了函数CreateCli 和 CreateSer,加入了对tcp发射、接收缓冲区大小的更改.
\b 版权
 CopyRight ? 2003-2010 ChengDu YuanWang Science & Technology.,LTD.
*/
template< class T >
class YWSocketServerT_TCP
{
	pthread_mutex_t _muClient;	// 用户列表访问互斥量

	int _iErr;	// 错误代码
	T* _pT;
	
	bool _bIsAccepting;	//! 接受连接线程是否在运行
	bool _bIsRecving;	//! 数据接收线程是否启动
/*  Variable Decalear*/
public:
	string s_message;
	//! 客户信息
	/*!
		客户信息，该结构用于存储客户相关信息（类作为服务端时有效）
		\ingroup group_Server
	*/
	typedef struct _ClIENTINFO
	{
		int iSocket;            //!< 客户的 socket 描述符
		char szIP[SIZE_IP];		//!< 客户的 IP
		char szName[SIZE_NAME];	//!< 客户的名称
		char szPass[SIZE_PASS];	//!< 客户的密码

		pthread_t hWnd;			//!< 客户的数据发送线程的句柄
		int  ValiSize;	//???? 		
		bool bWaitConnect;	//!< 是否等待客户 准备好传送数据
			
		bool bVerify;	//!< 客户是否通过验证
		int cntTime;	//!< 登陆计时器,
		bool bSetVerify; //!< 用户是否已经进行了验证

		_ClIENTINFO *pre;  //!< 指向前一个该结构
		_ClIENTINFO *next; //!< 指向下一个该结构
	}ClientInfo;

	//! 验证信息，记录客户的验证信息
	struct _VERIFY {
		int sFlag;   //!< 验证信息起始标记
		char name[ SIZE_NAME ];  //!< 验证的客户用户名
		char pass[ SIZE_PASS ];  //!< 验证的客户的密码
		int eFlag;  //!< 验证信息结束标记
	};

private:
	int m_socket;
	struct sockaddr_in m_sockaddr;
	int m_port;         //!< 端口号

/*	Function Decalear */
public:

	//! Get the error message
	/*!
	 \b 修改：
	   - lava_sdb 2003-11-20

	 \return The error string
	*/
	string GetError()
	{
		return s_message;
	}


	//! 构造函数
	YWSocketServerT_TCP()
	{
		_pT = (T*)this;

		memset( &_muClient, 0, sizeof( pthread_mutex_t ) );
		pthread_mutex_init( &_muClient, NULL );

		//init variable
		s_message	= "";

		m_socket	= -1;
		m_pClients  = NULL;
		m_pLastClient = NULL;
		m_bVerify = false;
		m_iVerifyTime = 10000;

		m_nClient   = 0;

		memset( &m_hRecThr, 0, sizeof( pthread_t) );
		memset( &m_hSerThr, 0, sizeof( pthread_t) );

		_bIsRecving = false;
		_bIsAccepting = false;

	}

	~YWSocketServerT_TCP()
	{
		Close();
		YWTRACE("~YWSocketServerT_TCP \n");

		pthread_mutex_destroy( &_muClient );
	}

//Attributes
	//! 得到 Socket 的描述符
	int  GetSocket()			{return m_socket;};
	
	//! 得到端口号，用于客户端，得到要连接的服务端的端口号
	int  GetPort()				{return m_port;};


	/*!
	sets a socket option.

	\param nOptionName Socket option for which the value is to be set. 
	\param lpOptionValue Pointer to the buffer in which the value for 
						the requested option is supplied. 			
	\param nOptionLen Size of the optval buffer
	\param nLevel Level at which the option is defined; the supported levels include SOL_SOCKET, 
	              IPPROTO_TCP, and IPPROTO_IP.
	*/
	bool SetSockOpt(int nOptionName, const void* lpOptionValue,
						int nOptionLen, int nLevel = SOL_SOCKET)
	{
		int iRet = 0, iReson = 0;
		if( m_socket > 0 ) 
		{
			iRet = setsockopt(m_socket, nLevel, nOptionName,(char*)lpOptionValue, nOptionLen);
			if( iRet == SOCKET_ERROR )
				iReson = WSAGetLastError();

		}

		return iRet==0?true:false;

	}


//! 设置验证超时时间
/*!
 如果验证选项为真，当用户在验证超时时间内没有通过验证，剔除该用户

 \param iLogTime 超时时间值	
*/
	void SetVerifyTime(int iLogTime)
	{
		m_iVerifyTime = iLogTime;  
	}

	//! 得到验证超时时间
	/*!
	 \return  超时时间值	
	*/
	int GetVerifyTime()
	{
		return m_iVerifyTime; 
	}


	/*!
	accept the client connected

	\param rConnectedSocket the connected client's socket decribe
	\param pszIP the connected client's IP

	\return 	success,	return TRUE;failed,		return FALSE

	*/
	bool Accept(int *rConnectedSocket, char *pszIP=NULL)
	{
		sockaddr_in sockAddr;
		int nAddrLength		= sizeof(sockAddr);
		*rConnectedSocket	= accept(GetSocket(),(SOCKADDR *)&sockAddr,&nAddrLength);
		if (*rConnectedSocket != -1)
		{

			if(pszIP!=NULL)
			{
				int p[4];

				for(int i=0;i<4;i++)
				{
					p[i]=((sockAddr.sin_addr.s_addr)>>(i*8))&0xff;
				}

				char sIP[SIZE_IP]={0};
				sprintf(sIP,"%d.%d.%d.%d",p[0],p[1],p[2],p[3]);
				strcpy( pszIP, sIP );
			}

			return true;

		}

		int iErr = WSAGetLastError();
		YWTRACE( "accept ERROR: %d \n", iErr );

		return false;
	}

	
	//! 终止服务端
	bool Close()
	{
		try
		{
			if(m_socket >0)
			{
				shutdown(m_socket, SHUT_BOTH);
				closesocket(m_socket);
				m_socket = -1;
			}

			if( IsAccepting() )
			{
				pthread_cancel( m_hSerThr );
				pthread_join( m_hSerThr, NULL );
			}

			YWTRACE("Shutting CRecThr.\n");
			if( IsRecving() )
			{
				pthread_cancel( m_hRecThr );
				pthread_join( m_hRecThr, NULL );
			}


			pthread_mutex_lock( &_muClient );
			ClientInfo *pInfo = m_pLastClient;
			while( pInfo != NULL)
			{
				if( pInfo->iSocket > 0)
				{
					shutdown( pInfo->iSocket, SHUT_BOTH );
					closesocket( pInfo->iSocket);

					pthread_cancel( pInfo->hWnd );
					pthread_join( pInfo->hWnd, NULL );
				}

				ClientInfo *pInfoTmp = pInfo;
				pInfo = pInfo->pre;
				delete pInfoTmp;
			}
			m_pLastClient = NULL;

			pthread_mutex_unlock( &_muClient );

			return true;
		}
		catch (...) 
		{
			return false;
		}
	}


	//! Send data to client ,the length of string must less than 1024;
	/*!
	\param lpBuf Buffer containing the data to be transmitted		
	\param nBufLen Length of the data in buf
	\param socket	Descriptor identifying a connected socket

	\b 修改：	
	- lava_sdb 2003-11-20   

	\return The total number of bytes sent
	*/
	int Send(void * lpBuf, int nBufLen,int socket=-1)
	{
		int sVal=0;

		if(socket>0) // if this class using Server
		{

			TIMEVAL tv01 = {0, 1};//1ms钟延迟,实际为9-10毫秒

			int nSelectRet;

			fd_set fdw;
			FD_ZERO(&fdw);

			FD_SET( socket, &fdw);

			nSelectRet=::select(0, NULL,&fdw, NULL, &tv01);//检查可写状态 

			if( (nSelectRet == SOCKET_ERROR) || (nSelectRet == 0) )
			{
				return -1;
			}else{
				sVal = send(socket,(char *)lpBuf,nBufLen,0);
			}

		}
		else	//if this class using Client
		{
			if(m_socket>0)
			{
				TIMEVAL tv01 = {0, 1};//1ms钟延迟,实际为9-10毫秒
				int nSelectRet;

				fd_set fdw;
				FD_ZERO(&fdw);

				FD_SET( m_socket, &fdw);

				nSelectRet=::select(0, NULL,&fdw ,NULL, &tv01);//检查可写状态 

				if( (nSelectRet == SOCKET_ERROR) || (nSelectRet == 0 ))
				{
					unsigned long dwErr = WSAGetLastError();
					return -1;
				}else{
					sVal = send(m_socket,(char *)lpBuf,nBufLen,0);
				}
			}else{
				//s_message = "Please crease the socket first!";
				return -1;
			}
		}

		return (sVal>0)?sVal:-1;
	};



	static int Send(int socket, void * lpBuf, int nBufLen )
	{
		int sVal=0;
		if(socket <= 0 ) return -1;

		TIMEVAL tv01 = {0, 1};//1ms钟延迟,实际为9-10毫秒

		int nSelectRet;


		fd_set fdw;
		FD_ZERO(&fdw);

		FD_SET( socket, &fdw);

		nSelectRet=::select(0, NULL,&fdw, NULL, &tv01);//检查可写状态 

		if( (nSelectRet == SOCKET_ERROR) || (nSelectRet == 0 ))
		{
			return nSelectRet;
		}else{
			sVal = send(socket,(char *)lpBuf,nBufLen,0);
		}

		return (sVal>0)?sVal:-1;
	}



	static int SendBlock(int socket, void * lpBuf, int nBufLen )
	{
		int sVal=0;
		if(socket <= 0 ) return -1;

		TIMEVAL tv01 = {0, 1};//1ms钟延迟,实际为9-10毫秒

		int nSelectRet;

		while(1)
		{
			fd_set fdw;
			FD_ZERO(&fdw);

			FD_SET( socket, &fdw);

			nSelectRet=::select(0, NULL,&fdw, NULL, &tv01);//检查可写状态 

			if( (nSelectRet == SOCKET_ERROR))
			{
				return nSelectRet;
			}
			else if(nSelectRet == 0 )
			{
				continue;
			}else{
				sVal = send(socket,(char *)lpBuf,nBufLen,0);
				break;
			}
		}

		return (sVal>0)?sVal:-1;
	}


//////////////////////////////////////////////////////////////////////////
//		Server, MutliThreads
//////////////////////////////////////////////////////////////////////////
	ClientInfo *m_pClients;   //!< 客户链表，用于服务端，记录当前连接上的客户
	ClientInfo *m_pLastClient;//!< 客户信息指针，指向客户链表的底端
	int m_nClient;		//!< 当前连接客户端的数量
	bool m_bVerify;		//!< 连接的客户端是否要经过验证
	int m_iVerifyTime;  //!< 验证的超时值，若客户在此时间内没有通过验证，则剔除该用户

	pthread_t m_hRecThr;		//!< 数据接收线程ID
	pthread_t m_hSerThr;		//!< 服务线程ID

protected:
	


	//! Using MulitThread Server, accept the MulitClient connected.
	/*!	
	  \b 修改：
	   - lava_sdb 2003-11-20   

	  \return NULL
	*/
	bool MAccept()
	{
		char szIP[ SIZE_IP ] = {0};
		int soc = 0;

		if(Accept(&soc, szIP))
		{
			ClientInfo * Info = new ClientInfo;
			YWTRACE("\n------- ClientInfo: %0x[%d]\n", Info, sizeof(ClientInfo) );

			memset( Info, 0, sizeof(ClientInfo));
			memset( &Info->hWnd, 0, sizeof( pthread_t) );

			Info->iSocket	= -1;
			Info->next = NULL;
			Info->pre = NULL;
			Info->cntTime = m_iVerifyTime / 100;
			Info->bSetVerify = false;
			Info->iSocket = soc;
			YWTRACE("-- Client socket: %d\n", soc);
			strcpy( Info->szIP, szIP );

			//// 添加用户信息到用户链表
			pthread_mutex_lock( &_muClient );
			if(m_pLastClient != NULL)  {
				m_pLastClient->next = Info;
				Info->pre = m_pLastClient;
			}

			m_pLastClient = Info;
			m_nClient++;
			pthread_mutex_unlock( &_muClient );

			T* pT = (T*)this;
			pT->OnEvent( Info, YW_EVENT_CONNECT );

			return true;
		}

		return false;
	}

	//! 清理服务线程
	static void *CB_CleanServerThr( void *pP )
	{
		YWSocketServerT_TCP< T > *pM = (YWSocketServerT_TCP<T> *)pP;
		pM->_bIsAccepting = false;

		return NULL;
	}


	//! Accept the MulitClient connected.
	/*!
		\b 修改：
				- lava_sdb 2003-11-20

		\return NULL
	*/
	static void* thrServerAccept( void* pP )
	{
		pthread_cleanup_push( CB_CleanServerThr, pP );

		YWSocketServerT_TCP< T > *pM = (YWSocketServerT_TCP<T> *)pP;
		pM->_bIsAccepting = true;
		for(;;)
		{
			pthread_testcancel();

			if( pM->MAccept() ){
				continue;
				YWTRACE( "Accept! \n" );
			}
		}

		pthread_cleanup_pop( 1 ); 
		return NULL;
	}


	//! 清理数据接收线程
	static void *CB_CleanReceiveThr( void *pP )
	{
		YWSocketServerT_TCP<T> *pM = (YWSocketServerT_TCP<T> *)pP;
		pM->_bIsRecving = false;

		return NULL;
	}

	//! Receive the command and data, parse and run it, Recevice Thread 
	/*!	
	 \param pP A YWSocket point	
					
	 \b 修改：
	   - lava_sdb 2003-11-20   

	  \return The thread exit code
	*/
	static void* thrDataRecv( void* pP )
	{
		pthread_cleanup_push( CB_CleanReceiveThr, pP );

		YWSocketServerT_TCP<T> *pM = (YWSocketServerT_TCP<T> *)pP;
		T* pT = (T*)pP;

		timeval timeout = { 1, 0 };
		bool bVerify = pM->m_bVerify;
		pM->_bIsRecving = true;

		for(;;)
		{
			pthread_testcancel();

			///   同步
			ClientInfo *pInfo = pM->m_pLastClient;
			if( pInfo == NULL )
			{
				Sleep( 50 );
				continue;
			}

			fd_set fds;
			FD_ZERO(&fds);
			while(pInfo != NULL)
			{
				FD_SET(pInfo->iSocket, &fds);
				pInfo = pInfo->pre;
			}

			// filter the socket that have data recvice
			int nReceived=0;
			nReceived = select( 0, &fds, NULL, NULL, &timeout );

			pthread_testcancel();
			if( nReceived == 0 ||  nReceived == SOCKET_ERROR )
			{
				continue;
			}

			pInfo = pM->m_pLastClient;
			while(pInfo != NULL)
			{
				pthread_testcancel();

				// Add You Handle here
				if(FD_ISSET( pInfo->iSocket, &fds ) )
				{
					char temp[10000];
					nReceived = recv( pInfo->iSocket, temp, sizeof(temp), 0 );

					pthread_testcancel();

					//YWTRACE("Receive: %d", nReceived);
					//	temp[nReceived+1]='\0';
					if (nReceived == -1 || nReceived==0)// Net error
					{
						char cIP[SIZE_IP + 5]={0};
						memcpy(cIP,pInfo->szIP, SIZE_IP);
						memcpy(cIP + 20, &pInfo->iSocket, sizeof(int));

						// 回调函数，等待上层中止对 pInfo 相关数据的引用。
						pT->OnDrop( pInfo->iSocket );

						YWTRACE("Drop %s 0x%X\n", cIP, pInfo);
						shutdown(pInfo->iSocket, 2);
						closesocket(pInfo->iSocket);

						//  用户发生意外，剔除用户
						ClientInfo* pTmpInfo = pInfo->pre;
						////  同步
						pthread_mutex_lock( &pM->_muClient );
						if(pInfo->pre != NULL && pInfo->next != NULL)
						{
							pInfo->pre->next = pInfo->next;
							pInfo->next->pre = pInfo->pre;
						}
						else if(pInfo->pre == NULL && pInfo->next != NULL) 
						{
							pInfo->next->pre = NULL;
						}
						else if(pInfo->pre != NULL && pInfo->next == NULL)
						{
							pInfo->pre->next = NULL;
							pM->m_pLastClient = pInfo->pre;
						}
						else
							pM->m_pLastClient = NULL;

						delete pInfo;
						pM->m_nClient--;
						pthread_mutex_unlock( &pM->_muClient );

						/////
						YWTRACE( "Client Drop1, %d \n", GetCurrentThreadId() );

						pInfo = pTmpInfo;
						continue;
					}
					else
					{
						// 不需要验证
						if( !pM->m_bVerify  || pInfo->bVerify )
						{
							pT->OnRecData(pInfo, &temp, nReceived);
						}
						else
						{
							/*
								1. 从接收缓冲获取用户名和密码获取验证信息；
								2. 进行用户权限验证
								2.1 验证失败，移除用户相关信息；
								2.2 验证通过，继续接收数据；
							*/
							_VERIFY *pVeri = (_VERIFY *)temp;
							pInfo->bSetVerify = true;

							if( pT->OnVerify(pInfo, pVeri->name, pVeri->pass ) )
							{
								pInfo = pInfo->pre;
								continue;
							}

							//if( pInfo->cntTime > 0)
							//{
							//	pInfo->cntTime--;
							//	pInfo = pInfo->pre;
							//	continue;
							//}

							//// 验证没有通过，移除用户相关信息
							ClientInfo* pTmpInfo = pInfo->pre;

							// { 踢出该用户
							////  同步
							YWTRACE("Kick 1 0x%X\n", pInfo);

							pT->OnEvent( pInfo, YW_EVENT_KICK );
							shutdown( pInfo->iSocket, 2 );
							closesocket( pInfo->iSocket );

							pthread_mutex_lock( &pM->_muClient );

							if(pInfo->pre != NULL && pInfo->next != NULL)
							{
								pInfo->pre->next = pInfo->next;
								pInfo->next->pre = pInfo->pre;
							}
							else if(pInfo->pre == NULL && pInfo->next != NULL) 
							{
								pInfo->next->pre = NULL;
							}
							else if(pInfo->pre != NULL && pInfo->next == NULL)
							{
								pInfo->pre->next = NULL;
								pM->m_pLastClient = pInfo->pre;
							}
							else
								pM->m_pLastClient = NULL;

							delete pInfo;
							pM->m_nClient--;
							
							pthread_mutex_unlock( &pM->_muClient );
							YWTRACE("Client Drop1, %d \n", GetCurrentThreadId());

							pInfo = pTmpInfo;
							continue;
							//} 
						}
					}
				}

				////////////
				pInfo = pInfo->pre;
			}
		}

		pthread_cleanup_pop( 1 );
		return 0;
	}

public:

	/*!
	\brief Create, bind ,start a MulitClient Server
	\param iPort The port server bind
	\param tcpBufLen TCP缓冲大小
	\param bVerify 是否需要验证
	\param szBindIP The IP you want server bind
	\param nConnectionBacklog The maxuim clients
	
	\return Success return true, fail return false
	
	\b 修改：	
	- lava_sdb 2003-11-20   
	- lava_sdb 2004-01-03 Add the error socket  
	*/
	bool CreateSer(unsigned int iPort,unsigned int tcpBufLen, bool bVerify=false, const char* szBindIP = NULL,int nConnectionBacklog=100 )
	{
		if(m_socket>0)
		{
			s_message = "Socket has been create!";
			return false;
		}

		/// 获取套接字
		m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );
		if (m_socket == -1 )
		{
			s_message = "Socket Create error.	Please check the sock has been init!";
			return false;
		}

		if( tcpBufLen > 0 )
		{
			// 设置接收缓冲区
			unsigned int recBuf,reclen=sizeof(int);
			getsockopt( m_socket,SOL_SOCKET ,SO_RCVBUF ,(char*)&recBuf, (int*)&reclen);
			if( recBuf < tcpBufLen )
			{
				recBuf = tcpBufLen;
				setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&recBuf, sizeof(int));
			}

			// 设置发送缓冲区
			unsigned int sndBuf,sndlen = sizeof(int);
			getsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, (int*)&sndlen);	
			if( sndBuf < tcpBufLen )
			{
				sndBuf = tcpBufLen;
				setsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, sizeof(int));			
			}
		}

		/// 绑定IP和端口号
		if( szBindIP )
			m_sockaddr.sin_addr.s_addr = inet_addr( (char*)szBindIP );
		else
			m_sockaddr.sin_addr.s_addr = INADDR_ANY;

		m_port = iPort;
		m_sockaddr.sin_family = AF_INET;

		m_sockaddr.sin_port = htons((u_short)GetPort());
		if( bind( m_socket, (struct sockaddr *)&m_sockaddr, sizeof(m_sockaddr) ) < 0 )
		{
			s_message = "Server Bind fail!\n";

			closesocket(m_socket);
			m_socket = -1;
			return false;
		}

		// 侦听客户连接
		if( listen(m_socket,nConnectionBacklog) != 0 )
		{
			s_message = "Server Listen fail!\n";
			shutdown(m_socket, SHUT_BOTH);
			closesocket(m_socket);
			m_socket = -1;
			return false;
		}

	
	// 设置发送，接收超时值
		int timeout   =   1000; 
		::setsockopt( m_socket,SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(int) ); //发送
		::setsockopt( m_socket,SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int) ); //接收

		/////
		m_bVerify = bVerify;

		pthread_create( &m_hSerThr, NULL, thrServerAccept, this );

		pthread_create( &m_hRecThr, NULL, thrDataRecv, this );

		_pT->OnEvent( NULL, YW_EVENT_SER_CRE_OK );

		return true;
	}


	//! 依据用户名查找用户是否连入服务端
	/*!
	 \param name 查找的客户的名称
	 \param pInfoOut 用户信息结构
	 \param size 连接的客户端的总数量

	 \b 修改：
	  - lava_sdb	2003-12-08

	  \return More than zero indicates success,and it is the client's socket descript. Less than zero indicates faiure.
	*/
	int FindAsName(string name,ClientInfo *pInfoOut=NULL, int size = 0)
	{
		ClientInfo *pInfo = m_pLastClient;
		int count = 0;

		while( pInfo != NULL)
		{
			if(strcmp(pInfo->szName, name.c_str()) == 0)
			{
				if(pInfoOut != NULL && count <= size)
				{
					memcpy(pInfoOut + count, pInfo, sizeof(ClientInfo));
					count ++;
				}
			}
			pInfo = pInfo->pre;
		}
		return count;
	}
	
	
	//! 依据用户IP查找用户是否连入服务端
	/*!
	 \param szIP The IP which you want Find.
	 \param name The client's name which you want find
	 \param pInfoOut 用户信息结构
	 \param size 连接的客户端的总数量

	 \b 修改：
	  - lava_sdb 2003-12-08

	 \return The number of client
	*/
	int FindAsIP(string szIP, string name="", ClientInfo *pInfoOut=NULL, int size = 0)
	{
		ClientInfo *pInfo = m_pLastClient;
		int count = 0;

		if(name == "") 
		{
			while( pInfo != NULL)
			{
				if(strcmp(pInfo->szIP, szIP.c_str()) == 0)
				{
					if(pInfoOut != NULL && count <= size)
					{
						memcpy(pInfoOut + count, pInfo, sizeof(ClientInfo));
						count ++;
					}
				}
				pInfo = pInfo->pre;
			}
		}
		else {
			while( pInfo != NULL)
			{
				if(strcmp(pInfo->szIP, szIP.c_str()) == 0 &&
					strcmp(pInfo->szName, name.c_str()) == 0)
				{
					memcpy(pInfoOut, pInfo, sizeof(ClientInfo));
					count = 1;
					break;
				}

				pInfo = pInfo->pre;
			}
		}

		return count;
	}


public:

	inline bool IsWorking()
	{
		return ( m_socket > 0 )?true:false;
	}

	//! 数据接收线程是否启动
	inline bool IsRecving() 
	{
		return _bIsRecving;
	}

	//! 客户端连接接收线程是否启动
	inline bool IsAccepting()
	{
		return _bIsAccepting;
	}

};



#endif




