
#ifndef _YWSOCKET_DEFINE_HEADER_070918
#define _YWSOCKET_DEFINE_HEADER_070918


//////////////////////////////////////////////////////////////////////////
#define YW_EVENT_RESOLVE_FALSE		0		/*! 解析网络地址失败  */
#define YW_EVENT_SER_CRE_OK			1		/*! 服务创建成功 */
#define YW_EVENT_SER_CRE_FALSE		2		/*! 服务创建失败 */
#define YW_EVENT_CONNECT			3		/*! 连接到服务端 */
#define YW_EVENT_CONNECT_FALSE		4		/*! 连接到服务端失败 */
#define YW_EVENT_DROP_REMOTE		5		/*! 远端连接正常关闭 */
#define YW_EVENT_DROP_ERROR			6		/*! 连接异常中断*/
#define YW_EVENT_AUTO_ERROR			7		/*! Auto Receive count has over*/
#define YW_EVENT_KICK				8		/*! kick a client( not user) */
#define YW_EVENT_DISCONNECT			9		/*! 正常中断连接 */
#define YW_EVENT_RECEIVE_ERROR		10		/*! 数据接收故障 */
#define YW_EVENT_SOCKET_CREATE_ERROR 11     /*! Socket 创建错误 */
#define YW_EVENT_SOCKET_BIND_ERROR  12		/*! Socket 绑定错误 */
#define YW_EVENT_RECEIVE_THREAD_START 13    /*! 数据接收线程启动 */
#define YW_EVENT_RECEIVE_THREAD_STOP 14		/*! 数据接收线程停止 */
#define YW_EVENT_UDP_CREATE_OK 15		/*! Socket 创建成功 */
#define YW_EVENT_RECEIVE_THREAD_EXIT 16  /*! 接收线程正常退出 */
#define YW_EVENT_RECEIVE_THREAD_TIMEOUT 17 /*! 接收线程接收数据超时 */
#define YW_EVENT_RECEIVE_DUPLICATE_PACKGE 18 /*! 接收到重复数据包 */
#define YW_EVENT_RECEIVE_LOST_PACKGE 19 /*! 丢失了数据包 */
#define YW_EVENT_RECEIVE_PACKGE_INFO 20 /*! 接收到的数据包的信息 */
/////////////////////////

#define STR_SOCKET_CREATE_ERROR "Socket create error!"
#define STR_SOCKET_BIND_ERROR "Socket bind error,please check ip or port!"

//////////////////////////////////////////////////////////////////////////
#define TCPSENDDEFAULTSIZE	8192
#define TCPRECEDEFAULTSIZE	65536

#define UDPSENDDEFAULTSIZE	8192
#define UDPRECEDEFAULTSIZE	65536


#define OPT_SOCKET_CONNECT 0	//! TCP 客户端连接信息
#define OPT_SOCKET_BIND 1		//! 绑定信息
#define OPT_BUFFER_INFO 2		//! Buffer 相关信息
#define OPT_LEAVEPACKGE_INFO 3  //! VXworks分包后的剩余Packge大小信息
#define OPT_THREAD_TYPE 4  //! 数据接收线程类型， 0，普通；1，测试线程；2.分片线程
#define OPT_SEND_DEST_INFO 5  //! 数据发送的目的信息，用于UDP；
#define OPT_MULTICAST 6	//组播组员端，增加到组

namespace CDYW
{
	namespace SOCKET
	{
#pragma pack(push,4)

		//! socket连接信息
		typedef struct _SOCKET_TCP
		{
			char szBindIP[20];	//!< 绑定的IP信息
			int iBindPort;		//!< 绑定的端口号
			char szConnIP[20];	//!< 连接的IP信息
			int iConnPort;		//!< 连接的端口号
		}tagSocketConnect;

		//! 绑定到本地的Socket信息
		typedef struct _SOCKET_BIND
		{
			char szBindIP[20];	//!< 绑定的IP信息
			int iBindPort;		//!< 绑定的端口号
		}tagSocketBind;

		typedef struct _SOCKET_SEND
		{
			char szSendIP[20];	//!< 发送的IP信息
			int iSendPort;		//!< 发送的端口号
		}tagSocketSend;

		/*! 
			\brief 缓冲数据结构

			Socket 提供缓冲功能,缓冲由一个和多个缓冲项组成
		*/
		typedef struct _BufferInfo
		{
			unsigned int RecBufferSize;		//网络内核接收缓存大小（通过setsockopt设置）
			unsigned int SendBufferSize;	//网络内核发送缓存大小（通过setsockopt设置）
		}tagBufferInfo;

		typedef struct _LeavePackInfo
		{
			int iLeaveSize;
		}tagLeavePackgeInfo;

		typedef struct _ReceThreadInfo
		{
			int iRecvThreadType; // 数据接收线程类型， 0，普通；1，测试线程；2.分片线程
		}tagReceThreadInfo;

		//! 数据包接收信息
		typedef struct _RecePackgeInfo
		{
			int iBuffSize; // 缓冲大小
			int iReceSize; // 接收到的数据大小
			int iReverd0;  // 保留0
			int iReverd1;  // 保留1
		}tagRecePackgeInfo;
		//! 组播信息选项设置结构体
		typedef struct _MultiCastInfo
		{
			char szMCIP[20];		//组播IP地址
			int	 iMCPort;			//组播端口
			char szLocalIP[20];		//本地IP地址
			int	 iLocalPort;		//本地IP端口
			int	 iSendBuffSize;		//发送缓冲区大小
			int	 iRecvBuffSize;		//接收缓冲区大小
		}tagMultiCastInfo;
#pragma pack(pop)
	}; // namespace SOCKET
}; // namespace CDYW



#endif
