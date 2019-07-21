#include "stdafx.h"
#include "TCPServer.h"

#define EMPTYIP	("NULL")
//#define CLOSENOMANGESOCKET(fd) {if(fd!=INVALID_SOCKET){evutil_closesocket(fd);fd=INVALID_SOCKET;}}
#define CLOSENOMANGESOCKET(fd) {evutil_closesocket(fd);}

extern CTCPServerPlugin* g_pMain;

CTCPServer::CTCPServer(
	const unsigned int RecePackSize,		/*接收包最大值*/
	const unsigned int SendPackSize,		/*发送包最大值*/
	const unsigned int ReceBuffSize,		/*接收缓冲区大小*/
	const unsigned int SendBuffSize
	)
{
	event_set_fatal_callback(libeventfatal_cb);

	//注意：以下调用不能在多个线程中调用，不然会影响发送
	//以下调用存在3个24Byte内存泄露，待libevent2.1版本调用libevent_global_shutdown()解决
	#ifdef WIN32
		evthread_use_windows_threads();//win上设置
	#else
		evthread_use_pthreads();    //unix上设置
	#endif
	m_startAndMonitorThreadHandle = NULL;
	m_ProcessOnEventThreadHandle = NULL;
	m_ReadThreadHandle = NULL;
	m_SendThreadHandle = NULL;
	m_Mange.pThreadHandle = NULL;
    m_pHandle = NULL;
    m_Mange.bRunning = false;
	//m_Mange.tid.p = NULL;
	m_Mange.base = NULL;
	m_Mange.plistener = NULL;
	m_Mange.pMain = this;
	m_bReadThreadRunning = m_bSendThreadRunning = false;
	pthread_mutex_init(&m_Mange.lock, NULL);	

	m_last_thread = 0;
	for (int i = 0; i < MAXTHREADNUM; i++)
	{
		m_threads[i].pThreadHandle = NULL;
		m_threads[i].base = NULL;
		m_threads[i].bRunning = false;
		m_threads[i].pBuff = new char[TCPMAXPACKSIZE];
		pthread_mutex_init(&m_threads[i].Mange.lock, NULL);
	}

	pthread_mutex_init(&loglock, NULL);

	pthread_mutex_init(&m_NetSendlock,NULL);
	pthread_mutex_init(&m_NetReadlock,NULL);
#ifdef WIN32
	//确认WinSock DLL支持2.2
	WSADATA wsaData; // winsock 结构体
	WORD wVersionRequested = MAKEWORD( 2, 2 ); // 初始化 Windows Socket
	int err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
		OutpuMessage(LOG_TYPE_ERROR,"winsock 的版本 错误！");
#endif

	m_StartThreadRunning = false;
	m_bStartThreadEndSignal = false;
	m_bReceiveStartFlag = false;

	m_PreConnectTick = GetTickCount();

	pthread_mutex_init(&m_OnDisLock,NULL);
	m_VecOnDisconnect.clear();
	m_bThreadProcessOnEventRunning = m_bThreadProcessOnEventEnd=false;
		
	m_CurConnectNum = 0;
	m_bReadError = m_bWriteError = false;

	initReadThreadBuff(RecePackSize,ReceBuffSize);
	initSendThreadBuff(SendPackSize,SendBuffSize);
}
//************************************
// Method:    initReadThreadBuff
// FullName:  CTCPServer::initReadThreadBuff
// Access:    private 
// Returns:   void
// Qualifier: 初始化读缓冲区（只在构造函数中调用一次）
// Parameter: const unsigned int RecePackSize	接收网络包大小
// Parameter: const unsigned int ReceBuffSize	接收缓冲区大小
//************************************
void CTCPServer::initReadThreadBuff(const unsigned int RecePackSize,const unsigned int ReceBuffSize)
{
	pthread_mutex_lock(&m_NetReadlock);
	memset(&m_RBuff,0,sizeof(tagNetBuff));

	//初始化网络包大小
	if(RecePackSize>TCPMAXPACKSIZE) m_RBuff.PackSize=TCPMAXPACKSIZE;
	else if(RecePackSize<TCPMINPACKSIZE) m_RBuff.PackSize=TCPMINPACKSIZE;
	else m_RBuff.PackSize=RecePackSize;

	//初始化接收缓冲区大小
	if(ReceBuffSize>TCPMAXBUFFERSIZE) m_RBuff.BuffSize=TCPMAXBUFFERSIZE;
	else if(ReceBuffSize<TCPMINBUFFERSIZE) m_RBuff.BuffSize=TCPMINBUFFERSIZE;
	else m_RBuff.BuffSize=ReceBuffSize;

	if(m_RBuff.pBuff!=NULL){delete[]m_RBuff.pBuff;m_RBuff.pBuff=NULL;}
	m_RBuff.pBuff = new char[m_RBuff.BuffSize];
	m_RBuff.MaxPackNum = m_RBuff.BuffSize/m_RBuff.PackSize;

	m_RBuff.pPack = new tagNetData[m_RBuff.MaxPackNum];
	for(int i=0;i<m_RBuff.MaxPackNum;i++)
		m_RBuff.pPack[i].pData=m_RBuff.pBuff+m_RBuff.PackSize*i;

	pthread_mutex_unlock(&m_NetReadlock);
}

//************************************
// Method:    initSendThreadBuff
// FullName:  CTCPServer::initSendThreadBuff
// Access:    private 
// Returns:   void
// Qualifier: 初始化发送缓冲区（只在构造函数调用一次）
// Parameter: const unsigned int SendPackSize	发送网络包大小
// Parameter: const unsigned int SendBuffSize	发送缓冲区大小
//************************************
void CTCPServer::initSendThreadBuff(const unsigned int SendPackSize,const unsigned int SendBuffSize)
{
	pthread_mutex_lock(&m_NetSendlock);
	memset(&m_SBuff,0,sizeof(tagNetBuff));
	//初始化网络包大小
	if(SendPackSize>TCPMAXPACKSIZE) m_SBuff.PackSize=TCPMAXPACKSIZE;
	else if(SendPackSize<TCPMINPACKSIZE) m_SBuff.PackSize=TCPMINPACKSIZE;
	else m_SBuff.PackSize=SendPackSize;

	//初始化接收缓冲区大小
	if(SendBuffSize>TCPMAXBUFFERSIZE) m_SBuff.BuffSize=TCPMAXBUFFERSIZE;
	else if(SendBuffSize<TCPMINBUFFERSIZE) m_SBuff.BuffSize=TCPMINBUFFERSIZE;
	else m_SBuff.BuffSize=SendBuffSize;

	if(m_SBuff.pBuff!=NULL){delete[]m_SBuff.pBuff;m_SBuff.pBuff=NULL;}
	m_SBuff.pBuff = new char[m_SBuff.BuffSize];
	m_SBuff.MaxPackNum = m_SBuff.BuffSize/m_SBuff.PackSize;

	m_SBuff.pPack = new tagNetData[m_SBuff.MaxPackNum];
	for(int i=0;i<m_SBuff.MaxPackNum;i++)
		m_SBuff.pPack[i].pData=m_SBuff.pBuff+m_SBuff.PackSize*i;

	pthread_mutex_unlock(&m_NetSendlock);
}
CTCPServer::~CTCPServer()
{
	EndStartThread();	
	DoStopServer();
	pthread_mutex_destroy(&m_Mange.lock);
	for (int i = 0; i < MAXTHREADNUM; i++)
	{
		pthread_mutex_destroy(&m_threads[i].Mange.lock);
		delete[] m_threads[i].pBuff;
	}

#ifdef WIN32
	//if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
	WSACleanup(); // 终止Winsock 2 DLL (Ws2_32.dll) 的使用
#endif

	pthread_mutex_destroy(&loglock);
	pthread_mutex_destroy(&m_NetSendlock);
	pthread_mutex_destroy(&m_NetReadlock);
	pthread_mutex_destroy(&m_OnDisLock);

	if(m_RBuff.pBuff!=NULL){delete[]m_RBuff.pBuff;m_RBuff.pBuff=NULL;}
	if(m_RBuff.pPack!=NULL){delete[]m_RBuff.pPack;m_RBuff.pPack=NULL;}

	if(m_SBuff.pBuff!=NULL){delete[]m_SBuff.pBuff;m_SBuff.pBuff=NULL;}
	if(m_SBuff.pPack!=NULL){delete[]m_SBuff.pPack;m_SBuff.pPack=NULL;}
}
//************************************
// Method:    ClearStartErrorStatus
// FullName:  CTCPServer::ClearStartErrorStatus
// Access:    private 
// Returns:   void
// Qualifier: TCP服务启动失败后的清理
//************************************
void  CTCPServer::ClearStartErrorStatus()
{	
	if(m_Mange.base != NULL) {event_base_free(m_Mange.base);m_Mange.base=NULL;}
	if(m_Mange.plistener!=NULL){evconnlistener_free(m_Mange.plistener);m_Mange.plistener = NULL;}
	for (int i = 0; i < MAXTHREADNUM; i++)
	{		
		if(m_threads[i].base!=NULL){event_base_free(m_threads[i].base);m_threads[i].base = NULL;}
	}
	m_bReadSetEnd = true;
	m_bSendSetEnd = true;
}
//************************************
// Method:    StartServer
// FullName:  CTCPServer::StartServer
// Access:    public 
// Returns:   bool
// Qualifier: TCP服务端启动函数
// Parameter: const char * IP						/*服务IP地址，为NULL或指向空字符串时不绑定指定IP*/
// Parameter: const unsigned short Port				/*绑定服务端口*/
// Parameter: const unsigned int iMaxNumConnections/*允许最大连接数*/
// Parameter: const bool bNeedPack					/*接收是否需要组包cdyw网络包格式,true 需要组包 */
// Parameter: const bool bNeedLogoin				/*客户端是否需要登录*/
// Parameter: const unsigned int ReceBuffSize		/*接收缓冲区大小*/
// Parameter: const unsigned int SendBuffSize		/*发送缓冲区大小*/
//************************************
bool CTCPServer::StartServer(const char *IP,		
	const unsigned short Port,						
	const unsigned int iMaxNumConnections,			
	const bool bNeedPack,							
	const bool bNeedLogoin)			
{
	if(IP==NULL)
		m_StartMessage.LocalIP = EMPTYIP;		
	else
		m_StartMessage.LocalIP = IP;
	if(m_StartMessage.LocalIP=="")
		m_StartMessage.LocalIP = EMPTYIP;

	m_StartMessage.LocalPort =Port;
	m_StartMessage.iMaxNumConnections = iMaxNumConnections;
	m_StartMessage.bNeedPack = bNeedPack;
	m_StartMessage.bNeedLogoin = bNeedLogoin;

	m_bReceiveStartFlag = true;		//设置启动标识
	if( m_StartThreadRunning == false )//如果启动线程未启动运行
	{
		/*pthread_t tid;
		pthread_create(&tid, NULL, thread_startAndMonitor, m_Mange.pMain);*/

		if (m_startAndMonitorThreadHandle != NULL) {CloseHandle(m_startAndMonitorThreadHandle); m_startAndMonitorThreadHandle = NULL;}
		m_startAndMonitorThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_startAndMonitor, (LPVOID)m_Mange.pMain, 0, NULL);
	}
	
	return true;
}
//************************************
// Method:    IsWork
// FullName:  CTCPServer::IsWork
// Access:    public 
// Returns:   bool
// Qualifier: 查询TCP服务是否启动成功
//************************************
bool CTCPServer::IsWork()
{
	return m_Mange.bRunning;
}
void CTCPServer::EndStartThread()
{
	int TCnt=200;
	if(m_StartThreadRunning)//如果启动线程正在运行
	{
		m_bStartThreadEndSignal = true;//发送启动线程结束信号，让启动线程自动退出
		while(m_StartThreadRunning&&(TCnt--)) Sleep(20);//等待启动线程退出
	}
}
//************************************
// Method:    StopServer
// FullName:  CTCPServer::StopServer
// Access:    public 
// Returns:   void
// Qualifier: 停止TCP服务
//************************************
void CTCPServer::StopServer()
{
	EndStartThread();
	DoStopServer();
}
void CTCPServer:: DoStopServer()
{
	//停止连接监听
	if(m_Mange.plistener!=NULL && m_Mange.bRunning)
		evconnlistener_disable(m_Mange.plistener);

	//停止主线程事件基循环
	if(m_Mange.base!=NULL && m_Mange.bRunning)
	{
		event_base_loopbreak(m_Mange.base);//base循环必须在一次事件发生时才能退出
		CLOSENOMANGESOCKET(m_Mange.listener_fd)
	}

	//停止线程池中所有线程
	for (int i = 0; i < MAXTHREADNUM; i++)
	{
		if(m_threads[i].base!=NULL&&m_threads[i].bRunning)
		{
			event_base_loopbreak(m_threads[i].base);//base循环必须在一次事件发生时才能退出

			//触发一个事件让线程退出
#ifdef WIN32
			send(m_threads[i].write_fd, " ", 1,0);
#else
			write(m_threads[i].write_fd, " ", 1);
#endif
		}
	}

	int TCnt=200;
	//等待Master线程退出
	while(m_Mange.bRunning&&(TCnt--)){Sleep(20);}

	//等待Work线程退出
	for (int i = 0; i < MAXTHREADNUM; i++)
	{
		TCnt=200;
		while(m_threads[i].bRunning&&(TCnt--)){Sleep(20);}
	}

	m_bReadSetEnd = true;
	//等待数据处理线程退出
	TCnt=200;
	while(m_bReadThreadRunning&&(TCnt--)){Sleep(20);}

	m_bSendSetEnd = true;
	//等待数据发送线程退出
	TCnt=200;
	while(m_bSendThreadRunning&&(TCnt--)){Sleep(20);}

	//删除已连接的所有客户端
	CloseSocketAndRemove(0,true);

	for (int i = 0; i < MAXTHREADNUM; i++)
	{		
		if(m_threads[i].base!=NULL){event_base_free(m_threads[i].base);m_threads[i].base = NULL;}
	}
	if(m_Mange.plistener!=NULL){evconnlistener_free(m_Mange.plistener);m_Mange.plistener = NULL;}
	if(m_Mange.base!=NULL){event_base_free(m_Mange.base);m_Mange.base = NULL;}
	
	if(m_bThreadProcessOnEventRunning)
	{
		m_bThreadProcessOnEventEnd = true;
		//等待数据发送线程退出
		TCnt=200;
		while(m_bThreadProcessOnEventRunning&&(TCnt--)){Sleep(20);}
		if (m_ProcessOnEventThreadHandle != NULL) {CloseHandle(m_ProcessOnEventThreadHandle); m_ProcessOnEventThreadHandle = NULL;}
	}	
}

//************************************
// Method:    SendData
// FullName:  CTCPServer::SendData
// Access:    public 
// Returns:   bool
// Qualifier: /*添加待发送数据到发送缓冲区，后续由发送线程处理*/
// Parameter: const char * pData		/*缓冲区地址*/
// Parameter: const unsigned int len	/*发送缓冲区长度*/
// Parameter: int Socket				/*待发送socket，如bAll为true，该值任意*/
// Parameter: bool bAll					/*是否发送给所有连接上的客户端*/
//************************************
bool CTCPServer::SendData(
	const char* pData,			
	/*缓冲区地址*/ const unsigned int len,		
	/*发送缓冲区长度*/ int Socket,/*发送的Socket，非指定的客户端，可以填写0*/ unsigned int iSendUserType/*=0*/)				
{
	if(m_Mange.bRunning)
	{
		AddDataToSendBuff(pData,len,Socket,iSendUserType);
	}
	return true;
}

//************************************
// Method:    RegHandle
// FullName:  CTCPServer::RegHandle
// Access:    public 
// Returns:   void
// Qualifier: 注意网络事件处理指针
// Parameter: ITCPServerSocketNet * pHandle
//************************************
void CTCPServer::RegHandle(ITCPServerSocketNet *pHandle)
{
	if(pHandle!=NULL)
		m_pHandle = pHandle;
}
//************************************
// Method:    DisconnectUser
// FullName:  CTCPServer::DisconnectUser
// Access:    public 
// Returns:   void
// Qualifier: 断开用户端连接（仅关闭socket，后libevent会自动触发error_cb回调）
// Parameter: int Socket
// Parameter: bool all
//************************************
void CTCPServer::DisconnectUser(int Socket,bool all)
{
	OutpuMessage(LOG_TYPE_DEBUG,"DisconnectClient::fd=%d,all=%d",Socket,all);
	pthread_mutex_lock(&m_Mange.lock);	
	for(unsigned int i=0;i<m_Mange.ClientVec.size();i++)
	{	
		if(all==true)
			CLOSENOMANGESOCKET(m_Mange.ClientVec[i].item.fd)
		else if(Socket==m_Mange.ClientVec[i].item.fd)
		{
			CLOSENOMANGESOCKET(m_Mange.ClientVec[i].item.fd)
			break;
		}
	}
	pthread_mutex_unlock(&m_Mange.lock);	
}
//************************************
// Method:    SetLoginSuccess
// FullName:  CTCPServer::SetLoginSuccess
// Access:    public 
// Returns:   void
// Qualifier: 设置用户登录状态
// Parameter: int Socket
// Parameter: int status
//************************************
void CTCPServer::SetLoginSuccess(int Socket,int status)
{
	pthread_mutex_lock(&m_Mange.lock);
	tagUserLogoinStatus* Restatus = (tagUserLogoinStatus*)&status;
	if(m_Mange.ClientVec.size()>0)
	{
		for(int i=m_Mange.ClientVec.size()-1;i>=0;i--)
		{
			if(m_Mange.ClientVec[i].item.fd==Socket)
			{
				m_Mange.ClientVec[i].iLogoin = Restatus->LogoinStatus;
				if(Restatus->WebSocket != -2)	//不赋值，用于等待WEB自动关闭Socket，超时后PT强行关闭Socket
				{
					m_Mange.ClientVec[i].bWebSocket = Restatus->WebSocket==0?false:true;
				}
				else
				{
					;//	//初始化连接时间
				}
				m_Mange.ClientVec[i].bWebSocket = Restatus->WebSocket==0?false:true;

				m_Mange.ClientVec[i].SendUserType = Restatus->SendUserType;
				if (m_Mange.ClientVec[i].SendUserType.cTypeBuf == 0 || m_Mange.ClientVec[i].SendUserType.all)
				{
					m_Mange.ClientVec[i].SendUserType.cTypeBuf = 0;
					m_Mange.ClientVec[i].SendUserType.Socket = 1;
				}				
				break;
			}
		}		
	}
	pthread_mutex_unlock(&m_Mange.lock);
}
//************************************
// Method:    WriteDataToSocket
// FullName:  CTCPServer::WriteDataToSocket
// Access:    public 
// Returns:   bool
// Qualifier: 写数据到socket发送缓冲区，后续libevent自动发送
// Parameter: const char * buff
// Parameter: unsigned int len
// Parameter: tagClient & client
//************************************
bool CTCPServer::WriteDataToSocket(const char *buff,unsigned int len,tagClient &client)
{	
	bool ret=false;
	if(client.iLogoin >LOGOINCLOSE && client.item.fd!=INVALID_SOCKET && client.bev!=NULL)		//必须发送过登录命令才能给他发数据
	{	
		if( (bufferevent_get_enabled(client.bev)&EV_WRITE) )
			if(bufferevent_write(client.bev,(void *)buff,len)==0)
				ret = true;
	}	
	return ret;
}


#define MAXLOGSIZE	500
//************************************
// Method:    OutpuMessage
// FullName:  CTCPServer::OutpuMessage
// Access:    private 
// Returns:   int
// Qualifier: LOG信息输出函数，注意LOG_TYPE_NETTEST为网络调试时才输出
// Parameter: int LOGTYPE
// Parameter: const char * format
// Parameter: ...
//************************************
int CTCPServer::OutpuMessage(int LOGTYPE,const char *format, ...)
{
	pthread_mutex_lock(&loglock);
	va_list args;
	va_start(args,   format);
	char _mess[MAXLOGSIZE+10]={0};
	memset(_mess,0,sizeof(_mess));
	int ret=_vsprintf_p(_mess,   MAXLOGSIZE,format,   args);	
	va_end(args);
	if(ret==-1)
		sprintf(_mess,"CTCPServer:OutpuMessage::无效的LOG输出信息");
	
	if(m_pHandle!=NULL)
		m_pHandle->AddLog(LOGTYPE, _mess);
#ifdef _NET_TEST_

	SYSTEMTIME tm;
	GetLocalTime(&tm);
	char str[MAXLOGSIZE]={0};
	memset(str,0,sizeof(str));
	sprintf(str,"%04d-%02d-%02d %02d:%02d:%02d(%03d): TID=%d,Wid=%d/%d,Rid=%d/%d:%s\n",
		tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond,tm.wMilliseconds,GetCurrentThreadId(),
		m_iDataNum_SendBuff,MAXDATABUFNUM,m_iDataNum_ReadBuff,MAXDATABUFNUM,_mess);
	OutputDebugStringA(str);

#endif	

	pthread_mutex_unlock(&loglock);	
	return 0;
}
//************************************
// Method:    CreateWorkThreadAndStart
// FullName:  CTCPServer::CreateWorkThreadAndStart
// Access:    private 
// Returns:   bool
// Qualifier: 创建连接处理线程（线程池管理）
//************************************
bool CTCPServer::CreateWorkThreadAndStart()
{
    //创建线程池,子线程事件基
    for (int i = 0; i < MAXTHREADNUM; i++)
    {
        if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fd) == -1)
        {
            OutpuMessage(LOG_TYPE_ERROR,"error:evutil_socketpair(),thread index %d!",i);
            return false;
        }
        m_threads[i].read_fd = fd[1];

        m_threads[i].write_fd = fd[0];
		m_threads[i].pMain = m_Mange.pMain;
		m_threads[i].Mange.ConnectVec.clear();
		m_threads[i].base = event_base_new();
		if (m_threads[i].base == NULL)
		{
			OutpuMessage(LOG_TYPE_ERROR,"error:event_base_new()");
			return false;
		}

		m_threads[i].Mange.ConnectVec.clear();
		m_threads[i].pMain = m_Mange.pMain;
		evthread_make_base_notifiable(m_threads[i].base);
		event_set(&m_threads[i].event, m_threads[i].read_fd, EV_READ | EV_PERSIST, libevent_Connect_process, &m_threads[i]);
		event_base_set(m_threads[i].base, &m_threads[i].event);
		if (event_add(&m_threads[i].event, 0) == -1)
		{
			event_base_free(m_threads[i].base);m_threads[i].base=NULL;
			OutpuMessage(LOG_TYPE_ERROR,"error:event_add()");
			return false;
		}
    }
    //pthread_t tid;
    for (int i = 0; i < MAXTHREADNUM; i++)
    {
        //pthread_create(&tid, NULL, thread_worker, &m_threads[i]);

		if (m_threads[i].pThreadHandle != NULL) { CloseHandle(m_threads[i].pThreadHandle); m_threads[i].pThreadHandle = NULL; }
		m_threads[i].pThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_worker, (LPVOID)(&m_threads[i]), 0, NULL);
    }
	//两秒等待所有线程初始化完成
	int cnt=100;//
	while(cnt--)//等待线程执行结果
	{
		bool flag=true;
		for(int i=0;i<MAXTHREADNUM;i++)
		{
			if(m_threads[i].bRunning==false)
				flag =false;
		}
		if(flag==false)	Sleep(20);
		else break;
	}
	if(cnt<=0) return false;
	return true;
}


//************************************
// Method:    thread_main
// FullName:  CTCPServer::thread_main
// Access:    private static 
// Returns:   void *
// Qualifier: 主事件基线程，libevent接受客户端连接
// Parameter: void * arg
//************************************
void * CTCPServer::thread_main( void *arg )
{
	CTCPServer *pThis = (CTCPServer *)arg;
	pThis->ResetReadBuff();
	pThis->ResetSendBuff();
	pThis->m_Mange.bRunning = true;
	pThis->OutpuMessage(LOG_TYPE_DEBUG,"Start Listening server IP:%s:%d",
		pThis->m_Mange.LocalIP.c_str(),pThis->m_Mange.port);
	evconnlistener_enable(pThis->m_Mange.plistener);
	event_base_loop(pThis->m_Mange.base, 0);	
	
	pThis->OutpuMessage(LOG_TYPE_DEBUG,"thread_main end:0x%x",pThis->m_Mange.pThreadHandle);
	pThis->m_Mange.bRunning = false;
	return NULL;
}

//************************************
// Method:    PutItem
// FullName:  CTCPServer::PutItem
// Access:    private 
// Returns:   void
// Qualifier: 添加连接用户到线程池对应处理线程
// Parameter: const tagItem & item
// Parameter: LIBEVENT_THREAD * thread
//************************************
void CTCPServer::PutItem(const tagItem&item,LIBEVENT_THREAD *thread)
{
    pthread_mutex_lock(&thread->Mange.lock);
    thread->Mange.ConnectVec.push_back(item);//新的连接加入到线程连接管理器中
    pthread_mutex_unlock(&thread->Mange.lock);
}

//************************************
// Method:    GetItem
// FullName:  CTCPServer::GetItem
// Access:    private 
// Returns:   bool
// Qualifier: 获取连接上的用户，后续处理
// Parameter: tagItem & item
// Parameter: LIBEVENT_THREAD * thread
//************************************
bool CTCPServer::GetItem(tagItem&item,LIBEVENT_THREAD *thread)
{
    bool ret=false;
    pthread_mutex_lock(&thread->Mange.lock);
    if(thread->Mange.ConnectVec.size()>0)
    {
        item = thread->Mange.ConnectVec[0];
        thread->Mange.ConnectVec.erase(thread->Mange.ConnectVec.begin());
        ret = true;
    }
    pthread_mutex_unlock(&thread->Mange.lock);

    return ret;
}
//************************************
// Method:    accept_conn_cb
// FullName:  CTCPServer::accept_conn_cb
// Access:    private static 
// Returns:   void
// Qualifier: libevent连接响应事件
// Parameter: struct evconnlistener * listener
// Parameter: evutil_socket_t fd
// Parameter: struct sockaddr_in * address
// Parameter: int socklen
// Parameter: void * ctx
//************************************
void CTCPServer::accept_conn_cb(struct evconnlistener *listener,
	evutil_socket_t fd, struct sockaddr_in *address, int socklen,
	void *ctx)
{	
	CTCPServer *pThis = (CTCPServer *)ctx;
	//设置新的连接为非阻塞方式
	evutil_make_socket_nonblocking(fd);
	if(pThis->m_CurConnectNum>=pThis->m_Mange.AllowMaxClientNum)
	{		
		unsigned int CurTime = GetTickCount();
		if((CurTime-pThis->m_PreTime)>5000)
		{
			pThis->OutpuMessage(LOG_TYPE_WARNING,"已达最大允许连接数据(%d),不接受新的网络连接!fd=%d",pThis->m_CurConnectNum,fd);
			pThis->m_PreTime=CurTime;
		}	
		CLOSENOMANGESOCKET(fd)	
		return;
	}
	else
		pThis->m_PreTime = GetTickCount();

	pThis->OutpuMessage(LOG_TYPE_NETTEST,"accept_conn_cb:fd=%d,IP=%s",fd,inet_ntoa(address->sin_addr));
	pThis->ProcessAddClient(fd,*address);	
}
//************************************
// Method:    ProcessAddClient
// FullName:  CTCPServer::ProcessAddClient
// Access:    public 
// Returns:   void
// Qualifier: 处理新客户端添加
//************************************
void CTCPServer::ProcessAddClient(int fd,struct sockaddr_in add)
{
	if(FindClient(fd)==false)
	{		
		tagItem item;
		memset(&item,0,sizeof(item));
		item.fd = fd;
		//接受连接
		strcpy(item.ip,inet_ntoa(add.sin_addr));	
		item.port = htons(add.sin_port);

		//线程调度
		int cnt=0,tid;
		LIBEVENT_THREAD *thread=NULL;
		while(cnt<MAXTHREADNUM)
		{
			tid=m_last_thread++;
			if(m_last_thread>=MAXTHREADNUM) m_last_thread=0;

			if(m_threads[tid].bRunning)
			{
				thread=&m_threads[tid];
				break;
			}
			cnt++;
		}

		if(thread==NULL)//没有可调度的线程则关闭该连接
		{
			CLOSENOMANGESOCKET(fd)
			//g_pMain->AddLogF(LOG_TYPE_ERROR,"线程池中无可调度线程，请检查网络或重启程序！fd=%d",fd);
			return;
		}

		thread->pMain->PutItem(item,thread);

		//发送信号给对应线程，该线程响应后处理新客户连接
#ifdef WIN32
		send(thread->write_fd, " ", 1,0);
#else
		write(thread->write_fd, " ", 1);
#endif	
	}
}
//************************************
// Method:    accept_error_cb
// FullName:  CTCPServer::accept_error_cb
// Access:    private static 
// Returns:   void
// Qualifier: 听错误事件响应函数
// Parameter: struct evconnlistener * listener
// Parameter: void * ctx
//************************************
void CTCPServer::accept_error_cb(struct evconnlistener *listener, void *ctx)
{
	CTCPServer *pThis = (CTCPServer *)ctx;
	struct event_base *base = evconnlistener_get_base(listener);
	int err = EVUTIL_SOCKET_ERROR();
	pThis->OutpuMessage(LOG_TYPE_MESSAGE, "Got an error %d (%s) on the listener. ", err, evutil_socket_error_to_string(err));
}

//************************************
// Method:    event_read
// FullName:  CTCPServer::event_read
// Access:    private static 
// Returns:   void
// Qualifier: 读事件响应函数，线程环境为thread_worker
// Parameter: struct bufferevent * bev
// Parameter: void * arg
//************************************
void CTCPServer::event_read(struct bufferevent *bev, void *arg)
{
    LIBEVENT_THREAD *me = (LIBEVENT_THREAD*)arg;
	CTCPServer *pMain=me->pMain;
	evutil_socket_t fd = bufferevent_getfd(bev);
	struct evbuffer *input = bufferevent_get_input(bev);//
	const size_t len = evbuffer_get_length(input);
	if(len>TCPMAXPACKSIZE)//超过网络最大包
	{
		evbuffer_drain(input,len);
		return;
	}
	//判断数据长度是否有效
	if(len<=0 || fd==INVALID_SOCKET ||input==NULL)//无数据
		return;
	/*if(pMain->m_Mange.port==8115)
		pMain->OutpuMessage(LOG_TYPE_DEBUG,"Rec Data:fd=%d,len=%d",fd,len);	*/
	int ret=-1;
	if(len<1024)//可能是WEB请求
	{
		char buff[1024]={0};
		ret = evbuffer_copyout(input,buff,len);
		if(ret==-1){return;}

		if(buff[0]=='G' && buff[1]=='E'&& buff[2]=='T'&& buff[6]=='H' && buff[7]=='T'&& buff[8]=='T'&& buff[9]=='P')//确认为web请求，响应允许连接
		{
			pMain->OutpuMessage(LOG_TYPE_NETTEST,"event_read:fd=%d,HTML5 Web请求安全验证响应",fd);
			//string stre=buff;
			//stre.find("Sec-WebSocket-Key:");

			//wxString stre(buff,wxConvLocal);
			//int  isecindxs = stre.Find(wxT("Sec-WebSocket-Key:"));
			//wxString strres = stre.Mid(isecindxs+19,24);
			//string strinput= strres.mb_str();
			//string resend=	CWebSokcet::generate_websocket_response(strinput);
			//ret=bufferevent_write(bev, resend.c_str(), strlen(resend.c_str()));//往该socket回写数据
			//if(ret==-1){return;}
			//ret=evbuffer_drain(input,len);//清除数据，不缓存
			//if(ret==-1){return;}
			return;
		}
		else if(buff[0]=='<' && buff[21]=='>')//确认为web请求，响应允许连接
		{
			evbuffer_drain(input,len);//清除数据，不缓存
			pMain->OutpuMessage(LOG_TYPE_NETTEST,"event_read:fd=%d,Flex Web request",fd);
			const char *pStr="<?xml version=\"1.0\"?><cross-domain-policy><site-control permitted-cross-domain-policies=\"all\"/><allow-access-from domain=\"*\" to-ports=\"*\"/></cross-domain-policy>";
			bufferevent_write(bev, pStr, strlen(pStr)+1);//往该socket回写数据					
			return;
		}
	}

	//处理数据
	if(!pMain->m_Mange.bNeedPack)//不需要组包数据直接外传
	{
		ret=evbuffer_remove(input,(void*)me->pBuff,len);
		pMain->AddDataToReadBuff(me->pBuff,len,fd);			
	}
	else//需要数据组包
	{
		size_t relen=len;//剩余数据量
		LibeventTCP::tagNetPackHead *pHead=(LibeventTCP::tagNetPackHead *)me->pBuff;
		LibeventTCP::tagNetPackEnd *pEnd;
		while (relen>sizeof(LibeventTCP::tagNetPackHead))
		{
			//从缓冲区复制数据但不删除
			ret=evbuffer_copyout(input,me->pBuff,sizeof(LibeventTCP::tagNetPackHead));
			if(ret==-1){return;}

			size_t WebSockeHeadSize=0;
			unsigned char WebSockFlag = me->pBuff[0];
			if ( WebSockFlag == 0x82)//WebSocket的头
			{				
				ret=evbuffer_copyout(input,me->pBuff,len);//将所有数据全部拷贝出来
				CWebSokcet::tryUnMarkIncomingPacket(me->pBuff,len,WebSockeHeadSize);//转换

			}
			if(pHead->SOI==NET_SOI)//包头标识正确
			{
				if(relen>=pHead->DataPackSize)//缓冲区中剩余数据量够组包
				{
					if (WebSockeHeadSize == 0)	//不是WEbSocket
					{
						ret=evbuffer_remove(input,me->pBuff,pHead->DataPackSize);//从缓冲区复制数据，并从源缓冲区删除指定长度数据
						if(ret==-1){return;}
					}
					else	//由于已经把数据转换出来，这里把buf中的数据删除
					{
						ret=evbuffer_drain(input,pHead->DataPackSize+WebSockeHeadSize);
						if(ret==-1){return;}
					}

					pEnd = (LibeventTCP::tagNetPackEnd *)(me->pBuff+pHead->DataPackSize-sizeof(LibeventTCP::tagNetPackEnd));
					if(pEnd->EOI==NET_EOI)//包尾标识正确
					{
						pMain->AddDataToReadBuff(me->pBuff,pHead->DataPackSize,fd);
						relen = evbuffer_get_length(input);//得到缓冲区中剩余数据量
						continue;//下次循环继续处理
					}
					else//尾标识错误，丢弃所有数据
					{
						return;
					}
				}
				else//数据量不够组包，留作下次处理，返回
				{
					return;
				}
			}
			else	//包头错误，当前缓冲区数据不能组包（无效），删除缓冲区中数据，并返回
			{
				ret=evbuffer_drain(input,relen);
				if(ret==-1){return;}
				return;
			}			
		}			
	}
}


//************************************
// Method:    error_cb
// FullName:  CTCPServer::error_cb
// Access:    private static 
// Returns:   void
// Qualifier: 连接断开事件响应函数，线程环境为thread_worker，主动或被动关闭socket会响应该函数
// Parameter: struct bufferevent * bev
// Parameter: short event
// Parameter: void * arg
//************************************
void CTCPServer::event_cb(struct bufferevent *bev, short event, void *arg)
{
	if ( (event&BEV_EVENT_EOF)||(event&BEV_EVENT_ERROR)||(event&BEV_EVENT_TIMEOUT))
	{
		bufferevent_disable(bev,EV_READ | EV_WRITE);
		LIBEVENT_THREAD *me = (LIBEVENT_THREAD*)arg;
		CTCPServer *pMain=me->pMain;
		evutil_socket_t fd = bufferevent_getfd(bev);
		int errid=EVUTIL_SOCKET_ERROR();
		pMain->OutpuMessage(LOG_TYPE_NETTEST,"网络断开事件响应:fd=%d,event=0x%04x,errid=%d:%s",
			fd,event,errid,evutil_socket_error_to_string(errid));

		pthread_mutex_lock(&pMain->m_Mange.lock);
		vector<tagClient>::iterator it=pMain->m_Mange.ClientVec.begin();
		while(it!=pMain->m_Mange.ClientVec.end())
		{
			if(it->item.fd==fd){it->iLogoin = LOGOINCLOSE;break;}
			it++;
		}
		pthread_mutex_unlock(&pMain->m_Mange.lock);		
	}
}
//************************************
// Method:    InserClient
// FullName:  CTCPServer::InserClient
// Access:    private 
// Returns:   void
// Qualifier: 添加新用户到用户端列表
// Parameter: const tagClient & client
// Parameter: void * arg
//************************************
void CTCPServer::InserClient( const LIBEVENT_THREAD *me,const tagItem &item)
{
	tagClient client;
	memset(&client,0,sizeof(client));
	client.item = item;
	client.ThreadID = GetCurrentThreadId();

	evutil_make_socket_nonblocking(client.item.fd);
	
	//为连接上到socket创建一个关联buffer
	client.bev = bufferevent_socket_new(me->base, item.fd, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE|BEV_OPT_DEFER_CALLBACKS);	
	if(client.bev==NULL)
	{
		CLOSENOMANGESOCKET(client.item.fd);
		OutpuMessage(LOG_TYPE_WARNING,"bufferevent_socket_new err（%d）",item.fd);
		return;
	}
	bufferevent_setcb(client.bev, event_read, NULL, event_cb, (void*)me);//设置事件响应回调函数

	if(m_Mange.bNeedLogoin)
		client.iLogoin = NOLOGOIN;
	else
		client.iLogoin = LOGOINTRUE;//如不需登录过程，则直接设置为登录正常
	client.bSendClose = false;
	client.ActiveTime = GetTickCount();//初始化连接活动时间
	
	pthread_mutex_lock(&m_Mange.lock);
	m_Mange.ClientVec.push_back(client);//新的连接加入到已连接管理器中
	m_CurConnectNum = m_Mange.ClientVec.size();
	pthread_mutex_unlock(&m_Mange.lock);

	int ret=bufferevent_enable(client.bev, EV_READ|EV_WRITE);//启用read/write事件
	if(ret==-1)
	{
		evutil_closesocket(item.fd);
		OutpuMessage(LOG_TYPE_WARNING,"bufferevent_enable err（%d）",item.fd);
		return ;
	}
	OutpuMessage(LOG_TYPE_NETTEST,"++网络连接成功:(%d)fd=%d,bev=0x%08x",m_CurConnectNum,item.fd,client.bev);	
	
	if(m_pHandle!=NULL)
		m_pHandle->OnConnected(item.fd,client.item.ip);
	
	
}

//************************************
// Method:    CloseSocketAndRemove
// FullName:  CTCPServer::CloseSocketAndRemove
// Access:    private 
// Returns:   void
// Qualifier: 关闭、删除客户端，指定客户端或全部客户端
// Parameter: int fd
// Parameter: bool bAll
//************************************
void CTCPServer::CloseSocketAndRemove(int fd,bool bAll)
{
    pthread_mutex_lock(&m_Mange.lock);
    vector<tagClient>::iterator it=m_Mange.ClientVec.begin();
    while(it!=m_Mange.ClientVec.end())
    {
        if(bAll || (it->item.fd==fd))
        {
			OutpuMessage(LOG_TYPE_NETTEST,"(T=%d)CloseSocketAndRemove: fd=%u,IP=%s,Port=%u,bev=0x%08x",
				m_Mange.ClientVec.size(),it->item.fd,it->item.ip,it->item.port,it->bev);
			  
			FreebevAndNotiyDisconnect(*it,true);				   
            m_Mange.ClientVec.erase(it);		
			if(bAll)
			{
				it=m_Mange.ClientVec.begin();
				continue;
			}
			else//found
				break;
	    }
		it++;
    }
	if(bAll)
	{
		m_Mange.ClientVec.clear();
		vector<tagClient>().swap(m_Mange.ClientVec);
	}

	m_CurConnectNum = m_Mange.ClientVec.size();
    pthread_mutex_unlock(&m_Mange.lock);
}

//************************************
// Method:    FindClientAndSetAct
// FullName:  CTCPServer::FindClientAndSetAct
// Access:    private 
// Returns:   bool
// Qualifier: 查找客户端，并设置为活动状态（该函数在read处理线程中调用，只有响应了数据才能被调用）
// Parameter: int fd
// Parameter: tagClient & client
//************************************
bool CTCPServer::FindClientAndSetAct(int fd,tagClient &client)
{
	bool ret=false;
	pthread_mutex_lock(&m_Mange.lock);
	vector<tagClient>::iterator it=m_Mange.ClientVec.begin();
	while(it!=m_Mange.ClientVec.end())
	{
		if(it->item.fd==fd)
		{
			it->ActiveTime = GetTickCount();//初始化连接活动时间
			client = *it;			
			ret = true;
			break;
		}
		it++;
	}
	pthread_mutex_unlock(&m_Mange.lock);
	return ret;
}

//************************************
// Method:    FindClient
// FullName:  CTCPServer::FindClient
// Access:    public 
// Returns:   bool
// Qualifier: 查找客户端socket释放已经存在
// Parameter: int fd
//************************************
bool CTCPServer::FindClient(int fd)
{
	pthread_mutex_lock(&m_Mange.lock);
	bool ret=false;	
	vector<tagClient>::iterator it=m_Mange.ClientVec.begin();
	while(it!=m_Mange.ClientVec.end())
	{
		if(it->item.fd==fd)
		{
			ret = true;
			break;
		}
		it++;
	}
	pthread_mutex_unlock(&m_Mange.lock);
	return ret;
}

//************************************
// Method:    CloseNotActiveClient
// FullName:  CTCPServer::CloseNotActiveClient
// Access:    private 
// Returns:   int
// Qualifier: 关闭不活动客户端
//************************************
bool CTCPServer::CheckAndCloseNotActiveClient(bool check)
{
	bool ret = true;
	if(check)
	{
		pthread_mutex_lock(&m_Mange.lock);
		unsigned long CurTime;	
		CurTime = GetTickCount();
		vector<tagClient>::iterator it=m_Mange.ClientVec.begin();
		while(it!=m_Mange.ClientVec.end())
		{			
			if((CurTime-it->ActiveTime)>CLIENTTIMEOUT && it->item.fd != INVALID_SOCKET && it->iLogoin != LOGOINCLOSE && it->bSendClose==false)//超时处理
			{
				bufferevent_disable(it->bev,EV_READ | EV_WRITE);
				if(it->iLogoin==LOGOINTRUE)	//活动超时
					OutpuMessage(LOG_TYPE_WARNING,"该客户端(%s:%d:%d:0x%08x)活动超时,断开连接！",it->item.ip,it->item.fd,it->ThreadID,it->bev);
				else 
					OutpuMessage(LOG_TYPE_WARNING,"该客户端(%s:%d:%d:0x%08x)超时%d,断开连接！",it->item.ip,it->item.fd,it->ThreadID,it->bev,it->iLogoin);
				CLOSENOMANGESOCKET(it->item.fd);
				it->bSendClose = true;
				ret = false;//有异常客户端
			}						
			it++;
		}
		pthread_mutex_unlock(&m_Mange.lock);
	}
	else
	{
		pthread_mutex_lock(&m_Mange.lock);
		vector<tagClient>::iterator it=m_Mange.ClientVec.begin();
		while(it!=m_Mange.ClientVec.end())
		{			
			if( it->iLogoin == LOGOINCLOSE || it->bSendClose)
			{
				OutpuMessage(LOG_TYPE_NETTEST,"(T=%d)CheckAndCloseNotActiveClient: fd=%u,IP=%s,Port=%u,bev=0x%08x",
					m_Mange.ClientVec.size()-1,it->item.fd,it->item.ip,it->item.port,it->bev);

				AddFdToOnDisconnectVect(it->item.fd);
				if(it->bev!=NULL){bufferevent_free(it->bev);it->bev=NULL;}				   
				m_Mange.ClientVec.erase(it);
				it=m_Mange.ClientVec.begin();				
				continue;
			}						
			it++;
		}
		m_CurConnectNum = m_Mange.ClientVec.size();
		pthread_mutex_unlock(&m_Mange.lock);
	}	
	return ret;
}

//************************************
// Method:    FreebevAndNotiyDisconnect
// FullName:  CTCPServer::FreebevAndNotiyDisconnect
// Access:    private 
// Returns:   void
// Qualifier: 关闭socket，并删除资源
// Parameter: tagClient & Client
// Parameter: bool bOnDisEvent
//************************************
void CTCPServer::FreebevAndNotiyDisconnect(tagClient&Client,bool bOnDisEvent)
{
	OutpuMessage(LOG_TYPE_NETTEST,"--FreebevAndNotiyDisconnect: (%d)fd=%u,IP=%s,Port=%u,bev=0x%08x",m_Mange.ClientVec.size()-1,
		Client.item.fd,Client.item.ip,Client.item.port,Client.bev);		
	if(Client.bev!=NULL){bufferevent_free(Client.bev);Client.bev=NULL;}
	CLOSENOMANGESOCKET(Client.item.fd);
	if(bOnDisEvent)
		AddFdToOnDisconnectVect(Client.item.fd);	
}

//************************************
// Method:    libevent_Connect_process
// FullName:  CTCPServer::libevent_Connect_process
// Access:    private static 
// Returns:   void
// Qualifier: 线程池，连接处理事件响应
// Parameter: int fd
// Parameter: short which
// Parameter: void * arg
//************************************
void CTCPServer::libevent_Connect_process(int fd, short which, void *arg)
{
    LIBEVENT_THREAD *me = (LIBEVENT_THREAD*)arg;
    CTCPServer *pMain=me->pMain;
    if (fd != me->read_fd)
    {
        pMain->OutpuMessage(LOG_TYPE_DEBUG,"libevent_Connect_process error : fd != me->read_fd");
        return;
    }
    char buf[1];
	int ret;
#ifdef WIN32
	ret = recv(fd, buf, 1,0);
#else
	ret = read(fd, buf, 1);
#endif 

    if(ret!=1)
    {
        pMain->OutpuMessage(LOG_TYPE_DEBUG,"Can't read from libevent pipe");
        return;
    }
    tagItem item;
    if(pMain->GetItem(item,me))
    {
		pMain->InserClient(me,item);  		
     }
    return;
}

//************************************
// Method:    thread_worker
// FullName:  CTCPServer::thread_worker
// Access:    private static 
// Returns:   void *
// Qualifier: 线程池中线程函数
// Parameter: void * arg
//************************************
void *CTCPServer::thread_worker(void *arg)
{
    LIBEVENT_THREAD *me = (LIBEVENT_THREAD*)arg;
	CTCPServer *pMain=me->pMain;
	me->bRunning = true;
	pMain->OutpuMessage(LOG_TYPE_DEBUG,"worker_thread(0x%x)::start...", me->pThreadHandle);

	int ret=event_base_loop(me->base, 0);
	int errid=EVUTIL_SOCKET_ERROR();

	me->bRunning = false;
	for(size_t i=0;i<me->Mange.ConnectVec.size();i++)//清除挂起的连接
		CLOSENOMANGESOCKET(me->Mange.ConnectVec[i].fd);
	pMain->OutpuMessage(LOG_TYPE_DEBUG,"worker_thread(%d)::end...(0x%x)%s", me->pThreadHandle,errid,evutil_socket_error_to_string(errid));
    return NULL;
}


//************************************
// Method:    libeventfatal_cb
// FullName:  CTCPServer::libeventfatal_cb
// Access:    private static 
// Returns:   void
// Qualifier: libevent异常响应处理函数
// Parameter: int err
//************************************
void CTCPServer::libeventfatal_cb(int err)
{
	//g_pMain->AddLogF(LOG_TYPE_ERROR,"发送网络致命错误，请重启程序!error id=%d",err);
	return;
}

////////////////////////////////////////////////////////////////////////////////////
//************************************
// Method:    AddDataToReadBuff
// FullName:  CTCPServer::AddDataToReadBuff
// Access:    public 
// Returns:   void
// Qualifier: 添加数据到网络处理缓冲区
// Parameter: const char * pData
// Parameter: const unsigned int len
// Parameter: const int socket
//************************************
void CTCPServer::AddDataToReadBuff(const char *pData,const unsigned int len,const int socket)
{
	if(len>m_RBuff.PackSize)
	{
		OutpuMessage(LOG_TYPE_WARNING,"读处理缓冲区S：待处理数据包大小超出允许范围:%d/%d(Bytes)",len,m_RBuff.PackSize);
		return;
	}
	if(m_RBuff.PackNum>=m_RBuff.MaxPackNum)
	{
		OutpuMessage(LOG_TYPE_WARNING,"读处理缓冲区S：待处理缓存包数已达最大值,%d/%d",m_RBuff.PackNum,m_RBuff.MaxPackNum);
		m_bReadError = true;
		return;
	}
	pthread_mutex_lock(&m_NetReadlock);
	if(m_bReadThreadRunning)
	{
		m_RBuff.pPack[m_RBuff.wIndex].len = len;
		m_RBuff.pPack[m_RBuff.wIndex].socket = socket;
		memcpy(m_RBuff.pPack[m_RBuff.wIndex].pData,pData,len);
		m_RBuff.wIndex++;

		m_RBuff.PackNum++;	
		if (m_RBuff.wIndex>=m_RBuff.MaxPackNum)
			m_RBuff.wIndex = 0;
	}	
	pthread_mutex_unlock(&m_NetReadlock);
	m_bReadError = false;
}
//************************************
// Method:    GetDataFromReadBuff
// FullName:  CTCPServer::GetDataFromReadBuff
// Access:    public 
// Returns:   bool
// Qualifier: 从网络缓冲区获取数据包
// Parameter: tagNetData & NetData
//************************************
bool CTCPServer::GetDataFromReadBuff(tagNetData &NetData)
{
	if(m_RBuff.PackNum<=0) return false;

	pthread_mutex_lock(&m_NetReadlock);
	NetData.len = m_RBuff.pPack[m_RBuff.rIndex].len;
	NetData.socket = m_RBuff.pPack[m_RBuff.rIndex].socket;
	memcpy(NetData.pData,m_RBuff.pPack[m_RBuff.rIndex].pData,NetData.len);

	m_RBuff.rIndex++;
	m_RBuff.PackNum--;
	if (m_RBuff.rIndex>=m_RBuff.MaxPackNum)
		m_RBuff.rIndex = 0;
	pthread_mutex_unlock(&m_NetReadlock);
	return true;
}
//************************************
// Method:    thread_readprocess
// FullName:  CTCPServer::thread_readprocess
// Access:    public static 
// Returns:   void *
// Qualifier: 读取到的数据线程处理函数
// Parameter: void * arg
//************************************
void *CTCPServer::thread_readprocess(void *arg)
{
	CTCPServer *pMain = (CTCPServer *)arg;
	pMain->m_bReadThreadRunning = true;
	pMain->m_bReadSetEnd = false;
	pMain->ResetReadBuff();
	tagNetData Data;
	Data.pData = new char[TCPMAXPACKSIZE];
	tagClient client;

	LibeventTCP::tagActPack actPack;
	memset(&actPack,0,sizeof(LibeventTCP::tagActPack));
	actPack.head.SOI = NET_SOI;
	actPack.head.DataPackSize = sizeof(LibeventTCP::tagActPack);
	actPack.head.DataType = NET_DATA_TYPE_ACTPACK;
	actPack.end.EOI = NET_EOI;

	tagNetData SendBuf;
	SendBuf.pData = new char[TCPMAXPACKSIZE];
	CWebSokcet::ChangeDataToWebSocket((char*)&actPack,sizeof(LibeventTCP::tagActPack),
										&SendBuf.pBaseData,SendBuf.baselen,
										SendBuf.pData,SendBuf.len,
										TCPMAXPACKSIZE);
	while(pMain->m_bReadSetEnd==false)
	{
		if(pMain->GetDataFromReadBuff(Data))
		{
			if(pMain->FindClientAndSetAct(Data.socket,client) && pMain->m_pHandle!=NULL)
			{
				if(pMain->m_Mange.bNeedPack)
				{
					//检查是否心跳包，是则过滤，心跳包上层不需要处理
					//同时若是心跳包，自动响应一个心跳包给客户端
					const LibeventTCP::tagNetPackHead *pHead  = (const LibeventTCP::tagNetPackHead *)Data.pData;
					if(pHead->DataType==NET_DATA_TYPE_ACTPACK)
					{
 						if (client.bWebSocket)
 						{
 							pMain->WriteDataToSocket((char*)SendBuf.pData,SendBuf.len,client);
 						}
 						else
						{
							/*if (pMain->m_Mange.port==8115)
								pMain->OutpuMessage(LOG_TYPE_DEBUG,"Rec Act:fd=%d,PickID=%d",client.item.fd,pHead->PackID);	*/
							pMain->WriteDataToSocket((char*)Data.pData,Data.len,client);
						}						
						continue;
					}
				}

				pMain->m_pHandle->OnReceiveData(Data.pData,Data.len,Data.socket,client.item.ip);
			}
			/////
			if(pMain->m_RBuff.PackNum>0) continue;
		}
		Sleep(50);
	}

//END_READPROCESS:
	pMain->m_bReadThreadRunning = false;
	pthread_mutex_lock(&pMain->m_NetReadlock);
	if(Data.pData!=NULL){delete[]Data.pData;Data.pData=NULL;}
	if(SendBuf.pData!=NULL){delete[]SendBuf.pData;SendBuf.pData=NULL;}
	pthread_mutex_unlock(&pMain->m_NetReadlock);	
	
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////
//************************************
// Method:    AddDataToSendBuff
// FullName:  CTCPServer::AddDataToSendBuff
// Access:    public 
// Returns:   void
// Qualifier: 添加数据包到发送缓冲区
// Parameter: const char * pData
// Parameter: const unsigned int len
// Parameter: const int socket
//************************************
void CTCPServer::AddDataToSendBuff(const char *pData,const unsigned int len,const int socket, const unsigned char sendusertype)
{
	if(m_bSendThreadRunning==false)
		return;
	if(len>m_SBuff.PackSize || len<=4)
	{
		OutpuMessage(LOG_TYPE_WARNING,"发送处理缓冲区S：待处理数据包大小超出允许范围:%d/%d(Bytes)",len,m_SBuff.PackSize);
		return;
	}
	if(m_SBuff.PackNum>=m_SBuff.MaxPackNum)
	{
		OutpuMessage(LOG_TYPE_WARNING,"发送处理缓冲区S：待处理缓存包数已达最大值,%d/%d",m_SBuff.PackNum,m_SBuff.MaxPackNum);
		m_bWriteError = true;
		return;
	}
	
	pthread_mutex_lock(&m_NetSendlock);
	if(m_bSendThreadRunning)
	{
		//这里转换WebSocket
		CWebSokcet::ChangeDataToWebSocket(pData,len,
			&m_SBuff.pPack[m_SBuff.wIndex].pBaseData,m_SBuff.pPack[m_SBuff.wIndex].baselen,
			m_SBuff.pPack[m_SBuff.wIndex].pData,m_SBuff.pPack[m_SBuff.wIndex].len,
			m_SBuff.PackSize);
		m_SBuff.pPack[m_SBuff.wIndex].socket = socket;
		m_SBuff.pPack[m_SBuff.wIndex].sendUserType.cTypeBuf = sendusertype;
		if (m_SBuff.pPack[m_SBuff.wIndex].sendUserType.cTypeBuf == 0 )	//防止数据发送时忘记赋值
		{
			//兼容以前的逻辑
			if ( socket == 0 )
			{
				m_SBuff.pPack[m_SBuff.wIndex].sendUserType.all = 1;
			}
			else
			{
				m_SBuff.pPack[m_SBuff.wIndex].sendUserType.Socket = 1;
			}			
		}
		m_SBuff.wIndex++;
		m_SBuff.PackNum++;	
		if (m_SBuff.wIndex>=m_SBuff.MaxPackNum)
			m_SBuff.wIndex = 0;
	}	
	pthread_mutex_unlock(&m_NetSendlock);
	m_bWriteError = false;
}
//************************************
// Method:    GetDataFromSendBuff
// FullName:  CTCPServer::GetDataFromSendBuff
// Access:    public 
// Returns:   bool
// Qualifier: 从发送缓冲区获取数据包
// Parameter: tagNetData & NetData
//************************************
bool CTCPServer::GetDataFromSendBuff(tagNetData &NetData)
{
	if(m_SBuff.PackNum<=0) return false;

	pthread_mutex_lock(&m_NetSendlock);

	NetData.socket = m_SBuff.pPack[m_SBuff.rIndex].socket;
	NetData.len = m_SBuff.pPack[m_SBuff.rIndex].len;
	NetData.baselen = m_SBuff.pPack[m_SBuff.rIndex].baselen;
	memcpy(NetData.pData,m_SBuff.pPack[m_SBuff.rIndex].pData,NetData.len);
	NetData.pBaseData = m_SBuff.pPack[m_SBuff.rIndex].pBaseData;
	NetData.sendUserType = m_SBuff.pPack[m_SBuff.rIndex].sendUserType;

	m_SBuff.rIndex++;
	m_SBuff.PackNum--;
	if (m_SBuff.rIndex>=m_SBuff.MaxPackNum)
		m_SBuff.rIndex = 0;
	pthread_mutex_unlock(&m_NetSendlock);
	return true;
}
//************************************
// Method:    SysUpdataSendVec
// FullName:  CTCPServer::SysUpdataSendVec
// Access:    public 
// Returns:   void
// Qualifier: 同步发送客户端容器
//************************************
void CTCPServer::SysUpdataSendVec()
{
	pthread_mutex_lock(&m_Mange.lock);
	m_SendVec = m_Mange.ClientVec;
	pthread_mutex_unlock(&m_Mange.lock);
}

//************************************
// Method:    thread_SendAndMainProcess
// FullName:  CTCPServer::thread_SendAndMainProcess
// Access:    public static 
// Returns:   void *
// Qualifier: 发送数据以及主处理线程（发送数据、处理连接、处理断开、处理异常、处理超时）
// Parameter: void * arg
//************************************
void *CTCPServer::thread_SendAndMainProcess(void *arg)
{
	CTCPServer *pMain = (CTCPServer *)arg;
	pMain->m_bSendThreadRunning = true;
	pMain->m_bSendSetEnd = false;
	pMain->ResetSendBuff();
	tagNetData Data;
	Data.pData = new char[pMain->m_SBuff.PackSize];

	unsigned long PreTime,NowTime;
	PreTime= GetTickCount();
	char *pSendPtr;
	size_t iSendSize;
	//unsigned int PreTicket=GetTickCount(),CurTicket;
	while(pMain->m_bSendSetEnd==false)
	{
		//////测试代码
		//CurTicket = GetTickCount();
		//if((CurTicket-PreTicket)>20000)
		//{
		//	pMain->OutpuMessage(LOG_TYPE_WARNING,"(%d)ReadBuff:%d/%d, SendBuff:%d/%d",pMain->m_Mange.ClientVec.size(),
		//		pMain->m_iDataNum_ReadBuff,MAXDATABUFNUM,
		//		pMain->m_iDataNum_SendBuff,MAXDATABUFNUM);
		//	PreTicket = CurTicket;			
		//}
		
		NowTime = GetTickCount();
		if((NowTime-PreTime)>=1000)
		{
			//定时检查客户端活动状态
			int temp=NowTime/1000;
			if(temp%2)//偶数秒
			{
				if(pMain->CheckAndCloseNotActiveClient(true)==false)//找到异常客户端
				{
					Sleep(20);
					pMain->CheckAndCloseNotActiveClient(false);
				}
			}				
			else//奇数秒
				pMain->CheckAndCloseNotActiveClient(false);		
			PreTime=NowTime;
		}
		
		//数据包发送
		if(pMain->GetDataFromSendBuff(Data))
		{			
			//同步发送列表
			pMain->SysUpdataSendVec();
			//为了加快全部发送的速度
			if( Data.sendUserType.all )//广播信息：登录正确的客户端才能给他广播
			{
				for(size_t i=0;i<pMain->m_SendVec.size();i++)
				{
					if ( pMain->m_SendVec[i].iLogoin==LOGOINTRUE )
					{
						if (pMain->m_SendVec[i].bWebSocket)
						{
							pSendPtr =	Data.pData;
							iSendSize = Data.len;
						}
						else
						{
							pSendPtr =	Data.pBaseData;
							iSendSize = Data.baselen;
						}
						pMain->WriteDataToSocket(pSendPtr,iSendSize,pMain->m_SendVec[i]);
					}					
				}				
			}
			else if (Data.sendUserType.Socket)
			{
				for(size_t i=0;i<pMain->m_SendVec.size();i++)
				{
					if(pMain->m_SendVec[i].item.fd == Data.socket)//指定客户端的信息
					{
						if (pMain->m_SendVec[i].bWebSocket)
						{
							pSendPtr =	Data.pData;
							iSendSize = Data.len;
						}
						else
						{
							pSendPtr =	Data.pBaseData;
							iSendSize = Data.baselen;
						}
						pMain->WriteDataToSocket(pSendPtr,iSendSize,pMain->m_SendVec[i]);
						break;
					}
				}
			}
			else	//查找制定类型的客户
			{
				for(size_t i=0;i<pMain->m_SendVec.size();i++)
				{
					if (pMain->m_SendVec[i].bWebSocket)
					{
						pSendPtr =	Data.pData;
						iSendSize = Data.len;
					}
					else
					{
						pSendPtr =	Data.pBaseData;
						iSendSize = Data.baselen;
					}
					
					//发送到指定类型的所有客户
					if( pMain->m_SendVec[i].SendUserType.Web && Data.sendUserType.Web )			//指定客户端的信息web
					{
						pMain->WriteDataToSocket(pSendPtr,iSendSize,pMain->m_SendVec[i]);
					}
					if( pMain->m_SendVec[i].SendUserType.iPad && Data.sendUserType.iPad)		//指定客户端的信息iPad
					{
						pMain->WriteDataToSocket(pSendPtr,iSendSize,pMain->m_SendVec[i]);
					}
					if( pMain->m_SendVec[i].SendUserType.UT && Data.sendUserType.UT)			//指定客户端的信息UT
					{
						pMain->WriteDataToSocket(pSendPtr,iSendSize,pMain->m_SendVec[i]);
					}
				}
			}
			
			if(pMain->m_SBuff.PackNum>0) continue;
		}

		if( pMain->m_SBuff.PackNum>0)
			continue;
		else
			Sleep(50);
	}


//END_SEBDPROCESS:
	pMain->m_bSendThreadRunning = false;
	pthread_mutex_lock(&pMain->m_NetSendlock);
	if(Data.pData!=NULL){delete[]Data.pData;Data.pData=NULL;}	
	pthread_mutex_unlock(&pMain->m_NetSendlock);	
	return NULL;
}
//************************************
// Method:    thread_startAndMonitor
// FullName:  CTCPServer::thread_startAndMonitor
// Access:    public static 
// Returns:   void *
// Qualifier: 在线程中启动TCP服务，并定期（30秒监控服务是否正常，服务异常则自动重启TCP服务）
// Parameter: void * arg
//************************************
void * CTCPServer::thread_startAndMonitor( void *arg )
{
	CTCPServer *pThis = (CTCPServer *)arg;
	pThis->m_StartThreadRunning = true;
	pThis->m_bStartThreadEndSignal = false;
	unsigned long PreTime,CurTime;
	PreTime = GetTickCount();
	while( pThis->m_bStartThreadEndSignal==false)
	{
		if(pThis->m_bReceiveStartFlag)//发过启动命令，立即启动
		{
			pThis->m_bReceiveStartFlag = false;//做过启动
			
			bool ret=pThis->DoStartServer(
				pThis->m_StartMessage.LocalIP,
				pThis->m_StartMessage.LocalPort,
				pThis->m_StartMessage.iMaxNumConnections,
				pThis->m_StartMessage.bNeedPack,
				pThis->m_StartMessage.bNeedLogoin);	
			if(ret==true && pThis->m_pHandle != NULL)
				pThis->m_pHandle->AddLog(1000, "服务端网络初始化成功");
			else if(ret == false && pThis->m_pHandle != NULL)
				pThis->m_pHandle->AddLog(1001, "服务端网络初始化失败");

			PreTime = GetTickCount();	
		}
		else//非线程第一次启动，进入监控逻辑
		{
			CurTime = GetTickCount();
			if( (CurTime-PreTime)>=SERVERMONITORTIME )//定时检查服务是否存在异常，是则自动重启服务
			{				
				if(pThis->CheckIsError()==false)//如果找到存在运行异常，自动重启
				{
					if (pThis->m_pHandle != NULL)
						pThis->m_pHandle->AddLog(1002, "服务端网络异常");
					pThis->OutpuMessage(LOG_TYPE_WARNING,"检测到TCP服务异常(%d,[%d,%d,%d,%d],%d)，准备自动重启！",pThis->m_Mange.bRunning,
					pThis->m_threads[0%MAXTHREADNUM].bRunning,pThis->m_threads[1%MAXTHREADNUM].bRunning,
					pThis->m_threads[2%MAXTHREADNUM].bRunning,pThis->m_threads[3%MAXTHREADNUM].bRunning,
					pThis->m_bReadThreadRunning,pThis->m_bSendThreadRunning);

					pThis->DoStopServer();
					if( pThis->m_bStartThreadEndSignal) break;
					bool ret = pThis->DoStartServer(
						pThis->m_StartMessage.LocalIP,
						pThis->m_StartMessage.LocalPort,
						pThis->m_StartMessage.iMaxNumConnections,
						pThis->m_StartMessage.bNeedPack,
						pThis->m_StartMessage.bNeedLogoin);
					if (ret == true && pThis->m_pHandle != NULL)
						pThis->m_pHandle->AddLog(1000, "服务端网络初始化成功");
					else if (ret == false && pThis->m_pHandle != NULL)
						pThis->m_pHandle->AddLog(1001, "服务端网络初始化失败");
				}
				PreTime = GetTickCount();
			}
		}	
		Sleep(100);	
	}
	
	pThis->m_StartThreadRunning = false;
	return NULL;
}
//************************************
// Method:    CheckIsError
// FullName:  CTCPServer::CheckIsError
// Access:    private 
// Returns:   bool
// Qualifier: 检查TCP服务端是否存在异常线程
//************************************
bool CTCPServer::CheckIsError()
{
	bool flag=true;

	//查找是否有线程未启动或线程运行异常
	if(m_Mange.bRunning==false) flag=false;

	//等待Work线程退出
	for (int i = 0; i < MAXTHREADNUM; i++)
		if(m_threads[i].bRunning==false) flag=false;

	if(m_bReadThreadRunning==false) flag=false;
	if(m_bSendThreadRunning==false) flag=false;

	if(m_bReadError) flag=false;
	if(m_bWriteError) flag=false;

	return flag;
}
//************************************
// Method:    DoStartServer
// FullName:  CTCPServer::DoStartServer
// Access:    public 
// Returns:   bool
// Qualifier: 启动TCP服务函数
// Parameter: const string & IP
// Parameter: const unsigned short Port
// Parameter: const unsigned int iMaxNumConnections
// Parameter: const bool bNeedPack
// Parameter: const bool bNeedLogoin
//************************************
bool CTCPServer::DoStartServer(const string &IP,		/*服务IP地址，为NULL或指向空字符串时不绑定指定IP*/
	const unsigned short Port,					/*绑定服务端口*/
	const unsigned int iMaxNumConnections,	/*允许最大连接数*/
	const bool bNeedPack,				/*接收是否需要组包cdyw网络包格式,true 需要组包 */
	const bool bNeedLogoin				/*客户端是否需要登录*/
	)
{
	if(m_Mange.bRunning)
	{
		if(IP!=m_Mange.LocalIP || m_Mange.port!=Port)//用户更改IP或端口
		{
			DoStopServer();
		}
		else//用户没有更改IP或端口或端口，保持连接返回
		{
			OutpuMessage(LOG_TYPE_DEBUG,"网络服务已启动，不能重复执行！%s:%u",IP.c_str(),Port);
			return true;
		}		
	}

	m_PreTime = GetTickCount();
	m_Mange.LocalIP = IP;
	m_Mange.port = Port;

	m_Mange.AllowMaxClientNum = iMaxNumConnections;
	m_Mange.bNeedPack = bNeedPack;
	m_Mange.bNeedLogoin = bNeedLogoin;

	//测试IP是否可以绑定//////////
	struct sockaddr_in sinadd;
	sinadd.sin_family = AF_INET;
	if(m_Mange.LocalIP==EMPTYIP)
		sinadd.sin_addr.s_addr = 0;
	else
		sinadd.sin_addr.s_addr = inet_addr(m_Mange.LocalIP.c_str());

	/////测试IP地址、端口是否可以绑定
	//if(m_Mange.LocalIP!=EMPTYIP)
	{
		evutil_socket_t fd = socket(AF_INET, SOCK_STREAM, 0);
		sinadd.sin_port = htons(m_Mange.port);
		if(bind(fd, (struct sockaddr *)&sinadd, sizeof(sinadd)) < 0)
		{
			ClearStartErrorStatus();
			OutpuMessage(LOG_TYPE_ERROR,"(TCPServer)本地地址绑定错误，请检查网络设置是否正确！IP=%s，PORT=%d",
				m_Mange.LocalIP.c_str(),m_Mange.port);
			return false;
		}
		else
			CLOSENOMANGESOCKET(fd)
	}	
	/////

	//创建主线程事件基
	m_Mange.base = event_base_new();
	if (m_Mange.base == NULL)
	{
		ClearStartErrorStatus();
		OutpuMessage(LOG_TYPE_ERROR,"Couldn't open event base!");		
		return false;
	}

	//创建数据发送、处理线程
	/*pthread_t tid;
	pthread_create(&tid, NULL, thread_readprocess, m_Mange.pMain);
	pthread_create(&tid, NULL, thread_SendAndMainProcess, m_Mange.pMain);*/

	if (m_ReadThreadHandle != NULL) { CloseHandle(m_ReadThreadHandle); m_ReadThreadHandle = NULL; }
	m_ReadThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_readprocess, (LPVOID)m_Mange.pMain, 0, NULL);

	if (m_SendThreadHandle != NULL) { CloseHandle(m_SendThreadHandle); m_SendThreadHandle = NULL; }
	m_SendThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_SendAndMainProcess, (LPVOID)m_Mange.pMain, 0, NULL);

	if (m_ProcessOnEventThreadHandle != NULL) {CloseHandle(m_ProcessOnEventThreadHandle); m_ProcessOnEventThreadHandle = NULL;}
	m_ProcessOnEventThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_ProcessOnEvent, (LPVOID)m_Mange.pMain, 0, NULL);

	//两秒等待所有线程初始化完成
	int cnt=100;//
	while(cnt--)//等待线程执行结果
	{
		if(m_bReadThreadRunning && m_bSendThreadRunning && m_ProcessOnEventThreadHandle) break;//初始化完成			
		Sleep(20);
	}
	if(m_bReadThreadRunning==false || m_bSendThreadRunning==false)
	{ClearStartErrorStatus();return false;}

	//创建连接响应子线程以及线程基，并启动线程
	if(CreateWorkThreadAndStart()==false)
	{
		ClearStartErrorStatus();
		return false;
	}

	{
		m_Mange.ClientVec.clear();
		vector<tagClient>().swap(m_Mange.ClientVec);
	}
	m_CurConnectNum = 0;
	//创建端口监听
	sinadd.sin_port = htons(m_Mange.port);
	m_Mange.plistener = evconnlistener_new_bind(m_Mange.base,
		(evconnlistener_cb)accept_conn_cb, this, LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE|LEV_OPT_THREADSAFE, -1,
		(struct sockaddr *)&sinadd, sizeof(struct sockaddr_in));
	if(m_Mange.plistener==NULL)
	{
		ClearStartErrorStatus();
		OutpuMessage(LOG_TYPE_ERROR,"本地地址绑定错误，请检查网络设置是否正确！IP=%s，PORT=%d",
			m_Mange.LocalIP.c_str(),m_Mange.port);
		return false;
	}

	m_Mange.listener_fd = evconnlistener_get_fd(m_Mange.plistener);
	evconnlistener_set_error_cb(m_Mange.plistener, accept_error_cb);
	///////////////////////////////////
	//pthread_create(&m_Mange.tid, NULL, thread_main, m_Mange.pMain);

	if (m_Mange.pThreadHandle != NULL) { CloseHandle(m_Mange.pThreadHandle); m_Mange.pThreadHandle = NULL; }
	m_Mange.pThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_main, (LPVOID)m_Mange.pMain, 0, NULL);

	//两秒等待所有线程初始化完成
	cnt=100;//
	while(cnt--)//等待线程执行结果
	{
		if(m_Mange.bRunning) break;//初始化完成			
		Sleep(20);
	}
	if(cnt<=0)//线程初始化超时
	{
		OutpuMessage(LOG_TYPE_ERROR,"服务端网络初始化错误，请检查配置参数是否正确！");
		DoStopServer();///////待测试逻辑
		return false;
	}
	else
		OutpuMessage(LOG_TYPE_MESSAGE,"服务端网络初始化成功.%s:%d",
		m_Mange.LocalIP.c_str(),m_Mange.port);	
	if(m_Mange.bRunning)
	{
		m_bReadError = m_bWriteError = false;
	}
	return m_Mange.bRunning;
}
//************************************
// Method:    AddFdToOnDisconnectVect
// FullName:  CTCPServer::AddFdToOnDisconnectVect
// Access:    public 
// Returns:   void
// Qualifier: 添加socket到断开容器，交给断开响应线程处理回调函数
// Parameter: int fd
//************************************
void CTCPServer::AddFdToOnDisconnectVect(int fd)
{
	pthread_mutex_lock(&m_OnDisLock);
	m_VecOnDisconnect.push_back(fd);
	pthread_mutex_unlock(&m_OnDisLock);
}
//************************************
// Method:    thread_ProcessOnEvent
// FullName:  CTCPServer::thread_ProcessOnEvent
// Access:    public static 
// Returns:   void *
// Qualifier: 客户端断开响应线程，在该线程中回调断开事件
// Parameter: void * arg
//************************************
void * CTCPServer::thread_ProcessOnEvent( void *arg )
{
	CTCPServer *pMain=(CTCPServer *)arg;
	pMain->m_bThreadProcessOnEventRunning = true;
	int OnDisconnectFd=-1;
	bool bDisconnect;
	while(pMain->m_bThreadProcessOnEventEnd==false)
	{
		//处理断开连接事件
		OnDisconnectFd = -1;
		bDisconnect = false;
		pthread_mutex_lock(&pMain->m_OnDisLock);
		if(pMain->m_VecOnDisconnect.size()>0)
		{
			OnDisconnectFd = pMain->m_VecOnDisconnect[0];
			pMain->m_VecOnDisconnect.erase(pMain->m_VecOnDisconnect.begin());
			if(pMain->m_VecOnDisconnect.size()>0)
				bDisconnect = true;
		}
		pthread_mutex_unlock(&pMain->m_OnDisLock);
		if(OnDisconnectFd!=-1 && pMain->m_pHandle!=NULL)
			pMain->m_pHandle->OnDisconnected(OnDisconnectFd);
		if(bDisconnect) continue;
		Sleep(20);
	}
	while(pMain->m_VecOnDisconnect.size()>0)
	{
		//处理断开连接事件
		OnDisconnectFd = -1;
		pthread_mutex_lock(&pMain->m_OnDisLock);
		//if(pMain->m_VecOnDisconnect.size()>0)
		{
			OnDisconnectFd = pMain->m_VecOnDisconnect[0];
			pMain->m_VecOnDisconnect.erase(pMain->m_VecOnDisconnect.begin());
		}
		pthread_mutex_unlock(&pMain->m_OnDisLock);
		if(OnDisconnectFd!=-1 && pMain->m_pHandle!=NULL)
			pMain->m_pHandle->OnDisconnected(OnDisconnectFd);
	}
	pMain->m_bThreadProcessOnEventRunning = false;
	return NULL;
}

void CTCPServer::ResetReadBuff()
{
	pthread_mutex_lock(&m_NetReadlock);
	m_RBuff.wIndex = 0;
	m_RBuff.rIndex = 0;
	m_RBuff.PackNum = 0;
	pthread_mutex_unlock(&m_NetReadlock);
}

void CTCPServer::ResetSendBuff()
{
	pthread_mutex_lock(&m_NetSendlock);
	m_SBuff.wIndex = 0;
	m_SBuff.rIndex = 0;
	m_SBuff.PackNum = 0;
	pthread_mutex_unlock(&m_NetSendlock);
}
