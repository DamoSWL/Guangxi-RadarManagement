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



//! YWSocket Զ������ͨ����
/**
YWSocket��Զ������ͨ���࣬���� Windows Socket������TCP/IPЭ�顣�������������˺Ϳͻ��ˡ�
��Ϊ�����ʱ�����ú���CreateSer(...)���ͻ���ʱ�����ú���CreateCli(...)��
\author lava_sdb
\ingroup group_CDYW
\date 2003 - 2006

\b ������ʷ:	
\arg \c lava_sdb, 20030804, ����
\arg \c lava_sdb, 20031118, add the MulitThread and Event
\arg \c lava_sdb, 20040102, modify the method which send data to client
\arg \c lava_sdb, 20040103, Add a socket Server using send the error message
\arg \c lava_sdb, 20041018, fix the bug: Server can't distinguish the data come from which client
\arg \c lava_sdb, 20041030, Add the Data Auto Receive Administa,can Set client's auto receive status. Can set the number of auto receive data.
\arg \c lava_sdb, 20050419, Fix the bug, when it as Server, client drop ,the W_Recv thread is not suspend, so CPU usege reach 100%. If call the Close the MAccept thread don't over.
\arg \c lava_sdb, 20050520, �����˶Զ�������֧��0
\arg \c lava_sdb, 20050805, �޸�����Ϊ�ͻ���ʱ�����������������������״̬�� BUG
\arg \c lava_sdb, 20050822, �޸���bug,����Ϊ�ͻ�����ͬ������ʱ��ֻ������һ�η����������⡣ �ͻ����ڰ�ʱ������Ϊ��ָ���˿ںţ���
\arg \c lava_sdb, 20060428, �޸����̹߳رպ���,������ʹ��TerminateThread(),�����̹߳رշ����쳣;
\arg \c lava_sdb, 20060428, ���Ӻ���CloseNoReceiveThr(),��Socket�ڽ����߳��з����쳣,Ҫ��ر�socketʱ,���ô˺���
\arg \c lava_sdb, 20070329, �޸����ڴ�й¶Bug,�ڹص�Socketʱ,û����ȷ���ͷŵ� m_buffer1 ������ڴ�
\arg \c lava_sdb, 20070329, �޸��˷��ͺ���Ϊ������ģʽ(ʹ��selectģ�������,��ֹsocket���쳣ʱ����),������SendBlock()������������ģʽ��������
\arg \c lava_sdb, 20070402, �޸��˺���CreateCli �� CreateSer,�����˶�tcp���䡢���ջ�������С�ĸ���.
\b ��Ȩ
 CopyRight ? 2003-2010 ChengDu YuanWang Science & Technology.,LTD.
*/
template< class T >
class YWSocketServerT_TCP
{
	pthread_mutex_t _muClient;	// �û��б���ʻ�����

	int _iErr;	// �������
	T* _pT;
	
	bool _bIsAccepting;	//! ���������߳��Ƿ�������
	bool _bIsRecving;	//! ���ݽ����߳��Ƿ�����
/*  Variable Decalear*/
public:
	string s_message;
	//! �ͻ���Ϣ
	/*!
		�ͻ���Ϣ���ýṹ���ڴ洢�ͻ������Ϣ������Ϊ�����ʱ��Ч��
		\ingroup group_Server
	*/
	typedef struct _ClIENTINFO
	{
		int iSocket;            //!< �ͻ��� socket ������
		char szIP[SIZE_IP];		//!< �ͻ��� IP
		char szName[SIZE_NAME];	//!< �ͻ�������
		char szPass[SIZE_PASS];	//!< �ͻ�������

		pthread_t hWnd;			//!< �ͻ������ݷ����̵߳ľ��
		int  ValiSize;	//???? 		
		bool bWaitConnect;	//!< �Ƿ�ȴ��ͻ� ׼���ô�������
			
		bool bVerify;	//!< �ͻ��Ƿ�ͨ����֤
		int cntTime;	//!< ��½��ʱ��,
		bool bSetVerify; //!< �û��Ƿ��Ѿ���������֤

		_ClIENTINFO *pre;  //!< ָ��ǰһ���ýṹ
		_ClIENTINFO *next; //!< ָ����һ���ýṹ
	}ClientInfo;

	//! ��֤��Ϣ����¼�ͻ�����֤��Ϣ
	struct _VERIFY {
		int sFlag;   //!< ��֤��Ϣ��ʼ���
		char name[ SIZE_NAME ];  //!< ��֤�Ŀͻ��û���
		char pass[ SIZE_PASS ];  //!< ��֤�Ŀͻ�������
		int eFlag;  //!< ��֤��Ϣ�������
	};

private:
	int m_socket;
	struct sockaddr_in m_sockaddr;
	int m_port;         //!< �˿ں�

/*	Function Decalear */
public:

	//! Get the error message
	/*!
	 \b �޸ģ�
	   - lava_sdb 2003-11-20

	 \return The error string
	*/
	string GetError()
	{
		return s_message;
	}


	//! ���캯��
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
	//! �õ� Socket ��������
	int  GetSocket()			{return m_socket;};
	
	//! �õ��˿ںţ����ڿͻ��ˣ��õ�Ҫ���ӵķ���˵Ķ˿ں�
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


//! ������֤��ʱʱ��
/*!
 �����֤ѡ��Ϊ�棬���û�����֤��ʱʱ����û��ͨ����֤���޳����û�

 \param iLogTime ��ʱʱ��ֵ	
*/
	void SetVerifyTime(int iLogTime)
	{
		m_iVerifyTime = iLogTime;  
	}

	//! �õ���֤��ʱʱ��
	/*!
	 \return  ��ʱʱ��ֵ	
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

	
	//! ��ֹ�����
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

	\b �޸ģ�	
	- lava_sdb 2003-11-20   

	\return The total number of bytes sent
	*/
	int Send(void * lpBuf, int nBufLen,int socket=-1)
	{
		int sVal=0;

		if(socket>0) // if this class using Server
		{

			TIMEVAL tv01 = {0, 1};//1ms���ӳ�,ʵ��Ϊ9-10����

			int nSelectRet;

			fd_set fdw;
			FD_ZERO(&fdw);

			FD_SET( socket, &fdw);

			nSelectRet=::select(0, NULL,&fdw, NULL, &tv01);//����д״̬ 

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
				TIMEVAL tv01 = {0, 1};//1ms���ӳ�,ʵ��Ϊ9-10����
				int nSelectRet;

				fd_set fdw;
				FD_ZERO(&fdw);

				FD_SET( m_socket, &fdw);

				nSelectRet=::select(0, NULL,&fdw ,NULL, &tv01);//����д״̬ 

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

		TIMEVAL tv01 = {0, 1};//1ms���ӳ�,ʵ��Ϊ9-10����

		int nSelectRet;


		fd_set fdw;
		FD_ZERO(&fdw);

		FD_SET( socket, &fdw);

		nSelectRet=::select(0, NULL,&fdw, NULL, &tv01);//����д״̬ 

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

		TIMEVAL tv01 = {0, 1};//1ms���ӳ�,ʵ��Ϊ9-10����

		int nSelectRet;

		while(1)
		{
			fd_set fdw;
			FD_ZERO(&fdw);

			FD_SET( socket, &fdw);

			nSelectRet=::select(0, NULL,&fdw, NULL, &tv01);//����д״̬ 

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
	ClientInfo *m_pClients;   //!< �ͻ��������ڷ���ˣ���¼��ǰ�����ϵĿͻ�
	ClientInfo *m_pLastClient;//!< �ͻ���Ϣָ�룬ָ��ͻ�����ĵ׶�
	int m_nClient;		//!< ��ǰ���ӿͻ��˵�����
	bool m_bVerify;		//!< ���ӵĿͻ����Ƿ�Ҫ������֤
	int m_iVerifyTime;  //!< ��֤�ĳ�ʱֵ�����ͻ��ڴ�ʱ����û��ͨ����֤�����޳����û�

	pthread_t m_hRecThr;		//!< ���ݽ����߳�ID
	pthread_t m_hSerThr;		//!< �����߳�ID

protected:
	


	//! Using MulitThread Server, accept the MulitClient connected.
	/*!	
	  \b �޸ģ�
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

			//// ����û���Ϣ���û�����
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

	//! ��������߳�
	static void *CB_CleanServerThr( void *pP )
	{
		YWSocketServerT_TCP< T > *pM = (YWSocketServerT_TCP<T> *)pP;
		pM->_bIsAccepting = false;

		return NULL;
	}


	//! Accept the MulitClient connected.
	/*!
		\b �޸ģ�
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


	//! �������ݽ����߳�
	static void *CB_CleanReceiveThr( void *pP )
	{
		YWSocketServerT_TCP<T> *pM = (YWSocketServerT_TCP<T> *)pP;
		pM->_bIsRecving = false;

		return NULL;
	}

	//! Receive the command and data, parse and run it, Recevice Thread 
	/*!	
	 \param pP A YWSocket point	
					
	 \b �޸ģ�
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

			///   ͬ��
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

						// �ص��������ȴ��ϲ���ֹ�� pInfo ������ݵ����á�
						pT->OnDrop( pInfo->iSocket );

						YWTRACE("Drop %s 0x%X\n", cIP, pInfo);
						shutdown(pInfo->iSocket, 2);
						closesocket(pInfo->iSocket);

						//  �û��������⣬�޳��û�
						ClientInfo* pTmpInfo = pInfo->pre;
						////  ͬ��
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
						// ����Ҫ��֤
						if( !pM->m_bVerify  || pInfo->bVerify )
						{
							pT->OnRecData(pInfo, &temp, nReceived);
						}
						else
						{
							/*
								1. �ӽ��ջ����ȡ�û����������ȡ��֤��Ϣ��
								2. �����û�Ȩ����֤
								2.1 ��֤ʧ�ܣ��Ƴ��û������Ϣ��
								2.2 ��֤ͨ���������������ݣ�
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

							//// ��֤û��ͨ�����Ƴ��û������Ϣ
							ClientInfo* pTmpInfo = pInfo->pre;

							// { �߳����û�
							////  ͬ��
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
	\param tcpBufLen TCP�����С
	\param bVerify �Ƿ���Ҫ��֤
	\param szBindIP The IP you want server bind
	\param nConnectionBacklog The maxuim clients
	
	\return Success return true, fail return false
	
	\b �޸ģ�	
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

		/// ��ȡ�׽���
		m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );
		if (m_socket == -1 )
		{
			s_message = "Socket Create error.	Please check the sock has been init!";
			return false;
		}

		if( tcpBufLen > 0 )
		{
			// ���ý��ջ�����
			unsigned int recBuf,reclen=sizeof(int);
			getsockopt( m_socket,SOL_SOCKET ,SO_RCVBUF ,(char*)&recBuf, (int*)&reclen);
			if( recBuf < tcpBufLen )
			{
				recBuf = tcpBufLen;
				setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&recBuf, sizeof(int));
			}

			// ���÷��ͻ�����
			unsigned int sndBuf,sndlen = sizeof(int);
			getsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, (int*)&sndlen);	
			if( sndBuf < tcpBufLen )
			{
				sndBuf = tcpBufLen;
				setsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, sizeof(int));			
			}
		}

		/// ��IP�Ͷ˿ں�
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

		// �����ͻ�����
		if( listen(m_socket,nConnectionBacklog) != 0 )
		{
			s_message = "Server Listen fail!\n";
			shutdown(m_socket, SHUT_BOTH);
			closesocket(m_socket);
			m_socket = -1;
			return false;
		}

	
	// ���÷��ͣ����ճ�ʱֵ
		int timeout   =   1000; 
		::setsockopt( m_socket,SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(int) ); //����
		::setsockopt( m_socket,SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int) ); //����

		/////
		m_bVerify = bVerify;

		pthread_create( &m_hSerThr, NULL, thrServerAccept, this );

		pthread_create( &m_hRecThr, NULL, thrDataRecv, this );

		_pT->OnEvent( NULL, YW_EVENT_SER_CRE_OK );

		return true;
	}


	//! �����û��������û��Ƿ���������
	/*!
	 \param name ���ҵĿͻ�������
	 \param pInfoOut �û���Ϣ�ṹ
	 \param size ���ӵĿͻ��˵�������

	 \b �޸ģ�
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
	
	
	//! �����û�IP�����û��Ƿ���������
	/*!
	 \param szIP The IP which you want Find.
	 \param name The client's name which you want find
	 \param pInfoOut �û���Ϣ�ṹ
	 \param size ���ӵĿͻ��˵�������

	 \b �޸ģ�
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

	//! ���ݽ����߳��Ƿ�����
	inline bool IsRecving() 
	{
		return _bIsRecving;
	}

	//! �ͻ������ӽ����߳��Ƿ�����
	inline bool IsAccepting()
	{
		return _bIsAccepting;
	}

};



#endif




