#pragma once

#include "type.h"

class CRadarStatusAlarm
{
public:
	CRadarStatusAlarm(void);
	~CRadarStatusAlarm(void);

public:
 	void getAlarmType();
	void getThresholdValue();
	void getRadarStatusData(const RADAR_STATUS_DATA& radarStatus);
	void RadarStatusCheck();

private:
	bool writeToDataBase(const std::string& name);
	bool writeToDataBasewithEndTime(const std::string& name);

private:
	std::map<std::string,std::string> m_alarmType;
	std::map<std::string,std::pair<FLOAT,FLOAT> > m_ThreadholdValues;
	std::map<std::string,FLOAT> m_RadarStatusValues;
	std::map<std::string,bool> m_RadarStatusExpFlag;
	std::map<std::string,std::pair<UINT,UINT> > m_RadarStatusEndureTime;
	std::map<std::string,std::string> m_RadarStatusContent;
	std::map<std::string,std::string> m_RadarStatusConcreteContent;

};

