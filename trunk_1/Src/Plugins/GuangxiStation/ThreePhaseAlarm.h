#pragma once


class CThreePhaseAlarm
{
public:
	CThreePhaseAlarm(void);
	~CThreePhaseAlarm(void);

public:
	void getAlarmType();
	void getThresholdValue();
	void getThreePhaseValue(const ThreePhaseParam& param);
	void ThreePhaseCheck();

private:
	bool writeToDataBase(const std::string name);
	bool writeToDataBasewithEndTime(const std::string name);

private:
	std::map<std::string,FLOAT> m_ThreePhaseValues;
	std::map<std::string,bool> m_ThreePhaseExpFlag;
	std::map<std::string,std::pair<UINT,UINT> > m_ThreePhaseEndureTime;
	std::map<std::string,std::string> m_ThreePhaseContent;
	std::map<std::string,std::string> m_ThreePhaseConcreteContent;
	std::map<std::string,std::pair<FLOAT,FLOAT> > m_ThreadholdValues;
	std::map<std::string,std::string> m_alarmType;
};

