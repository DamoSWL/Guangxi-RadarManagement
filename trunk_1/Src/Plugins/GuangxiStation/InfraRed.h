#pragma once


#include "IProcess.h"

class CInfraRed  : public wxEvtHandler,
	public IProcess
{
public:
	CInfraRed(void);
	virtual ~CInfraRed(void);
	
public:
	virtual void setID(const std::set<UINT>& id);
	virtual void setIPAddress(const std::string& IPStr);
	virtual void processCmd(const char* pData,unsigned int len);
	virtual void startTimer(int interval);
	virtual void stopTimer();

public:
	static int processData(void *pHandle, const char *pData, UINT Len);
	static void processEvent(void *pHandle, int Opt, char *pData = nullptr, UINT Len = 0);

private:
	void startWriteDataTimer();
	void sendInfraRedCmd(UINT id);
	void onwriteDataTimeout(wxTimerEvent& event);
	void onCmdTimeout(wxTimerEvent& event);

	bool writeToDataBase();

private:
	wxTimer m_cmdTimer;
	wxTimer m_writeDataTimer;

	std::set<UINT> m_IDSet;
	std::map<UINT,UINT> m_InfraRedParam;
	std::map<UINT,bool> m_newInfraRedFlag;

	std::string m_IPAddress;
	UINT m_Count;

	DECLARE_EVENT_TABLE()

};

