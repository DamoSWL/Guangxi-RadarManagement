#pragma once

#include "type.h"
#include "RadarStatusAlarm.h"

class CRadarStatus
{
public:
	CRadarStatus(void);
	~CRadarStatus(void);


public:
	void getFileContent(std::map<wxString,UINT>& filesInfo);
	bool writeToDataBase();
	void radarStatusCheck();

private:
	std::map<UINT,RADAR_STATUS_DATA> m_radarStatus;
	CRadarStatusAlarm m_radarStatusAlarm;
		 
};

