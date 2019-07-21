#pragma once


#include "IProcess.h"

class CSixRoute : public wxEvtHandler,
	public IProcess
{
public:
	CSixRoute(void);
	virtual ~CSixRoute(void);

	virtual void setID(const std::set<UINT>& id);
	virtual void setIPAddress(const std::string& IPStr);
	virtual void processCmd(const char* pData,unsigned int len);
	virtual void startTimer(int interval);
	virtual void stopTimer();
	

public:
	static int processData(void *pHandle, const char *pData, UINT Len);
	static void processEvent(void *pHandle, int Opt, char *pData = nullptr, UINT Len = 0);

private:
	typedef struct
	{
		FLOAT voltage;
		FLOAT current;
		FLOAT activePower;
		FLOAT activeTotalPower;
		FLOAT powerFactor;
		FLOAT frequency;
	}SixRouteParam;

private:
	void startWriteDataTimer();
	void sendSixRouteCmd(UINT id);
	void onwriteDataTimeout(wxTimerEvent& event);
	void onCmdTimeout(wxTimerEvent& event);

	bool writeToDataBase();

private:
	wxTimer m_cmdTimer;
	wxTimer m_writeDataTimer;

	std::set<UINT> m_IDSet;

	std::map<UINT,std::map<UINT,UINT> > m_sixRouteParam;
	std::map<UINT,std::map<UINT,SixRouteParam>> m_newSixRouteParam;

	std::map<UINT,bool> m_newSixRouteFlag;


	std::string m_IPAddress;

	DECLARE_EVENT_TABLE()

};

