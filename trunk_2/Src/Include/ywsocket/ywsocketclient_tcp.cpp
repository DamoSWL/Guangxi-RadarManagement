/*
ywsocketclient_tcp.cpp
*/


#include "ywsocketclient_tcp.h"


YWSocketClient_TCP::YWSocketClient_TCP()
{
	//init variable
	m_bManul = false;
	s_message	= "";
	m_socket	= -1;
	memset( &m_thrRec, 0, sizeof( pthread_t ) );
	memset( &m_thrConn, 0, sizeof( pthread_t ) );

	m_iRecBuffLen	= 0;
	m_pBufRec = NULL;

	m_bIsConnecting = false;
	m_bIsConnected = false;
	m_pThis = this;
}


//--------------
YWSocketClient_TCP::~YWSocketClient_TCP()
{
	Close( false );

	if(m_pBufRec != NULL) {
		delete []m_pBufRec;
		m_pBufRec = NULL;
	}
}


//! Send data to client ,the length of string must less than 1024;
/*!
\param lpBuf Buffer containing the data to be transmitted		
\param nBufLen Length of the data in buf
\param socket	Descriptor identifying a connected socket

- lava_sdb 2003-11-20   

\return The total number of bytes sent

M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
int YWSocketClient_TCP::SendSelect(void * lpBuf, int nBufLen )
{
	int sVal=0;

	if(m_socket>0)
	{
		TIMEVAL tv01 = {0, 1};//1ms 超时等待
		int nSelectRet;

		fd_set fdw;
		FD_ZERO(&fdw);

		FD_SET( m_socket, &fdw);

		nSelectRet=::select(m_socket+1, NULL,&fdw ,NULL, &tv01);//妫�鏌ュ彲鍐欑姸鎬?

		if( (nSelectRet == SOCKET_ERROR) || (nSelectRet == 0 ))
		{
			//DWORD dwErr = WSAGetLastError();
			return -1;
		}else{
			sVal = send(m_socket,(char *)lpBuf,nBufLen,0);
		}
	}else{
		//s_message = "Please crease the socket first!";
		return -1;
	}

	if(sVal>0)
	{
		//s_message="Send ok";
		return sVal;
	}else{
		//s_message="Send is wrong!";
		return -1;
	}
}

int YWSocketClient_TCP::Send(void * lpBuf, int nBufLen )
{
	if( m_socket > 0 ) {
		return send( m_socket, (char *)lpBuf, nBufLen, 0 );
	}

	return -1;
}



bool YWSocketClient_TCP::Socket(int nAddressFormat,int nSocketType,int nProtocolType)
{
	m_socket = socket(nAddressFormat,nSocketType,nProtocolType);
	if ( m_socket != -1 ){
		s_message = "Socket Create OK!";
		return true;
	}else{
		s_message = "Socket Create error.	Please check the sock has been init!";
		return false;
	}
}

bool YWSocketClient_TCP::Bind( unsigned int nSocketPort, const char* cIP)
{
	// / ADD
	string szIP(cIP);

	if(szIP != "")
		m_sockaddr.sin_addr.s_addr = inet_addr( (char*)cIP );
	else
		m_sockaddr.sin_addr.s_addr = INADDR_ANY;

	// // /
	m_port = nSocketPort;
	m_sockaddr.sin_family = AF_INET;

	m_sockaddr.sin_port = htons((u_short)GetPort());
	if( bind( m_socket, (struct sockaddr *)&m_sockaddr, sizeof(m_sockaddr) ) < 0 )
	{
		s_message = "Bind error,Please check the port";
		return false;
	}

	s_message = "Bind OK";

	return true;
}


bool YWSocketClient_TCP::Close( bool bMan )
{
	try
	{
		m_bManul = bMan;

		if( m_bIsConnected ) {
			pthread_cancel( m_thrRec );
			pthread_join( m_thrRec, NULL);
		}

		if( m_bIsConnecting ) {
			pthread_cancel( m_thrConn );
			pthread_join( m_thrConn, NULL );
		}

		if(m_socket >0)
		{
			shutdown(m_socket, 2);
			CloseSocket(m_socket);
		}

		m_socket = -1;
		m_bIsConnected = false;
		m_bIsConnecting = false;

		return true;
	}
	catch (...) 
	{
		return false;
	}
}

//! Close the socket, only using client
/*!
\param nHow The type that Closeing

\return 正常关闭Socket返回TRUE，否则返回FALSE
*/
bool YWSocketClient_TCP::ShutDown(int nHow)
{

	if(m_socket!=NULL)
	{
		if(shutdown(m_socket,nHow))
			return true;	
	}

	return false;

}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M

Method:   YWSocketClient_TCP::Connect

Summary:	When this class as a Client, this Method using connect the server

Args:		[in]
string lpszHostAddress,		the Server's IP
UINT nHostPort,				the Server's Port	

Modifies:	lava_sdb		2003-11-20   

Returns:	NULL

M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
bool  YWSocketClient_TCP::Connect(const char *szServerIP, unsigned int nHostPort)
{

	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_addr.s_addr = inet_addr( szServerIP );
	m_sockaddr.sin_port = htons((u_short)nHostPort);


	// 设置 TCP 不延迟发包
	//long optval = true ;
	//setsockopt(m_socket,IPPROTO_TCP,TCP_NODELAY,(char*)&optval, sizeof(optval));

	if(m_socket!=-1)
	{
		unsigned long ul = 1;
		unsigned long ul1= 0 ;

		void *pTT = this;
		int rVal=connect(m_socket,(struct sockaddr *)&m_sockaddr,sizeof(sockaddr_in));

		//select
		struct timeval timeout ;
		fd_set w;

		FD_ZERO(&w);
		FD_SET(m_socket, &w);
		timeout.tv_sec = 5; // 超时值
		timeout.tv_usec = 0;
		int ret = select(0, 0, &w, 0, &timeout);
		if ( ret <= 0 )
		{
#ifdef WIN32
			// 获得错误代码
			int errorCode = WSAGetLastError();
			//PL_TRACE("Socket connect ErrorCode: %d\n", errorCode);
			s_message	= "Connect error!";
#endif
			// 
			///
			return false;
		}
		else
		{
			int error = -1;
			int len=sizeof(error);
			bool bOK = false;

			// 
			if(getsockopt(m_socket, SOL_SOCKET, SO_ERROR,  (char *)&error, &len) == 0) 
			{
				if( error == 0 ) {
					s_message	= "Connect OK!";
					bOK = true;
				}
			}

			return bOK;
		}
	}
	else
	{
		s_message="Socket Create failu!";
		return false;
	}
}



void * YWSocketClient_TCP::thrReceive( void *pParam)
{
	YWSocketClient_TCP *pSock = (YWSocketClient_TCP *)pParam;
	return pSock->RecData();
}

void *YWSocketClient_TCP::RecData()
{
	timeval timeout = {0,100};

	pthread_cleanup_push( CB_CleanReceiveThr, this );
	int iLen = m_iRecBuffLen ; // 缂撳啿鍖鸿繕鍓╀綑鐨勫瓧鑺傛暟

	for(;;)
	{
		pthread_testcancel();

		fd_set fds;
		FD_ZERO(&fds);

		FD_SET( m_socket,&fds);

		// filter the socket that have data recvice
		int nReceived=0;
		nReceived = select( m_socket+1, &fds, NULL, NULL, &timeout );

		if( nReceived == 0 )//超时
		{
			// select 
		}
		else if( nReceived == -1)//网络错误
		{
			OnEvent(YW_EVENT_RECEIVE_ERROR);
			break;
		}
		else
		{
			if(FD_ISSET( m_socket, &fds ) )
			{
				pthread_testcancel();

				nReceived = recv( m_socket,(char*)m_pBufRec, iLen, 0);
				pthread_testcancel();

				if (nReceived == -1)
				{
					OnEvent(YW_EVENT_DROP_ERROR);
					break;
				}
				else if( nReceived == 0 )
				{
					OnEvent(YW_EVENT_DROP_REMOTE);
					break;
				}
				else 
				{
					OnReceiveData( m_pBufRec, nReceived );
				}
			}
		}

		pthread_testcancel();
	}

	pthread_cleanup_pop( 1 );

	return NULL;
}

#ifdef WIN32
	void *YWSocketClient_TCP::CB_CleanReceiveThr( void *pSock )
#else
	void YWSocketClient_TCP::CB_CleanReceiveThr( void *pSock )
#endif

{
	YWSocketClient_TCP *socket = (YWSocketClient_TCP *)pSock;
	shutdown(socket->m_socket,2);
	CloseSocket(socket->m_socket);
	socket->m_socket = -1;
	socket->m_bIsConnected = false;

	if( socket->m_bManul ) {
		//socket->_EventBuf.pThis = pSock;
		//socket->_EventBuf.EventType = YW_EVENT_DISCONNECT;
		//socket->_EventBuf.pBuf = NULL;
		//socket->_EventBuf.len = 0;

		socket->OnEvent( YW_EVENT_DISCONNECT, NULL, 0 );

		socket->m_bManul = false;
	}

#ifdef WIN32
	return NULL;
#endif

}


//! Get the error message
/*!
- lava_sdb 2003-11-20

\return The error string
*/
const char *YWSocketClient_TCP::GetError()
{
	return s_message.c_str();
}


bool YWSocketClient_TCP::CreateClient(const char* szServerIP, unsigned int nServerPort,  const char* szBindIP/*=""*/,const unsigned int ReceBufLen/*=8192*/ ,const unsigned int SendBufLen/*=8192*/)
{
	if(m_socket>0)
	{
		s_message = "Socket has been create!";
		return false;
	}

	// Create the Socket
	if(!Socket())
	{
		s_message = "Socket Create fail!\n";
		return false;
	}
	
	// 设置接收缓冲区
	int recBuf=0,reclen=sizeof(int);
	getsockopt( m_socket,SOL_SOCKET ,SO_RCVBUF ,(char*)&recBuf, (int*)&reclen);	//Windows系统默认8192的缓冲区大小
	if(recBuf<ReceBufLen)
	{
		recBuf = ReceBufLen;
		setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&recBuf, sizeof(int));
	}


	// 设置发送缓冲区
	int sndBuf=0,sndlen = sizeof(int);
	getsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, (int*)&sndlen);	
	if(sndBuf<SendBufLen)
	{
		sndBuf = SendBufLen;
		setsockopt( m_socket,SOL_SOCKET ,SO_SNDBUF ,(char*)&sndBuf, sizeof(int));
	}			


	// 创建接收缓冲区
	if( m_pBufRec != NULL ) {delete []m_pBufRec;m_pBufRec=NULL;}
	m_iRecBuffLen = ReceBufLen;
	m_pBufRec = new char[ m_iRecBuffLen ];		


	if(szBindIP != "") 
	{
		if(!Bind(0, szBindIP))
		{
			s_message = "Server Bind fail!\n";
			CloseSocket(m_socket);
			m_socket = -1;
			return false;
		}
	}

	// Connect to the Host
	int timeout   =   1000; 
	::setsockopt( m_socket,SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(int) ); // 设置 SOCKET 发送超时值
	::setsockopt( m_socket,SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int) ); // 设置 SOCKET 接收超时值

	strcpy( _szHostIP, szServerIP );
	_nHostPort = nServerPort;

	// 启动连接线程
	pthread_create( &m_thrConn, NULL, thrConnect , m_pThis);

	return true;
}



//! 网络事件处理函数
/*!  
\param EventType The type of event
\param data The point of data buffer
\param len The buffer len
\return
*/
void YWSocketClient_TCP::OnEvent(int EventType,LPVOID data,unsigned int len)
{
	switch(EventType)
	{
	case YW_EVENT_SER_CRE_OK:
		cout<<"Server Create OK!"<<endl;
		break;
	case YW_EVENT_SER_CRE_FALSE:
		cout<<""<<endl;
		break;
	case YW_EVENT_CONNECT:
		cout<<" Has connect"<<endl;
		//cout<<"Server IP is : "<<data<<endl;
		break;
	case YW_EVENT_DROP_ERROR:
		cout<<"Client has Disconnect!"<<endl;
		//cout<<"Client IP is"<<data <<endl;
		break;
	case YW_EVENT_AUTO_ERROR:
		// 骞挎挱妯″紡鐢ㄦ埛鏁伴噺杈惧埌鏈�澶у�?
		break;
	case YW_EVENT_DISCONNECT:
		// "缁堟杩炴帴"
		break;
	}

}

#ifdef WIN32
void *YWSocketClient_TCP::CB_CleanConnectThr( void *pSock )
#else
void YWSocketClient_TCP::CB_CleanConnectThr( void *pSock )
#endif
{
	return NULL;
}


void* YWSocketClient_TCP::thrConnect( void* pParam )
{
	YWSocketClient_TCP *pThis = (YWSocketClient_TCP *)pParam;
	return pThis->Connectting();
}

void* YWSocketClient_TCP::Connectting()
{
	//pthread_cleanup_push( CB_CleanConnectThr, this );
	///////////////////////////////////////////////////////////////

	m_bIsConnecting = true;
	if(Connect( _szHostIP, _nHostPort))
	{
		m_bIsConnected = true;
		m_bIsConnecting = false;

		//////////////////////////////////////////////////////////////////////////
		// 设置 Tcp 的 KeepAlive
		tcp_keepalive inKeepAlive = {0}; //
		unsigned long ulInLen = sizeof(tcp_keepalive);

		tcp_keepalive outKeepAlive = {0}; //
		unsigned long ulOutLen = sizeof(tcp_keepalive);

		unsigned long ulBytesReturn = 0;

#ifdef WIN32

		inKeepAlive.onoff = 1;
		inKeepAlive.keepaliveinterval = 3000;
		inKeepAlive.keepalivetime = 2;

		if (WSAIoctl(m_socket, SIO_KEEPALIVE_VALS,
			(LPVOID)&inKeepAlive, ulInLen,
			(LPVOID)&outKeepAlive, ulOutLen,
			&ulBytesReturn, NULL, NULL) == SOCKET_ERROR)
		{
			DWORD dwErr = WSAGetLastError();
		}

#endif
		pthread_create( &m_thrRec, NULL, thrReceive , m_pThis);

		OnEvent(YW_EVENT_CONNECT);
	}
	else
	{
		m_bIsConnecting = false;
		s_message = "Socket Connect fail!\n";
		CloseSocket(m_socket);
		m_socket = -1;
		OnEvent( YW_EVENT_CONNECT_FALSE );
	}

	pthread_detach( pthread_self() );
	//////////////////////////////////////////////////////////////
	//pthread_cleanup_pop( 1 );

	return NULL;
}


