#include "stdafx.h"

#include "TCPClientPlugin.h"

#include "TCPClient.h"

#include <math.h>
#include <MSTcpIP.h>
#include <ws2tcpip.h>

//#define _NET_TEST_
#define EMPTYIP	("NULL")
extern CTCPClientPlugin* g_pMain;

CTCPClient::CTCPClient(const unsigned int RecePackSize,		/*接收包最大值*/
	const unsigned int SendPackSize,		/*发送包最大值*/
	const unsigned int ReceBuffSize,		/*接收缓冲区大小*/
	const unsigned int SendBuffSize			/*发送缓冲区大小*/)
{
	event_set_fatal_callback(libeventfatal_cb);

	//注意：以下调用不能在多个线程中调用，不然会影响发送
	//以下调用存在3个24Byte内存泄露，待libevent2.1版本调用libevent_global_shutdown()解决
	#ifdef WIN32
		evthread_use_windows_threads();//win上设置
	#else
		evthread_use_pthreads();    //unix上设置
	#endif
	m_ConnectThreadHandle = NULL;
	m_ReadThreadHandle = NULL;
	m_SendThreadHandle = NULL;
    m_pHandle = NULL;
    m_Mange.bConnecting = false;
	m_Mange.bConnected = false;
	m_Mange.bEnableReadWrite = false;
	m_Mange.tid.p = NULL;
	m_Mange.base = NULL;
	m_Mange.pReadBuff = NULL;
	m_Mange.bev = NULL;
	m_Mange.pMain = this;
	
	m_bReadThreadRun = false;
	m_bSendThreadRun = false;

	m_Mange.socket = INVALID_SOCKET;
	m_Mange.bDispatchRunning = false;

	pthread_mutex_init(&loglock, NULL);

	pthread_mutex_init(&m_NetSendlock,NULL);
	pthread_mutex_init(&m_NetReadlock,NULL);

#ifdef WIN32
	//确认WinSock DLL支持2.2
	WSADATA wsaData; // winsock 结构体
	WORD wVersionRequested = MAKEWORD( 1, 2 ); // 初始化 Windows Socket
	int err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
		OutpuMessage(LOG_TYPE_ERROR,"winsock 的版本 错误！");
#endif
	initReadThreadBuff(RecePackSize,ReceBuffSize);
	initSendThreadBuff(SendPackSize,SendBuffSize);
}

CTCPClient::~CTCPClient()
{
	SendQuitSignal();	
	QuitAndClear();
	//等待连接关闭
	while(m_Mange.bConnecting){Sleep(10);}
#ifdef WIN32
	//if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
	WSACleanup(); // 终止Winsock 2 DLL (Ws2_32.dll) 的使用
#endif

	pthread_mutex_destroy(&loglock);
	pthread_mutex_destroy(&m_NetSendlock);
	pthread_mutex_destroy(&m_NetReadlock);
	if(m_Mange.pReadBuff!=NULL){delete[]m_Mange.pReadBuff;m_Mange.pReadBuff=NULL;}

	if(m_RBuff.pBuff!=NULL){delete[]m_RBuff.pBuff;m_RBuff.pBuff=NULL;}
	if(m_RBuff.pPack!=NULL){delete[]m_RBuff.pPack;m_RBuff.pPack=NULL;}

	if(m_SBuff.pBuff!=NULL){delete[]m_SBuff.pBuff;m_SBuff.pBuff=NULL;}
	if(m_SBuff.pPack!=NULL){delete[]m_SBuff.pPack;m_SBuff.pPack=NULL;}
	if (m_ConnectThreadHandle != NULL) {CloseHandle(m_ConnectThreadHandle); m_ConnectThreadHandle = NULL;}
	if (m_ReadThreadHandle != NULL) { CloseHandle(m_ReadThreadHandle); m_ReadThreadHandle = NULL; }
	if (m_SendThreadHandle != NULL) { CloseHandle(m_SendThreadHandle); m_SendThreadHandle = NULL; }
	
}

//************************************
// Method:    ConnectServer
// FullName:  CTCPClient::ConnectServer
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const char * LocalIP			/*本地IP，可以为空*/
// Parameter: const char * IP				/*服务IP地址，为NULL或指向空字符串时不绑定指定IP*/
// Parameter: const unsigned short Port		/*服务端断开*/
// Parameter: const bool bNeedPack			/*释放需要网络组包*/
//************************************
bool CTCPClient::ConnectServer(const char *LocalIP,const char *IP,		
	const unsigned short Port,						
	const bool bNeedPack)			
{
	//wxCriticalSectionLocker locker(m_NetStartStopCS);
	if(LocalIP==NULL)
		m_ConnectMessage.LocalIP = EMPTYIP;		
	else
		m_ConnectMessage.LocalIP = LocalIP;
	if(IP==NULL)
		m_ConnectMessage.DstIP = "";
	else
		m_ConnectMessage.DstIP = IP;
	if(m_ConnectMessage.DstIP.empty())
	{
		OutpuMessage(LOG_TYPE_ERROR,"TCP服务端地址为空，不允许连接！");
		return false;
	}
	m_ConnectMessage.DstPort =Port;
	m_ConnectMessage.bNeedPack = bNeedPack;

	if(m_Mange.bConnected )//之前连接成功且连接参数无变化，则不重连
	{
		if(m_Mange.LocalIP == m_ConnectMessage.LocalIP &&
			m_Mange.DstIP == m_ConnectMessage.DstIP && m_Mange.port==m_ConnectMessage.DstPort)
		{
			OutpuMessage(LOG_TYPE_DEBUG,"网络连接正常，不需要重连。");
			return NULL;
		}
		else//网络参数有变化断开上一次连接
		{
			SendQuitSignal();			
		}
	}
	else if(m_Mange.bConnecting)//正在连接中，但一直没连上
	{
		unsigned long CurTime;
		CurTime = GetTickCount();
		if( (CurTime-m_Mange.StartConnectTime)>(30*1000) )//连接时间超过限制
		{
			OutpuMessage(LOG_TYPE_DEBUG,"Connect timeout!%d",(CurTime-m_Mange.StartConnectTime));
			QuitAndClear();
		}
	}

	if(m_Mange.bConnecting==false)//必须之前的连接线程退出后才能创建新线程
	{
		//pthread_t tid;
		//pthread_create(&tid, NULL, thread_connect, m_Mange.pMain);

		if (m_ConnectThreadHandle != NULL) {CloseHandle(m_ConnectThreadHandle); m_ConnectThreadHandle = NULL;}
		m_ConnectThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_connect, (LPVOID)m_Mange.pMain, 0, NULL);
	}
	else
	{
		OutpuMessage(LOG_TYPE_DEBUG,"正在连接中！请等待.");
	}
	
	return true;
}
//************************************
// Method:    IsWork
// FullName:  CTCPClient::IsWork
// Access:    public 
// Returns:   bool
// Qualifier: 查询TCP服务是否启动成功
//************************************
bool CTCPClient::IsConneted()
{
	return m_Mange.bConnected;
}
//************************************
// Method:    StopServer
// FullName:  CTCPClient::StopServer
// Access:    public 
// Returns:   void
// Qualifier: 停止TCP服务
//************************************
void CTCPClient::DisconnectServer()
{
	SendQuitSignal();
	/*unsigned long PreTick=GetTickCount();
	while(1) 
	{
		if(m_Mange.bConnected==false && m_Mange.bConnecting==false) break;
		Sleep(100);
		if( (GetTickCount()-PreTick)>5000) break;
	}*/
}
void CTCPClient::SendQuitSignal()
{
	if(m_Mange.socket!=INVALID_SOCKET)
	{
		OutpuMessage(LOG_TYPE_DEBUG,"发送断开连接信号，fd=%d",m_Mange.socket);
		m_Mange.bEnableReadWrite = false;
		evutil_closesocket(m_Mange.socket);
		m_Mange.socket = INVALID_SOCKET;		
	}
}
void CTCPClient::QuitAndClear()
{
	OutpuMessage(LOG_TYPE_DEBUG,"CTCPClient::QuitAndClear...");
	int TCnt=200;
	m_Mange.bEnableReadWrite = false;
	//关闭连接
	if(m_Mange.bConnected)//如处于连接状态，关闭连接
	{
		if(m_Mange.socket!=INVALID_SOCKET)
		{			
			evutil_closesocket(m_Mange.socket);
			m_Mange.socket = INVALID_SOCKET;
		}
		//while(m_Mange.bConnected&&(TCnt--)){Sleep(20);}
	}

	m_bReadSetEnd = true;
	//等待数据处理线程退出
	TCnt=200;
	//while(m_bReadThreadRun&&(TCnt--)){Sleep(20);}
	while(m_bReadThreadRun){Sleep(20);}

	m_bSendSetEnd = true;
	//等待数据发送线程退出
	TCnt=200;
	//while(m_bSendThreadRun&&(TCnt--)){Sleep(20);}
	while(m_bSendThreadRun){Sleep(20);}

	//关闭事件基循环
	if(m_Mange.base!=NULL && m_Mange.bDispatchRunning)
		event_base_loopbreak(m_Mange.base);//base循环必须在一次事件发生时才能退出

	m_Mange.bConnected = false;
	if (m_ConnectThreadHandle != NULL) { CloseHandle(m_ConnectThreadHandle); m_ConnectThreadHandle = NULL; }
}
//************************************
// Method:    SendData
// FullName:  CTCPClient::SendData
// Access:    public 
// Returns:   bool
// Qualifier: /*添加待发送数据到发送缓冲区，后续由发送线程处理*/
// Parameter: const char * pData		/*缓冲区地址*/
// Parameter: const unsigned int len	/*发送缓冲区长度*/
// Parameter: int Socket				/*待发送socket，如bAll为true，该值任意*/
// Parameter: bool bAll					/*是否发送给所有连接上的客户端*/
//************************************
bool CTCPClient::SendData(
	const char* pData,			
	const unsigned int len)				
{
	if(m_bSendThreadRun==false) return false;

	if(m_Mange.bNeedPack)//不需要上层发送心跳包（为历史兼容处理）
	{
		LibeventTCP::tagNetPackHead *pHead=(LibeventTCP::tagNetPackHead *)pData;
		if(pHead->DataType==NET_DATA_TYPE_ACTPACK)
			return true;
	}
	AddDataToSendBuff(pData,len);
	return true;
}

//************************************
// Method:    RegHandle
// FullName:  CTCPClient::RegHandle
// Access:    public 
// Returns:   void
// Qualifier: 注意网络事件处理指针
// Parameter: ITCPClientSocketNet * pHandle
//************************************
void CTCPClient::RegHandle(ITCPClientSocketNet *pHandle)
{
	if(pHandle!=NULL)
		m_pHandle = pHandle;
}

//************************************
// Method:    WriteDataToSocket
// FullName:  CTCPClient::WriteDataToSocket
// Access:    public 
// Returns:   bool
// Qualifier: 写数据到socket发送缓冲区，后续libevent自动发送
// Parameter: const char * buff
// Parameter: unsigned int len
//************************************
bool CTCPClient::WriteDataToSocket(const char *buff,unsigned int len)
{	
	//wxCriticalSectionLocker locker(m_NetStartStopCS);
	if(m_Mange.bConnected && m_Mange.bEnableReadWrite)
	{		
		if(m_Mange.bev!=NULL)
		{
			struct evbuffer *output=bufferevent_get_output(m_Mange.bev);//	
			if(output!=NULL)
				evbuffer_add(output,(void *)buff,len);
			m_Mange.bWriteFlag = false;
		}
		
	}
	//bufferevent_write(pClient->bev, buff, len);//往该socket写数据
	
	return true;
}


#define MAXLOGSIZE	500
int CTCPClient::OutpuMessage(int LOGTYPE,const char *format, ...)
{
	pthread_mutex_lock(&loglock);
	va_list args;
	va_start(args,   format);
	char _mess[MAXLOGSIZE+10]={0};
	int ret=_vsprintf_p(_mess,   MAXLOGSIZE,format,   args);	
	va_end(args);
	if(ret==-1)
		sprintf(_mess,"CTCPClient:OutpuMessage::无效的LOG输出信息");
	if (m_pHandle)
		m_pHandle->AddLog(LOGTYPE, _mess);
	//g_pMain->AddLog(LOGTYPE,_mess);

#ifdef _NET_TEST_

	SYSTEMTIME tm;
	GetLocalTime(&tm);
	char str[MAXLOGSIZE]={0};
	memset(str,0,sizeof(str));
	sprintf(str,"%04d-%02d-%02d %02d:%02d:%02d(%03d): TID=%d,Wid=%d/%d,Rid=%d/%d:%s\n",
		tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond,tm.wMilliseconds,GetCurrentThreadId(),
		m_SBuff.PackNum,m_SBuff.MaxPackNum,m_RBuff.PackNum,m_RBuff.MaxPackNum,_mess);
	OutputDebugStringA(str);

#endif
	pthread_mutex_unlock(&loglock);	


	return 0;
}

//************************************
// Method:    read_cb
// FullName:  CTCPClient::read_cb
// Access:    private static 
// Returns:   void
// Qualifier: 读事件响应函数，线程环境为thread_worker
// Parameter: struct bufferevent * bev
// Parameter: void * arg
//************************************
void CTCPClient::read_cb(struct bufferevent *bev, void *arg)
{
	CTCPClient *pMain = ( CTCPClient *)arg;
	struct evbuffer *input = bufferevent_get_input(bev);//
	const size_t len = evbuffer_get_length(input);
	pMain->m_Mange.ActTime = GetTickCount();
	if(len>TCPMAXPACKSIZE)//超过网络最大包
	{
		evbuffer_drain(input,len);
		return;
	}
	//判断数据长度是否有效
	if(len<=0)//无数据
	{
		return;
	}
	//处理数据
	if(!pMain->m_Mange.bNeedPack)//不需要组包数据直接外传
	{
		evbuffer_remove(input,(void*)pMain->m_Mange.pReadBuff,len);
		pMain->AddDataToReadBuff(pMain->m_Mange.pReadBuff,len);		
	}
	else//需要数据组包
	{
		size_t relen=len;//剩余数据量
		LibeventTCP::tagNetPackHead *pHead=(LibeventTCP::tagNetPackHead *)pMain->m_Mange.pReadBuff;
		LibeventTCP::tagNetPackEnd *pEnd;
		while (relen>sizeof(LibeventTCP::tagNetPackHead))
		{
			//从缓冲区复制数据但不删除
			evbuffer_copyout(input,pMain->m_Mange.pReadBuff,sizeof(LibeventTCP::tagNetPackHead));

			if(pHead->SOI==NET_SOI)//包头标识正确
			{
				if(relen>=pHead->DataPackSize)//缓冲区中剩余数据量够组包
				{
					evbuffer_remove(input,pMain->m_Mange.pReadBuff,pHead->DataPackSize);//从缓冲区复制数据，并从源缓冲区删除指定长度数据
					pEnd = (LibeventTCP::tagNetPackEnd *)(pMain->m_Mange.pReadBuff+pHead->DataPackSize-sizeof(LibeventTCP::tagNetPackEnd));
					if(pEnd->EOI==NET_EOI)//包尾标识正确
					{
						pMain->AddDataToReadBuff(pMain->m_Mange.pReadBuff,pHead->DataPackSize);
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
				evbuffer_drain(input,relen);
				return;
			}			
		}			
	}
}

void CTCPClient::write_cb(struct bufferevent *bev, void *arg) 
{
	CTCPClient *pMain=(CTCPClient*)arg;
	pMain->m_Mange.bWriteFlag = true;
}

//************************************
// Method:    error_cb
// FullName:  CTCPClient::error_cb
// Access:    private static 
// Returns:   void
// Qualifier: 连接断开事件响应函数，线程环境为thread_worker，主动或被动关闭socket会响应该函数
// Parameter: struct bufferevent * bev
// Parameter: short event
// Parameter: void * arg
//************************************
void CTCPClient::event_cb(struct bufferevent *bev, short event, void *arg)
{
	CTCPClient *pMain=(CTCPClient*)arg;
	pMain->OutpuMessage(LOG_TYPE_DEBUG,"CTCPClient::even=0x%04x!",event);
	enum _DISCONNECT_STATUS_ errtype = TCPCLIENT_NOERROR;
	if (event & BEV_EVENT_CONNECTED) //连接成功
	{
		pMain->m_Mange.socket = bufferevent_getfd(bev);
		pMain->OutpuMessage(LOG_TYPE_DEBUG,"连接成功，fd=%u，event=0x%04x",pMain->m_Mange.socket,event);

		pMain->m_Mange.bEnableReadWrite = true;
		pMain->m_Mange.bConnected = true;
		pMain->m_ConnectMessage.bConnectFlag = true;
		pMain->m_Mange.bWriteFlag = true;	//可以写入
		pMain->m_Mange.ActTime = GetTickCount();
		if(pMain->m_pHandle!=NULL)
		{
			//不能根据调用的接口个数来区分，这里在外部进行区分调用
			if (g_pMain->GetInstanceSize() > 0)
			{
				ITCPClientInstance *pInstance=(ITCPClientInstance *)pMain;
				pMain->m_pHandle->OnConnected(pInstance); 
				pMain->m_pHandle->OnConnected(); 
			}			
		}
		return;
	}
	else if( (event & BEV_EVENT_TIMEOUT) && (event & BEV_EVENT_WRITING) )//连接超时，目的地址不可用或不可达
		errtype = TCPCLIENT_CONNECTTIMEOUT;
	else if ( (event & BEV_EVENT_ERROR) && (event & BEV_EVENT_READING) )//本地主动断开连接
		errtype = TCPCLIENT_DISCONNECT;
	else if ( (event & BEV_EVENT_EOF) && (event & BEV_EVENT_READING) )//服务端断开连接
		errtype = TCPCLIENT_SERDISCONNECT;
	else if ( (event & BEV_EVENT_ERROR) )//服务端IP地址存在，但未启动监听
		errtype = TCPCLIENT_SERPORTERR;

	if(errtype != TCPCLIENT_NOERROR)
	{
		pMain->m_Mange.bEnableReadWrite = false;
		/*pMain->m_Mange.bConnected = false;
		if(pMain->m_Mange.bev!=NULL)
		bufferevent_disable(pMain->m_Mange.bev,EV_READ|EV_WRITE|EV_PERSIST);*/
		evutil_closesocket(pMain->m_Mange.socket);
		pMain->m_Mange.socket = INVALID_SOCKET;
	}	
}

void CTCPClient::libeventfatal_cb(int err)
{
	//g_pMain->AddLogF(LOG_TYPE_ERROR,"网络致命错误，请重启程序!error id=%d",err);
	return;
}

////////////////////////////////////////////////////////////////////////////////////
void CTCPClient::AddDataToReadBuff(const char *pData,const unsigned int len)
{
	if(len>m_RBuff.PackSize)
	{
		OutpuMessage(LOG_TYPE_WARNING,"读处理缓冲区C：待处理数据包大小超出允许范围:%d/%d(Bytes)",len,m_RBuff.PackSize);
		return;
	}
	if(m_RBuff.PackNum>=m_RBuff.MaxPackNum)
	{
		OutpuMessage(LOG_TYPE_WARNING,"读处理缓冲区C：待处理缓存包数已达最大值,%d/%d",m_RBuff.PackNum,m_RBuff.MaxPackNum);
		return;
	}
	if(m_Mange.bNeedPack)//检查是否心跳包，是则过滤，心跳包上层不需要处理
	{
		const LibeventTCP::tagNetPackHead *pHead  = (const LibeventTCP::tagNetPackHead *)pData;
		if(pHead->DataType==NET_DATA_TYPE_ACTPACK)
		{
			/*if (m_Mange.port==8115)
				OutpuMessage(LOG_TYPE_DEBUG,"Rec Act:fd=%d,PickID=%d",m_Mange.socket,pHead->PackID);*/	
			return;
		}
	}
	
	pthread_mutex_lock(&m_NetReadlock);
	if( m_bReadThreadRun )
	{
		m_RBuff.pPack[m_RBuff.wIndex].len = len;
		memcpy(m_RBuff.pPack[m_RBuff.wIndex].pData,pData,len);
		m_RBuff.wIndex++;

		m_RBuff.PackNum++;	
		if (m_RBuff.wIndex>=m_RBuff.MaxPackNum)
			m_RBuff.wIndex = 0;
	}	
	pthread_mutex_unlock(&m_NetReadlock);
}
bool CTCPClient::GetDataFromReadBuff(tagNetData &NetData)
{
	if(m_RBuff.PackNum==0) return false;

	pthread_mutex_lock(&m_NetReadlock);

	NetData.len = m_RBuff.pPack[m_RBuff.rIndex].len;
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
// FullName:  CTCPClient::thread_readprocess
// Access:    public static 
// Returns:   void *
// Qualifier: 读取到的数据线程处理函数
// Parameter: void * arg
//************************************
void *CTCPClient::thread_readprocess(void *arg)
{
	CTCPClient *pMain = (CTCPClient *)arg;
	pMain->m_bReadThreadRun = true;
	pMain->m_bReadSetEnd = false;
	pMain->ResetReadBuff();
	tagNetData Data;
	Data.pData = new char[pMain->m_RBuff.PackSize];
	while(pMain->m_bReadSetEnd==false)
	{
		if(pMain->GetDataFromReadBuff(Data))
		{
			//回调数据
			//不能根据调用的接口个数来区分，这里在外部进行区分调用
			if (g_pMain->GetInstanceSize() > 0)
			{
				ITCPClientInstance *pInstance=(ITCPClientInstance *)pMain;
				pMain->m_pHandle->OnReceiveData(pInstance,Data.pData,Data.len);
				pMain->m_pHandle->OnReceiveData(Data.pData,Data.len);
			}
			
			if(pMain->m_RBuff.PackNum>0) continue;
		}
		Sleep(50);
	}

//END_READPROCESS:	
	pMain->m_bReadThreadRun = false;
	pthread_mutex_lock(&pMain->m_NetReadlock);
	if(Data.pData!=NULL){delete[]Data.pData;Data.pData=NULL;}
	pthread_mutex_unlock(&pMain->m_NetReadlock);
	
	return NULL;
}

//************************************
// Method:    AddDataToSendBuff
// FullName:  CTCPClient::AddDataToSendBuff
// Access:    public 
// Returns:   void
// Qualifier: 添加数据包到发送缓冲区
// Parameter: const char * pData
// Parameter: const unsigned int len
//************************************
void CTCPClient::AddDataToSendBuff(const char *pData,const unsigned int len)
{
	if(len>m_SBuff.PackSize)
	{
		OutpuMessage(LOG_TYPE_WARNING,"发送处理缓冲区C：待处理数据包大小超出允许范围:%d/%d(Bytes)",len,m_SBuff.PackSize);
		return;
	}
	if(m_SBuff.PackNum>=m_SBuff.MaxPackNum)
	{
		OutpuMessage(LOG_TYPE_WARNING,"发送处理缓冲区C：待处理缓存包数已达最大值,%d/%d",m_SBuff.PackNum,m_SBuff.MaxPackNum);
		return;
	}

	pthread_mutex_lock(&m_NetSendlock);
	if(m_bSendThreadRun )
	{
		m_SBuff.pPack[m_SBuff.wIndex].len = len;
		memcpy(m_SBuff.pPack[m_SBuff.wIndex].pData,pData,len);
		m_SBuff.wIndex++;

		m_SBuff.PackNum++;	
		if (m_SBuff.wIndex>=m_SBuff.MaxPackNum)
			m_SBuff.wIndex = 0;
	}	
	pthread_mutex_unlock(&m_NetSendlock);
}
//************************************
// Method:    GetDataFromSendBuff
// FullName:  CTCPClient::GetDataFromSendBuff
// Access:    public 
// Returns:   bool
// Qualifier: 从发送缓冲区获取数据包
// Parameter: tagNetData & NetData
//************************************
bool CTCPClient::GetDataFromSendBuff(tagNetData &NetData)
{
	if(m_SBuff.PackNum==0) return false;

	pthread_mutex_lock(&m_NetSendlock);

	NetData.len = m_SBuff.pPack[m_SBuff.rIndex].len;
	memcpy(NetData.pData,m_SBuff.pPack[m_SBuff.rIndex].pData,NetData.len);

	m_SBuff.rIndex++;
	m_SBuff.PackNum--;
	if (m_SBuff.rIndex>=m_SBuff.MaxPackNum)
		m_SBuff.rIndex = 0;
	pthread_mutex_unlock(&m_NetSendlock);
	return true;
}
//************************************
// Method:    thread_sendprocess
// FullName:  CTCPClient::thread_sendprocess
// Access:    public static 
// Returns:   void *
// Qualifier: 发送缓冲处理线程
// Parameter: void * arg
//************************************
void *CTCPClient::thread_sendprocess(void *arg)
{
	CTCPClient *pMain = (CTCPClient *)arg;
	pMain->m_bSendThreadRun = true;
	pMain->m_bSendSetEnd = false;
	pMain->ResetReadBuff();

	tagNetData Data;
	Data.pData = new char[pMain->m_SBuff.PackSize];
	unsigned int PackID=0;
	LibeventTCP::tagNetPackHead *pHead=(LibeventTCP::tagNetPackHead *)Data.pData;

	LibeventTCP::tagActPack actPack;
	memset(&actPack,0,sizeof(LibeventTCP::tagActPack));
	actPack.head.SOI = NET_SOI;
	actPack.head.DataPackSize = sizeof(LibeventTCP::tagActPack);
	actPack.head.DataType = NET_DATA_TYPE_ACTPACK;
	actPack.head.FrameIndex =0;
	actPack.head.TotalFrame = 1;
	actPack.head.FrameSize = 40;
	actPack.end.EOI = NET_EOI;
	pMain->m_Mange.bWriteFlag = true;
	unsigned long NowTime=0,ActSendTime=0;

	while(pMain->m_bSendSetEnd==false)
	{
		if( (pMain->m_Mange.bWriteFlag||evbuffer_get_length(pMain->m_Mange.bev->output)<32768) && pMain->GetDataFromSendBuff(Data))
		{
			if(pMain->m_Mange.bNeedPack)//如果是组包数据自动添加包累加计数
				pHead->PackID=PackID++;
			pMain->WriteDataToSocket(Data.pData,Data.len);
			
			NowTime = GetTickCount();
			//定时自动心跳包发送
			if( abs((double)NowTime-ActSendTime)>=ACTSENDTIME )
			{
				actPack.head.PackID=PackID++;	
				/*if (pMain->m_Mange.port==8115)
					pMain->OutpuMessage(LOG_TYPE_DEBUG,"Data:SendAct:fd=%d,PackID=%d,RBuff=%d/%d,SBuf=%d/%d",pMain->m_Mange.socket,actPack.head.PackID,
					pMain->m_RBuff.PackNum,pMain->m_RBuff.MaxPackNum,pMain->m_SBuff.PackNum,pMain->m_SBuff.MaxPackNum);*/							
				pMain->WriteDataToSocket((char*)&actPack,actPack.head.DataPackSize);
				ActSendTime = GetTickCount();
			}
			
			if(pMain->m_SBuff.PackNum>0) continue;
		}
		if(pMain->m_SBuff.PackNum==0) Sleep(50);
		else Sleep(1);

		if(pMain->m_Mange.bConnected&&pMain->m_Mange.bNeedPack&&pMain->m_Mange.socket!=INVALID_SOCKET)
		{
			NowTime = GetTickCount();

			//定时自动心跳包发送
			if( abs((double)NowTime-ActSendTime)>=ACTSENDTIME )
			{
				actPack.head.PackID=PackID++;	
				/*if (pMain->m_Mange.port==8115)
					pMain->OutpuMessage(LOG_TYPE_DEBUG,"SendAct:fd=%d,PackID=%d,RBuff=%d/%d,SBuf=%d/%d",pMain->m_Mange.socket,actPack.head.PackID,
						pMain->m_RBuff.PackNum,pMain->m_RBuff.MaxPackNum,pMain->m_SBuff.PackNum,pMain->m_SBuff.MaxPackNum);	*/		
				pMain->WriteDataToSocket((char*)&actPack,actPack.head.DataPackSize);
				ActSendTime = GetTickCount();			
			}

			//心跳检测，检测到活动异常则主动断开
			if((NowTime-pMain->m_Mange.ActTime)>ACTTIMEOUT)
			{
				/*if (pMain->m_Mange.port==8115)*/
					pMain->OutpuMessage(LOG_TYPE_DEBUG,"连接活动超时，fd=%d",pMain->m_Mange.socket);
				pMain->SendQuitSignal();
				continue;
			}			
		}			
	}

//END_SEBDPROCESS:	
	pMain->m_bSendThreadRun = false;
	pthread_mutex_lock(&pMain->m_NetSendlock);
	if(Data.pData!=NULL){delete[]Data.pData;Data.pData=NULL;}
	pthread_mutex_unlock(&pMain->m_NetSendlock);	
	
	return NULL;
}
//************************************
// Method:    initReadThreadBuff
// FullName:  CTCPClient::initReadThreadBuff
// Access:    private 
// Returns:   void
// Qualifier: 初始化读缓冲区（只在构造函数中调用一次）
// Parameter: const unsigned int RecePackSize	接收网络包大小
// Parameter: const unsigned int ReceBuffSize	接收缓冲区大小
//************************************
void CTCPClient::initReadThreadBuff(const unsigned int RecePackSize,const unsigned int ReceBuffSize)
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
// FullName:  CTCPClient::initSendThreadBuff
// Access:    private 
// Returns:   void
// Qualifier: 初始化发送缓冲区（只在构造函数调用一次）
// Parameter: const unsigned int SendPackSize	发送网络包大小
// Parameter: const unsigned int SendBuffSize	发送缓冲区大小
//************************************
void CTCPClient::initSendThreadBuff(const unsigned int SendPackSize,const unsigned int SendBuffSize)
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
//************************************
// Method:    GetIPFromAdd
// FullName:  CTCPClient::GetIPFromAdd
// Access:    public 
// Returns:   bool
// Qualifier: 从域名得到IP地址
// Parameter: const string & szAddress
// Parameter: string & IP
//************************************
bool CTCPClient::GetIPFromAdd(const string &szAddress,string &IP)
{
	struct addrinfo hints;
	struct addrinfo *res=NULL;
	char szIP[20]={0};
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;     /* Allow IPv4 */
	hints.ai_flags = AI_PASSIVE;/* For wildcard IP address */
	hints.ai_protocol = 0;         /* Any protocol */
	hints.ai_socktype = SOCK_STREAM;

	if(-1==getaddrinfo(szAddress.c_str(), NULL,&hints,&res))
		return false;
	else
	{ 
		if(res==NULL) return false;

		struct sockaddr_in *addr;
		addr = (struct sockaddr_in *)res->ai_addr;
		sprintf(szIP, "%d.%d.%d.%d",
			(*addr).sin_addr.S_un.S_un_b.s_b1,
			(*addr).sin_addr.S_un.S_un_b.s_b2,
			(*addr).sin_addr.S_un.S_un_b.s_b3,
			(*addr).sin_addr.S_un.S_un_b.s_b4);
			
	}
	IP=szIP;
	return true;
}
//************************************
// Method:    thread_connect
// FullName:  CTCPClient::thread_connect
// Access:    public static 
// Returns:   void *
// Qualifier: 网络连接线程，同一时刻只能一条线程去连接服务端，知道服务端正常连接或断开或超时
// Parameter: void * arg
//************************************
void * CTCPClient::thread_connect( void *arg )
{
	CTCPClient *pThis = (CTCPClient *)arg;
	pThis->m_Mange.StartConnectTime = GetTickCount();
	pThis->m_Mange.bConnecting = true;	
	pThis->DoConnectServer(
		pThis->m_ConnectMessage.LocalIP,
		pThis->m_ConnectMessage.DstIP,
		pThis->m_ConnectMessage.DstPort,
		pThis->m_ConnectMessage.bNeedPack);


	//连接完成后同一线程环境中释放事件基
	if(pThis->m_Mange.bev!=NULL){bufferevent_free(pThis->m_Mange.bev);pThis->m_Mange.bev = NULL;}
	if(pThis->m_Mange.base!=NULL){event_base_free(pThis->m_Mange.base);pThis->m_Mange.base = NULL;}

	pThis ->m_Mange.bConnecting = false;
	return NULL;
}
//************************************
// Method:    DoConnectServer
// FullName:  CTCPClient::DoConnectServer
// Access:    public 
// Returns:   bool
// Qualifier: 创建事件基，连接网络
// Parameter: const string & LocalIP
// Parameter: const string & IP
// Parameter: const unsigned short Port
// Parameter: const bool bNeedPack
//************************************
bool CTCPClient::DoConnectServer(const string &LocalIP,const string &IP,		/*服务IP地址，为NULL或指向空字符串时不绑定指定IP*/
	const unsigned short Port,					/*绑定服务端口*/
	const bool bNeedPack				/*接收是否需要组包cdyw网络包格式,true 需要组包 */
	)
{
	SendQuitSignal();
	QuitAndClear();
	m_ConnectMessage.bConnectFlag = false;
	m_Mange.LocalIP = LocalIP;
	m_Mange.DstIP = IP;
	m_Mange.port = Port;
	string sDstIP="";
	if(GetIPFromAdd(m_Mange.DstIP,sDstIP)==false)
	{
		OutpuMessage(LOG_TYPE_ERROR,"网络地址解析错误！%s:%d",m_Mange.DstIP.c_str(),m_Mange.port);
		QuitAndClear();	
		return false;
	}
		
	/////测试IP地址是否可以绑定
	if(m_Mange.LocalIP.empty()==false)
	{
		struct sockaddr_in sinadd;
		sinadd.sin_family = AF_INET;
		sinadd.sin_addr.s_addr = inet_addr(m_Mange.LocalIP.c_str());
		evutil_socket_t fd = socket(AF_INET, SOCK_STREAM, 0);
		sinadd.sin_port = htons(0);
		if(bind(fd, (struct sockaddr *)&sinadd, sizeof(sinadd)) < 0)
		{
			QuitAndClear();
			m_pHandle->OnDisconnected((ITCPClientInstance *)m_Mange.pMain,TCPCLIENT_BINDERROR);
			m_pHandle->OnDisconnected(TCPCLIENT_BINDERROR);
			return false;
		}
		else
			evutil_closesocket(fd);
	}	
	/////

	m_Mange.bNeedPack = bNeedPack;

	if(m_Mange.pReadBuff==NULL)	
		m_Mange.pReadBuff = new char[TCPMAXPACKSIZE];

	////创建数据发送、处理线程
	/*pthread_t tid;
	pthread_create(&tid, NULL, thread_readprocess, m_Mange.pMain);
	pthread_create(&tid, NULL, thread_sendprocess, m_Mange.pMain);*/

	if (m_ReadThreadHandle != NULL) { CloseHandle(m_ReadThreadHandle); m_ReadThreadHandle = NULL; }
	m_ReadThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_readprocess, (LPVOID)m_Mange.pMain, 0, NULL);

	if (m_SendThreadHandle != NULL) { CloseHandle(m_SendThreadHandle); m_SendThreadHandle = NULL; }
	m_SendThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_sendprocess, (LPVOID)m_Mange.pMain, 0, NULL);
	
	//两秒等待所有线程初始化完成
	int cnt=100;//
	while(cnt--)//等待线程执行结果
	{
		if(m_bReadThreadRun && m_bSendThreadRun)//初始化完成
			break;
		Sleep(20);
	}
	if(cnt<=0)
	{
		OutpuMessage(LOG_TYPE_ERROR,"客户端网络初始化错误，请检查配置参数是否正确！%d,%d",m_bReadThreadRun,m_bSendThreadRun);	
		QuitAndClear();
		return false;
	}	

	////////////////////////////////////////
	//创建主线程事件基
	m_Mange.base = event_base_new();//m_Mange.base只能在同一线程环境下释放，不然会异常
	if (m_Mange.base == NULL)
	{		
		OutpuMessage(LOG_TYPE_ERROR,"error:event_base_new()!");
		QuitAndClear();		
		return false;
	}

	m_Mange.bev = bufferevent_socket_new(m_Mange.base,-1,BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);//m_Mange.bev只能在同一线程环境下释放，不然会异常
	if(m_Mange.bev == NULL)
	{
		OutpuMessage(LOG_TYPE_ERROR,"初始化错误！bufferevent_socket_new()");
		QuitAndClear();		
		return false;
	}

	struct sockaddr_in dstaddr;
	memset(&dstaddr,0,sizeof(dstaddr));
	dstaddr.sin_family = AF_INET;
	dstaddr.sin_addr.s_addr = inet_addr(sDstIP.c_str());
	dstaddr.sin_port = htons(m_Mange.port);
	if (bufferevent_socket_connect(m_Mange.bev, (struct sockaddr *)&dstaddr, sizeof(dstaddr)) < 0)
	{
		QuitAndClear();
		m_pHandle->OnDisconnected((ITCPClientInstance *)m_Mange.pMain,TCPCLIENT_ERROR);
		m_pHandle->OnDisconnected(TCPCLIENT_ERROR);				
		return false;
	}	
	bufferevent_setcb(m_Mange.bev, read_cb,write_cb,event_cb,m_Mange.pMain);

	struct timeval readtimeout,writetimeout;
	readtimeout.tv_sec = 6;readtimeout.tv_usec = 0;
	writetimeout.tv_sec = 5;writetimeout.tv_usec = 0;
	//bufferevent_set_timeouts(m_Mange.bev,&readtimeout,&writetimeout);//设置读写超时值
	//bufferevent_set_timeouts(m_Mange.bev,NULL,&writetimeout);//设置读写超时值
	ResetReadBuff();
	ResetSendBuff();
	bufferevent_enable(m_Mange.bev, EV_READ|EV_WRITE|EV_PERSIST);
	
	OutpuMessage(LOG_TYPE_DEBUG,"CTCPClient::Start Connect...");
	m_Mange.bDispatchRunning = true; 
	event_base_dispatch(m_Mange.base);
	m_Mange.bDispatchRunning = false;
	QuitAndClear();	
		
	//回调用户端断开处理函数
	//不能根据调用的接口个数来区分，这里在外部进行区分调用
	if(m_pHandle!=NULL)
	{
		if (g_pMain->GetInstanceSize() > 0)
		{
			if(m_ConnectMessage.bConnectFlag==true)
			{
				m_pHandle->OnDisconnected((ITCPClientInstance *)m_Mange.pMain,TCPCLIENT_DISCONNECT);
				m_pHandle->OnDisconnected(TCPCLIENT_DISCONNECT);
			}
			else
			{
				m_pHandle->OnDisconnected((ITCPClientInstance *)m_Mange.pMain,TCPCLIENT_CONNECTTIMEOUT);
				m_pHandle->OnDisconnected(TCPCLIENT_CONNECTTIMEOUT);
			}			
		}			
	}

	if(m_Mange.bev!=NULL){bufferevent_free(m_Mange.bev);m_Mange.bev = NULL;}
	if(m_Mange.base!=NULL){event_base_free(m_Mange.base);m_Mange.base = NULL;}
	OutpuMessage(LOG_TYPE_DEBUG,"CTCPClient::Connect End.");
	return true;
}

void CTCPClient::ResetReadBuff()
{
	pthread_mutex_lock(&m_NetReadlock);
	m_RBuff.wIndex = 0;
	m_RBuff.rIndex = 0;
	m_RBuff.PackNum = 0;
	pthread_mutex_unlock(&m_NetReadlock);
}

void CTCPClient::ResetSendBuff()
{
	pthread_mutex_lock(&m_NetSendlock);
	m_SBuff.wIndex = 0;
	m_SBuff.rIndex = 0;
	m_SBuff.PackNum = 0;
	pthread_mutex_unlock(&m_NetSendlock);
}
