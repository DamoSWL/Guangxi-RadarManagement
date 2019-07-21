/********************************************************************
	created:	2014/01/19
	created:	19:1:2014   13:44
	filename: 	E:\Work\GRS\Framework\Src\include\cdyw\ProcessShareMemory.h
	file path:	E:\Work\GRS\Framework\Src\include\cdyw
	file base:	ProcessShareMemory
	file ext:	h
	author:		wwm
	
	purpose:	基于该类实现GRS程序与Bugreport程序之间的进程通信
*********************************************************************/
#pragma once

#include "ProcessLocker.h"


#define szGlobalLockName "LockasdfGRSFrameworkAppShareMemoryghjkfiLock"
#define szGlobalMemoryName L"Global\\asdfGRSFrameworkAppShareMemoryghjkfi"

#define MAXECHOSIZE		1000
#define MAXECHOCNT		10
typedef struct _MEMSHOWR_
{
	unsigned int OpenCnt;		//打开句柄数目

	unsigned int numOfEcho;		//未处理的消息数据
	unsigned int WriteIndex;
	unsigned int ReadIndex;
	char DataBuff[MAXECHOCNT][MAXECHOSIZE];
}tagShareMemmory;

#define BUF_SIZE sizeof(tagShareMemmory)

class CProcessShareMemory
{
public:
	CProcessShareMemory(void)
	{
		m_MemoryHandle = NULL;
		m_pShareMemory = NULL;

		bool bRet=false;
		CProcessLocker lock(wxT(szGlobalLockName),bRet,2000);
		if(bRet==false) return;

		if(m_pShareMemory==NULL)
		{
			m_MemoryHandle=CreateFileMapping(	INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,BUF_SIZE,szGlobalMemoryName); 
			if(m_MemoryHandle!=NULL)
			{
				bool bInitFlag=false;
				YWTRACE0("CProcessShareMemory::CreateHandle=0x%08x",m_MemoryHandle);
				if(GetLastError()==ERROR_ALREADY_EXISTS)//全局共享内存已经存在
				{
					CloseHandle(m_MemoryHandle);m_MemoryHandle=NULL;
					m_MemoryHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,szGlobalMemoryName);  
					if(m_MemoryHandle==NULL) //打开失败
						return;

					//打开成功
					YWTRACE0("CProcessShareMemory::ALREADY_EXISTS! Open Handle = 0x%08x",m_MemoryHandle);					
				}
				else
					bInitFlag = true;

				m_pShareMemory = (tagShareMemmory*) MapViewOfFile(m_MemoryHandle, FILE_MAP_ALL_ACCESS,0,0,BUF_SIZE);
				if(m_pShareMemory!=NULL && bInitFlag)//第一次创建全局共享内存，进行初始化
				{
					memset(m_pShareMemory,0,BUF_SIZE);
					YWTRACE0("CProcessShareMemory::CreateShareMemory! Handle=0x%08x",m_MemoryHandle);
				}
			}
			else//共享内存创建失败
				return;

			if(m_pShareMemory!=NULL)//打开成功
			{
				m_pShareMemory->OpenCnt++;
				YWTRACE0("CProcessShareMemory::OPenCnt=%d,Handle=0x%08x",m_pShareMemory->OpenCnt,m_MemoryHandle);
			}
		}
	}
	~CProcessShareMemory(void)
	{
		Close();
	}
public:
	//************************************
	// Method:    Close
	// FullName:  CProcessShareMemory::Close
	// Access:    public 
	// Returns:   void
	// Qualifier: 关闭函数
	//************************************
	void Close()
	{
		wxCriticalSectionLocker lockcs(m_cs);
		if(m_pShareMemory!=NULL)	
		{
			YWTRACE0("Close::OPenCnt=%d,Handle=0x%08x",m_pShareMemory->OpenCnt-1,m_MemoryHandle);
			m_pShareMemory->OpenCnt--;
			UnmapViewOfFile(m_pShareMemory);
			m_pShareMemory = NULL;			
		}
		if(m_MemoryHandle!=NULL) {CloseHandle(m_MemoryHandle);m_MemoryHandle=NULL;}
	}
	//************************************
	// Method:    SendEchoToShareMemory
	// FullName:  CProcessShareMemory::SendEchoToShareMemory
	// Access:    public 
	// Returns:   void
	// Qualifier: 发送消息到进程共享内存空间
	// Parameter: const wxString & Echo
	//************************************
	void SendEchoToShareMemory(const wxString &Echo)
	{
		wxCriticalSectionLocker lockcs(m_cs);
		if(m_pShareMemory==NULL ||m_MemoryHandle==NULL ) return;

		long Len=strlen(Echo.mb_str(wxConvLocal))+1;//计算字符串所占空间大小
		if(Len>MAXECHOSIZE) return;

		bool bRet=false;
		CProcessLocker lock(wxT(szGlobalLockName),bRet,2000);
		if(bRet==false) return;		

		if(m_pShareMemory!=NULL)
		{
			sprintf_s(m_pShareMemory->DataBuff[m_pShareMemory->WriteIndex++],MAXECHOSIZE,"%s",Echo.mb_str(wxConvLocal));
			
			if(m_pShareMemory->WriteIndex==MAXECHOCNT) m_pShareMemory->WriteIndex=0;
			m_pShareMemory->numOfEcho++;
			if(m_pShareMemory->numOfEcho>MAXECHOCNT) m_pShareMemory->numOfEcho=MAXECHOCNT;

			YWTRACE0("SendEchoToShareMemory::Handle=0x%08x,OPenCnt=%d,W=%d,R=%d,N=%d,Mess=%s",m_MemoryHandle,
				m_pShareMemory->OpenCnt,
				m_pShareMemory->WriteIndex,m_pShareMemory->ReadIndex,m_pShareMemory->numOfEcho,
				Echo.mb_str(wxConvLocal));
		}
	}
	//************************************
	// Method:    GetEchoFromShareMemory
	// FullName:  CProcessShareMemory::GetEchoFromShareMemory
	// Access:    public 
	// Returns:   bool
	// Qualifier: 获取消息从进程共享内存空间
	// Parameter: const wxString & Echo
	//************************************
	bool GetEchoFromShareMemory(wxString &Echo,unsigned int &numOfEcho )
	{
		wxCriticalSectionLocker lockcs(m_cs);
		numOfEcho=0;
		bool bRet=false;
		CProcessLocker lock(wxT(szGlobalLockName),bRet,2000);
		if(bRet==false) return false;

		if(m_pShareMemory!=NULL && m_pShareMemory->numOfEcho>0) //共享内存空间存在消息
		{
			Echo=wxString(m_pShareMemory->DataBuff[m_pShareMemory->ReadIndex],wxConvLocal);
			memset(m_pShareMemory->DataBuff[m_pShareMemory->ReadIndex],0,MAXECHOSIZE);

			m_pShareMemory->ReadIndex++;
			if(m_pShareMemory->ReadIndex==MAXECHOCNT) m_pShareMemory->ReadIndex=0;
			m_pShareMemory->numOfEcho--;
			numOfEcho = m_pShareMemory->numOfEcho;

			YWTRACE0("----GetEchoFromShareMemory::OPenCnt=%d,W=%d,R=%d,N=%d,Handle=0x%08x",
				m_pShareMemory->OpenCnt,m_pShareMemory->WriteIndex,m_pShareMemory->ReadIndex,m_pShareMemory->numOfEcho,m_MemoryHandle);
			return true;
		}
		Echo = wxEmptyString;
		return false;
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
	HANDLE m_MemoryHandle;			//共享内存句柄
	tagShareMemmory *m_pShareMemory;//共享内存指针
	wxCriticalSection m_cs;
};

