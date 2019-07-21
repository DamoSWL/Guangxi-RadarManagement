

#ifndef __HEADER_ywsocketclient_udp_20081110
#define __HEADER_ywsocketclient_udp_20081110

#include "../../include/cdyw/Lock.h"
#include "../../include/ywsocket/ywsocket_udp.h"
#include "../../include/ywsocket/ywsocketdefine.h"

#include "UDP.h"
//typedef int (*FunDataRecv)( void *pHandle, const char *pData, unsigned int uiLen );
//typedef void (*FunDataEvent)( void *pHandle, int uiOpt , char *pData, unsigned int uiLen );

typedef struct _ST_HANDLE
{
	void *pHandle;
	FunDataRecv sigRecv;
	FunDataEvent sigEvent;
}tagSTHandle;

using namespace CDYW;
using namespace CDYW::SOCKET;
/*! \class UDPDataReceive
	\brief UDP模式的 ISocketNet 实现

	\ingroup DataAdminGroup
*/
class UDPDataReceive:
	public ISocketNet
	,public YWSocket_UDP
{
	int _iGlobalNum;   // TCP receive所属的实例的全局编号

	pthread_mutex_t _muHandle;  // 事件处理函数的处理句柄
public:
	UDPDataReceive( int iNumIns );
	~UDPDataReceive();
	// 成员变量定义
	tagSTHandle m_handle;

public:

	//! 启动数据接收
	bool StartReceive(const int Type,const char *LocalIP,const unsigned short LocalPort,
		const char *DstIP,const unsigned short DstPort,
		const unsigned int RevBuffLen,const unsigned int SendBuffLen,const unsigned int TimeOutMS);

	//! 停止数据接收
	bool StopReceive();

	bool IsWorking();

	void RegHandle( void *pThis , FunDataRecv sigRecv, FunDataEvent sigEvent );

	void UnregHandle( void* pHandle);

	//! 发送数据到网络连接的另一端
	bool SendData( const char *pData, unsigned int uiLen,const char *DstIP=NULL,unsigned short DstPort=0 );

	//////////////////////////////////////////////////////////////////////////
	////  Socket
	//! 当PT有数据到达时，调用此函数

	int OnReceiveData( const char *pData, unsigned int len );
	void OnEvent(int EventType, void * pData=NULL, unsigned int len=0);

	const char *GetMessage();
};


#endif // __HEADER_ywsocketclient_udp_20081110

