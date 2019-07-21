#pragma once

#include <vector>
#include <map>

class CAlarmFileCheck
{
public:
	CAlarmFileCheck(void);
	~CAlarmFileCheck(void);

public:
	typedef struct				//报警结构
	{	
		char  STATUS_DATA[9];
		char  STATUS_TIME[7];
		int   MONITOR_ALARM_FLAG;		
		int   ALARM_NO;			//雷达报警码	
	}RADAR_ALARM_DATA;

	enum
	{
		ALARMON = 1,
		ALARMOFF
	};

public:
	void fileMonitor();
	void quit();
	static DWORD WINAPI alarmFileMonitor(LPVOID para);
	void getLatestFileName(std::map<UINT,wxString>& fileInfo,UINT* newTime);
	void getAlarmInfo(const RADAR_ALARM_DATA& data);
	bool parseAlarmInfo(UINT newTime);
	void setUpdateTime(UINT time){m_updateTime = time;}

private:
	UINT getDateTime(const RADAR_ALARM_DATA& alarmInfo);
	bool writeToDataBase(const RADAR_ALARM_DATA& alarmInfo);

private:
	HANDLE m_alarmCheckHandler;

	UINT m_updateTime;
	std::vector<RADAR_ALARM_DATA> m_alarmInfo;
	
	 
};

