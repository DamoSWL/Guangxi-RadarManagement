#pragma once

#include "IProcess.h"
#include "ThreePhaseAlarm.h"


class CThreePhasePower : public wxEvtHandler,
	public IProcess
{
public:
	CThreePhasePower(void);
	virtual ~CThreePhasePower(void);

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
	void sendThreePhaseCmd(UINT id);
	void onwriteDataTimeout(wxTimerEvent& event);
	void onCmdTimeout(wxTimerEvent& event);

	bool writeToDataBase();

private:
	wxTimer m_cmdTimer;
	wxTimer m_writeDataTimer;

	std::set<UINT> m_IDSet;
	std::map<UINT,ThreePhaseParam> m_threePhaseParam;
	std::map<UINT,bool> m_newThreePhaseFlag;

	//std::map<UINT,CThreePhaseAlarm*> m_ThreePhaseAlarm;

	std::string m_IPAddress;


	DECLARE_EVENT_TABLE()


};

