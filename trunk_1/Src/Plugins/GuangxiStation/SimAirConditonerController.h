#pragma once
#include "UDP.h"

class CSimAirConditonerController
{
public:
	CSimAirConditonerController(void);
	~CSimAirConditonerController(void);

public:
	bool startReceive();
	bool stopReceive();
	bool sendData(const char* cmd,UINT len,const char* dstIP,UINT port);
	void setID(const std::set<UINT>& ID) {m_IDSet = ID;}
	const std::set<UINT>& getID() {return m_IDSet;}

public:
	static int processData(void *pHandle, const char *pData, UINT Len);
	static void processEvent(void *pHandle, int Opt, char *pData = nullptr, UINT Len = 0);

private:
	CUDP m_UDPFactory;
	ISocketNet* m_UDPHandler;
	std::set<UINT> m_IDSet;
};

