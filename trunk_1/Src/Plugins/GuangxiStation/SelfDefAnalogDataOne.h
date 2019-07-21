#pragma once
#include "IProcess.h"
#include "UPSDataStructure.h"


class CSelfDefAnalogDataOne : public wxEvtHandler,
	public IProcess
{
public:
	CSelfDefAnalogDataOne(void);
	virtual ~CSelfDefAnalogDataOne(void);

	virtual void processCmd(const char* pData,unsigned int len);
	virtual void startTimer(int interval);
	virtual void stopTimer();
	virtual void setID(const std::set<UINT>& id);
	virtual void setIPAddress(const std::string& IPStr);

public:
	static int processData(void *pHandle, const char *pData, UINT Len);
	static void processEvent(void *pHandle, int Opt, char *pData = nullptr, UINT Len = 0);

private:
	void startWriteDataTimer();
	void sendUPSCmd(UINT id);
	void onwriteDataTimeout(wxTimerEvent& event);
	void onCmdTimeout(wxTimerEvent& event);

	bool writeToDataBase();
	bool isCheckSumValid(const char* pData,UINT Len);

private:
	wxTimer m_cmdTimer;
	wxTimer m_writeDataTimer;

	std::set<UINT> m_IDSet;
	std::map<UINT, bool> m_newUPSDataFlag;
	std::map<UINT,UPSSelfDefAnalogDataStructureOne> m_UPSParam;

	std::string m_IPAddress;


	DECLARE_EVENT_TABLE()

};

