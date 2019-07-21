// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� TCPSERVER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// TCPSERVER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef TCPSERVER_EXPORTS
#define TCPSERVER_API __declspec(dllexport)
#else
#define TCPSERVER_API __declspec(dllimport)
#endif
#pragma once

#define WSMAXSIZE	32		//WebSockͷ����ֽ���
enum UserLogoinStatus
{
	NOLOGOIN = -1,		//û�е�¼
	LOGOINCLOSE = 0,		//�ѹر�
	LOGOINFALSE = 1,		//��¼���󣬿����û�������
	LOGOINREPEAT = 2,		//���û��Ѿ�����
	LOGOINTRUE = 3		//��¼��ȷ	
};

#pragma pack(push, 1)
typedef struct _SENDDATATYPE_
{
	union
	{
		unsigned char cTypeBuf;
		struct
		{
			unsigned char all : 1;			//�����еĿͻ��˷���
			unsigned char Socket : 1;		//����ָ�������ݵ�socket
			unsigned char Web : 1;			//������WEB�Ŀͻ��˷���
			unsigned char iPad : 1;		//������iPad�Ŀͻ��˷���
			unsigned char UT : 1;			//������UT�Ŀͻ��˷���
			unsigned char cTmp : 3;
		};
	};
}tagSendDataType;

typedef struct _UserLogoinStatus_
{
	union
	{
		int STBuf;
		struct
		{
			char LogoinStatus;
			char WebSocket;
			tagSendDataType SendUserType;
			char tmp1;
		};
	};
}tagUserLogoinStatus;
#pragma pack( pop )

class  ITCPServerSocketNet
{
public:
	virtual void OnReceiveData(const char *pData, unsigned int len, int Socket, const char *IP) = 0;//���ݽ�����Ӧ
	virtual void OnConnected(int Socket, const char *IP) = 0;		//�û��˳���¼
	virtual void OnDisconnected(int Socket) = 0;	//�û��˳���¼	
	virtual void AddLog(int type, const char *format) = 0;	//Log���
};
class  ITCPInstance
{
public:
	virtual bool StartServer(const char *IP,		/*����IP��ַ��ΪNULL��ָ����ַ���ʱ����ָ��IP*/
		const unsigned short Port,					/*�󶨷���˿�*/
		const unsigned int iMaxNumConnections = 1,	/*�������������*/
		const bool bNeedPack = false,				/*�����Ƿ���Ҫ���cdyw�������ʽ,true ��Ҫ��� */
		const bool bNeedLogoin = false				/*�ͻ����Ƿ���Ҫ��¼*/
	) = 0;
	virtual void StopServer() = 0;
	virtual bool IsWork() = 0;
	virtual bool SendData(
		const char* pData,					/*��������ַ*/
		const unsigned int len,				/*���ͻ���������*/
		int Socket,							/*���͵�Socket����ָ���Ŀͻ��ˣ�������д0*/
		unsigned int iSendUserType = 1) = 0;	/*�������ݵ�ָ���Ŀͷ��ˣ�Ĭ�Ϸ��͵�ָ��socket��*/
	virtual void RegHandle(ITCPServerSocketNet *pHandle) = 0;	//ע��ut��������
	virtual void DisconnectUser(int Socket, bool bAll = false) = 0;				//�����Ͽ�ָ��Socket����
	virtual void SetLoginSuccess(int Socket, int status) = 0;	//���õ�¼״̬��status��enum UserLogoinStatus
};

// �����Ǵ� TCPServer.dll ������
class TCPSERVER_API CTCPServerPlugin
{
public:
	CTCPServerPlugin(void);
	~CTCPServerPlugin();

public:
	ITCPInstance *GetInstance(const unsigned int RecePackSize,		/*���հ����ֵ*/
		const unsigned int SendPackSize,		/*���Ͱ����ֵ*/
		const unsigned int ReceBuffSize,		/*���ջ�������С*/
		const unsigned int SendBuffSize);
	bool ReleaseInstance(ITCPInstance *pIns);
};


