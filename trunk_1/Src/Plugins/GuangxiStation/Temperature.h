#pragma once

#include "IProcess.h"
#include <tuple>
#include "TemperatureRadarAlarm.h"
#include "TemperaturePowerAlarm.h"

class CTemperature : public wxEvtHandler,
	public IProcess
{
public:
	CTemperature(void);
	virtual ~CTemperature(void);

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
	void sendTemperatureCmd(UINT id);
	void onwriteDataTimeout(wxTimerEvent& event);
	void onCmdTimeout(wxTimerEvent& event);

	bool writeToDataBase();

private:
	wxTimer m_cmdTimer;
	wxTimer m_writeDataTimer;

	std::set<UINT> m_IDSet;
	std::map<UINT,std::tuple<FLOAT,UINT,UINT> > m_TemperatureParam;
	std::map<UINT,bool> m_newTemperatureFlag;

	std::string m_IPAddress;

	//std::map<UINT,CTemperatureRadarAlarm*> m_temperatureRadarAlarm;
	//std::map<UINT,CTemperaturePowerAlarm*> m_temperaturePowerAlarm;

	DECLARE_EVENT_TABLE()

};

