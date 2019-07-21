// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� TCPCLIENT_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// TCPCLIENT_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef TCPCLIENT_EXPORTS
#define TCPCLIENT_API __declspec(dllexport)
#else
#define TCPCLIENT_API __declspec(dllimport)
#endif
#pragma once;

#define WSMAXSIZE	32		//WebSockͷ����ֽ���
enum _DISCONNECT_STATUS_
{
	TCPCLIENT_NOERROR = 0,	//�޴���
	TCPCLIENT_DISCONNECT,		//���������Ͽ�����
	TCPCLIENT_CONNECTTIMEOUT,	//���ӳ�ʱ��Ŀ�ĵ�ַ�����û򲻴���
	TCPCLIENT_SERDISCONNECT,	//����������Ͽ�����
	TCPCLIENT_SERPORTERR,		//�����IP��ַ���ڣ���δ��������
	TCPCLIENT_ERROR,			//�������Ӵ���
	TCPCLIENT_BINDERROR,		//���ص�ַ�󶨴���
};
class ITCPClientInstance;
class  ITCPClientSocketNet
{
public:
	virtual void OnReceiveData(const char *pData, unsigned int len) = 0;//���ݽ�����Ӧ
	virtual void OnConnected() = 0;		//�û��˳���¼
	virtual void OnDisconnected(enum _DISCONNECT_STATUS_ type) = 0;	//�û��˳���¼	
																	//
	virtual void OnReceiveData(ITCPClientInstance *Instance, const char *pData, unsigned int len) {};//���ݽ�����Ӧ
	virtual void OnConnected(ITCPClientInstance *Instance) {};		//�û��˳���¼
	virtual void OnDisconnected(ITCPClientInstance *Instance, enum _DISCONNECT_STATUS_ type) {};	//�û��˳���¼	

	virtual void AddLog(int type, const char *format) = 0;
};
class  ITCPClientInstance
{
public:
	virtual bool ConnectServer(const char *LocalIP, const char *IP,		/*����IP��ַ��ΪNULL��ָ����ַ���ʱ����ָ��IP*/
		const unsigned short Port,					/*�󶨷���˿�*/
		const bool bNeedPack = false				/*�����Ƿ���Ҫ���cdyw�������ʽ,true ��Ҫ��� ��ͬʱ�Զ������������Զ��������*/
	) = 0;
	virtual void DisconnectServer() = 0;
	virtual bool IsConneted() = 0;
	virtual bool SendData(
		const char* pData,			/*��������ַ*/
		const unsigned int len) = 0;		/*���ͻ���������*/
	virtual void RegHandle(ITCPClientSocketNet *pHandle) = 0;	//ע��ut��������
};

// �����Ǵ� TCPClient.dll ������
class TCPCLIENT_API CTCPClientPlugin {
public:
	CTCPClientPlugin(void);
	~CTCPClientPlugin();
public:
	ITCPClientInstance *GetInstance(
		const unsigned int RecePackSize,		/*���հ����ֵ*/
		const unsigned int SendPackSize,		/*���Ͱ����ֵ*/
		const unsigned int ReceBuffSize,		/*���ջ�������С*/
		const unsigned int SendBuffSize			/*���ͻ�������С*/
	);					//��ȡTCP����ʵ�������Ի�ȡ���
	void ReleaseInstance(ITCPClientInstance *pInstance);	//�ͷ�TCP����ʵ��

	int GetInstanceSize();
};

