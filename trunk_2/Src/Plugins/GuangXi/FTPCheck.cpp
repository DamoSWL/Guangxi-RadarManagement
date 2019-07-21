#include "FTPCheck.h"

CFTPCheck::CFTPCheck(void)
{

}


CFTPCheck::~CFTPCheck(void)
{
}

void CFTPCheck::fileMonitor()
{

	m_FTPBaseCheck.setUpdateTime(GetPlug()->getFtpUpdateTime().baseDataTime);
	m_FTPBaseCheck.insertFileSpec(wxT("*_BaseDataFtp2.log"));
	m_BaseDataHandler = CreateThread(nullptr, 0, CFTPBaseDataCheck::fileMonitor, &m_FTPBaseCheck, 0, nullptr);

	m_FTPProductCheck.setUpdateTime(GetPlug()->getFtpUpdateTime().productTime);
	m_FTPProductCheck.insertFileSpec(wxT("*_ProductFtp2.log"));
	m_ProductDataHandler = CreateThread(nullptr, 0, CFTPProductDataCheck::fileMonitor, &m_FTPProductCheck, 0, nullptr);

	m_FTPStateCheck.setUpdateTime(GetPlug()->getFtpUpdateTime().zhuangtaiTime);
	m_FTPStateCheck.insertFileSpec(wxT("*_StatusFtp2.log"));
	m_FTPStateCheck.insertFileSpec(wxT("*_StatusFtp3.log"));
	m_StateDataHandler = CreateThread(nullptr, 0, CFTPStateDataCheck::fileMonitor, &m_FTPStateCheck, 0, nullptr);

}

void CFTPCheck::quit()
{

	if(WaitForSingleObject(m_BaseDataHandler,INFINITE) == WAIT_OBJECT_0)
	{
		CloseHandle(m_BaseDataHandler);
	}

	if(WaitForSingleObject(m_ProductDataHandler,INFINITE) == WAIT_OBJECT_0)
	{
		CloseHandle(m_ProductDataHandler);
	}

	if(WaitForSingleObject(m_StateDataHandler,INFINITE) == WAIT_OBJECT_0)
	{
		CloseHandle(m_StateDataHandler);
	}


}

