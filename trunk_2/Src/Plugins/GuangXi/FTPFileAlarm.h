#pragma once

class CFTPFileAlarm
{
public:
	CFTPFileAlarm(void);
	~CFTPFileAlarm(void);

public:
	bool writeToDataBase(UINT time,const std::string& fileName, BOOL flag);

private:
	std::map<UINT,std::string> m_FailFileNames;

};

