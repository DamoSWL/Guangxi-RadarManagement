#pragma once


#include <Windows.h>
#include "BaseDataCheck.h"
#include "ProductCheck.h"
#include "ZhuangtaiCheck.h"
#include "Pintucheck.h"

class CFileGenerationCheck
{
public:
	CFileGenerationCheck(void);
	~CFileGenerationCheck(void);

public:
	void fileMonitor();
	void quit();


private:
	CBaseDataCheck m_baseDataNewFileCheck;
	CProductCheck m_productNewFileCheck;
	CZhuangtaiCheck m_zhuangtaiNewFileCheck;
	CPintucheck m_pintuNewFileCheck;

	HANDLE m_baseDataNewFileHandler;
	HANDLE m_productNewFileHandler;
	HANDLE m_zhuangtaiNewFileHandler;
	HANDLE m_pintuNewFileHandler;

};

