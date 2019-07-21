#pragma once

#include "IProcess.h"

class CAirCompressor : public wxEvtHandler,
	public IProcess
{
public:
	CAirCompressor(void);
	virtual ~CAirCompressor(void);
	

public:
	virtual void setID(const std::set<UINT>& id);
	virtual void processCmd(const char* pData,unsigned int len);
	virtual void startTimer(int interval);
	virtual void stopTimer();
	virtual void setIPAddress(const std::string& IPStr);

public:
	static int processData(void *pHandle, const char *pData, UINT Len);
	static void processEvent(void *pHandle, int Opt, char *pData = nullptr, UINT Len = 0);
	   
private:
	void startWriteDataTimer();
	void sendAirCompressorCmd(UINT id);
	void onwriteDataTimeout(wxTimerEvent& event);
	void onCmdTimeout(wxTimerEvent& event);

	bool writeToDataBase();

private:
	wxTimer m_cmdTimer;
	wxTimer m_writeDataTimer;

	std::set<UINT> m_IDSet;
	std::map<UINT,std::pair<UINT,UINT> > m_AirCompressorParam;
	std::map<UINT,bool> m_newAirCompressorFlag;

	std::string m_IPAddress;

	DECLARE_EVENT_TABLE()

};

