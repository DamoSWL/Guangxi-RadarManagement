#include "FileGenerationCheck.h"


CFileGenerationCheck::CFileGenerationCheck(void)
{
}


CFileGenerationCheck::~CFileGenerationCheck(void)
{
}

void CFileGenerationCheck::fileMonitor()
{
	m_baseDataNewFileCheck.setUpdateTime(GetPlug()->getUpdateTime().baseDataTime);
	m_productNewFileCheck.setUpdateTime(GetPlug()->getUpdateTime().productTime);
	m_zhuangtaiNewFileCheck.setUpdateTime(GetPlug()->getUpdateTime().zhuangtaiTime);
	m_pintuNewFileCheck.setUpdateTime(GetPlug()->getUpdateTime().pintuTime);

	m_baseDataNewFileHandler = CreateThread(nullptr, 0, CBaseDataCheck::fileMonitor, &m_baseDataNewFileCheck, 0, nullptr);
	m_productNewFileHandler = CreateThread(nullptr, 0, CProductCheck::fileMonitor, &m_productNewFileCheck, 0, nullptr);
	m_zhuangtaiNewFileHandler = CreateThread(nullptr, 0, CZhuangtaiCheck::fileMonitor, &m_zhuangtaiNewFileCheck, 0, nullptr);
	m_pintuNewFileHandler = CreateThread(nullptr, 0, CPintucheck::fileMonitor, &m_pintuNewFileCheck, 0, nullptr);

}

void CFileGenerationCheck::quit()
{
	if(WaitForSingleObject(m_baseDataNewFileHandler,INFINITE) == WAIT_OBJECT_0)
	{
		CloseHandle(m_baseDataNewFileHandler);
	}

	if(WaitForSingleObject(m_productNewFileHandler,INFINITE) == WAIT_OBJECT_0)
	{
		CloseHandle(m_productNewFileHandler);
	}

	if(WaitForSingleObject(m_zhuangtaiNewFileHandler,INFINITE) == WAIT_OBJECT_0)
	{
		CloseHandle(m_zhuangtaiNewFileHandler);
	}

	if(WaitForSingleObject(m_pintuNewFileHandler,INFINITE) == WAIT_OBJECT_0)
	{
		CloseHandle(m_pintuNewFileHandler);
	}
}
