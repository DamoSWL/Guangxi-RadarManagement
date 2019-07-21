#pragma once

class CTemperaturePowerAlarm
{
public:
	CTemperaturePowerAlarm(void);
	~CTemperaturePowerAlarm(void);
	
public:
	void getAlarmType();
	void getThresholdValue();
	void getEnvValue(FLOAT temperature,UINT humidity);
	void envCheck();

private:
	bool writeToDataBase(const std::string name);
	bool writeToDataBasewithEndTime(const std::string name);

private:
	std::map<std::string,FLOAT> m_envValues;
	std::map<std::string,bool> m_envExpFlag;
	std::map<std::string,std::pair<UINT,UINT> > m_envEndureTime;
	std::map<std::string,std::string> m_envContent;
	std::map<std::string,std::string> m_envConcreteContent;
	std::map<std::string,std::pair<FLOAT,FLOAT> > m_ThreadholdValues;
	std::map<std::string,std::string> m_alarmType;
};

