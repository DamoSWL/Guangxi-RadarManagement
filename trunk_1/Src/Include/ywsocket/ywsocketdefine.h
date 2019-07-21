
#ifndef _YWSOCKET_DEFINE_HEADER_070918
#define _YWSOCKET_DEFINE_HEADER_070918


//////////////////////////////////////////////////////////////////////////
#define YW_EVENT_RESOLVE_FALSE		0		/*! ���������ַʧ��  */
#define YW_EVENT_SER_CRE_OK			1		/*! ���񴴽��ɹ� */
#define YW_EVENT_SER_CRE_FALSE		2		/*! ���񴴽�ʧ�� */
#define YW_EVENT_CONNECT			3		/*! ���ӵ������ */
#define YW_EVENT_CONNECT_FALSE		4		/*! ���ӵ������ʧ�� */
#define YW_EVENT_DROP_REMOTE		5		/*! Զ�����������ر� */
#define YW_EVENT_DROP_ERROR			6		/*! �����쳣�ж�*/
#define YW_EVENT_AUTO_ERROR			7		/*! Auto Receive count has over*/
#define YW_EVENT_KICK				8		/*! kick a client( not user) */
#define YW_EVENT_DISCONNECT			9		/*! �����ж����� */
#define YW_EVENT_RECEIVE_ERROR		10		/*! ���ݽ��չ��� */
#define YW_EVENT_SOCKET_CREATE_ERROR 11     /*! Socket �������� */
#define YW_EVENT_SOCKET_BIND_ERROR  12		/*! Socket �󶨴��� */
#define YW_EVENT_RECEIVE_THREAD_START 13    /*! ���ݽ����߳����� */
#define YW_EVENT_RECEIVE_THREAD_STOP 14		/*! ���ݽ����߳�ֹͣ */
#define YW_EVENT_UDP_CREATE_OK 15		/*! Socket �����ɹ� */
#define YW_EVENT_RECEIVE_THREAD_EXIT 16  /*! �����߳������˳� */
#define YW_EVENT_RECEIVE_THREAD_TIMEOUT 17 /*! �����߳̽������ݳ�ʱ */
#define YW_EVENT_RECEIVE_DUPLICATE_PACKGE 18 /*! ���յ��ظ����ݰ� */
#define YW_EVENT_RECEIVE_LOST_PACKGE 19 /*! ��ʧ�����ݰ� */
#define YW_EVENT_RECEIVE_PACKGE_INFO 20 /*! ���յ������ݰ�����Ϣ */
/////////////////////////

#define STR_SOCKET_CREATE_ERROR "Socket create error!"
#define STR_SOCKET_BIND_ERROR "Socket bind error,please check ip or port!"

//////////////////////////////////////////////////////////////////////////
#define TCPSENDDEFAULTSIZE	8192
#define TCPRECEDEFAULTSIZE	65536

#define UDPSENDDEFAULTSIZE	8192
#define UDPRECEDEFAULTSIZE	65536


#define OPT_SOCKET_CONNECT 0	//! TCP �ͻ���������Ϣ
#define OPT_SOCKET_BIND 1		//! ����Ϣ
#define OPT_BUFFER_INFO 2		//! Buffer �����Ϣ
#define OPT_LEAVEPACKGE_INFO 3  //! VXworks�ְ����ʣ��Packge��С��Ϣ
#define OPT_THREAD_TYPE 4  //! ���ݽ����߳����ͣ� 0����ͨ��1�������̣߳�2.��Ƭ�߳�
#define OPT_SEND_DEST_INFO 5  //! ���ݷ��͵�Ŀ����Ϣ������UDP��
#define OPT_MULTICAST 6	//�鲥��Ա�ˣ����ӵ���

namespace CDYW
{
	namespace SOCKET
	{
#pragma pack(push,4)

		//! socket������Ϣ
		typedef struct _SOCKET_TCP
		{
			char szBindIP[20];	//!< �󶨵�IP��Ϣ
			int iBindPort;		//!< �󶨵Ķ˿ں�
			char szConnIP[20];	//!< ���ӵ�IP��Ϣ
			int iConnPort;		//!< ���ӵĶ˿ں�
		}tagSocketConnect;

		//! �󶨵����ص�Socket��Ϣ
		typedef struct _SOCKET_BIND
		{
			char szBindIP[20];	//!< �󶨵�IP��Ϣ
			int iBindPort;		//!< �󶨵Ķ˿ں�
		}tagSocketBind;

		typedef struct _SOCKET_SEND
		{
			char szSendIP[20];	//!< ���͵�IP��Ϣ
			int iSendPort;		//!< ���͵Ķ˿ں�
		}tagSocketSend;

		/*! 
			\brief �������ݽṹ

			Socket �ṩ���幦��,������һ���Ͷ�����������
		*/
		typedef struct _BufferInfo
		{
			unsigned int RecBufferSize;		//�����ں˽��ջ����С��ͨ��setsockopt���ã�
			unsigned int SendBufferSize;	//�����ں˷��ͻ����С��ͨ��setsockopt���ã�
		}tagBufferInfo;

		typedef struct _LeavePackInfo
		{
			int iLeaveSize;
		}tagLeavePackgeInfo;

		typedef struct _ReceThreadInfo
		{
			int iRecvThreadType; // ���ݽ����߳����ͣ� 0����ͨ��1�������̣߳�2.��Ƭ�߳�
		}tagReceThreadInfo;

		//! ���ݰ�������Ϣ
		typedef struct _RecePackgeInfo
		{
			int iBuffSize; // �����С
			int iReceSize; // ���յ������ݴ�С
			int iReverd0;  // ����0
			int iReverd1;  // ����1
		}tagRecePackgeInfo;
		//! �鲥��Ϣѡ�����ýṹ��
		typedef struct _MultiCastInfo
		{
			char szMCIP[20];		//�鲥IP��ַ
			int	 iMCPort;			//�鲥�˿�
			char szLocalIP[20];		//����IP��ַ
			int	 iLocalPort;		//����IP�˿�
			int	 iSendBuffSize;		//���ͻ�������С
			int	 iRecvBuffSize;		//���ջ�������С
		}tagMultiCastInfo;
#pragma pack(pop)
	}; // namespace SOCKET
}; // namespace CDYW



#endif
