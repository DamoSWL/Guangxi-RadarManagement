// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� UDP_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// UDP_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef UDP_EXPORTS
#define UDP_API __declspec(dllexport)
#else
#define UDP_API __declspec(dllimport)
#endif

#ifndef _UDP_20160803_
#define _UDP_20160803_

/*! ���ݽ��ջص�����
	���ص����������ڴ������ݵ��ϲ����
	\param pHandle �ص������������ʵ����ָ��
	\param pData ָ�򸽼ӵ����ݻ����ָ��
	\param uiLen �������ݵĳ���
*/
typedef int (*FunDataRecv)( void *pHandle, const char *pData, unsigned int uiLen );


/*! �¼����ջص�����
	\param pHandle �ص������������ʵ����ָ��
	\param uiOpt �¼�����
	\param pData ָ�򸽼ӵ����ݻ����ָ��
	\param uiLen �������ݵĳ���
*/
typedef void (*FunDataEvent)( void *pHandle, int uiOpt , char *pData, unsigned int uiLen );


/*! \class ISocketNet
	\brief ���ݽ��սӿ�
*/
class ISocketNet
{
public:
	
	//************************************
	// Method:    StartReceive
	// FullName:  ISocketNet::StartReceive
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier: ����UDP
	// Parameter: const int Type					0��Ե㡢1�鲥��2�㲥
	// Parameter: const char * LocalIP				��IP
	// Parameter: const unsigned short LocalPort	���ض˿�
	// Parameter: const char * DstIP				�鲥IP
	// Parameter: const unsigned short DstPort		�鲥�˿�
	// Parameter: const unsigned int RevBuffLen		���ջ�������С
	// Parameter: const unsigned int SendBuffLen	���ͻ�������С
	// Parameter: const unsigned int TimeOutMS		��ʱֵ�����
	//************************************
	virtual bool StartReceive(const int Type,const char *LocalIP,const unsigned short LocalPort,
		const char *DstIP,const unsigned short DstPort,
		const unsigned int RevBuffLen,const unsigned int SendBuffLen,const unsigned int TimeOutMS) = 0;

	//! ֹͣ���ݽ���
	virtual bool StopReceive() = 0;

	/*! Socket ��ǰ�Ƿ��ڹ���(���ݽ���)״̬
	*/
	virtual bool IsWorking() = 0;

	/*! \brief ע����Ϣ����ص�����

		ע��Ĳ�����˳�ʱ������� UnregHandle ������ע�����ݽ����¼�

		\param pHandle sigRecv��sigEvent�ص������������ʵ����ָ��
		\param sigRecv ���ݽ��ջص�����
		\param sigEvent �¼����ջص�����
		\see UnregHandle
	*/
	virtual void RegHandle( void* pHandle, FunDataRecv sigRecv, FunDataEvent sigEvent ) = 0;

	/*! ע����Ϣ����ص�����
		\param pHandle sigRecv��sigEvent�ص������������ʵ����ָ��
		\param sigRecv ���ݽ��ջص�����
		\param sigEvent �¼����ջص�����
		\see RegHandle
	*/
	virtual void UnregHandle( void* pHandle) = 0;

	/*! �������ݵ��������ӵ���һ��
		\param pData ָ�������ݻ����ָ��
		\param uiLen �������ݵĳ���
	*/ 
	virtual bool SendData( const char *pData, unsigned int uiLen ,const char *DstIP=0,unsigned short DstPort=0) = 0;

	virtual const char *GetMessage() = 0;

};

// �����Ǵ� UDP.dll ������
class UDP_API CUDP 
{
public:
	CUDP(void);
	~CUDP();
public:
	
	/*! \brief ��ȡSocketʵ��
		\return ʵ��ָ��
	*/
	ISocketNet *GetInstance();


	/*! \brief �ͷ�Socketʵ��
		\param pIns Socket��ʵ��ָ��
	*/
	bool ReleaseInstance( ISocketNet *pIns);
};

#endif