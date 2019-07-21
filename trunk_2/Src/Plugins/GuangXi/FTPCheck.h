#pragma once

#include "FTPBaseDataCheck.h"
#include "FTPProductDataCheck.h"
#include "FTPStateDataCheck.h"


class CFTPCheck
{
public:
	CFTPCheck(void);
	~CFTPCheck(void);


public:
	void fileMonitor();
	void quit();

private:
	CFTPBaseDataCheck m_FTPBaseCheck;
	CFTPProductDataCheck m_FTPProductCheck;
	CFTPStateDataCheck m_FTPStateCheck;

	HANDLE m_BaseDataHandler;
	HANDLE m_ProductDataHandler;
	HANDLE m_StateDataHandler;

};

