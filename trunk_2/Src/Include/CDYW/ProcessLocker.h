/********************************************************************
	created:	2014/01/18
	created:	18:1:2014   10:21
	filename: 	e:\Work\GRS\Framework\Src\WRISFramework\ProcessLocker.h
	file path:	e:\Work\GRS\Framework\Src\WRISFramework
	file base:	ProcessLocker
	file ext:	h
	author:		wwm
	
	purpose:	进程间临界区保护锁
	如：CProcessLocker locker(wxT("lockname"),10000);//locker对象析构时释放该锁
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
	// Qualifier:	带返回值进程锁构造函数
	// Parameter: Parameter: const wxString & LocalName			//进程间同步锁名称，该名称不能同名，最好设置为特定 长字符串如“12324323khkh98009dfgdf”
	//															//与系统给其它全局信号、事件等同名时可能导致锁失效
	// Parameter: bool & ret							//进程所获取状态返回值，“ture”获取成功，“false”获取失败
	// Parameter: const unsigned long timeoutms			//保护超时值，A进程获取锁，B进程进入等待超时值，最大0xfffffff毫秒
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
				if(GetLastError()==ERROR_ALREADY_EXISTS)//全局共享内存已经存在，循环重新创建直到创建成功，达到锁定功能
				{
					YWTRACE0( "WaitReGetProcessLock");
					CloseHandle(hMemShare);hMemShare=NULL;
					if(TimeOutMS==0) break;//不等待，直接返回
					wxMilliSleep(77);
					wxTimeSpan sp=wxDateTime::Now()-StartTime;
					if(sp.GetMilliseconds()>=TimeOutMS) //达到超时值后不再继续创建
					{
						YWTRACE0( "WaitReGetProcessLock TimeOut");
						break;
					}
					continue;
				}
				else		//成功或失败
					break;
			}
			else //创建失败
			{
				break;	
			}
		}
		YWTRACE0( "GetProcessLock，name=%s",GlobalName.mb_str(wxConvLocal) );
		if(hMemShare!=NULL) ret = true;
		else ret = false;		
	}
	//************************************
	// Method:    CProcessLocker
	// FullName:  CProcessLocker::CProcessLocker
	// Access:    public 
	// Returns:   
	// Qualifier:	无返回值进程锁构造函数
	// Parameter: const wxString & LocalName			//进程间同步锁名称，该名称不能同名，最好设置为特定 长字符串如“12324323khkh98009dfgdf”
	//													//与系统给其它全局信号、事件等同名时可能导致锁失效
	// Parameter: const unsigned long timeoutms			//保护超时值，A进程获取锁，B进程进入等待超时值，最大0xfffffff毫秒
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
				if(GetLastError()==ERROR_ALREADY_EXISTS)//全局共享内存已经存在，循环重新创建直到创建成功，达到锁定功能
				{
					YWTRACE0( "WaitReGetProcessLock");
					CloseHandle(hMemShare);hMemShare=NULL;
					wxMilliSleep(77);					
					if( (GetTickCount()-StartClock)>=TimeOutMS) //达到超时值后不再继续创建
					{
						YWTRACE0( "WaitReGetProcessLock TimeOut");
						break;
					}
					continue;
				}
				else		//成功或失败
					break;
			}
			else //创建失败
			{
				break;	
			}
		}
		YWTRACE0( "GetProcessLock，name=%s",GlobalName.mb_str(wxConvLocal) );
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

