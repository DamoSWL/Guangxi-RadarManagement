/********************************************************************
	created:	2014/01/18
	created:	18:1:2014   10:21
	filename: 	e:\Work\GRS\Framework\Src\WRISFramework\ProcessLocker.h
	file path:	e:\Work\GRS\Framework\Src\WRISFramework
	file base:	ProcessLocker
	file ext:	h
	author:		wwm
	
	purpose:	���̼��ٽ���������
	�磺CProcessLocker locker(wxT("lockname"),10000);//locker��������ʱ�ͷŸ���
*********************************************************************/
#pragma once
class CProcessLocker
{
public:
	//************************************
	// Method:    CProcessLocker
	// FullName:  CProcessLocker::CProcessLocker
	// Access:    public 
	// Returns:   
	// Qualifier:	������ֵ���������캯��
	// Parameter: Parameter: const wxString & LocalName			//���̼�ͬ�������ƣ������Ʋ���ͬ�����������Ϊ�ض� ���ַ����硰12324323khkh98009dfgdf��
	//															//��ϵͳ������ȫ���źš��¼���ͬ��ʱ���ܵ�����ʧЧ
	// Parameter: bool & ret							//��������ȡ״̬����ֵ����ture����ȡ�ɹ�����false����ȡʧ��
	// Parameter: const unsigned long timeoutms			//������ʱֵ��A���̻�ȡ����B���̽���ȴ���ʱֵ�����0xfffffff����
	//************************************
	CProcessLocker(const wxString &LocalName,bool &ret,const unsigned long TimeOutMS=0xffffffff)
	{
		hMemShare = NULL;		
		wxString GlobalName=wxT("Global\\")+LocalName;
		wxDateTime StartTime=wxDateTime::Now();
		while(hMemShare==NULL)
		{
			hMemShare = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(int),GlobalName.c_str());
			if(hMemShare!=NULL)
			{
				if(GetLastError()==ERROR_ALREADY_EXISTS)//ȫ�ֹ����ڴ��Ѿ����ڣ�ѭ�����´���ֱ�������ɹ����ﵽ��������
				{
					YWTRACE0( "WaitReGetProcessLock");
					CloseHandle(hMemShare);hMemShare=NULL;
					if(TimeOutMS==0) break;//���ȴ���ֱ�ӷ���
					wxMilliSleep(77);
					wxTimeSpan sp=wxDateTime::Now()-StartTime;
					if(sp.GetMilliseconds()>=TimeOutMS) //�ﵽ��ʱֵ���ټ�������
					{
						YWTRACE0( "WaitReGetProcessLock TimeOut");
						break;
					}
					continue;
				}
				else		//�ɹ���ʧ��
					break;
			}
			else //����ʧ��
			{
				break;	
			}
		}
		YWTRACE0( "GetProcessLock��name=%s",GlobalName.mb_str(wxConvLocal) );
		if(hMemShare!=NULL) ret = true;
		else ret = false;		
	}
	//************************************
	// Method:    CProcessLocker
	// FullName:  CProcessLocker::CProcessLocker
	// Access:    public 
	// Returns:   
	// Qualifier:	�޷���ֵ���������캯��
	// Parameter: const wxString & LocalName			//���̼�ͬ�������ƣ������Ʋ���ͬ�����������Ϊ�ض� ���ַ����硰12324323khkh98009dfgdf��
	//													//��ϵͳ������ȫ���źš��¼���ͬ��ʱ���ܵ�����ʧЧ
	// Parameter: const unsigned long timeoutms			//������ʱֵ��A���̻�ȡ����B���̽���ȴ���ʱֵ�����0xfffffff����
	//************************************
	CProcessLocker(const wxString &LocalName,const unsigned long TimeOutMS=0xffffffff)
	{
		hMemShare = NULL;		
		wxString GlobalName=wxT("Global\\")+LocalName;		
		unsigned long StartClock = GetTickCount();
		while(hMemShare==NULL)
		{
			hMemShare = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(int),GlobalName.c_str());
			if(hMemShare!=NULL)
			{
				if(GetLastError()==ERROR_ALREADY_EXISTS)//ȫ�ֹ����ڴ��Ѿ����ڣ�ѭ�����´���ֱ�������ɹ����ﵽ��������
				{
					YWTRACE0( "WaitReGetProcessLock");
					CloseHandle(hMemShare);hMemShare=NULL;
					wxMilliSleep(77);					
					if( (GetTickCount()-StartClock)>=TimeOutMS) //�ﵽ��ʱֵ���ټ�������
					{
						YWTRACE0( "WaitReGetProcessLock TimeOut");
						break;
					}
					continue;
				}
				else		//�ɹ���ʧ��
					break;
			}
			else //����ʧ��
			{
				break;	
			}
		}
		YWTRACE0( "GetProcessLock��name=%s",GlobalName.mb_str(wxConvLocal) );
	}
	~CProcessLocker(void)
	{
		if(hMemShare!=NULL) {CloseHandle(hMemShare);hMemShare=NULL;}
		YWTRACE0( "ReleaseProcessLock");
	}

	void YWTRACE0( const char *format, ... )
	{
		/*char _mess[500];

		va_list args;
		va_start(args,   format);
		vsprintf(_mess,   format,   args);
		va_end(args);

		::OutputDebugStringA( _mess );*/
	}
private:
	HANDLE hMemShare;
};

